#pragma once
#include "scene.hpp"
#include "sphere.hpp"
#include "../materials/lambert.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iostream>

// SceneLoader handles parsing of simple scene files for educational multi-primitive scenes
// File format: Simple key-value pairs with educational transparency and error handling
// Educational focus: demonstrates scene data management and file I/O integration
class SceneLoader {
public:
    // Load scene from file with comprehensive error handling and educational output
    // Returns: Complete Scene object with primitives and materials loaded from file
    static Scene load_from_file(const std::string& filename) {
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
        return load_from_string(content);
    }
    
    // Parse scene from string content with educational debugging output
    // Algorithm: line-by-line parsing with material and sphere registration
    static Scene load_from_string(const std::string& content) {
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
                if (parse_material(line_stream, scene, material_name_to_index)) {
                    materials_loaded++;
                } else {
                    std::cout << "WARNING: Failed to parse material on line " << line_number << std::endl;
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
    // Parse material definition with error handling and validation
    // Format: material name red green blue
    static bool parse_material(std::istringstream& stream, Scene& scene, 
                              std::map<std::string, int>& material_map) {
        std::string name;
        float r, g, b;
        
        if (!(stream >> name >> r >> g >> b)) {
            std::cout << "ERROR: Invalid material format. Expected: material name r g b" << std::endl;
            return false;
        }
        
        std::cout << "Parsing material: " << name << " (" << r << ", " << g << ", " << b << ")" << std::endl;
        
        // Validate color values
        if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f) {
            std::cout << "WARNING: Color values outside [0,1] range, clamping" << std::endl;
            r = std::max(0.0f, std::min(1.0f, r));
            g = std::max(0.0f, std::min(1.0f, g));
            b = std::max(0.0f, std::min(1.0f, b));
        }
        
        // Create material and add to scene
        LambertMaterial material(Vector3(r, g, b));
        int material_index = scene.add_material(material);
        material_map[name] = material_index;
        
        std::cout << "Material '" << name << "' registered at index " << material_index << std::endl;
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