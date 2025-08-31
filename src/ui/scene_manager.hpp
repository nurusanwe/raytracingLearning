#pragma once
#include "../core/scene.hpp"
#include "../core/scene_loader.hpp"
#include "../core/performance_timer.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

// Interactive Scene File Loading and Management System
// Provides command-line interface for runtime scene switching and file validation
// Educational focus: understanding scene composition and lighting setups

class SceneManager {
public:
    // Main interactive scene loading interface
    // Returns true if scene was loaded successfully, false otherwise
    // Parameters:
    //   scene - reference to Scene object to populate
    //   current_scene_name - reference to string tracking current scene filename
    static bool load_scene_interactive(Scene& scene, std::string& current_scene_name) {
        std::cout << "\n=== Interactive Scene Loading ===\n" << std::endl;
        
        display_available_scenes();
        
        std::string selected_file;
        if (prompt_for_scene_selection(selected_file)) {
            return load_scene_with_feedback(selected_file, scene, current_scene_name);
        }
        
        std::cout << "Scene loading cancelled." << std::endl;
        return false;
    }

    // Display list of available scene files with descriptions
    static void display_available_scenes() {
        std::cout << "=== Available Scene Files ===" << std::endl;
        
        std::vector<std::string> scene_files = get_scene_files("../assets");
        
        if (scene_files.empty()) {
            std::cout << "No scene files found in assets/ directory." << std::endl;
            std::cout << "Expected scene file extension: .scene" << std::endl;
            return;
        }
        
        std::cout << "\nFound " << scene_files.size() << " scene file(s):" << std::endl;
        for (size_t i = 0; i < scene_files.size(); ++i) {
            std::cout << (i + 1) << ". " << scene_files[i] << std::endl;
            explain_scene_content(scene_files[i]);
        }
        
        std::cout << "\nScene File Format Information:" << std::endl;
        std::cout << "• Multi-material support: Lambert and Cook-Torrance materials" << std::endl;
        std::cout << "• Multi-light support: Point, directional, and area lights" << std::endl;
        std::cout << "• Primitive support: Spheres with material assignment" << std::endl;
        std::cout << "• Educational comments: Inline explanations of scene setup" << std::endl;
    }

    // Browse scene files in specified directory
    static bool browse_scene_files(const std::string& directory) {
        try {
            if (!std::filesystem::exists(directory)) {
                std::cout << "Directory '" << directory << "' does not exist." << std::endl;
                return false;
            }
            
            std::vector<std::string> files = get_scene_files(directory);
            if (files.empty()) {
                std::cout << "No .scene files found in '" << directory << "'." << std::endl;
                return false;
            }
            
            std::cout << "\nScene files in '" << directory << "':" << std::endl;
            for (const auto& file : files) {
                std::cout << "• " << file << std::endl;
            }
            
            return true;
            
        } catch (const std::filesystem::filesystem_error& e) {
            std::cout << "Error accessing directory '" << directory << "': " << e.what() << std::endl;
            return false;
        }
    }

    // Validate scene file format and content
    static bool validate_scene_file(const std::string& filename) {
        try {
            // Basic file existence check
            if (!std::filesystem::exists(filename)) {
                std::cout << "Scene file '" << filename << "' not found." << std::endl;
                return false;
            }
            
            // File extension check
            if (filename.substr(filename.find_last_of('.')) != ".scene") {
                std::cout << "Warning: File '" << filename << "' does not have .scene extension." << std::endl;
            }
            
            // Try to load scene for validation
            std::cout << "\nValidating scene file: " << filename << std::endl;
            
            Scene test_scene = SceneLoader::load_from_file(filename);
            
            if (!test_scene.primitives.empty()) {
                std::cout << "✓ Scene file format is valid" << std::endl;
                std::cout << "✓ Found " << test_scene.materials.size() << " material(s)" << std::endl;
                std::cout << "✓ Found " << test_scene.primitives.size() << " primitive(s)" << std::endl;
                std::cout << "✓ Found " << test_scene.lights.size() << " light(s)" << std::endl;
                return true;
            } else {
                std::cout << "✗ Scene file validation failed" << std::endl;
                std::cout << "✗ Check file format and syntax" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error validating scene file: " << e.what() << std::endl;
            return false;
        }
    }

private:
    // Get list of .scene files from specified directory
    static std::vector<std::string> get_scene_files(const std::string& directory) {
        std::vector<std::string> scene_files;
        
        try {
            if (!std::filesystem::exists(directory)) {
                return scene_files; // Return empty vector
            }
            
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && 
                    entry.path().extension() == ".scene") {
                    scene_files.push_back(entry.path().string());
                }
            }
            
            // Sort files alphabetically for consistent display
            std::sort(scene_files.begin(), scene_files.end());
            
        } catch (const std::filesystem::filesystem_error& e) {
            std::cout << "Warning: Could not read directory '" << directory << "': " << e.what() << std::endl;
        }
        
