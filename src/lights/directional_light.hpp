#pragma once

#include "light_base.hpp"
#include "../core/vector3.hpp"
#include "../core/ray.hpp"
#include <iostream>
#include <cmath>
#include <limits>

// Forward declaration for Scene to avoid circular dependency
class Scene;

class DirectionalLight : public Light {
public:
    Vector3 direction;  // Direction the light is pointing (normalized)
    
    DirectionalLight(const Vector3& light_direction, const Vector3& light_color, float light_intensity)
        : Light(light_color, light_intensity, LightType::Directional) {
        // Normalize the direction vector for consistent behavior
        float length = light_direction.length();
        if (length > 1e-6f) {
            direction = light_direction * (1.0f / length);
        } else {
            direction = Vector3(0, -1, 0);  // Default downward direction
        }
    }
    
    // Core light evaluation interface implementation
    Vector3 illuminate(const Vector3& point, Vector3& light_direction, float& distance) const override {
        // For directional lights, direction is constant everywhere
        light_direction = direction * -1.0f;  // Light rays travel opposite to light direction
        
        // Distance is infinite for directional lights (sun-like illumination)
        distance = std::numeric_limits<float>::max();
        
        // Directional lights have constant intensity (no falloff with distance)
        return color * intensity;
    }
    
    bool is_occluded(const Vector3& point, const Vector3& light_direction, float distance, const Scene& scene) const override {
        // Create shadow ray with small epsilon offset to avoid self-intersection
        const float epsilon = 0.001f;
        Vector3 offset_point = point + light_direction * epsilon;
        Ray shadow_ray(Point3(offset_point.x, offset_point.y, offset_point.z), light_direction);
        
        // Test intersection with scene - for directional lights, any intersection blocks the light
        Scene::Intersection hit = scene.intersect(shadow_ray, false);  // Disable verbose output
        
        // Any intersection along the ray blocks the directional light
        return hit.hit && hit.t > epsilon;
    }
    
    Vector3 sample_direction(const Vector3& point, float& pdf) const override {
        // For directional lights, sampling is deterministic (single direction)
        pdf = 1.0f;
        return direction * -1.0f;  // Light rays travel opposite to light direction
    }
    
    // Educational debugging methods
    void explain_light_calculation(const Vector3& point) const override {
        Light::explain_light_calculation(point);  // Call base class method
        
        std::cout << "=== Directional Light Specific Calculation ===" << std::endl;
        std::cout << "Light Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
        std::cout << "Light Ray Direction: (" << (-direction.x) << ", " << (-direction.y) << ", " << (-direction.z) << ")" << std::endl;
        std::cout << "Distance to Light: INFINITE (directional light)" << std::endl;
        std::cout << "Attenuation: NONE (constant intensity)" << std::endl;
        
        Vector3 contribution = color * intensity;
        std::cout << "Light Contribution: (" << contribution.x << ", " << contribution.y << ", " << contribution.z << ")" << std::endl;
        std::cout << "Physical Model: Distant light source (like the Sun)" << std::endl;
        std::cout << "Key Property: Parallel rays with constant intensity everywhere" << std::endl;
        std::cout << "Usage: Outdoor lighting, sun/sky simulation, fill lighting" << std::endl;
        std::cout << "====================================" << std::endl;
    }
    
    std::string get_light_info() const override {
        return "Directional Light pointing in direction (" + 
               std::to_string(direction.x) + ", " + 
               std::to_string(direction.y) + ", " + 
               std::to_string(direction.z) + 
               ") with intensity " + std::to_string(intensity);
    }
    
    // Additional validation for directional light specific parameters
    bool validate_parameters() const override {
        if (!Light::validate_parameters()) {
            return false;
        }
        
        // Check direction is finite
        if (!std::isfinite(direction.x) || !std::isfinite(direction.y) || !std::isfinite(direction.z)) {
            return false;
        }
        
        // Check direction is not zero vector
        if (direction.length_squared() < 1e-12f) {
            return false;
        }
        
        return true;
    }
    
    // Clamp directional light parameters
    void clamp_parameters() override {
        Light::clamp_parameters();
        
        // Ensure direction is normalized
        float length = direction.length();
        if (length > 1e-6f) {
            direction = direction * (1.0f / length);
        } else {
            // Set to default downward direction if invalid
            direction = Vector3(0, -1, 0);
        }
    }
    
    // Get the direction from which light arrives (opposite to light direction)
    Vector3 get_light_ray_direction() const {
        return direction * -1.0f;
    }
};