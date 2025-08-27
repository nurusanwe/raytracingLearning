#pragma once

#include "light_base.hpp"
#include "../core/vector3.hpp"
#include "../core/ray.hpp"
#include <iostream>
#include <cmath>
#include <random>

// Forward declaration for Scene to avoid circular dependency
class Scene;

class AreaLight : public Light {
public:
    Vector3 center;     // Center position of the rectangular area light
    Vector3 normal;     // Outward normal direction of the light surface (normalized)
    float width;        // Width of the rectangular light
    float height;       // Height of the rectangular light
    Vector3 u_axis;     // Local U axis (width direction, normalized)
    Vector3 v_axis;     // Local V axis (height direction, normalized)
    
    // Random number generator for Monte Carlo sampling
    mutable std::mt19937 rng{std::random_device{}()};
    mutable std::uniform_real_distribution<float> uniform_dist{0.0f, 1.0f};
    
    AreaLight(const Vector3& light_center, const Vector3& surface_normal, 
              float light_width, float light_height,
              const Vector3& light_color, float light_intensity)
        : Light(light_color, light_intensity, LightType::Area), 
          center(light_center), width(light_width), height(light_height) {
        
        // Normalize the normal vector
        float normal_length = surface_normal.length();
        if (normal_length > 1e-6f) {
            normal = surface_normal * (1.0f / normal_length);
        } else {
            normal = Vector3(0, 0, 1);  // Default normal pointing up
        }
        
        // Generate orthonormal basis for the rectangle
        // Choose arbitrary vector not parallel to normal for cross product
        Vector3 arbitrary = (std::abs(normal.x) < 0.9f) ? Vector3(1, 0, 0) : Vector3(0, 1, 0);
        u_axis = normal.cross(arbitrary).normalize();
        v_axis = normal.cross(u_axis);  // Already normalized due to orthogonal vectors
    }
    
    // Core light evaluation interface implementation
    Vector3 illuminate(const Vector3& point, Vector3& light_direction, float& distance) const override {
        // For area lights, we sample a random point on the light surface
        // This provides Monte Carlo integration for soft shadows
        Vector3 sample_point = sample_point_on_surface();
        
        Vector3 light_vector = sample_point - point;
        distance = light_vector.length();
        
        if (distance < 1e-6f) {
            light_direction = Vector3(0, 0, 1);
            return Vector3(0, 0, 0);
        }
        
        light_direction = light_vector * (1.0f / distance);
        
        // Check if the sample point faces the surface point
        float cos_theta = normal.dot(light_direction * -1.0f);
        if (cos_theta <= 0.0f) {
            // Back-facing, no contribution
            return Vector3(0, 0, 0);
        }
        
        // Area lights have 1/distance² falloff but also depend on the area and angle
        float area = width * height;
        float attenuation = (cos_theta * area) / (distance * distance);
        
        return color * intensity * attenuation;
    }
    
    bool is_occluded(const Vector3& point, const Vector3& light_direction, float distance, const Scene& scene) const override {
        // Create shadow ray with small epsilon offset to avoid self-intersection
        const float epsilon = 0.001f;
        Vector3 offset_point = point + light_direction * epsilon;
        Ray shadow_ray(Point3(offset_point.x, offset_point.y, offset_point.z), light_direction);
        
        // Test intersection with scene
        Scene::Intersection hit = scene.intersect(shadow_ray, false);  // Disable verbose output
        
        // Check if intersection occurs before reaching the light
        return hit.hit && hit.t < (distance - epsilon);
    }
    
    Vector3 sample_direction(const Vector3& point, float& pdf) const override {
        // Sample a random point on the area light surface
        Vector3 sample_point = sample_point_on_surface();
        Vector3 light_vector = sample_point - point;
        float distance = light_vector.length();
        
        if (distance < 1e-6f) {
            pdf = 0.0f;
            return Vector3(0, 0, 1);
        }
        
        Vector3 direction = light_vector * (1.0f / distance);
        
        // Calculate PDF based on solid angle from the point
        // For simplicity, we use uniform area sampling
        float area = width * height;
        float cos_theta = normal.dot(direction * -1.0f);
        
        if (cos_theta > 0.0f) {
            pdf = (distance * distance) / (area * cos_theta);
        } else {
            pdf = 0.0f;
        }
        
        return direction;
    }
    
