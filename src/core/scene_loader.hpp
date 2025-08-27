#pragma once
#include "scene.hpp"
#include "sphere.hpp"
#include "../materials/lambert.hpp"
#include "../materials/cook_torrance.hpp"
#include "../materials/material_base.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <memory>

// SceneLoader handles parsing of simple scene files for educational multi-primitive scenes
// File format: Simple key-value pairs with educational transparency and error handling
// Educational focus: demonstrates scene data management and file I/O integration
class SceneLoader {
public:
    // Load scene from file with comprehensive error handling and educational output
    // Returns: Complete Scene object with primitives and materials loaded from file
    static Scene load_from_file(const std::string& filename, const std::string& material_type = "lambert") {
        std::cout << "\n=== Loading Scene from File ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "ERROR: Cannot open scene file: " << filename << std::endl;
            return Scene(); // Return empty scene
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        std::cout << "File loaded successfully, size: " << content.size() << " bytes" << std::endl;
        return load_from_string(content, material_type);
    }
    
    // Parse scene from string content with educational debugging output
    // Algorithm: line-by-line parsing with material and sphere registration
    static Scene load_from_string(const std::string& content, const std::string& material_type = "lambert") {
        std::cout << "\n=== Parsing Scene Content ===" << std::endl;
        
        Scene scene;
        std::map<std::string, int> material_name_to_index;
        
        std::istringstream stream(content);
        std::string line;
        int line_number = 0;
        int materials_loaded = 0;
        int spheres_loaded = 0;
        
        while (std::getline(stream, line)) {
            line_number++;
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            std::cout << "Processing line " << line_number << ": " << line << std::endl;
            
            std::istringstream line_stream(line);
            std::string command;
            line_stream >> command;
            
            if (command == "material") {
                // Legacy Lambert material format for backward compatibility
                if (parse_lambert_material_legacy(line_stream, scene, material_name_to_index)) {
                    materials_loaded++;
                } else {
                    std::cout << "WARNING: Failed to parse legacy material on line " << line_number << std::endl;
                }
            }
            else if (command == "material_lambert") {
                // Explicit Lambert material format  
                if (parse_lambert_material(line_stream, scene, material_name_to_index)) {
                    materials_loaded++;
                } else {
                    std::cout << "WARNING: Failed to parse Lambert material on line " << line_number << std::endl;
                }
            }
            else if (command == "material_cook_torrance") {
                // Cook-Torrance material format
                if (parse_cook_torrance_material(line_stream, scene, material_name_to_index)) {
                    materials_loaded++;
                } else {
                    std::cout << "WARNING: Failed to parse Cook-Torrance material on line " << line_number << std::endl;
                }
            }
            else if (command == "sphere") {
                if (parse_sphere(line_stream, scene, material_name_to_index)) {
                    spheres_loaded++;
                } else {
                    std::cout << "WARNING: Failed to parse sphere on line " << line_number << std::endl;
                }
            }
            else if (command == "scene_name" || command == "description") {
                // Skip metadata for now
                std::cout << "Metadata: " << command << std::endl;
            }
            else {
                std::cout << "Unknown command: " << command << " on line " << line_number << std::endl;
            }
        }
        
        std::cout << "\n=== Scene Loading Summary ===" << std::endl;
        std::cout << "Lines processed: " << line_number << std::endl;
        std::cout << "Materials loaded: " << materials_loaded << std::endl;
        std::cout << "Spheres loaded: " << spheres_loaded << std::endl;
        std::cout << "=== Scene loading complete ===" << std::endl;
        
        return scene;
    }
    
private:
    // Parse legacy Lambert material for backward compatibility
    // Format: material name red green blue
    static bool parse_lambert_material_legacy(std::istringstream& stream, Scene& scene, 
                                             std::map<std::string, int>& material_map) {
        std::string name;
        float r, g, b;
        
        if (!(stream >> name >> r >> g >> b)) {
            std::cout << "ERROR: Invalid legacy material format. Expected: material name r g b" << std::endl;
            return false;
        }
        
        std::cout << "Parsing legacy Lambert material: " << name << " (" << r << ", " << g << ", " << b << ")" << std::endl;
        
        // Create Lambert material with validation
        auto material = std::make_unique<LambertMaterial>(Vector3(r, g, b));
        
        // Validate parameters and show educational output
        if (!material->validate_parameters()) {
            std::cout << "WARNING: Material parameters outside valid range, clamping" << std::endl;
            material->clamp_to_valid_ranges();
            std::cout << "Clamped color: (" << material->base_color.x << ", " 
                      << material->base_color.y << ", " << material->base_color.z << ")" << std::endl;
        }
        
        int material_index = scene.add_material(std::move(material));
        material_map[name] = material_index;
        
        std::cout << "Legacy Lambert material '" << name << "' registered at index " << material_index << std::endl;
        return true;
    }
    