        return scene_files;
    }

    // Provide educational explanation of scene content
    static void explain_scene_content(const std::string& filename) {
        // Extract base filename for pattern matching
        std::string base_name = std::filesystem::path(filename).stem().string();
        
        std::cout << "   Description: ";
        
        // Pattern-based scene description
        if (base_name.find("simple") != std::string::npos) {
            std::cout << "Basic scene for learning fundamentals (single primitive, basic lighting)";
        } else if (base_name.find("showcase") != std::string::npos) {
            std::cout << "Comprehensive demonstration scene (multiple materials and lighting)";
        } else if (base_name.find("cook_torrance") != std::string::npos || base_name.find("cook-torrance") != std::string::npos) {
            std::cout << "Cook-Torrance material demonstration (microfacet theory examples)";
        } else if (base_name.find("multi") != std::string::npos) {
            std::cout << "Multi-primitive scene (complex geometry interactions)";
        } else if (base_name.find("light") != std::string::npos) {
            std::cout << "Lighting-focused scene (multiple light types and shadows)";
        } else {
            std::cout << "Custom scene (check file for specific content)";
        }
        
        std::cout << std::endl;
    }

    // Prompt user to select scene file from available options
    static bool prompt_for_scene_selection(std::string& selected_filename) {
        std::vector<std::string> scene_files = get_scene_files("../assets");
        
        if (scene_files.empty()) {
            return false;
        }
        
        std::cout << "\nEnter scene selection:" << std::endl;
        std::cout << "• Number (1-" << scene_files.size() << ") to select from list above" << std::endl;
        std::cout << "• Full path to custom scene file" << std::endl;
        std::cout << "• 'c' to cancel" << std::endl;
        std::cout << "Selection: ";
        
        std::string input;
        std::cin >> input;
        
        if (input == "c" || input == "C") {
            return false;
        }
        
        // Try to parse as number first
        try {
            int selection = std::stoi(input);
            if (selection >= 1 && selection <= static_cast<int>(scene_files.size())) {
                selected_filename = scene_files[selection - 1];
                std::cout << "Selected: " << selected_filename << std::endl;
                return true;
            } else {
                std::cout << "Invalid selection number. Range: 1-" << scene_files.size() << std::endl;
                return false;
            }
        } catch (const std::exception&) {
            // Not a number, treat as filename
            selected_filename = input;
            std::cout << "Using custom path: " << selected_filename << std::endl;
            return true;
        }
    }

    // Load scene with comprehensive feedback and timing
    static bool load_scene_with_feedback(const std::string& filename, Scene& scene, std::string& current_scene_name) {
        std::cout << "\n=== Scene Loading Process ===" << std::endl;
        
        // Validate file before attempting to load
        if (!validate_scene_file(filename)) {
            std::cout << "Scene loading aborted due to validation failure." << std::endl;
            return false;
        }
        
        // Load scene with performance timing
        auto load_start = std::chrono::steady_clock::now();
        
        std::cout << "\nAttempting to load scene: " << filename << std::endl;
        
        Scene new_scene = SceneLoader::load_from_file(filename);
        
        if (!new_scene.primitives.empty()) {
            // Loading successful - provide detailed feedback
            std::cout << "\n=== Scene Loading Success ===" << std::endl;
            
            // Educational scene composition analysis
            std::cout << "Scene composition analysis:" << std::endl;
            std::cout << "• Materials loaded: " << new_scene.materials.size() << std::endl;
            std::cout << "• Primitives loaded: " << new_scene.primitives.size() << std::endl;
            std::cout << "• Lights loaded: " << new_scene.lights.size() << std::endl;
            
            // Material type breakdown
            size_t lambert_count = 0;
            size_t cook_torrance_count = 0;
            
            for (const auto& material : new_scene.materials) {
                if (material->type == MaterialType::Lambert) {
                    lambert_count++;
                } else if (material->type == MaterialType::CookTorrance) {
                    cook_torrance_count++;
                }
            }
            
            if (lambert_count > 0 || cook_torrance_count > 0) {
                std::cout << "Material type breakdown:" << std::endl;
                if (lambert_count > 0) {
                    std::cout << "  - Lambert materials: " << lambert_count << " (diffuse shading)" << std::endl;
                }
                if (cook_torrance_count > 0) {
                    std::cout << "  - Cook-Torrance materials: " << cook_torrance_count << " (microfacet shading)" << std::endl;
                }
            }
            
            // Light type breakdown
            if (new_scene.lights.size() > 0) {
                std::cout << "Lighting setup:" << std::endl;
                // Note: Detailed light type breakdown would require light type identification
                std::cout << "  - Total lights: " << new_scene.lights.size() << " (mixed types supported)" << std::endl;
                std::cout << "  - Shadow rays: Enabled for all light types" << std::endl;
                std::cout << "  - Multi-light accumulation: Active" << std::endl;
            }
            
            // Calculate and report loading time
            auto load_end = std::chrono::steady_clock::now();
            auto load_duration = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);
            std::cout << "\nScene loading time: " << load_duration.count() << " ms" << std::endl;
            
            // Replace current scene with loaded scene
            scene = std::move(new_scene);
            current_scene_name = std::filesystem::path(filename).stem().string();
            
            std::cout << "\nScene successfully loaded and activated!" << std::endl;
            std::cout << "Current scene: " << current_scene_name << std::endl;
            
            return true;
            
        } else {
            std::cout << "\n=== Scene Loading Failed ===" << std::endl;
            std::cout << "Could not load scene file: " << filename << std::endl;
            std::cout << "Common issues:" << std::endl;
            std::cout << "• File format errors (check syntax)" << std::endl;
            std::cout << "• Missing material definitions" << std::endl;
            std::cout << "• Invalid primitive parameters" << std::endl;
            std::cout << "• Malformed light definitions" << std::endl;
            std::cout << "\nCurrent scene unchanged." << std::endl;
            
            return false;
        }
    }
};