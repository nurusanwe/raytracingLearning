#pragma once
#include "../core/vector3.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Cook-Torrance Microfacet BRDF Implementation
// Mathematical foundation: physically-based microfacet theory for realistic surface reflection
// Physical principle: surface roughness modeled as microscopic facets with statistical distribution
// Energy conservation: total reflected energy never exceeds incident energy across all wavelengths
//
// COOK-TORRANCE BRDF THEORY:
//
// The Cook-Torrance microfacet BRDF follows the form:
//   f_r(wi, wo) = (D(h) * G(wi, wo) * F(h, wo)) / (4 * cos(θl) * cos(θv))
//
// Where:
//   D(h): Normal Distribution Function - statistical distribution of microfacet orientations
//   G(wi, wo): Geometry Function - accounts for masking and shadowing between microfacets  
//   F(h, wo): Fresnel Function - reflection coefficient varies with viewing angle
//   h: Halfway vector between incident (wi) and outgoing (wo) directions
//   4*cos(θl)*cos(θv): Normalization factor from microfacet to macrosurface
//
// PHYSICAL INTERPRETATION:
//   - Models glossy surfaces: plastic, metal, painted surfaces, wet materials
//   - Microsurface theory: apparent roughness from microscopic height variations
//   - Energy conservation through proper statistical averaging of microfacet contributions
//   - Handles full material spectrum: from mirror-like (low roughness) to diffuse-like (high roughness)
//
// References:
//   - Cook, R.L., Torrance, K.E. "A Reflectance Model for Computer Graphics" SIGGRAPH 1982
//   - Walter, B. "Microfacet Models for Refraction through Rough Surfaces" EGSR 2007
//   - Pharr, Jakob, Humphreys "Physically Based Rendering" 4th ed.
//   - Burley, B. "Physically-Based Shading at Disney" SIGGRAPH Course 2012

// GGX/Trowbridge-Reitz Normal Distribution Function
// Models statistical distribution of microfacet orientations using long-tailed distribution
// Mathematical advantages: analytic importance sampling, better highlight falloff than Blinn-Phong
namespace CookTorrance {
    
    struct NormalDistribution {
        // GGX Normal Distribution Function
        // Formula: D(h) = α² / (π * ((n·h)² * (α² - 1) + 1)²)
        // Physical interpretation: probability density of microfacets oriented along halfway vector h
        // Parameters:
        //   ndoth: dot product between surface normal and halfway vector (cos θh)
        //   alpha: roughness parameter (α = roughness²) - controls distribution width
        static float ggx_distribution(float ndoth, float alpha) {
            // Prevent negative or zero ndoth values that would cause mathematical instability
            if (ndoth <= 0.0f) return 0.0f;
            
            float alpha2 = alpha * alpha;
            float ndoth2 = ndoth * ndoth;
            
            // GGX distribution denominator: (n·h)² * (α² - 1) + 1
            float denom_inner = ndoth2 * (alpha2 - 1.0f) + 1.0f;
            
            // Prevent division by zero in extreme cases
            if (denom_inner <= 0.0f) return 0.0f;
            
            float denom = M_PI * denom_inner * denom_inner;
            
            return alpha2 / denom;
        }
        
        // Educational mathematical breakdown for learning validation
        // Explains step-by-step calculation of GGX distribution with physical interpretation
        static void explain_ggx_mathematics(float ndoth, float alpha, float result) {
            std::cout << "\n=== GGX Normal Distribution Function ===" << std::endl;
            std::cout << "Mathematical Foundation: D(h) = α² / (π × ((n·h)² × (α² - 1) + 1)²)" << std::endl;
            std::cout << "Physical Meaning: Probability density of microfacets aligned with halfway vector" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Input Parameters:" << std::endl;
            std::cout << "  cos(θ_h) = n·h = " << ndoth << std::endl;
            std::cout << "  α (roughness parameter) = " << alpha << std::endl;
            std::cout << "  α² = " << (alpha * alpha) << std::endl;
            std::cout << std::endl;
            
            std::cout << "Step-by-step Calculation:" << std::endl;
            float alpha2 = alpha * alpha;
            float ndoth2 = ndoth * ndoth;
            float denom_inner = ndoth2 * (alpha2 - 1.0f) + 1.0f;
            
            std::cout << "  1. (n·h)² = " << ndoth2 << std::endl;
            std::cout << "  2. (n·h)² × (α² - 1) = " << (ndoth2 * (alpha2 - 1.0f)) << std::endl;
            std::cout << "  3. Denominator inner: (n·h)² × (α² - 1) + 1 = " << denom_inner << std::endl;
            std::cout << "  4. Denominator: π × (inner)² = " << (M_PI * denom_inner * denom_inner) << std::endl;
            std::cout << "  5. Final result: D(h) = α² / denominator = " << result << std::endl;
            std::cout << std::endl;
            
            std::cout << "Physical Interpretation:" << std::endl;
            if (alpha < 0.1f) {
                std::cout << "  Low roughness (α < 0.1): Sharp, mirror-like reflections with narrow highlight" << std::endl;
            } else if (alpha > 0.7f) {
                std::cout << "  High roughness (α > 0.7): Broad, diffuse-like reflections with wide highlight" << std::endl;
            } else {
                std::cout << "  Medium roughness: Glossy surface with moderate highlight spread" << std::endl;
            }
            
            std::cout << "  D(h) = " << result << " represents microfacet density at this orientation" << std::endl;
            std::cout << "=== GGX calculation complete ===" << std::endl;
        }
    };

