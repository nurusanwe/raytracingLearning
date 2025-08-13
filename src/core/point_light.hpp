#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include <cmath>
#include <iostream>

// Point light source - emits light uniformly in all directions from a single point
// Physical model: inverse square law for intensity falloff with distance
// Mathematical foundation: radiant intensity distributed over solid angles
struct PointLight {
    Point3 position;    // 3D position of light source in world space
    Vector3 color;      // Light color (RGB intensities, can exceed 1.0 for bright lights)
    float intensity;    // Overall light intensity multiplier (1.0 = standard intensity)

    // Constructor with explicit position, color, and intensity
    // Intensity interpretation: multiplier for color values (1.0 = standard, 2.0 = twice as bright)
    // Color interpretation: RGB values representing spectral power distribution
    PointLight(const Point3& pos, const Vector3& col, float intensity = 1.0f)
        : position(pos), color(col), intensity(intensity) {}

    // Calculate direction vector from surface point to light source
    // Geometric interpretation: normalized vector pointing from surface toward light
    // Usage: needed for BRDF evaluation and n·l calculations
    Vector3 sample_direction(const Point3& surface_point) const {
        std::cout << "\n=== Point Light Direction Calculation ===" << std::endl;
        std::cout << "Light position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Surface point: (" << surface_point.x << ", " << surface_point.y << ", " << surface_point.z << ")" << std::endl;
        
        // Calculate displacement vector from surface to light
        Vector3 displacement = position - surface_point;
        std::cout << "Displacement vector: (" << displacement.x << ", " << displacement.y << ", " << displacement.z << ")" << std::endl;
        
        // Calculate distance for verification
        float distance = displacement.length();
        std::cout << "Distance to light: " << distance << std::endl;
        
        // Normalize to get unit direction vector
        Vector3 direction = displacement.normalize();
        std::cout << "Normalized direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
        std::cout << "Direction length verification: " << direction.length() << " (should be ≈ 1.0)" << std::endl;
        std::cout << "=== Direction calculation complete ===" << std::endl;
        
        return direction;
    }

    // Calculate irradiance (incident light power per unit area) at surface point
    // Physical law: inverse square law - irradiance ∝ 1/distance²
    // Mathematical formula: E = (I * color) / (4π * distance²)
    // Result: incident radiance for use in rendering equation
    Vector3 calculate_irradiance(const Point3& surface_point) const {
        std::cout << "\n=== Point Light Irradiance Calculation ===" << std::endl;
        std::cout << "Light intensity: " << intensity << std::endl;
        std::cout << "Light color: (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
        
        // Calculate distance from light to surface point
        Vector3 displacement = position - surface_point;
        float distance = displacement.length();
        std::cout << "Distance to surface: " << distance << std::endl;
        
        // Handle degenerate case: light at surface point
        if (distance < 1e-6f) {
            std::cout << "WARNING: Light and surface point are coincident - returning zero irradiance" << std::endl;
            return Vector3(0, 0, 0);
        }
        
        // Apply inverse square law: irradiance ∝ 1/distance²
        // Factor of 4π comes from solid angle of complete sphere
        float distance_squared = distance * distance;
        float falloff_factor = 1.0f / (4.0f * M_PI * distance_squared);
        std::cout << "Distance squared: " << distance_squared << std::endl;
        std::cout << "Falloff factor (1/4πd²): " << falloff_factor << std::endl;
        
        // Calculate final irradiance: intensity * color * falloff
        Vector3 irradiance = color * (intensity * falloff_factor);
        std::cout << "Final irradiance: (" << irradiance.x << ", " << irradiance.y << ", " << irradiance.z << ")" << std::endl;
        
        // Verify irradiance is non-negative
        if (irradiance.x < 0 || irradiance.y < 0 || irradiance.z < 0) {
            std::cout << "WARNING: Negative irradiance detected - this violates physical laws" << std::endl;
        }
        
        std::cout << "=== Irradiance calculation complete ===" << std::endl;
        return irradiance;
    }
    
    // Validate light source configuration
    // Checks: finite position, non-negative color/intensity
    bool validate_light() const {
        // Check position validity
        if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)) {
            std::cout << "Invalid light: position coordinates must be finite" << std::endl;
            return false;
        }
        
        // Check color validity  
        if (!std::isfinite(color.x) || !std::isfinite(color.y) || !std::isfinite(color.z)) {
            std::cout << "Invalid light: color values must be finite" << std::endl;
            return false;
        }
        
        if (color.x < 0 || color.y < 0 || color.z < 0) {
            std::cout << "Invalid light: color values must be non-negative" << std::endl;
            return false;
        }
        
        // Check intensity validity
        if (!std::isfinite(intensity) || intensity < 0) {
            std::cout << "Invalid light: intensity must be finite and non-negative" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Calculate luminous power (total light emission)
    // Formula: Φ = intensity * |color| * 4π (integrated over all directions)
    float luminous_power() const {
        float color_magnitude = std::sqrt(color.x * color.x + color.y * color.y + color.z * color.z);
        return intensity * color_magnitude * 4.0f * M_PI;
    }
};