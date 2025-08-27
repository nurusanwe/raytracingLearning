#pragma once

#include "light_base.hpp"
#include "../core/point3.hpp"
#include "../core/vector3.hpp"
#include "../core/ray.hpp"
#include <cmath>
#include <iostream>

// Forward declaration for Scene to avoid circular dependency
class Scene;

class PointLight : public Light {
public:
    Vector3 position;  // Using Vector3 for position instead of Point3 for consistency
    
    PointLight(const Vector3& pos, const Vector3& light_color, float light_intensity)
        : Light(light_color, light_intensity, LightType::Point), position(pos) {}
    
    // Core light evaluation interface implementation
    Vector3 illuminate(const Vector3& point, Vector3& light_direction, float& distance) const override {
        // Calculate displacement vector from surface to light
        Vector3 light_vector = position - point;
        distance = light_vector.length();
        
        // Handle degenerate case: light at surface point
        if (distance < 1e-6f) {
            light_direction = Vector3(0, 0, 1);  // Default direction
            return Vector3(0, 0, 0);  // No contribution
        }
        
        // Normalize to get light direction
        light_direction = light_vector * (1.0f / distance);  // Efficient normalization
        
        // Apply inverse square law falloff
        float attenuation = 1.0f / (distance * distance);
        
        // Return color * intensity * attenuation
        return color * intensity * attenuation;
    }
    
    bool is_occluded(const Vector3& point, const Vector3& light_direction, float distance, const Scene& scene) const override {
        // Create shadow ray with small epsilon offset to avoid self-intersection
        const float epsilon = 0.001f;
        Vector3 offset_point = point + light_direction * epsilon;
        Ray shadow_ray(Point3(offset_point.x, offset_point.y, offset_point.z), light_direction);
        
        // Test intersection with scene
        Scene::Intersection hit = scene.intersect(shadow_ray, false);  // Disable verbose output for shadow rays
        
        // Check if intersection occurs before reaching the light
        return hit.hit && hit.t < (distance - epsilon);
    }
    
    Vector3 sample_direction(const Vector3& point, float& pdf) const override {
        Vector3 light_vector = position - point;
        float distance = light_vector.length();
        
        if (distance < 1e-6f) {
            pdf = 0.0f;
            return Vector3(0, 0, 1);
        }
        
        // For point lights, sampling is deterministic (single direction)
        pdf = 1.0f;
        return light_vector * (1.0f / distance);
    }
    
    // Educational debugging methods
    void explain_light_calculation(const Vector3& point) const override {
        Light::explain_light_calculation(point);  // Call base class method
        
        std::cout << "=== Point Light Specific Calculation ===" << std::endl;
        std::cout << "Light Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        Vector3 light_vector = position - point;
        float distance = light_vector.length();
        
        std::cout << "Light Vector: (" << light_vector.x << ", " << light_vector.y << ", " << light_vector.z << ")" << std::endl;
        std::cout << "Distance to Light: " << distance << std::endl;
        
        if (distance > 1e-6f) {
            Vector3 direction = light_vector * (1.0f / distance);
            float attenuation = 1.0f / (distance * distance);
            Vector3 contribution = color * intensity * attenuation;
            
            std::cout << "Normalized Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
            std::cout << "Inverse Square Attenuation: 1/" << distance << "² = " << attenuation << std::endl;
            std::cout << "Light Contribution: (" << contribution.x << ", " << contribution.y << ", " << contribution.z << ")" << std::endl;
            std::cout << "Physical Law: Point light follows inverse square law (1/d²)" << std::endl;
        } else {
            std::cout << "WARNING: Light and surface point coincident - no contribution" << std::endl;
        }
        std::cout << "====================================" << std::endl;
    }
    
    std::string get_light_info() const override {
        return "Point Light at (" + std::to_string(position.x) + ", " + 
               std::to_string(position.y) + ", " + std::to_string(position.z) + 
               ") with intensity " + std::to_string(intensity);
    }
    
    // Additional validation for point light specific parameters
    bool validate_parameters() const override {
        if (!Light::validate_parameters()) {
            return false;
        }
        
        // Check position is finite
        if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)) {
            return false;
        }
        
        return true;
    }
    
    // Clamp point light parameters
    void clamp_parameters() override {
        Light::clamp_parameters();
        
        // Clamp position to reasonable bounds (prevent extreme values)
        const float max_coord = 1000.0f;
        position.x = std::max(-max_coord, std::min(max_coord, position.x));
        position.y = std::max(-max_coord, std::min(max_coord, position.y));
        position.z = std::max(-max_coord, std::min(max_coord, position.z));
    }
};