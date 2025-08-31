#pragma once
#include "../core/vector3.hpp"
#include "../materials/cook_torrance.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// Interactive Material Parameter Editor
// Provides command-line interface for real-time Cook-Torrance material parameter adjustment
// Educational focus: immediate visual feedback and microfacet theory explanations

class MaterialEditor {
public:
    // Main interactive parameter adjustment interface
    // Returns true if any parameters were changed, false if cancelled or no changes
    static bool get_user_parameter_input(CookTorranceMaterial* material) {
        if (!material) {
            std::cout << "Error: Material pointer is null" << std::endl;
            return false;
        }

        display_current_parameters(*material);
        display_parameter_menu();
        
        // Always validate parameters before and after changes for educational purposes
        validate_current_parameters(*material);

        int choice;
        std::cout << "\nEnter your choice (1-4, 0 to cancel): ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                float new_roughness = material->roughness;
                if (prompt_for_roughness(new_roughness)) {
                    material->set_roughness(new_roughness);
                    return true;
                }
                break;
            }
            case 2: {
                float new_metallic = material->metallic;
                if (prompt_for_metallic(new_metallic)) {
                    material->set_metallic(new_metallic);
                    return true;
                }
                break;
            }
            case 3: {
                Vector3 new_color = material->base_color;
                if (prompt_for_base_color(new_color)) {
                    material->set_base_color(new_color);
                    return true;
                }
                break;
            }
            case 4: {
                // Adjust multiple parameters
                bool any_changed = false;
                
                float new_roughness = material->roughness;
                if (prompt_for_roughness(new_roughness)) {
                    material->set_roughness(new_roughness);
                    any_changed = true;
                }
                
                float new_metallic = material->metallic;
                if (prompt_for_metallic(new_metallic)) {
                    material->set_metallic(new_metallic);
                    any_changed = true;
                }
                
                Vector3 new_color = material->base_color;
                if (prompt_for_base_color(new_color)) {
                    material->set_base_color(new_color);
                    any_changed = true;
                }
                
                return any_changed;
            }
            case 0:
                std::cout << "Parameter adjustment cancelled." << std::endl;
                return false;
            default:
                std::cout << "Invalid choice. No parameters changed." << std::endl;
                return false;
        }
        
        return false;
    }

    // Display interactive parameter adjustment menu
    static void display_parameter_menu() {
        std::cout << "\n=== Material Parameter Adjustment Menu ===" << std::endl;
        std::cout << "1. Adjust roughness (microfacet surface variance)" << std::endl;
        std::cout << "2. Adjust metallic (conductor vs dielectric blend)" << std::endl;
        std::cout << "3. Adjust base color (surface albedo/reflectance)" << std::endl;
        std::cout << "4. Adjust multiple parameters" << std::endl;
        std::cout << "0. Cancel (no changes)" << std::endl;
    }

    // Display current material parameters with educational context
    static void display_current_parameters(const CookTorranceMaterial& material) {
        std::cout << "\n=== Current Material Parameters ===" << std::endl;
        std::cout << std::fixed << std::setprecision(3);
        
        std::cout << "Base Color:  (" << material.base_color.x << ", " 
                  << material.base_color.y << ", " << material.base_color.z << ")" << std::endl;
        std::cout << "Roughness:   " << material.roughness << " (0.0=mirror, 1.0=diffuse)" << std::endl;
        std::cout << "Metallic:    " << material.metallic << " (0.0=dielectric, 1.0=conductor)" << std::endl;
        std::cout << "Specular:    " << material.specular << " (dielectric F0 reflectance)" << std::endl;
        
        // Material type interpretation
        std::string material_type = (material.metallic > 0.5f) ? "Conductor (Metal)" : "Dielectric (Non-metal)";
        std::cout << "Type:        " << material_type << std::endl;
        
        // Surface characteristic interpretation
        std::string surface_char;
        if (material.roughness < 0.2f) {
            surface_char = "Glossy/Mirror-like";
        } else if (material.roughness > 0.7f) {
            surface_char = "Rough/Diffuse-like";
        } else {
            surface_char = "Semi-glossy";
        }
        std::cout << "Surface:     " << surface_char << std::endl;
    }

