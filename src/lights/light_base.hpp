#pragma once

#include "../core/vector3.hpp"
#include <string>
#include <iostream>
#include <algorithm>

// Forward declaration for Scene to avoid circular dependency
class Scene;

enum class LightType {
    Point,
    Directional,
    Area
};

class Light {
public:
    Vector3 color;           // RGB color components [0.0-1.0]
    float intensity;         // Dimensionless intensity multiplier (educational units)
    LightType type;
    
    Light(const Vector3& light_color, float light_intensity, LightType light_type) 
        : color(light_color), intensity(light_intensity), type(light_type) {}
    
    virtual ~Light() = default;
    
    // Core light evaluation interface
    virtual Vector3 illuminate(const Vector3& point, Vector3& light_direction, float& distance) const = 0;
    virtual bool is_occluded(const Vector3& point, const Vector3& light_direction, float distance, const Scene& scene) const = 0;
    virtual Vector3 sample_direction(const Vector3& point, float& pdf) const = 0;
    
    // Educational debugging methods
    virtual void explain_light_calculation(const Vector3& point) const {
        std::cout << "=== Light Calculation Debug ===" << std::endl;
        std::cout << "Light Type: " << static_cast<int>(type) << std::endl;
        std::cout << "Light Color: (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
        std::cout << "Light Intensity: " << intensity << " (dimensionless multiplier)" << std::endl;
        std::cout << "Calculation Point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
    }
    
    virtual std::string get_light_info() const = 0;

    // Utility method to validate light parameters
    virtual bool validate_parameters() const {
        // Check color components are in valid range [0.0, 1.0]
        if (color.x < 0.0f || color.x > 1.0f ||
            color.y < 0.0f || color.y > 1.0f ||
            color.z < 0.0f || color.z > 1.0f) {
            return false;
        }
        
        // Check intensity is non-negative (allow zero for educational purposes)
        if (intensity < 0.0f) {
            return false;
        }
        
        return true;
    }
    
    // Utility method to clamp parameters to valid ranges
    virtual void clamp_parameters() {
        color.x = std::max(0.0f, std::min(1.0f, color.x));
        color.y = std::max(0.0f, std::min(1.0f, color.y));
        color.z = std::max(0.0f, std::min(1.0f, color.z));
        intensity = std::max(0.0f, intensity); // Allow zero intensity for educational purposes
    }
};