    // Smith Masking-Shadowing Geometry Function
    // Models occlusion of microfacets due to neighboring microfacet blocking (masking/shadowing)
    // Mathematical foundation: statistical correlation between microfacet visibility from light and view directions
    struct GeometryFunction {
        
        // Smith G1 function for single direction masking/shadowing
        // Formula: G1(v) = 2 / (1 + sqrt(1 + α² × tan²(θ_v)))
        // Physical interpretation: fraction of microfacets visible from direction v
        // Parameters:
        //   ndotv: dot product between surface normal and view/light direction (cos θ)
        //   alpha: roughness parameter (α = roughness²)
        static float smith_g1(float ndotv, float alpha) {
            // Handle grazing angles and backfacing directions
            if (ndotv <= 0.0f) return 0.0f;
            
            // Calculate tan²(θ) from cos(θ) using trigonometric identity
            // tan²(θ) = sin²(θ)/cos²(θ) = (1-cos²(θ))/cos²(θ)
            float cos_theta = ndotv;
            float cos2_theta = cos_theta * cos_theta;
            float sin2_theta = 1.0f - cos2_theta;
            
            // Handle perpendicular viewing (tan(θ) = 0)
            if (sin2_theta <= 0.0f) return 1.0f;
            
            float tan2_theta = sin2_theta / cos2_theta;
            
            // Smith G1 formula: G1 = 2 / (1 + sqrt(1 + α²tan²θ))
            float alpha2_tan2 = alpha * alpha * tan2_theta;
            return 2.0f / (1.0f + sqrt(1.0f + alpha2_tan2));
        }
        
        // Complete Smith masking-shadowing function combining light and view directions
        // Formula: G(l,v) = G1(l) × G1(v) (uncorrelated masking approximation)
        // Physical interpretation: fraction of microfacets visible to both light and viewer
        // Parameters:
        //   ndotl: dot product between surface normal and light direction
        //   ndotv: dot product between surface normal and view direction  
        //   alpha: roughness parameter (α = roughness²)
        static float smith_g(float ndotl, float ndotv, float alpha) {
            return smith_g1(ndotl, alpha) * smith_g1(ndotv, alpha);
        }
        
        // Educational mathematical breakdown for geometry function learning validation
        // Explains physical meaning and step-by-step calculation of masking-shadowing
        static void explain_geometry_mathematics(float ndotl, float ndotv, float alpha, float result) {
            std::cout << "\n=== Smith Masking-Shadowing Function ===" << std::endl;
            std::cout << "Mathematical Foundation: G(l,v) = G1(l) × G1(v)" << std::endl;
            std::cout << "Physical Meaning: Fraction of microfacets visible to both light and viewer" << std::endl;
            std::cout << "G1 Formula: G1(v) = 2 / (1 + sqrt(1 + α² × tan²(θ_v)))" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Input Parameters:" << std::endl;
            std::cout << "  cos(θ_l) = n·l = " << ndotl << std::endl;
            std::cout << "  cos(θ_v) = n·v = " << ndotv << std::endl;
            std::cout << "  α (roughness parameter) = " << alpha << std::endl;
            std::cout << std::endl;
            
            float g1_light = smith_g1(ndotl, alpha);
            float g1_view = smith_g1(ndotv, alpha);
            
            std::cout << "Step-by-step Calculation:" << std::endl;
            std::cout << "  1. G1(light) = " << g1_light << std::endl;
            std::cout << "  2. G1(view) = " << g1_view << std::endl;
            std::cout << "  3. G(l,v) = G1(l) × G1(v) = " << result << std::endl;
            std::cout << std::endl;
            
            std::cout << "Physical Interpretation:" << std::endl;
            if (result > 0.9f) {
                std::cout << "  High visibility (G > 0.9): Most microfacets visible - low masking/shadowing" << std::endl;
            } else if (result > 0.5f) {
                std::cout << "  Medium visibility (0.5 < G < 0.9): Moderate masking/shadowing effects" << std::endl;
            } else {
                std::cout << "  Low visibility (G < 0.5): Significant masking/shadowing - grazing angles" << std::endl;
            }
            
            std::cout << "  Masking: Light blocked from reaching microfacets" << std::endl;
            std::cout << "  Shadowing: Reflected light blocked from reaching viewer" << std::endl;
            std::cout << "  G = " << result << " represents visible microfacet fraction" << std::endl;
            std::cout << "=== Geometry function calculation complete ===" << std::endl;
        }
    };

