#pragma once
#include "../core/vector3.hpp"
#include "material_base.hpp"
#include <cmath>
#include <iostream>

// Lambert BRDF (Bidirectional Reflectance Distribution Function) material
// Mathematical foundation: perfectly diffuse reflection following Lambert's cosine law
// Physical principle: light scattered equally in all directions over hemisphere
// Energy conservation: total reflected energy never exceeds incident energy
//
// RENDERING EQUATION THEORY:
//
// The rendering equation (Kajiya, 1986):
//   L_o(p, ω_o) = L_e(p, ω_o) + ∫_Ω f_r(p, ω_i, ω_o) L_i(p, ω_i) (n · ω_i) dω_i
//
// Where:
//   L_o = outgoing radiance
//   L_e = emitted radiance (0 for non-emissive surfaces)
//   f_r = BRDF (bidirectional reflectance distribution function)
//   L_i = incoming radiance
//   n = surface normal
//   ω_i = incident direction
//   ω_o = outgoing direction
//   Ω = hemisphere of directions
//
// LAMBERT BRDF DERIVATION:
//
// For perfectly diffuse surfaces (Lambertian):
//   - Reflected radiance independent of viewing angle
//   - Equal probability of scattering in all hemisphere directions
//   - BRDF must be constant: f_r(ω_i, ω_o) = ρ/π
//
// The π factor comes from energy conservation:
//   ∫_hemisphere f_r cos(θ) dω = ρ
//   For constant f_r: f_r ∫_hemisphere cos(θ) dω = ρ
//   Since ∫_hemisphere cos(θ) dω = π, we get f_r = ρ/π
//
// ASCII Art Diagram:
//
//   Lambert Reflection (Equal brightness from all viewing angles):
//
//         viewer 1    viewer 2    viewer 3
//             ↖        ↑          ↗
//              \\       |         /
//               \\      |        /
//                \\     |       /
//           ================== surface
//                     ↑
//               incident light
//
//   All viewers see same brightness despite different angles!
//
// PHYSICAL INTERPRETATION:
//   - Models matte surfaces like chalk, unpolished wood, paper
//   - Microscopic surface roughness causes isotropic scattering
//   - Quantum mechanics: photons scatter uniformly in all directions
//
// References:
//   - Lambert, J.H. "Photometria" (1760) - original cosine law
//   - Kajiya, J.T. "The Rendering Equation" SIGGRAPH 1986
//   - Nicodemus, F.E. "Reflectance Nomenclature" NBS 1977
//   - Pharr, Jakob, Humphreys "Physically Based Rendering" 4th ed.
class LambertMaterial : public Material {
public:
    // Constructor with explicit albedo color
    // Albedo interpretation: percentage of incident light reflected at each wavelength
    // Example: (0.8, 0.8, 0.8) reflects 80% of incident light across all wavelengths
    // Mathematical constraint: each component must be ∈ [0,1] for energy conservation
    LambertMaterial(const Vector3& color = Vector3(0.7f, 0.7f, 0.7f)) 
        : Material(color, MaterialType::Lambert) {
        // Automatically clamp parameters to physically valid ranges
        clamp_to_valid_ranges();
    }

