#pragma once
#include "../core/vector3.hpp"
#include <iostream>
#include <string>
#include <cmath>

// Parameter Validation System with Educational Feedback
// Provides comprehensive validation for Cook-Torrance material parameters
// Educational focus: understanding physical constraints and microfacet theory

class ParameterValidator {
public:
    // Comprehensive parameter validation with educational explanations
    struct ValidationResult {
        bool is_valid;
        std::string message;
        std::string educational_note;
        
        ValidationResult(bool valid, const std::string& msg, const std::string& note = "")
            : is_valid(valid), message(msg), educational_note(note) {}
    };

    // Validate roughness parameter with microfacet theory context
    static ValidationResult validate_roughness(float roughness) {
        if (roughness < 0.01f) {
            return ValidationResult(false, 
                "Roughness too low: " + std::to_string(roughness) + " < 0.01",
                "Minimum roughness prevents numerical instability in GGX distribution. Perfect mirrors (roughness=0) cause division by zero in D term calculations.");
        }
        
        if (roughness > 1.0f) {
            return ValidationResult(false,
                "Roughness too high: " + std::to_string(roughness) + " > 1.0",
                "Maximum roughness represents completely rough surfaces. Values > 1.0 are non-physical and can violate energy conservation.");
        }
        
        // Generate educational feedback based on roughness value
        std::string educational_note;
        if (roughness < 0.2f) {
            educational_note = "Glossy surface: Sharp specular highlights. GGX distribution will be narrow, creating mirror-like reflections.";
        } else if (roughness > 0.7f) {
            educational_note = "Rough surface: Broad specular highlights. GGX distribution will be wide, approaching diffuse behavior.";
        } else {
            educational_note = "Semi-glossy surface: Moderate specular highlights. Balanced between sharp and diffuse reflection characteristics.";
        }
        
        return ValidationResult(true, "Roughness valid: " + std::to_string(roughness), educational_note);
    }

    // Validate metallic parameter with conductor/dielectric theory context
    static ValidationResult validate_metallic(float metallic) {
        if (metallic < 0.0f) {
            return ValidationResult(false,
                "Metallic too low: " + std::to_string(metallic) + " < 0.0",
                "Negative metallic values are non-physical. Metallic parameter represents blend between dielectric and conductor behavior.");
        }
        
        if (metallic > 1.0f) {
            return ValidationResult(false,
                "Metallic too high: " + std::to_string(metallic) + " > 1.0",
                "Metallic values > 1.0 are non-physical. Pure conductors are represented by metallic = 1.0.");
        }
        
        // Generate educational feedback based on metallic value
        std::string educational_note;
        if (metallic < 0.1f) {
            educational_note = "Dielectric material: Low reflectance (~4%), strong Fresnel effect, achromatic specular. Examples: plastic, glass, ceramic.";
        } else if (metallic > 0.9f) {
            educational_note = "Conductor material: High reflectance, weak Fresnel variation, colored specular. Examples: gold, silver, copper.";
        } else {
            educational_note = "Mixed material: Blend between dielectric and conductor. Useful for artistic control but not physically pure.";
        }
        
        return ValidationResult(true, "Metallic valid: " + std::to_string(metallic), educational_note);
    }

    // Validate base color with energy conservation context
    static ValidationResult validate_base_color(const Vector3& base_color) {
        // Check individual channel bounds
        if (base_color.x < 0.0f || base_color.y < 0.0f || base_color.z < 0.0f) {
            return ValidationResult(false,
                "Base color has negative components: (" + std::to_string(base_color.x) + ", " + 
                std::to_string(base_color.y) + ", " + std::to_string(base_color.z) + ")",
                "Negative color values are non-physical. Colors represent energy reflection/absorption coefficients.");
        }
        
        if (base_color.x > 1.0f || base_color.y > 1.0f || base_color.z > 1.0f) {
            return ValidationResult(false,
                "Base color exceeds maximum: (" + std::to_string(base_color.x) + ", " + 
                std::to_string(base_color.y) + ", " + std::to_string(base_color.z) + ")",
                "Color values > 1.0 violate energy conservation. Materials cannot reflect more energy than they receive.");
        }
        
        // Calculate luminance for educational feedback
        float luminance = 0.299f * base_color.x + 0.587f * base_color.y + 0.114f * base_color.z;
        
        std::string educational_note;
        if (luminance > 0.8f) {
            educational_note = "High albedo color: Reflects most incident light. Results in bright appearance under all lighting conditions.";
        } else if (luminance < 0.2f) {
            educational_note = "Low albedo color: Absorbs most incident light. Results in dark appearance, highly dependent on lighting intensity.";
        } else {
            educational_note = "Medium albedo color: Balanced reflection/absorption. Versatile appearance across different lighting conditions.";
        }
        
        return ValidationResult(true, 
            "Base color valid: (" + std::to_string(base_color.x) + ", " + 
            std::to_string(base_color.y) + ", " + std::to_string(base_color.z) + ")", 
            educational_note);
    }

    // Validate specular parameter for dielectric materials
    static ValidationResult validate_specular(float specular) {
        if (specular < 0.0f) {
            return ValidationResult(false,
                "Specular too low: " + std::to_string(specular) + " < 0.0",
                "Negative specular values are non-physical. Specular parameter represents dielectric F0 reflectance.");
        }
        
        if (specular > 1.0f) {
            return ValidationResult(false,
                "Specular too high: " + std::to_string(specular) + " > 1.0",
                "Specular values > 1.0 violate energy conservation. Even highly reflective dielectrics have F0 < 1.0.");
        }
        
        // Educational feedback based on common material ranges
        std::string educational_note;
        if (specular < 0.02f) {
            educational_note = "Very low reflectance: Rare for real materials. Most dielectrics have F0 ≥ 0.02 (IOR ≥ 1.3).";
        } else if (specular > 0.08f) {
            educational_note = "High dielectric reflectance: Typical of glass, water, or high-IOR materials (IOR > 1.5).";
        } else {
            educational_note = "Typical dielectric reflectance: Common for plastic, ceramic, painted surfaces (F0 ≈ 0.04).";
        }
        
        return ValidationResult(true, "Specular valid: " + std::to_string(specular), educational_note);
    }