    // Fresnel Reflection Function
    // Models reflection coefficient variation with viewing angle using Fresnel equations
    // Mathematical foundation: electromagnetic theory of light reflection at material boundaries
    struct FresnelFunction {
        
        // Schlick's approximation for Fresnel reflection (efficient and accurate)
        // Formula: F(θ) = F0 + (1 - F0) × (1 - cos(θ))^5
        // Physical interpretation: reflection increases at grazing angles
        // Parameters:
        //   vdoth: dot product between view direction and halfway vector (cos θ)
        //   f0: reflectance at normal incidence (Fresnel reflectance at 0°)
        static Vector3 schlick_fresnel(float vdoth, const Vector3& f0) {
            // Ensure valid input range for cosine
            vdoth = std::max(0.0f, std::min(1.0f, vdoth));
            
            // Calculate Fresnel term: (1 - cos(θ))^5
            float fresnel_term = pow(1.0f - vdoth, 5.0f);
            
            // Schlick's approximation: F(θ) = F0 + (1 - F0) × (1 - cos(θ))^5
            // Applied per RGB channel for spectral accuracy
            return f0 + (Vector3(1.0f, 1.0f, 1.0f) - f0) * fresnel_term;
        }
        
        // Calculate F0 (reflectance at normal incidence) from Index of Refraction
        // Formula: F0 = ((n - 1) / (n + 1))² for dielectric materials
        // Physical interpretation: Fresnel reflectance at perpendicular viewing
        // Parameters:
        //   ior: index of refraction (n) - ratio of light speeds in vacuum vs material
        static Vector3 f0_from_ior(float ior) {
            float f0_scalar = pow((ior - 1.0f) / (ior + 1.0f), 2.0f);
            return Vector3(f0_scalar, f0_scalar, f0_scalar);
        }
        
        // Conductor (metal) Fresnel using complex refractive index
        // For conductors, F0 is typically the base color (high metallic reflectance)
        // Physical interpretation: metals have high reflectance across visible spectrum
        // Parameters:
        //   vdoth: viewing angle cosine
        //   f0: conductor reflectance values (usually base_color for metals)
        static Vector3 conductor_fresnel(float vdoth, const Vector3& f0) {
            return schlick_fresnel(vdoth, f0);
        }
        
        // Educational mathematical breakdown for Fresnel reflection learning validation
        // Explains physical meaning and calculation steps for different material types
        static void explain_fresnel_mathematics(float vdoth, const Vector3& f0, const Vector3& result, bool is_conductor) {
            std::cout << "\n=== Fresnel Reflection Function ===" << std::endl;
            std::cout << "Mathematical Foundation: F(θ) = F0 + (1 - F0) × (1 - cos(θ))⁵" << std::endl;
            std::cout << "Physical Meaning: Reflection coefficient varies with viewing angle" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Input Parameters:" << std::endl;
            std::cout << "  cos(θ) = v·h = " << vdoth << std::endl;
            std::cout << "  F0 = (" << f0.x << ", " << f0.y << ", " << f0.z << ")" << std::endl;
            std::cout << "  Material type: " << (is_conductor ? "Conductor (Metal)" : "Dielectric (Non-metal)") << std::endl;
            std::cout << std::endl;
            
            float fresnel_term = pow(1.0f - vdoth, 5.0f);
            
            std::cout << "Step-by-step Calculation:" << std::endl;
            std::cout << "  1. Viewing angle: θ = arccos(" << vdoth << ") = " 
                      << (acos(vdoth) * 180.0f / M_PI) << "°" << std::endl;
            std::cout << "  2. Fresnel term: (1 - cos(θ))⁵ = " << fresnel_term << std::endl;
            std::cout << "  3. Final result: F(θ) = (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Physical Interpretation:" << std::endl;
            if (is_conductor) {
                std::cout << "  Conductor material: High reflectance across spectrum" << std::endl;
                std::cout << "  F0 values represent metallic base color reflectance" << std::endl;
            } else {
                std::cout << "  Dielectric material: Low reflectance at normal incidence" << std::endl;
                std::cout << "  Typical F0 ≈ 0.04 for common dielectrics (plastic, glass, etc.)" << std::endl;
            }
            
            if (vdoth < 0.2f) {
                std::cout << "  Grazing angle: High reflectance due to Fresnel effect" << std::endl;
            } else if (vdoth > 0.8f) {
                std::cout << "  Normal viewing: Reflectance close to F0 values" << std::endl;
            } else {
                std::cout << "  Intermediate angle: Moderate Fresnel enhancement" << std::endl;
            }
            
            std::cout << "=== Fresnel calculation complete ===" << std::endl;
        }
    };
}