    // Parse explicit Lambert material 
    // Format: material_lambert name red green blue
    static bool parse_lambert_material(std::istringstream& stream, Scene& scene, 
                                      std::map<std::string, int>& material_map) {
        std::string name;
        float r, g, b;
        
        if (!(stream >> name >> r >> g >> b)) {
            std::cout << "ERROR: Invalid Lambert material format. Expected: material_lambert name r g b" << std::endl;
            std::cout << "Example: material_lambert matte_red 0.8 0.3 0.3" << std::endl;
            return false;
        }
        
        std::cout << "Parsing Lambert material: " << name << " (" << r << ", " << g << ", " << b << ")" << std::endl;
        
        // Create Lambert material with validation
        auto material = std::make_unique<LambertMaterial>(Vector3(r, g, b));
        
        // Educational parameter validation
        if (!material->validate_parameters()) {
            std::cout << "WARNING: Lambert material parameters outside [0,1] range" << std::endl;
            std::cout << "Educational note: Albedo > 1.0 would violate energy conservation (amplify light)" << std::endl;
            material->clamp_to_valid_ranges();
            std::cout << "Clamped albedo: (" << material->base_color.x << ", " 
                      << material->base_color.y << ", " << material->base_color.z << ")" << std::endl;
        }
        
        int material_index = scene.add_material(std::move(material));
        material_map[name] = material_index;
        
        std::cout << "Lambert material '" << name << "' registered at index " << material_index 
                  << " (Type: Lambert (Diffuse))" << std::endl;
        return true;
    }
    