    // Comprehensive validation of all Cook-Torrance parameters
    static void validate_all_parameters(float roughness, float metallic, const Vector3& base_color, float specular) {
        std::cout << "\n=== Comprehensive Parameter Validation ===" << std::endl;
        
        // Validate each parameter individually
        auto roughness_result = validate_roughness(roughness);
        auto metallic_result = validate_metallic(metallic);
        auto color_result = validate_base_color(base_color);
        auto specular_result = validate_specular(specular);
        
        // Display validation results
        display_validation_result("Roughness", roughness_result);
        display_validation_result("Metallic", metallic_result);
        display_validation_result("Base Color", color_result);
        display_validation_result("Specular", specular_result);
        
        // Overall validation status
        bool all_valid = roughness_result.is_valid && metallic_result.is_valid && 
                        color_result.is_valid && specular_result.is_valid;
        
        std::cout << "\nOverall validation: " << (all_valid ? "✓ PASS" : "✗ FAIL") << std::endl;
        
        if (all_valid) {
            provide_material_analysis(roughness, metallic, base_color, specular);
        } else {
            std::cout << "Fix invalid parameters before rendering to ensure physical accuracy." << std::endl;
        }
    }

    // Automatic parameter clamping with educational warnings
    static Vector3 clamp_base_color(const Vector3& color) {
        Vector3 clamped_color(
            std::max(0.0f, std::min(1.0f, color.x)),
            std::max(0.0f, std::min(1.0f, color.y)),
            std::max(0.0f, std::min(1.0f, color.z))
        );
        
        if ((color - clamped_color).length() > 0.001f) {
            std::cout << "\n⚠ Base Color Clamped:" << std::endl;
            std::cout << "  Original: (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
            std::cout << "  Clamped:  (" << clamped_color.x << ", " << clamped_color.y << ", " << clamped_color.z << ")" << std::endl;
            std::cout << "  Reason: Maintaining energy conservation and physical validity" << std::endl;
        }
        
        return clamped_color;
    }

    static float clamp_parameter(float value, float min_val, float max_val, const std::string& param_name) {
        float clamped_value = std::max(min_val, std::min(max_val, value));
        
        if (std::abs(value - clamped_value) > 0.001f) {
            std::cout << "\n⚠ " << param_name << " Clamped:" << std::endl;
            std::cout << "  Original: " << value << std::endl;
            std::cout << "  Clamped:  " << clamped_value << std::endl;
            std::cout << "  Range:    [" << min_val << ", " << max_val << "]" << std::endl;
        }
        
        return clamped_value;
    }

private:
    // Display validation result with formatting
    static void display_validation_result(const std::string& param_name, const ValidationResult& result) {
        std::cout << "\n" << param_name << ": " << (result.is_valid ? "✓" : "✗") << " " << result.message << std::endl;
        
        if (!result.educational_note.empty()) {
            std::cout << "  → " << result.educational_note << std::endl;
        }
    }

    // Provide comprehensive material analysis based on all parameters
    static void provide_material_analysis(float roughness, float metallic, const Vector3& base_color, float specular) {
        std::cout << "\n=== Material Characteristics Analysis ===" << std::endl;
        
        // Determine primary material type
        std::string material_type = (metallic > 0.5f) ? "Conductor (Metal)" : "Dielectric (Non-metal)";
        std::cout << "Material Type: " << material_type << std::endl;
        
        // Surface characteristic analysis
        std::string surface_char;
        if (roughness < 0.2f) {
            surface_char = "Glossy/Mirror-like";
        } else if (roughness > 0.7f) {
            surface_char = "Rough/Diffuse-like";
        } else {
            surface_char = "Semi-glossy";
        }
        std::cout << "Surface Character: " << surface_char << std::endl;
        
        // Calculate luminance for appearance prediction
        float luminance = 0.299f * base_color.x + 0.587f * base_color.y + 0.114f * base_color.z;
        std::cout << "Appearance Brightness: " << (luminance > 0.6f ? "Bright" : 
                                                     luminance < 0.3f ? "Dark" : "Medium") << std::endl;
        
        // Provide rendering expectations
        std::cout << "\n=== Rendering Expectations ===" << std::endl;
        
        if (metallic > 0.5f) {
            std::cout << "• Colored specular reflections based on base color" << std::endl;
            std::cout << "• High overall reflectance" << std::endl;
            std::cout << "• Minimal diffuse scattering" << std::endl;
        } else {
            std::cout << "• Achromatic (white/gray) specular reflections" << std::endl;
            std::cout << "• Colored diffuse/subsurface scattering" << std::endl;
            std::cout << "• Strong Fresnel effect at grazing angles" << std::endl;
        }
        
        if (roughness < 0.3f) {
            std::cout << "• Sharp, concentrated highlights" << std::endl;
            std::cout << "• Clear environmental reflections" << std::endl;
        } else {
            std::cout << "• Broad, soft highlights" << std::endl;
            std::cout << "• Diffused environmental reflections" << std::endl;
        }
        
        std::cout << "✓ All parameters are physically valid and ready for rendering!" << std::endl;
    }
};