private:
    // Prompt user for roughness parameter with validation and educational explanations
    static bool prompt_for_roughness(float& roughness) {
        explain_parameter_meaning("roughness");
        
        std::cout << "\nCurrent roughness: " << roughness << std::endl;
        std::cout << "Enter new roughness value [0.01-1.0] (or 'c' to cancel): ";
        
        std::string input;
        std::cin >> input;
        
        if (input == "c" || input == "C") {
            std::cout << "Roughness adjustment cancelled." << std::endl;
            return false;
        }
        
        try {
            float new_value = std::stof(input);
            
            if (new_value < 0.01f || new_value > 1.0f) {
                std::cout << "Warning: Roughness value " << new_value << " outside valid range [0.01-1.0]" << std::endl;
                std::cout << "Value will be automatically clamped to valid range." << std::endl;
                new_value = std::max(0.01f, std::min(1.0f, new_value));
            }
            
            if (std::abs(new_value - roughness) < 0.001f) {
                std::cout << "No significant change in roughness value." << std::endl;
                return false;
            }
            
            roughness = new_value;
            std::cout << "Roughness updated to: " << roughness << std::endl;
            explain_roughness_impact(roughness);
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "Invalid input '" << input << "'. Roughness unchanged." << std::endl;
            return false;
        }
    }

    // Prompt user for metallic parameter with validation and educational explanations
    static bool prompt_for_metallic(float& metallic) {
        explain_parameter_meaning("metallic");
        
        std::cout << "\nCurrent metallic: " << metallic << std::endl;
        std::cout << "Enter new metallic value [0.0-1.0] (or 'c' to cancel): ";
        
        std::string input;
        std::cin >> input;
        
        if (input == "c" || input == "C") {
            std::cout << "Metallic adjustment cancelled." << std::endl;
            return false;
        }
        
        try {
            float new_value = std::stof(input);
            
            if (new_value < 0.0f || new_value > 1.0f) {
                std::cout << "Warning: Metallic value " << new_value << " outside valid range [0.0-1.0]" << std::endl;
                std::cout << "Value will be automatically clamped to valid range." << std::endl;
                new_value = std::max(0.0f, std::min(1.0f, new_value));
            }
            
            if (std::abs(new_value - metallic) < 0.001f) {
                std::cout << "No significant change in metallic value." << std::endl;
                return false;
            }
            
            metallic = new_value;
            std::cout << "Metallic updated to: " << metallic << std::endl;
            explain_metallic_impact(metallic);
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "Invalid input '" << input << "'. Metallic unchanged." << std::endl;
            return false;
        }
    }

    // Prompt user for base color with validation and educational explanations
    static bool prompt_for_base_color(Vector3& base_color) {
        explain_parameter_meaning("base_color");
        
        std::cout << "\nCurrent base color: (" << base_color.x << ", " 
                  << base_color.y << ", " << base_color.z << ")" << std::endl;
        std::cout << "Enter new base color RGB values [0.0-1.0] separated by spaces (or 'c' to cancel): ";
        std::cout << "\nFormat: R G B (e.g., 0.8 0.3 0.3 for red): ";
        
        std::string line;
        std::cin.ignore(); // Clear any remaining newline
        std::getline(std::cin, line);
        
        if (line == "c" || line == "C") {
            std::cout << "Base color adjustment cancelled." << std::endl;
            return false;
        }
        
        std::istringstream iss(line);
        float r, g, b;
        
        if (!(iss >> r >> g >> b)) {
            std::cout << "Invalid format. Expected three float values. Base color unchanged." << std::endl;
            return false;
        }
        
        // Validate and clamp color components
        if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f) {
            std::cout << "Warning: Color components outside valid range [0.0-1.0]" << std::endl;
            std::cout << "Values will be automatically clamped to valid range." << std::endl;
        }
        
        r = std::max(0.0f, std::min(1.0f, r));
        g = std::max(0.0f, std::min(1.0f, g));
        b = std::max(0.0f, std::min(1.0f, b));
        
        Vector3 new_color(r, g, b);
        
        // Check for significant change
        float color_change = (new_color - base_color).length();
        if (color_change < 0.001f) {
            std::cout << "No significant change in base color." << std::endl;
            return false;
        }
        
        base_color = new_color;
        std::cout << "Base color updated to: (" << base_color.x << ", " 
                  << base_color.y << ", " << base_color.z << ")" << std::endl;
        explain_color_impact(base_color);
        return true;
    }

    // Educational explanations for parameter meanings and microfacet theory
    static void explain_parameter_meaning(const std::string& parameter_name) {
        std::cout << "\n=== " << parameter_name << " Parameter Education ===" << std::endl;
        
        if (parameter_name == "roughness") {
            std::cout << "ROUGHNESS controls the statistical distribution of microfacet orientations:" << std::endl;
            std::cout << "• 0.01-0.2: Glossy/mirror-like surfaces (sharp reflections)" << std::endl;
            std::cout << "• 0.2-0.7: Semi-glossy surfaces (moderate reflection spread)" << std::endl;
            std::cout << "• 0.7-1.0: Rough/diffuse-like surfaces (broad reflection spread)" << std::endl;
            std::cout << "\nMicrofacet Theory: Controls the Normal Distribution Function (D term)" << std::endl;
            std::cout << "• Low roughness = narrow D function = sharp highlights" << std::endl;
            std::cout << "• High roughness = wide D function = broad highlights" << std::endl;
        }
        else if (parameter_name == "metallic") {
            std::cout << "METALLIC controls the blend between dielectric and conductor behavior:" << std::endl;
            std::cout << "• 0.0: Pure dielectric (glass, plastic, ceramic, wood)" << std::endl;
            std::cout << "• 0.5: Mixed behavior (rare in reality, used for artistic control)" << std::endl;
            std::cout << "• 1.0: Pure conductor (metal: gold, silver, copper, aluminum)" << std::endl;
            std::cout << "\nMicrofacet Theory: Controls the Fresnel Function (F term) behavior" << std::endl;
            std::cout << "• Dielectric: F0 ≈ 0.04 (low reflectance, strong Fresnel effect)" << std::endl;
            std::cout << "• Conductor: F0 = base_color (high reflectance, colored reflection)" << std::endl;
        }
        else if (parameter_name == "base_color") {
            std::cout << "BASE COLOR controls the surface albedo and absorption characteristics:" << std::endl;
            std::cout << "• For dielectrics: Diffuse albedo (transmitted light color)" << std::endl;
            std::cout << "• For conductors: Specular reflectance (reflected light color)" << std::endl;
            std::cout << "• Range [0.0-1.0] per RGB channel" << std::endl;
            std::cout << "\nMicrofacet Theory: Influences both diffuse and specular contributions" << std::endl;
            std::cout << "• Energy conservation: absorbed + reflected ≤ incident energy" << std::endl;
            std::cout << "• Physical realism: darker colors absorb more energy" << std::endl;
        }
    }

    // Educational explanation of roughness parameter impact on appearance
    static void explain_roughness_impact(float roughness_value) {
        std::cout << "\n=== Roughness Impact Analysis ===" << std::endl;
        
        if (roughness_value < 0.2f) {
            std::cout << "GLOSSY SURFACE CHARACTERISTICS:" << std::endl;
            std::cout << "• Sharp, concentrated specular highlights" << std::endl;
            std::cout << "• Clear reflections of environment" << std::endl;
            std::cout << "• High contrast between lit and unlit areas" << std::endl;
            std::cout << "• Examples: polished metal, wet surfaces, mirrors" << std::endl;
        } else if (roughness_value > 0.7f) {
            std::cout << "ROUGH SURFACE CHARACTERISTICS:" << std::endl;
            std::cout << "• Broad, diffused specular highlights" << std::endl;
            std::cout << "• Soft, scattered reflections" << std::endl;
            std::cout << "• Approaches Lambert-like behavior" << std::endl;
            std::cout << "• Examples: matte paint, unpolished wood, fabric" << std::endl;
        } else {
            std::cout << "SEMI-GLOSSY SURFACE CHARACTERISTICS:" << std::endl;
            std::cout << "• Moderate specular highlight spread" << std::endl;
            std::cout << "• Balanced reflection characteristics" << std::endl;
            std::cout << "• Versatile appearance for many materials" << std::endl;
            std::cout << "• Examples: satin finish, semi-gloss paint, skin" << std::endl;
        }
        
        std::cout << "\nVisual Expectation: " << (roughness_value < 0.3f ? "Sharp highlights" : 
                                                   roughness_value > 0.7f ? "Soft, broad highlights" : 
                                                   "Moderate highlight spread") << std::endl;
    }

    // Educational explanation of metallic parameter impact on appearance  
    static void explain_metallic_impact(float metallic_value) {
        std::cout << "\n=== Metallic Impact Analysis ===" << std::endl;
        
        if (metallic_value < 0.1f) {
            std::cout << "DIELECTRIC MATERIAL BEHAVIOR:" << std::endl;
            std::cout << "• Low reflectance at normal viewing (~4%)" << std::endl;
            std::cout << "• Strong Fresnel effect at grazing angles" << std::endl;
            std::cout << "• Achromatic reflections (white/gray specular)" << std::endl;
            std::cout << "• Base color influences transmitted/diffuse light" << std::endl;
            std::cout << "• Examples: plastic, glass, ceramic, wood, skin" << std::endl;
        } else if (metallic_value > 0.9f) {
            std::cout << "CONDUCTOR MATERIAL BEHAVIOR:" << std::endl;
            std::cout << "• High reflectance across visible spectrum" << std::endl;
            std::cout << "• Weak Fresnel variation with angle" << std::endl;
            std::cout << "• Colored reflections based on base color" << std::endl;
            std::cout << "• No subsurface scattering (opaque)" << std::endl;
            std::cout << "• Examples: gold, silver, copper, aluminum, iron" << std::endl;
        } else {
            std::cout << "MIXED MATERIAL BEHAVIOR:" << std::endl;
            std::cout << "• Blend between dielectric and conductor properties" << std::endl;
            std::cout << "• Useful for artistic control or oxidized metals" << std::endl;
            std::cout << "• Not physically realistic for pure materials" << std::endl;
            std::cout << "• Can represent layered materials or coatings" << std::endl;
        }
        
        std::cout << "\nVisual Expectation: " << (metallic_value < 0.3f ? "Colored diffuse, white specular" : 
                                                   metallic_value > 0.7f ? "Colored specular, no diffuse" : 
                                                   "Mixed colored reflections") << std::endl;
    }

    // Educational explanation of base color impact on material appearance
    static void explain_color_impact(const Vector3& color_value) {
        std::cout << "\n=== Base Color Impact Analysis ===" << std::endl;
        
        // Calculate color characteristics
        float luminance = 0.299f * color_value.x + 0.587f * color_value.y + 0.114f * color_value.z;
        float max_component = std::max({color_value.x, color_value.y, color_value.z});
        float min_component = std::min({color_value.x, color_value.y, color_value.z});
        float saturation = (max_component > 0.0f) ? (max_component - min_component) / max_component : 0.0f;
        
        std::cout << "COLOR CHARACTERISTICS:" << std::endl;
        std::cout << "• Luminance: " << std::fixed << std::setprecision(3) << luminance << " (brightness perception)" << std::endl;
        std::cout << "• Saturation: " << saturation << " (color purity)" << std::endl;
        
        // Dominant color analysis
        if (color_value.x > color_value.y && color_value.x > color_value.z) {
            std::cout << "• Dominant: Red channel (warm appearance)" << std::endl;
        } else if (color_value.y > color_value.x && color_value.y > color_value.z) {
            std::cout << "• Dominant: Green channel (natural appearance)" << std::endl;
        } else if (color_value.z > color_value.x && color_value.z > color_value.y) {
            std::cout << "• Dominant: Blue channel (cool appearance)" << std::endl;
        } else {
            std::cout << "• Balanced: Neutral gray appearance" << std::endl;
        }
        
        // Energy conservation implications
        std::cout << "\nENERGY CONSERVATION IMPACT:" << std::endl;
        if (luminance > 0.8f) {
            std::cout << "• High albedo: Reflects most incident light" << std::endl;
            std::cout << "• Low absorption: Minimal energy loss" << std::endl;
            std::cout << "• Bright appearance under all lighting" << std::endl;
        } else if (luminance < 0.2f) {
            std::cout << "• Low albedo: Absorbs most incident light" << std::endl;
            std::cout << "• High absorption: Significant energy loss" << std::endl;
            std::cout << "• Dark appearance, depends on lighting intensity" << std::endl;
        } else {
            std::cout << "• Medium albedo: Balanced reflection/absorption" << std::endl;
            std::cout << "• Moderate energy conservation" << std::endl;
            std::cout << "• Versatile appearance across lighting conditions" << std::endl;
        }
        
        std::cout << "\nVisual Expectation: " << (saturation > 0.5f ? "Colorful, saturated appearance" : 
                                                   "Desaturated, neutral appearance") << std::endl;
    }
    
    // Validate current material parameters and provide educational feedback
    static void validate_current_parameters(const CookTorranceMaterial& material) {
        std::cout << "\n=== Parameter Validation Summary ===" << std::endl;
        
        // Validate roughness
        if (material.roughness >= 0.01f && material.roughness <= 1.0f) {
            std::cout << "✓ Roughness (" << material.roughness << ") is within valid range [0.01-1.0]" << std::endl;
        } else {
            std::cout << "⚠ Roughness (" << material.roughness << ") outside recommended range [0.01-1.0]" << std::endl;
        }
        
        // Validate metallic
        if (material.metallic >= 0.0f && material.metallic <= 1.0f) {
            std::cout << "✓ Metallic (" << material.metallic << ") is within valid range [0.0-1.0]" << std::endl;
        } else {
            std::cout << "⚠ Metallic (" << material.metallic << ") outside valid range [0.0-1.0]" << std::endl;
        }
        
        // Validate base color
        bool color_valid = (material.base_color.x >= 0.0f && material.base_color.x <= 1.0f &&
                           material.base_color.y >= 0.0f && material.base_color.y <= 1.0f &&
                           material.base_color.z >= 0.0f && material.base_color.z <= 1.0f);
        if (color_valid) {
            std::cout << "✓ Base color components within valid range [0.0-1.0]" << std::endl;
        } else {
            std::cout << "⚠ Base color contains components outside valid range [0.0-1.0]" << std::endl;
        }
        
        // Energy conservation check
        float max_color = std::max({material.base_color.x, material.base_color.y, material.base_color.z});
        if (max_color <= 1.0f) {
            std::cout << "✓ Material satisfies energy conservation (max albedo ≤ 1.0)" << std::endl;
        } else {
            std::cout << "⚠ Material may violate energy conservation (max albedo > 1.0)" << std::endl;
        }
        
        // Physical realism assessment
        if ((material.metallic < 0.1f || material.metallic > 0.9f) && 
            material.roughness >= 0.01f && material.roughness <= 1.0f) {
            std::cout << "✓ Material configuration is physically realistic" << std::endl;
        } else {
            std::cout << "⚠ Mixed metallic values (0.1-0.9) are rarely physically realistic" << std::endl;
        }
    }
};