    // Parse Cook-Torrance material
    // Format: material_cook_torrance name base_r base_g base_b roughness metallic specular  
    static bool parse_cook_torrance_material(std::istringstream& stream, Scene& scene,
                                            std::map<std::string, int>& material_map) {
        std::string name;
        float r, g, b, roughness, metallic, specular;
        
        if (!(stream >> name >> r >> g >> b >> roughness >> metallic >> specular)) {
            std::cout << "ERROR: Invalid Cook-Torrance material format" << std::endl;
            std::cout << "Expected: material_cook_torrance name r g b roughness metallic specular" << std::endl;
            std::cout << "Example: material_cook_torrance gold 1.0 0.8 0.3 0.1 1.0 0.04" << std::endl;
            std::cout << "Parameters:" << std::endl;
            std::cout << "  roughness: [0.01, 1.0] (0.01=mirror, 1.0=diffuse)" << std::endl;
            std::cout << "  metallic: [0.0, 1.0] (0.0=dielectric, 1.0=conductor)" << std::endl;
            std::cout << "  specular: [0.0, 1.0] (typical dielectric F0: 0.04)" << std::endl;
            return false;
        }
        
        std::cout << "Parsing Cook-Torrance material: " << name << std::endl;
        std::cout << "  Base color: (" << r << ", " << g << ", " << b << ")" << std::endl;
        std::cout << "  Roughness: " << roughness << ", Metallic: " << metallic << ", Specular: " << specular << std::endl;
        
        // Create Cook-Torrance material with validation
        auto material = std::make_unique<CookTorranceMaterial>(Vector3(r, g, b), roughness, metallic, specular, true);
        
        // Educational parameter validation with detailed output
        if (!material->validate_parameters()) {
            std::cout << "WARNING: Cook-Torrance material parameters outside valid ranges" << std::endl;
            std::cout << "Educational notes:" << std::endl;
            std::cout << "  - Roughness < 0.01 causes numerical instability in GGX distribution" << std::endl;
            std::cout << "  - Roughness > 1.0 represents unphysical surface behavior" << std::endl;
            std::cout << "  - Metallic outside [0,1] breaks PBR material model assumptions" << std::endl;
            std::cout << "  - Color values > 1.0 would violate energy conservation" << std::endl;
            material->clamp_to_valid_ranges();
            std::cout << "Parameters clamped to valid ranges for physical accuracy" << std::endl;
        }
        
        // Educational material analysis
        std::cout << "Material analysis:" << std::endl;
        if (material->metallic > 0.5f) {
            std::cout << "  Type: Conductor (Metal) - high reflectance with colored F0" << std::endl;
        } else {
            std::cout << "  Type: Dielectric (Non-metal) - low F0, transmissive at normal incidence" << std::endl;
        }
        
        if (material->roughness < 0.2f) {
            std::cout << "  Surface: Glossy/Mirror-like - sharp, concentrated reflections" << std::endl;
        } else if (material->roughness > 0.7f) {
            std::cout << "  Surface: Rough/Diffuse-like - broad, scattered reflections" << std::endl;
        } else {
            std::cout << "  Surface: Semi-glossy - moderate reflection spreading" << std::endl;
        }
        
        int material_index = scene.add_material(std::move(material));
        material_map[name] = material_index;
        
        std::cout << "Cook-Torrance material '" << name << "' registered at index " << material_index 
                  << " (Type: Microfacet BRDF)" << std::endl;
        return true;
    }
    
    // Parse sphere definition with material name resolution
    // Format: sphere center_x center_y center_z radius material_name
    static bool parse_sphere(std::istringstream& stream, Scene& scene,
                            const std::map<std::string, int>& material_map) {
        float x, y, z, radius;
        std::string material_name;
        
        if (!(stream >> x >> y >> z >> radius >> material_name)) {
            std::cout << "ERROR: Invalid sphere format. Expected: sphere x y z radius material_name" << std::endl;
            return false;
        }
        
        std::cout << "Parsing sphere: center(" << x << ", " << y << ", " << z 
                 << "), radius=" << radius << ", material=" << material_name << std::endl;
        
        // Validate sphere parameters
        if (radius <= 0.0f) {
            std::cout << "ERROR: Invalid sphere radius " << radius << " (must be > 0)" << std::endl;
            return false;
        }
        
        if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) || !std::isfinite(radius)) {
            std::cout << "ERROR: Non-finite sphere parameters" << std::endl;
            return false;
        }
        
        // Look up material index
        auto material_it = material_map.find(material_name);
        if (material_it == material_map.end()) {
            std::cout << "ERROR: Unknown material '" << material_name << "'" << std::endl;
            std::cout << "Available materials:";
            for (const auto& pair : material_map) {
                std::cout << " " << pair.first;
            }
            std::cout << std::endl;
            return false;
        }
        
        int material_index = material_it->second;
        
        // Create sphere and add to scene
        Sphere sphere(Point3(x, y, z), radius, material_index);
        int sphere_index = scene.add_sphere(sphere);
        
        if (sphere_index >= 0) {
            std::cout << "Sphere added at index " << sphere_index 
                     << " with material index " << material_index << std::endl;
            return true;
        } else {
            std::cout << "ERROR: Failed to add sphere to scene" << std::endl;
            return false;
        }
    }
};