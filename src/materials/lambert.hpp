#pragma once
#include "../core/vector3.hpp"
#include <cmath>
#include <iostream>

// Lambert BRDF (Bidirectional Reflectance Distribution Function) material
// Mathematical foundation: perfectly diffuse reflection following Lambert's cosine law
// Physical principle: light scattered equally in all directions over hemisphere
// Energy conservation: total reflected energy never exceeds incident energy
class LambertMaterial {
public:
    Vector3 base_color;  // Albedo color - fraction of light reflected per wavelength (RGB channels)
                        // Mathematical constraint: each component must be ∈ [0,1] for energy conservation

    // Constructor with explicit albedo color
    // Albedo interpretation: percentage of incident light reflected at each wavelength
    // Example: (0.8, 0.8, 0.8) reflects 80% of incident light across all wavelengths
    LambertMaterial(const Vector3& color) : base_color(color) {}

    // Lambert BRDF evaluation: computes reflectance for given incident/outgoing directions
    // Mathematical formula: f_r(wi, wo) = ρ/π where ρ is albedo (base_color)
    // Physical interpretation: diffuse reflection independent of viewing angle
    // Energy conservation: integral over hemisphere equals ρ ≤ 1
    // 
    // Parameters:
    // wi: incident light direction (pointing toward surface, normalized)
    // wo: outgoing view direction (pointing toward camera, normalized) 
    // normal: surface normal at intersection point (outward-pointing, normalized)
    Vector3 evaluate_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const {
        std::cout << "\n=== Lambert BRDF Evaluation ===" << std::endl;
        std::cout << "Incident direction (wi): (" << wi.x << ", " << wi.y << ", " << wi.z << ")" << std::endl;
        std::cout << "Outgoing direction (wo): (" << wo.x << ", " << wo.y << ", " << wo.z << ")" << std::endl;
        std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        std::cout << "Material albedo: (" << base_color.x << ", " << base_color.y << ", " << base_color.z << ")" << std::endl;

        // Lambert BRDF is constant for all direction pairs (perfectly diffuse)
        // Formula: f_r = ρ/π where ρ = albedo, π ensures energy conservation
        // Mathematical derivation: ∫hemisphere f_r * cos(θ) * dω = ρ when f_r = ρ/π
        Vector3 brdf_value = base_color * (1.0f / M_PI);
        std::cout << "Lambert BRDF value: f_r = ρ/π = (" << brdf_value.x << ", " << brdf_value.y << ", " << brdf_value.z << ")" << std::endl;
        
        // Verify energy conservation constraint: each component ≤ 1/π
        std::cout << "Energy conservation check: ρ/π ≤ 1/π requires ρ ≤ 1" << std::endl;
        std::cout << "Albedo constraint satisfied: " << 
                     (base_color.x <= 1.0f && base_color.y <= 1.0f && base_color.z <= 1.0f ? "YES" : "NO") << std::endl;

        std::cout << "=== BRDF evaluation complete ===" << std::endl;
        return brdf_value;
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
                         const Vector3& normal, const Vector3& incident_radiance) const {
        std::cout << "\n=== Lambert Light Scattering Calculation ===" << std::endl;
        std::cout << "Light direction: (" << light_direction.x << ", " << light_direction.y << ", " << light_direction.z << ")" << std::endl;
        std::cout << "View direction: (" << view_direction.x << ", " << view_direction.y << ", " << view_direction.z << ")" << std::endl;
        std::cout << "Surface normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        std::cout << "Incident radiance: (" << incident_radiance.x << ", " << incident_radiance.y << ", " << incident_radiance.z << ")" << std::endl;

        // Calculate cosine term: n·l (normal dot light_direction)
        // Geometric interpretation: how much surface faces toward light source
        // Physics: Lambert's cosine law - apparent brightness ∝ cos(angle)
        float cos_theta = normal.dot(light_direction);
        std::cout << "Raw cosine term n·l = " << cos_theta << std::endl;

        // Clamp to positive values: surfaces don't emit light when facing away
        // Physical constraint: no negative light contribution from backlit surfaces
        cos_theta = std::max(0.0f, cos_theta);
        std::cout << "Clamped cosine term max(0, n·l) = " << cos_theta << std::endl;

        // Evaluate BRDF for this direction pair
        Vector3 brdf = evaluate_brdf(light_direction, view_direction, normal);

        // Full rendering equation evaluation: L_o = f_r * L_i * cos(θ)
        // Each component computed separately for RGB channels
        Vector3 outgoing_radiance = Vector3(
            brdf.x * incident_radiance.x * cos_theta,  // Red channel
            brdf.y * incident_radiance.y * cos_theta,  // Green channel  
            brdf.z * incident_radiance.z * cos_theta   // Blue channel
        );

        std::cout << "Final outgoing radiance: L_o = f_r * L_i * cos(θ) = (" 
                  << outgoing_radiance.x << ", " << outgoing_radiance.y << ", " << outgoing_radiance.z << ")" << std::endl;
        std::cout << "=== Light scattering calculation complete ===" << std::endl;

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

    // Calculate hemispherical reflectance (total fraction of light reflected)
    // For Lambert BRDF: R = ρ (albedo value)
    // Physical interpretation: what fraction of incident light is scattered back
    Vector3 hemispherical_reflectance() const {
        return base_color;  // For Lambert BRDF, hemispherical reflectance equals albedo
    }
};