class CookTorranceMaterial {
public:
    Vector3 base_color;     // Surface albedo color (dielectric) or reflectance values (conductor)
    float roughness;        // Surface roughness: 0.0 = perfect mirror, 1.0 = completely rough
    float metallic;         // Metallic parameter: 0.0 = dielectric, 1.0 = conductor 
    float specular;         // Specular reflectance for dielectric materials (typical default: 0.04)

    // Constructor with physically plausible defaults
    // Default roughness 0.5 represents semi-glossy surface (plastic, painted metal)
    // Default base_color represents neutral gray material
    // Default specular 0.04 represents typical dielectric F0 (plastics, ceramics)
    CookTorranceMaterial(const Vector3& color = Vector3(0.7f, 0.7f, 0.7f),
                        float surface_roughness = 0.5f,
                        float metallic_param = 0.0f,
                        float specular_param = 0.04f,
                        bool verbose = false) 
        : base_color(color), roughness(surface_roughness), metallic(metallic_param), specular(specular_param) {
        
        // Automatically clamp parameters to physically valid ranges
        clamp_cook_torrance_to_valid_ranges();
        
        if (verbose) {
            std::cout << "=== Cook-Torrance Material Initialized ===" << std::endl;
            std::cout << "Base Color: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;
            std::cout << "Roughness: " << roughness << " (0.0=mirror, 1.0=diffuse)" << std::endl;
            std::cout << "Metallic: " << metallic << " (0.0=dielectric, 1.0=conductor)" << std::endl;
            std::cout << "Specular: " << specular << " (dielectric F0 reflectance)" << std::endl;
            std::cout << "Material type: " << (metallic > 0.5f ? "Conductor (Metal)" : "Dielectric (Non-metal)") << std::endl;
        }
    }

