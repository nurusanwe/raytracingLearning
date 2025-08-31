#pragma once
#include "../core/vector3.hpp"
#include "../materials/cook_torrance.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>

// Material Preset Management System
// Provides library of common Cook-Torrance material configurations
// Educational focus: understanding real-world material properties

struct MaterialPreset {
    std::string name;
    std::string description;
    Vector3 base_color;
    float roughness;
    float metallic;
    float specular;
    
    MaterialPreset(const std::string& preset_name, const std::string& preset_desc,
                   const Vector3& color, float r, float m, float s)
        : name(preset_name), description(preset_desc), base_color(color), 
          roughness(r), metallic(m), specular(s) {}
};

class PresetManager {
public:
    // Get library of default Cook-Torrance material presets
    static std::vector<MaterialPreset> get_default_presets() {
        std::vector<MaterialPreset> presets;
        
        // Metal presets - High metallic, varying roughness, realistic base colors
        presets.push_back(create_polished_gold_preset());
        presets.push_back(create_brushed_aluminum_preset());
        presets.push_back(create_oxidized_copper_preset());
        presets.push_back(create_rough_iron_preset());
        
        // Dielectric presets - Zero metallic, varying roughness and specular
        presets.push_back(create_glossy_plastic_preset());
        presets.push_back(create_matte_ceramic_preset());
        presets.push_back(create_clear_glass_preset());
        presets.push_back(create_painted_wood_preset());
        
        return presets;
    }

    // Apply preset to material with educational feedback
    static void apply_preset(CookTorranceMaterial* material, const MaterialPreset& preset) {
        if (!material) {
            std::cout << "Error: Material pointer is null" << std::endl;
            return;
        }
        
        std::cout << "\n=== Applying Material Preset ===" << std::endl;
        std::cout << "Preset: " << preset.name << std::endl;
        std::cout << "Description: " << preset.description << std::endl;
        
        // Store old values for comparison
        Vector3 old_color = material->base_color;
        float old_roughness = material->roughness;
        float old_metallic = material->metallic;
        float old_specular = material->specular;
        
        // Apply preset parameters
        material->set_base_color(preset.base_color);
        material->set_roughness(preset.roughness);
        material->set_metallic(preset.metallic);
        material->specular = preset.specular; // Direct assignment for specular
        
        std::cout << "\n=== Parameter Changes ===" << std::endl;
        std::cout << "Base Color: (" << old_color.x << ", " << old_color.y << ", " << old_color.z << ") → (" 
                  << preset.base_color.x << ", " << preset.base_color.y << ", " << preset.base_color.z << ")" << std::endl;
        std::cout << "Roughness:  " << old_roughness << " → " << preset.roughness << std::endl;
        std::cout << "Metallic:   " << old_metallic << " → " << preset.metallic << std::endl;
        std::cout << "Specular:   " << old_specular << " → " << preset.specular << std::endl;
        
        explain_preset_characteristics(preset);
        
        std::cout << "\nPreset applied successfully!" << std::endl;
    }