    // Lambert BRDF evaluation: computes reflectance for given incident/outgoing directions
    // Mathematical formula: f_r(wi, wo) = ρ/π where ρ is albedo (base_color)
    // Physical interpretation: diffuse reflection independent of viewing angle
    // Energy conservation: integral over hemisphere equals ρ ≤ 1
    // 
    // Parameters:
    // wi: incident light direction (pointing toward surface, normalized)
    // wo: outgoing view direction (pointing toward camera, normalized) 
    // normal: surface normal at intersection point (outward-pointing, normalized)
    Vector3 evaluate_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal, bool verbose = true) const override {
        if (verbose) {
            std::cout << "\n=== Lambert BRDF Evaluation ===" << std::endl;
            std::cout << "Incident direction (wi): (" << wi.x << ", " << wi.y << ", " << wi.z << ")" << std::endl;
            std::cout << "Outgoing direction (wo): (" << wo.x << ", " << wo.y << ", " << wo.z << ")" << std::endl;
            std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
            std::cout << "Material albedo: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;
        }

        // Lambert BRDF is constant for all direction pairs (perfectly diffuse)
        // Formula: f_r = ρ/π where ρ = albedo, π ensures energy conservation
        // Mathematical derivation: ∫hemisphere f_r * cos(θ) * dω = ρ when f_r = ρ/π
        Vector3 brdf_value = base_color * (1.0f / M_PI);
        if (verbose) {
            std::cout << "Lambert BRDF value: f_r = ρ/π = (" << brdf_value.x << ", " << brdf_value.y << ", " << brdf_value.z << ")" << std::endl;
            
            // Verify energy conservation constraint: each component ≤ 1/π
            std::cout << "Energy conservation check: ρ/π ≤ 1/π requires ρ ≤ 1" << std::endl;
            std::cout << "Albedo constraint satisfied: " << 
                         (base_color.x <= 1.0f && base_color.y <= 1.0f && base_color.z <= 1.0f ? "YES" : "NO") << std::endl;

            std::cout << "=== BRDF evaluation complete ===" << std::endl;
        }
        return brdf_value;
    }

    // Lambert-specific parameter validation implementation
    // Validates that albedo values are within physically valid [0,1] range for energy conservation
    bool validate_parameters() const override {
        bool red_valid = (base_color.x >= 0.0f && base_color.x <= 1.0f);
        bool green_valid = (base_color.y >= 0.0f && base_color.y <= 1.0f);
        bool blue_valid = (base_color.z >= 0.0f && base_color.z <= 1.0f);
        
        return red_valid && green_valid && blue_valid;
    }
    
    // Lambert-specific parameter clamping implementation
    // Automatically clamps albedo values to [0,1] range to ensure energy conservation
    void clamp_to_valid_ranges() override {
        base_color.x = std::max(0.0f, std::min(1.0f, base_color.x));
        base_color.y = std::max(0.0f, std::min(1.0f, base_color.y));
        base_color.z = std::max(0.0f, std::min(1.0f, base_color.z));
    }

    // Calculate Lambert scattering with n·l term for light transport equation
    // Full rendering equation component: L_o = f_r * L_i * cos(θ_i) * dω
    // cos(θ_i) = max(0, normal · light_direction) ensures no negative contribution
    // 
    // Parameters:
    // light_direction: direction from surface to light source (normalized)
    // view_direction: direction from surface to camera (normalized)
    // normal: surface normal at intersection point (outward-pointing, normalized)
    // incident_radiance: incoming light energy (L_i in rendering equation)
    Vector3 scatter_light(const Vector3& light_direction, const Vector3& view_direction, 
                         const Vector3& normal, const Vector3& incident_radiance, bool verbose = true) const override {
        if (verbose) {
            std::cout << "\n=== Lambert Light Scattering Calculation ===" << std::endl;
            std::cout << "Light direction: (" << light_direction.x << ", " << light_direction.y << ", " << light_direction.z << ")" << std::endl;
            std::cout << "View direction: (" << view_direction.x << ", " << view_direction.y << ", " << view_direction.z << ")" << std::endl;
            std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
            std::cout << "Incident radiance: (" << incident_radiance.x << ", " << incident_radiance.y << ", " << incident_radiance.z << ")" << std::endl;
        }

        // Calculate cosine term: n·l (normal dot light_direction)
        // Geometric interpretation: how much surface faces toward light source
        // Physics: Lambert's cosine law - apparent brightness ∝ cos(angle)
        float cos_theta = normal.dot(light_direction);
        if (verbose) {
            std::cout << "Raw cosine term n·l = " << cos_theta << std::endl;
        }

        // Clamp to positive values: surfaces don't emit light when facing away
        // Physical constraint: no negative light contribution from backlit surfaces
        cos_theta = std::max(0.0f, cos_theta);
        if (verbose) {
            std::cout << "Clamped cosine term max(0, n·l) = " << cos_theta << std::endl;
        }

        // Evaluate BRDF for this direction pair
        Vector3 brdf = evaluate_brdf(light_direction, view_direction, normal, verbose);

        // Full rendering equation evaluation: L_o = f_r * L_i * cos(θ)
        // Each component computed separately for RGB channels
        Vector3 outgoing_radiance = Vector3(
            brdf.x * incident_radiance.x * cos_theta,  // Red channel
            brdf.y * incident_radiance.y * cos_theta,  // Green channel  
            brdf.z * incident_radiance.z * cos_theta   // Blue channel
        );

        if (verbose) {
            std::cout << "Final outgoing radiance: L_o = f_r * L_i * cos(θ) = (" 
                      << outgoing_radiance.x << ", " << outgoing_radiance.y << ", " << outgoing_radiance.z << ")" << std::endl;
            std::cout << "=== Light scattering calculation complete ===" << std::endl;
        }

        return outgoing_radiance;
    }

    // Validate energy conservation for Lambert BRDF
    // Mathematical requirement: ∫hemisphere f_r * cos(θ) * dω ≤ 1.0 for each color channel
    // For Lambert BRDF: integral evaluates to ρ, so we need ρ ≤ 1.0
    bool validate_energy_conservation() const {
        std::cout << "\n=== Energy Conservation Validation ===" << std::endl;
        std::cout << "Testing albedo constraint: ρ ≤ 1.0 for energy conservation" << std::endl;
        std::cout << "Current albedo: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;

        // Check each color channel independently
        bool red_valid = (base_color.x >= 0.0f && base_color.x <= 1.0f);
        bool green_valid = (base_color.y >= 0.0f && base_color.y <= 1.0f);
        bool blue_valid = (base_color.z >= 0.0f && base_color.z <= 1.0f);

        std::cout << "Red channel [0,1]: " << (red_valid ? "PASS" : "FAIL") << std::endl;
        std::cout << "Green channel [0,1]: " << (green_valid ? "PASS" : "FAIL") << std::endl;
        std::cout << "Blue channel [0,1]: " << (blue_valid ? "PASS" : "FAIL") << std::endl;

        bool is_energy_conserving = red_valid && green_valid && blue_valid;
        std::cout << "Overall energy conservation: " << (is_energy_conserving ? "PASS" : "FAIL") << std::endl;

        if (is_energy_conserving) {
            // Calculate maximum possible reflectance for verification
            float max_reflectance = std::max(base_color.x, std::max(base_color.y, base_color.z));
            std::cout << "Maximum reflectance: " << max_reflectance << " (should be ≤ 1.0)" << std::endl;
            std::cout << "Energy conservation verified: material cannot reflect more energy than received" << std::endl;
        } else {
            std::cout << "WARNING: Energy conservation violated - material would amplify light!" << std::endl;
        }

        std::cout << "=== Energy conservation validation complete ===" << std::endl;
        return is_energy_conserving;
    }

    // Lambert-specific educational BRDF explanation override
    // Provides detailed mathematical breakdown of Lambert diffuse reflection theory
    void explain_brdf_evaluation(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "=== LAMBERT BRDF EDUCATIONAL BREAKDOWN ===" << std::endl;
        std::cout << "=======================================================" << std::endl;
        std::cout << "\nTHEORETICAL FOUNDATION:" << std::endl;
        std::cout << "Lambert BRDF models perfectly diffuse reflection following Lambert's cosine law." << std::endl;
        std::cout << "Physical principle: light scattered equally in all directions over hemisphere." << std::endl;
        std::cout << "Mathematical foundation: BRDF is constant for all direction pairs." << std::endl;
        std::cout << "\nBRDF Formula: f_r(wi,wo) = ρ/π" << std::endl;
        std::cout << "Where:" << std::endl;
        std::cout << "• ρ (albedo): fraction of light reflected per wavelength" << std::endl;
        std::cout << "• π: normalization factor ensuring energy conservation" << std::endl;
        std::cout << "\nPhysical Interpretation:" << std::endl;
        std::cout << "• Models matte surfaces: chalk, unpolished wood, paper, fabric" << std::endl;
        std::cout << "• Microscopic surface roughness causes isotropic scattering" << std::endl;
        std::cout << "• Viewing angle independent brightness (unlike metals/glossy surfaces)" << std::endl;
        
        // Demonstrate actual calculation
        Vector3 result = evaluate_brdf(wi, wo, normal, false);
        std::cout << "\n=== LIVE CALCULATION DEMONSTRATION ===" << std::endl;
        std::cout << "Current albedo ρ: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;
        std::cout << "Lambert BRDF value: ρ/π = (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
        
        std::cout << "\n=== ENERGY CONSERVATION ANALYSIS ===" << std::endl;
        bool energy_valid = validate_parameters();
        if (energy_valid) {
            std::cout << "✓ Albedo values within [0,1] range - energy conservation maintained" << std::endl;
            std::cout << "✓ Material cannot reflect more energy than received (physically plausible)" << std::endl;
        } else {
            std::cout << "⚠ Albedo values outside [0,1] range - potential energy conservation violation" << std::endl;
            std::cout << "⚠ Material might amplify light (non-physical behavior)" << std::endl;
        }
        
        std::cout << "\n=== LAMBERT vs OTHER MATERIALS ===" << std::endl;
        std::cout << "Comparison with other BRDF models:" << std::endl;
        std::cout << "• Lambert: Constant f_r, viewing angle independent" << std::endl;
        std::cout << "• Cook-Torrance: Variable f_r with viewing angle, specular highlights" << std::endl;
        std::cout << "• Phong: Empirical model, not physically based" << std::endl;
        std::cout << "\nLambert limitations:" << std::endl;
        std::cout << "• No specular highlights or reflections" << std::endl;
        std::cout << "• Cannot model glossy or metallic surfaces" << std::endl;
        std::cout << "• Pure diffuse assumption rarely matches real materials exactly" << std::endl;
        
        std::cout << "\n=======================================================" << std::endl;
        std::cout << "=== LAMBERT EDUCATIONAL BREAKDOWN COMPLETE ===" << std::endl;
        std::cout << "=======================================================" << std::endl;
    }

    // Calculate hemispherical reflectance (total fraction of light reflected)
    // For Lambert BRDF: R = ρ (albedo value)
    // Physical interpretation: what fraction of incident light is scattered back
    Vector3 hemispherical_reflectance() const {
        return base_color;  // For Lambert BRDF, hemispherical reflectance equals albedo
    }
};