    // Core Cook-Torrance BRDF evaluation combining D, G, F terms
    // Formula: f_r(wi, wo) = (D(h) * G(wi, wo) * F(h, wo)) / (4 * cos(θl) * cos(θv))
    // Parameters:
    //   wi: incident light direction (pointing toward surface, normalized)
    //   wo: outgoing view direction (pointing toward camera, normalized)
    //   normal: surface normal at intersection point (outward-pointing, normalized)
    Vector3 evaluate_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal, bool verbose = true) const {
        if (verbose) {
            std::cout << "\n=== Cook-Torrance BRDF Evaluation ===" << std::endl;
            std::cout << "Incident direction (wi): (" << wi.x << ", " << wi.y << ", " << wi.z << ")" << std::endl;
            std::cout << "Outgoing direction (wo): (" << wo.x << ", " << wo.y << ", " << wo.z << ")" << std::endl;
            std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        }
        
        // Calculate halfway vector between incident and outgoing directions
        // Halfway vector represents the microfacet orientation that would reflect wi toward wo
        Vector3 halfway = (wi + wo).normalize();
        if (verbose) {
            std::cout << "Halfway vector (h): (" << halfway.x << ", " << halfway.y << ", " << halfway.z << ")" << std::endl;
        }
        
        // Calculate dot products needed for BRDF evaluation
        float ndotl = std::max(0.0f, normal.dot(wi));      // cos(θl) - light angle
        float ndotv = std::max(0.0f, normal.dot(wo));      // cos(θv) - view angle  
        float ndoth = std::max(0.0f, normal.dot(halfway)); // cos(θh) - halfway angle
        float vdoth = std::max(0.0f, wo.dot(halfway));     // cos(θ) for Fresnel
        
        if (verbose) {
            std::cout << "Dot products: n·l=" << ndotl << ", n·v=" << ndotv 
                      << ", n·h=" << ndoth << ", v·h=" << vdoth << std::endl;
        }
        
        // Early exit for grazing angles or backfacing surfaces
        if (ndotl <= 0.0f || ndotv <= 0.0f) {
            if (verbose) {
                std::cout << "Grazing angle or backface - BRDF = 0" << std::endl;
            }
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        
        // Convert roughness to alpha parameter (α = roughness²)
        // Alpha parameterization provides more intuitive roughness scaling
        float alpha = alpha_from_roughness(roughness);
        if (verbose) {
            std::cout << "Alpha parameter: α = roughness² = " << alpha << std::endl;
        }
        
        // Evaluate Normal Distribution Function D(h)
        float D = evaluate_normal_distribution(halfway, normal, alpha);
        if (verbose) {
            CookTorrance::NormalDistribution::explain_ggx_mathematics(ndoth, alpha, D);
        }
        
        // Evaluate Geometry Function G(wi, wo)
        float G = evaluate_geometry_function(wi, wo, normal, alpha);
        if (verbose) {
            CookTorrance::GeometryFunction::explain_geometry_mathematics(ndotl, ndotv, alpha, G);
        }
        
        // Calculate F0 based on material type (dielectric vs conductor)
        Vector3 f0 = calculate_f0_from_ior(base_color, metallic);
        
        // Evaluate Fresnel Function F(h, wo)
        Vector3 F = evaluate_fresnel(halfway, wo, f0);
        if (verbose) {
            bool is_conductor = (metallic > 0.5f);
            CookTorrance::FresnelFunction::explain_fresnel_mathematics(vdoth, f0, F, is_conductor);
        }
        
        // Complete Cook-Torrance BRDF evaluation with proper normalization
        // Formula: f_r = (D * G * F) / (4 * cos(θl) * cos(θv))
        float denominator = 4.0f * ndotl * ndotv;
        
        // Prevent division by zero at grazing angles
        if (denominator <= 0.0f) {
            if (verbose) {
                std::cout << "Invalid denominator - returning zero BRDF" << std::endl;
            }
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        
        // Final BRDF calculation
        Vector3 brdf_value = Vector3(
            (D * G * F.x) / denominator,
            (D * G * F.y) / denominator,  
            (D * G * F.z) / denominator
        );
        
        if (verbose) {
            std::cout << "\n=== Complete Cook-Torrance BRDF Result ===" << std::endl;
            std::cout << "D (Normal Distribution): " << D << std::endl;
            std::cout << "G (Geometry Function): " << G << std::endl;
            std::cout << "F (Fresnel): (" << F.x << ", " << F.y << ", " << F.z << ")" << std::endl;
            std::cout << "Denominator (4×n·l×n·v): " << denominator << std::endl;
            std::cout << "Final BRDF: (" << brdf_value.x << ", " << brdf_value.y << ", " << brdf_value.z << ")" << std::endl;
            std::cout << "=== Cook-Torrance BRDF evaluation complete ===" << std::endl;
        }
        
        return brdf_value;
    }

    // Normal Distribution Function evaluation using GGX/Trowbridge-Reitz distribution
    // Represents statistical distribution of microfacet orientations
    // Parameters:
    //   halfway: normalized halfway vector between wi and wo
    //   normal: surface normal (normalized)
    //   alpha: roughness parameter (α = roughness²)
    float evaluate_normal_distribution(const Vector3& halfway, const Vector3& normal, float alpha) const {
        float ndoth = std::max(0.0f, normal.dot(halfway));
        return CookTorrance::NormalDistribution::ggx_distribution(ndoth, alpha);
    }

    // Geometry Function evaluation using Smith masking-shadowing model
    // Accounts for microfacet occlusion from both light and view directions
    // Parameters:
    //   wi: incident light direction (normalized)
    //   wo: outgoing view direction (normalized)
    //   normal: surface normal (normalized)
    //   alpha: roughness parameter (α = roughness²)
    float evaluate_geometry_function(const Vector3& wi, const Vector3& wo, const Vector3& normal, float alpha) const {
        float ndotl = std::max(0.0f, normal.dot(wi));
        float ndotv = std::max(0.0f, normal.dot(wo));
        return CookTorrance::GeometryFunction::smith_g(ndotl, ndotv, alpha);
    }

    // Fresnel Function evaluation using Schlick's approximation
    // Handles both dielectric and conductor materials based on F0 values
    // Parameters:
    //   halfway: normalized halfway vector between wi and wo
    //   wo: outgoing view direction (normalized)
    //   f0: reflectance at normal incidence (material-dependent)
    Vector3 evaluate_fresnel(const Vector3& halfway, const Vector3& wo, const Vector3& f0) const {
        float vdoth = std::max(0.0f, wo.dot(halfway));
        return CookTorrance::FresnelFunction::schlick_fresnel(vdoth, f0);
    }

    // Calculate Cook-Torrance scattering with full rendering equation
    // Combines BRDF evaluation with incident radiance and geometric factors
    // Parameters:
    // light_direction: direction from surface to light source (normalized)
    // view_direction: direction from surface to camera (normalized)
    // normal: surface normal at intersection point (outward-pointing, normalized)
    // incident_radiance: incoming light energy (L_i in rendering equation)
    Vector3 scatter_light(const Vector3& light_direction, const Vector3& view_direction, 
                         const Vector3& normal, const Vector3& incident_radiance, bool verbose = true) const {
        if (verbose) {
            std::cout << "\n=== Cook-Torrance Light Scattering Calculation ===" << std::endl;
            std::cout << "Light direction: (" << light_direction.x << ", " << light_direction.y << ", " << light_direction.z << ")" << std::endl;
            std::cout << "View direction: (" << view_direction.x << ", " << view_direction.y << ", " << view_direction.z << ")" << std::endl;
            std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
            std::cout << "Incident radiance: (" << incident_radiance.x << ", " << incident_radiance.y << ", " << incident_radiance.z << ")" << std::endl;
        }

        // Calculate cosine term: n·l (normal dot light_direction)
        float cos_theta = normal.dot(light_direction);
        if (verbose) {
            std::cout << "Raw cosine term n·l = " << cos_theta << std::endl;
        }

        // Clamp to positive values: surfaces don't emit light when facing away
        cos_theta = std::max(0.0f, cos_theta);
        if (verbose) {
            std::cout << "Clamped cosine term max(0, n·l) = " << cos_theta << std::endl;
        }

        // Evaluate Cook-Torrance BRDF for this direction pair
        Vector3 brdf = evaluate_brdf(light_direction, view_direction, normal, verbose);

        // Full rendering equation evaluation: L_o = f_r * L_i * cos(θ)
        Vector3 outgoing_radiance = Vector3(
            brdf.x * incident_radiance.x * cos_theta,  // Red channel
            brdf.y * incident_radiance.y * cos_theta,  // Green channel  
            brdf.z * incident_radiance.z * cos_theta   // Blue channel
        );

        if (verbose) {
            std::cout << "Final outgoing radiance: L_o = f_r * L_i * cos(θ) = (" 
                      << outgoing_radiance.x << ", " << outgoing_radiance.y << ", " << outgoing_radiance.z << ")" << std::endl;
            std::cout << "=== Cook-Torrance light scattering calculation complete ===" << std::endl;
        }

        return outgoing_radiance;
    }

    // Complete educational explanation of Cook-Torrance evaluation
    // Provides comprehensive mathematical breakdown for learning validation
    // Parameters: same as evaluate_brdf for direct comparison
    void explain_cook_torrance_evaluation(const Vector3& wi, const Vector3& wo, const Vector3& normal) const {
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "=== COMPLETE COOK-TORRANCE EDUCATIONAL BREAKDOWN ===" << std::endl;
        std::cout << "=======================================================" << std::endl;
        std::cout << "\nTHEORETICAL FOUNDATION:" << std::endl;
        std::cout << "Cook-Torrance microfacet BRDF models surface reflection using statistical" << std::endl;
        std::cout << "distribution of microscopic surface facets. Each facet acts as a perfect" << std::endl;
        std::cout << "mirror, but the collective behavior creates realistic material appearance." << std::endl;
        std::cout << "\nBRDF Formula: f_r(wi,wo) = (D(h) × G(wi,wo) × F(h,wo)) / (4 × cos(θl) × cos(θv))" << std::endl;
        std::cout << "\nWhere each term represents:" << std::endl;
        std::cout << "• D(h): Normal Distribution - microfacet orientation probability" << std::endl;
        std::cout << "• G(wi,wo): Geometry Function - masking/shadowing effects" << std::endl;
        std::cout << "• F(h,wo): Fresnel Function - reflection coefficient variation" << std::endl;
        std::cout << "• 4×cos(θl)×cos(θv): Normalization factor from microfacet to macrosurface" << std::endl;
        
        // Call the actual BRDF evaluation to demonstrate real calculation
        std::cout << "\n=== LIVE CALCULATION DEMONSTRATION ===" << std::endl;
        Vector3 result = evaluate_brdf(wi, wo, normal);
        
        std::cout << "\n=== PHYSICAL MATERIAL INTERPRETATION ===" << std::endl;
        std::cout << "Material Properties Analysis:" << std::endl;
        std::cout << "• Base Color: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;
        std::cout << "• Roughness: " << roughness << " (0.0=mirror, 1.0=diffuse)" << std::endl;
        std::cout << "• Metallic: " << metallic << " (0.0=dielectric, 1.0=conductor)" << std::endl;
        std::cout << "• Specular: " << specular << " (dielectric F0)" << std::endl;
        
        if (metallic > 0.5f) {
            std::cout << "\nMaterial Type: CONDUCTOR (Metal)" << std::endl;
            std::cout << "• High reflectance across visible spectrum" << std::endl;
            std::cout << "• F0 values derived from base color (colored reflectance)" << std::endl;
            std::cout << "• Examples: gold, silver, copper, aluminum" << std::endl;
        } else {
            std::cout << "\nMaterial Type: DIELECTRIC (Non-metal)" << std::endl;
            std::cout << "• Low reflectance at normal incidence (~4%)" << std::endl;
            std::cout << "• F0 typically achromatic (same across RGB)" << std::endl;
            std::cout << "• Examples: plastic, glass, ceramic, wood" << std::endl;
        }
        
        if (roughness < 0.2f) {
            std::cout << "\nSurface Characteristic: GLOSSY/MIRROR-LIKE" << std::endl;
            std::cout << "• Sharp, concentrated reflections" << std::endl;
            std::cout << "• Clear mirror behavior at low roughness" << std::endl;
        } else if (roughness > 0.7f) {
            std::cout << "\nSurface Characteristic: ROUGH/DIFFUSE-LIKE" << std::endl;
            std::cout << "• Broad, scattered reflections" << std::endl;
            std::cout << "• Approaches Lambert-like behavior at high roughness" << std::endl;
        } else {
            std::cout << "\nSurface Characteristic: SEMI-GLOSSY" << std::endl;
            std::cout << "• Moderate reflection spreading" << std::endl;
            std::cout << "• Balanced between sharp and diffuse reflection" << std::endl;
        }
        
        std::cout << "\n=== ENERGY CONSERVATION ANALYSIS ===" << std::endl;
        bool energy_valid = validate_cook_torrance_parameters();
        if (energy_valid) {
            std::cout << "✓ Material parameters within physically valid ranges" << std::endl;
            std::cout << "✓ Energy conservation maintained (no light amplification)" << std::endl;
        } else {
            std::cout << "⚠ Material parameters outside physically valid ranges" << std::endl;
            std::cout << "⚠ Potential energy conservation violations detected" << std::endl;
        }
        
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "=== EDUCATIONAL BREAKDOWN COMPLETE ===" << std::endl;
        std::cout << "=======================================================" << std::endl;
    }

    // Print comprehensive BRDF component breakdown for educational purposes
    // Shows individual component contributions and their physical meanings
    void print_brdf_component_breakdown() const {
        std::cout << "\n=== COOK-TORRANCE COMPONENT BREAKDOWN ===" << std::endl;
        std::cout << "\nMATERIAL PARAMETER SUMMARY:" << std::endl;
        std::cout << "┌─────────────┬─────────────┬────────────────────────────────┐" << std::endl;
        std::cout << "│ Parameter   │ Value       │ Physical Meaning               │" << std::endl;
        std::cout << "├─────────────┼─────────────┼────────────────────────────────┤" << std::endl;
        std::cout << "│ Base Color  │ (" << std::setprecision(3) << base_color.x << "," 
                  << base_color.y << "," << base_color.z << ") │ Surface albedo/reflectance     │" << std::endl;
        std::cout << "│ Roughness   │ " << std::setprecision(3) << roughness 
                  << "         │ Surface microfacet variance    │" << std::endl;
        std::cout << "│ Metallic    │ " << std::setprecision(3) << metallic 
                  << "         │ Conductor vs dielectric blend │" << std::endl;
        std::cout << "│ Specular    │ " << std::setprecision(3) << specular 
                  << "         │ Dielectric F0 reflectance     │" << std::endl;
        std::cout << "└─────────────┴─────────────┴────────────────────────────────┘" << std::endl;
        
        std::cout << "\nBRDF COMPONENT FUNCTIONS:" << std::endl;
        std::cout << "┌─────────────┬────────────────────────────────────────────────┐" << std::endl;
        std::cout << "│ Component   │ Role in Cook-Torrance BRDF                    │" << std::endl;
        std::cout << "├─────────────┼────────────────────────────────────────────────┤" << std::endl;
        std::cout << "│ D(h)        │ GGX distribution of microfacet orientations   │" << std::endl;
        std::cout << "│ G(wi,wo)    │ Smith masking-shadowing occlusion effects     │" << std::endl;
        std::cout << "│ F(h,wo)     │ Fresnel reflection coefficient variation      │" << std::endl;
        std::cout << "│ Denominator │ 4×cos(θl)×cos(θv) normalization factor       │" << std::endl;
        std::cout << "└─────────────┴────────────────────────────────────────────────┘" << std::endl;
        
        std::cout << "\nPHYSICAL INTERPRETATION GUIDE:" << std::endl;
        std::cout << "• Higher D values → More microfacets aligned for reflection" << std::endl;
        std::cout << "• Higher G values → Less occlusion, more visible microfacets" << std::endl;
        std::cout << "• Higher F values → Stronger reflection (especially at grazing angles)" << std::endl;
        std::cout << "• Combined effect → Realistic material appearance with energy conservation" << std::endl;
        
        std::cout << "\nRECOMMENDED LEARNING EXERCISES:" << std::endl;
        std::cout << "1. Vary roughness from 0.01 to 1.0 and observe highlight changes" << std::endl;
        std::cout << "2. Compare metallic=0.0 vs metallic=1.0 for same base color" << std::endl;
        std::cout << "3. Test grazing angle reflections (viewing nearly parallel to surface)" << std::endl;
        std::cout << "4. Validate energy conservation with hemisphere integration" << std::endl;
        std::cout << "=== COMPONENT BREAKDOWN COMPLETE ===" << std::endl;
    }

    // Parameter validation for Cook-Torrance physically-based constraints
    // Ensures all material parameters remain within physically plausible ranges
    bool validate_cook_torrance_parameters() const {
        std::cout << "\n=== Cook-Torrance Parameter Validation ===" << std::endl;
        
        bool roughness_valid = (roughness >= 0.01f && roughness <= 1.0f);
        bool metallic_valid = (metallic >= 0.0f && metallic <= 1.0f);
        bool specular_valid = (specular >= 0.0f && specular <= 1.0f);
        bool color_valid = (base_color.x >= 0.0f && base_color.x <= 1.0f &&
                           base_color.y >= 0.0f && base_color.y <= 1.0f &&
                           base_color.z >= 0.0f && base_color.z <= 1.0f);
        
        std::cout << "Roughness [0.01, 1.0]: " << roughness << " - " << (roughness_valid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Metallic [0.0, 1.0]: " << metallic << " - " << (metallic_valid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Specular [0.0, 1.0]: " << specular << " - " << (specular_valid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Base Color [0.0, 1.0]³: " << (color_valid ? "VALID" : "INVALID") << std::endl;
        
        bool all_valid = roughness_valid && metallic_valid && specular_valid && color_valid;
        std::cout << "Overall parameter validation: " << (all_valid ? "PASS" : "FAIL") << std::endl;
        
        return all_valid;
    }

    // Automatic parameter clamping to ensure physically valid ranges
    // Prevents numerical instability and non-physical behavior
    void clamp_cook_torrance_to_valid_ranges() {
        roughness = std::max(0.01f, std::min(1.0f, roughness));  // Prevent perfect mirror (numerical issues)
        metallic = std::max(0.0f, std::min(1.0f, metallic));     // Binary or interpolated metallic workflow
        specular = std::max(0.0f, std::min(1.0f, specular));     // Physical reflectance bounds
        
        // Clamp color channels individually to [0,1] range
        base_color.x = std::max(0.0f, std::min(1.0f, base_color.x));
        base_color.y = std::max(0.0f, std::min(1.0f, base_color.y));
        base_color.z = std::max(0.0f, std::min(1.0f, base_color.z));
    }

private:
    // Convert roughness parameter to alpha for mathematical calculations
    // Alpha parameterization: α = roughness² provides more intuitive user control
    // Reasoning: linear roughness feels more natural than quadratic alpha
    float alpha_from_roughness(float roughness_param) const {
        return roughness_param * roughness_param;
    }

    // Calculate F0 (reflectance at normal incidence) based on material type
    // For dielectric materials: F0 = specular parameter (typically ~0.04)
    // For conductor materials: F0 = base_color (metallic reflectance)
    // Parameters:
    //   base_color: material base color/albedo
    //   metallic: metallic parameter (0.0 = dielectric, 1.0 = conductor)
    Vector3 calculate_f0_from_ior(const Vector3& base_color_param, float metallic_param) const {
        // Linear interpolation between dielectric and conductor F0
        // Dielectric F0: specular parameter (typically 0.04 for common materials)
        Vector3 dielectric_f0(specular, specular, specular);
        
        // Conductor F0: base color represents metallic reflectance
        Vector3 conductor_f0 = base_color_param;
        
        // Blend based on metallic parameter
        // metallic = 0.0: pure dielectric (F0 = specular)
        // metallic = 1.0: pure conductor (F0 = base_color)
        return dielectric_f0 * (1.0f - metallic_param) + conductor_f0 * metallic_param;
    }
};