    // Interactive preset selection interface
    static bool select_and_apply_preset(CookTorranceMaterial* material) {
        if (!material) {
            std::cout << "Error: Material pointer is null" << std::endl;
            return false;
        }
        
        display_preset_library();
        
        std::vector<MaterialPreset> presets = get_default_presets();
        
        std::cout << "\nEnter preset selection (1-" << presets.size() << ", or 'c' to cancel): ";
        std::string input;
        std::cin >> input;
        
        if (input == "c" || input == "C") {
            std::cout << "Preset selection cancelled." << std::endl;
            return false;
        }
        
        try {
            int selection = std::stoi(input);
            if (selection >= 1 && selection <= static_cast<int>(presets.size())) {
                apply_preset(material, presets[selection - 1]);
                return true;
            } else {
                std::cout << "Invalid selection. Range: 1-" << presets.size() << std::endl;
                return false;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid input. Expected number or 'c'." << std::endl;
            return false;
        }
    }

    // Display available preset library with descriptions
    static void display_preset_library() {
        std::cout << "\n=== Cook-Torrance Material Preset Library ===" << std::endl;
        
        std::vector<MaterialPreset> presets = get_default_presets();
        
        std::cout << "\nAvailable Presets:" << std::endl;
        for (size_t i = 0; i < presets.size(); ++i) {
            const auto& preset = presets[i];
            std::cout << (i + 1) << ". " << preset.name << std::endl;
            std::cout << "   " << preset.description << std::endl;
            std::cout << "   Color: (" << preset.base_color.x << ", " << preset.base_color.y << ", " << preset.base_color.z << ")" 
                      << " | Roughness: " << preset.roughness 
                      << " | Metallic: " << preset.metallic << std::endl;
        }
        
        std::cout << "\nPreset Categories:" << std::endl;
        std::cout << "• Metal Materials (1-4): Conductors with high reflectance and colored specular" << std::endl;
        std::cout << "• Dielectric Materials (5-8): Non-metals with low reflectance and achromatic specular" << std::endl;
    }

    // Save current material as custom preset
    static bool save_material_preset(const CookTorranceMaterial& material, const std::string& name) {
        try {
            // Create presets directory if it doesn't exist
            std::filesystem::create_directories("assets/material_presets");
            
            std::string filename = "assets/material_presets/" + name + ".preset";
            
            // Check if file already exists
            if (std::filesystem::exists(filename)) {
                std::cout << "Preset file '" << filename << "' already exists." << std::endl;
                std::cout << "Overwrite? (y/n): ";
                
                std::string response;
                std::cin >> response;
                
                if (response != "y" && response != "Y") {
                    std::cout << "Save cancelled." << std::endl;
                    return false;
                }
            }
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cout << "Error: Could not create preset file '" << filename << "'" << std::endl;
                return false;
            }
            
            // Write preset file with educational metadata
            file << "# Cook-Torrance Material Preset\n";
            file << "# Name: " << name << "\n";
            file << "# Generated by Interactive Material Editor\n";
            file << "# Format: base_color_r base_color_g base_color_b roughness metallic specular\n";
            file << "\n";
            
            file << std::fixed << std::setprecision(6);
            file << material.base_color.x << " " << material.base_color.y << " " << material.base_color.z << " ";
            file << material.roughness << " " << material.metallic << " " << material.specular << "\n";
            
            file << "\n# Material Characteristics:\n";
            file << "# Type: " << (material.metallic > 0.5f ? "Conductor (Metal)" : "Dielectric (Non-metal)") << "\n";
            file << "# Surface: " << (material.roughness < 0.2f ? "Glossy" : 
                                     material.roughness > 0.7f ? "Rough" : "Semi-glossy") << "\n";
            
            file.close();
            
            std::cout << "Material preset saved successfully: " << filename << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "Error saving preset: " << e.what() << std::endl;
            return false;
        }
    }

    // Load material preset from file
    static bool load_material_preset(const std::string& filename, MaterialPreset& preset) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cout << "Error: Could not open preset file '" << filename << "'" << std::endl;
                return false;
            }
            
            std::string line;
            Vector3 base_color;
            float roughness, metallic, specular;
            bool found_data = false;
            
            while (std::getline(file, line)) {
                // Skip comments and empty lines
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                // Parse parameter line
                std::istringstream iss(line);
                if (iss >> base_color.x >> base_color.y >> base_color.z >> roughness >> metallic >> specular) {
                    found_data = true;
                    break;
                }
            }
            
            file.close();
            
            if (!found_data) {
                std::cout << "Error: Invalid preset file format in '" << filename << "'" << std::endl;
                return false;
            }
            
            // Extract name from filename
            std::string preset_name = std::filesystem::path(filename).stem().string();
            
            preset = MaterialPreset(preset_name, "Custom preset loaded from file", 
                                   base_color, roughness, metallic, specular);
            
            std::cout << "Preset loaded successfully from: " << filename << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "Error loading preset: " << e.what() << std::endl;
            return false;
        }
    }