    // Sample a random point on the area light surface
    Vector3 sample_point_on_surface() const {
        // Generate random coordinates in [-0.5, 0.5] range
        float u = uniform_dist(rng) - 0.5f;  // [-0.5, 0.5]
        float v = uniform_dist(rng) - 0.5f;  // [-0.5, 0.5]
        
        // Map to world coordinates using local basis
        return center + u_axis * (u * width) + v_axis * (v * height);
    }
    
    // Educational debugging methods
    void explain_light_calculation(const Vector3& point) const override {
        Light::explain_light_calculation(point);  // Call base class method
        
        std::cout << "=== Area Light Specific Calculation ===" << std::endl;
        std::cout << "Light Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
        std::cout << "Light Normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
        std::cout << "Light Dimensions: " << width << " x " << height << " units" << std::endl;
        std::cout << "Light Area: " << (width * height) << " square units" << std::endl;
        
        Vector3 sample_point = sample_point_on_surface();
        Vector3 light_vector = sample_point - point;
        float distance = light_vector.length();
        
        std::cout << "Sample Point: (" << sample_point.x << ", " << sample_point.y << ", " << sample_point.z << ")" << std::endl;
        std::cout << "Distance to Sample: " << distance << std::endl;
        
        if (distance > 1e-6f) {
            Vector3 direction = light_vector * (1.0f / distance);
            float cos_theta = normal.dot(direction * -1.0f);
            float area = width * height;
            float attenuation = (cos_theta * area) / (distance * distance);
            Vector3 contribution = color * intensity * attenuation;
            
            std::cout << "Light Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
            std::cout << "Surface Angle (cos θ): " << cos_theta << std::endl;
            std::cout << "Area Attenuation: " << attenuation << std::endl;
            std::cout << "Light Contribution: (" << contribution.x << ", " << contribution.y << ", " << contribution.z << ")" << std::endl;
            
        }
        
        std::cout << "Physical Model: Rectangular surface emitting light uniformly" << std::endl;
        std::cout << "Key Property: Soft shadows through Monte Carlo sampling" << std::endl;
        std::cout << "Usage: Studio lighting, window light, large light fixtures" << std::endl;
        std::cout << "====================================" << std::endl;
    }
    
    std::string get_light_info() const override {
        return "Area Light at (" + std::to_string(center.x) + ", " + 
               std::to_string(center.y) + ", " + std::to_string(center.z) + 
               ") size " + std::to_string(width) + "x" + std::to_string(height) +
               " with intensity " + std::to_string(intensity);
    }
    
    // Additional validation for area light specific parameters
    bool validate_parameters() const override {
        if (!Light::validate_parameters()) {
            return false;
        }
        
        // Check center position is finite
        if (!std::isfinite(center.x) || !std::isfinite(center.y) || !std::isfinite(center.z)) {
            return false;
        }
        
        // Check normal is finite and non-zero
        if (!std::isfinite(normal.x) || !std::isfinite(normal.y) || !std::isfinite(normal.z)) {
            return false;
        }
        
        if (normal.length_squared() < 1e-12f) {
            return false;
        }
        
        // Check dimensions are positive
        if (width <= 0.0f || height <= 0.0f) {
            return false;
        }
        
        if (!std::isfinite(width) || !std::isfinite(height)) {
            return false;
        }
        
        return true;
    }
    
    // Clamp area light parameters
    void clamp_parameters() override {
        Light::clamp_parameters();
        
        // Clamp center to reasonable bounds
        const float max_coord = 1000.0f;
        center.x = std::max(-max_coord, std::min(max_coord, center.x));
        center.y = std::max(-max_coord, std::min(max_coord, center.y));
        center.z = std::max(-max_coord, std::min(max_coord, center.z));
        
        // Ensure normal is normalized
        float normal_length = normal.length();
        if (normal_length > 1e-6f) {
            normal = normal * (1.0f / normal_length);
        } else {
            normal = Vector3(0, 0, 1);
        }
        
        // Clamp dimensions to reasonable range
        width = std::max(0.01f, std::min(100.0f, width));
        height = std::max(0.01f, std::min(100.0f, height));
        
        // Regenerate basis vectors if needed
        Vector3 arbitrary = (std::abs(normal.x) < 0.9f) ? Vector3(1, 0, 0) : Vector3(0, 1, 0);
        u_axis = normal.cross(arbitrary).normalize();
        v_axis = normal.cross(u_axis);
    }
};