private:
    // Preset creation functions with realistic material properties
    
    static MaterialPreset create_polished_gold_preset() {
        return MaterialPreset(
            "Polished Gold",
            "Shiny gold metal with low roughness and warm yellow coloration",
            Vector3(1.0f, 0.766f, 0.336f), // Gold's characteristic color
            0.05f, // Very smooth
            1.0f,  // Pure metal
            0.04f  // Standard dielectric fallback
        );
    }
    
    static MaterialPreset create_brushed_aluminum_preset() {
        return MaterialPreset(
            "Brushed Aluminum", 
            "Matte aluminum with moderate roughness and neutral gray coloration",
            Vector3(0.913f, 0.921f, 0.925f), // Aluminum's characteristic color
            0.4f,  // Moderately rough
            1.0f,  // Pure metal
            0.04f  // Standard dielectric fallback
        );
    }
    
    static MaterialPreset create_oxidized_copper_preset() {
        return MaterialPreset(
            "Oxidized Copper",
            "Weathered copper with high roughness and reddish-brown patina",
            Vector3(0.7f, 0.27f, 0.08f), // Oxidized copper color
            0.8f,  // Very rough
            0.9f,  // Mostly metallic with some dielectric character
            0.04f  // Standard dielectric fallback
        );
    }
    
    static MaterialPreset create_rough_iron_preset() {
        return MaterialPreset(
            "Rough Iron",
            "Unpolished iron with high roughness and dark metallic appearance",
            Vector3(0.562f, 0.565f, 0.578f), // Iron's characteristic color
            0.9f,  // Very rough
            1.0f,  // Pure metal
            0.04f  // Standard dielectric fallback
        );
    }
    
    static MaterialPreset create_glossy_plastic_preset() {
        return MaterialPreset(
            "Glossy Plastic",
            "Shiny plastic with low roughness and neutral dielectric properties",
            Vector3(0.8f, 0.2f, 0.2f), // Red plastic color
            0.1f,  // Very smooth
            0.0f,  // Pure dielectric
            0.04f  // Typical plastic F0
        );
    }
    
    static MaterialPreset create_matte_ceramic_preset() {
        return MaterialPreset(
            "Matte Ceramic",
            "Unglazed ceramic with high roughness and low reflectance",
            Vector3(0.9f, 0.85f, 0.7f), // Cream ceramic color
            0.9f,  // Very rough
            0.0f,  // Pure dielectric
            0.04f  // Typical ceramic F0
        );
    }
    
    static MaterialPreset create_clear_glass_preset() {
        return MaterialPreset(
            "Clear Glass",
            "Transparent glass with low roughness and higher dielectric reflectance",
            Vector3(0.95f, 0.95f, 0.95f), // Nearly white for clear glass
            0.02f, // Very smooth
            0.0f,  // Pure dielectric
            0.08f  // Higher F0 for glass (IOR ≈ 1.5)
        );
    }
    
    static MaterialPreset create_painted_wood_preset() {
        return MaterialPreset(
            "Painted Wood",
            "Semi-gloss painted wood surface with moderate roughness",
            Vector3(0.4f, 0.6f, 0.3f), // Green painted wood
            0.5f,  // Semi-glossy
            0.0f,  // Pure dielectric
            0.04f  // Typical paint F0
        );
    }
    
    // Educational explanation of preset characteristics
    static void explain_preset_characteristics(const MaterialPreset& preset) {
        std::cout << "\n=== Preset Characteristics Explanation ===" << std::endl;
        
        // Material type analysis
        if (preset.metallic > 0.8f) {
            std::cout << "Material Type: Pure Conductor (Metal)" << std::endl;
            std::cout << "• High reflectance across visible spectrum" << std::endl;
            std::cout << "• Specular reflections match base color" << std::endl;
            std::cout << "• No subsurface scattering (opaque)" << std::endl;
        } else if (preset.metallic > 0.3f) {
            std::cout << "Material Type: Mixed Conductor/Dielectric" << std::endl;
            std::cout << "• Blended metallic and dielectric properties" << std::endl;
            std::cout << "• Partially colored specular reflections" << std::endl;
            std::cout << "• Useful for weathered or oxidized metals" << std::endl;
        } else {
            std::cout << "Material Type: Pure Dielectric (Non-metal)" << std::endl;
            std::cout << "• Low reflectance at normal incidence" << std::endl;
            std::cout << "• Achromatic (white/gray) specular reflections" << std::endl;
            std::cout << "• Base color affects diffuse/transmitted light" << std::endl;
        }
        
        // Surface characteristics
        if (preset.roughness < 0.2f) {
            std::cout << "\nSurface Character: Glossy/Mirror-like" << std::endl;
            std::cout << "• Sharp, concentrated specular highlights" << std::endl;
            std::cout << "• Clear environmental reflections" << std::endl;
        } else if (preset.roughness > 0.7f) {
            std::cout << "\nSurface Character: Rough/Matte" << std::endl;
            std::cout << "• Broad, soft specular highlights" << std::endl;
            std::cout << "• Diffused environmental reflections" << std::endl;
        } else {
            std::cout << "\nSurface Character: Semi-glossy" << std::endl;
            std::cout << "• Moderate specular highlight spread" << std::endl;
            std::cout << "• Balanced reflection characteristics" << std::endl;
        }
        
        // Expected visual outcome
        std::cout << "\nExpected Visual Result:" << std::endl;
        if (preset.metallic > 0.5f && preset.roughness < 0.3f) {
            std::cout << "• Mirror-like metallic reflections" << std::endl;
        } else if (preset.metallic > 0.5f && preset.roughness > 0.7f) {
            std::cout << "• Matte metallic appearance with soft highlights" << std::endl;
        } else if (preset.metallic < 0.3f && preset.roughness < 0.3f) {
            std::cout << "• Glossy plastic/ceramic appearance" << std::endl;
        } else {
            std::cout << "• Matte dielectric appearance with minimal highlights" << std::endl;
        }
    }
};