#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "../core/image.hpp"
#include "../core/logger.hpp"

// ArgumentParser encapsulates all command-line argument parsing logic
// Extracted from main.cpp to improve testability and maintainability
class ArgumentParser {
public:
    // Parsed configuration
    struct Config {
        // Resolution settings
        Resolution resolution = Resolution::parse_from_string("1024x768");

        // Scene settings
        std::string scene_filename = "../assets/showcase_scene.scene";
        bool use_scene_file = true;
        bool scene_explicitly_set = false;

        // Material settings
        std::string material_type = "lambert";
        float roughness = 0.5f;
        float metallic = 0.0f;
        float specular = 0.04f;

        // Mode settings
        bool interactive_mode = false;
        VerbosityLevel verbosity = VerbosityLevel::Verbose;

        // Parse result
        bool show_help = false;
        bool has_error = false;
        std::string error_message;
    };

    // Parse command-line arguments
    // Returns false if parsing failed or help was requested
    static bool parse(int argc, char* argv[], Config& config) {
        // Check for help request first
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
                config.show_help = true;
                return false;
            }
        }

        // Parse all arguments
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--scene") == 0 && i + 1 < argc) {
                config.scene_filename = argv[i + 1];
                config.use_scene_file = true;
                config.scene_explicitly_set = true;
                std::cout << "Scene file override: " << config.scene_filename << std::endl;
                i++;
            }
            else if (std::strcmp(argv[i], "--no-scene") == 0) {
                config.use_scene_file = false;
                config.scene_explicitly_set = true;
                std::cout << "Scene loading disabled - using hardcoded sphere" << std::endl;
            }
            else if (std::strcmp(argv[i], "--resolution") == 0 && i + 1 < argc) {
                if (!parse_resolution(argv[i + 1], config)) {
                    return false;
                }
                i++;
            }
            else if (std::strcmp(argv[i], "--preset") == 0 && i + 1 < argc) {
                if (!apply_preset(argv[i + 1], config)) {
                    return false;
                }
                i++;
            }
            else if (std::strcmp(argv[i], "--showcase") == 0) {
                apply_preset("showcase", config);
            }
            else if (std::strcmp(argv[i], "--cook-torrance") == 0) {
                apply_preset("cook-torrance", config);
            }
            else if (std::strcmp(argv[i], "--performance") == 0) {
                apply_preset("performance", config);
            }
            else if (std::strcmp(argv[i], "--quality") == 0) {
                apply_preset("quality", config);
            }
            else if (std::strcmp(argv[i], "--material") == 0 && i + 1 < argc) {
                if (!parse_material(argv[i + 1], config)) {
                    return false;
                }
                i++;
            }
            else if (std::strcmp(argv[i], "--roughness") == 0 && i + 1 < argc) {
                config.roughness = parse_float_clamped(argv[i + 1], 0.01f, 1.0f);
                std::cout << "Roughness override: " << config.roughness << std::endl;
                i++;
            }
            else if (std::strcmp(argv[i], "--metallic") == 0 && i + 1 < argc) {
                config.metallic = parse_float_clamped(argv[i + 1], 0.0f, 1.0f);
                std::cout << "Metallic override: " << config.metallic << std::endl;
                i++;
            }
            else if (std::strcmp(argv[i], "--specular") == 0 && i + 1 < argc) {
                config.specular = parse_float_clamped(argv[i + 1], 0.0f, 1.0f);
                std::cout << "Specular override: " << config.specular << std::endl;
                i++;
            }
            else if (std::strcmp(argv[i], "--quiet") == 0) {
                config.verbosity = VerbosityLevel::Normal;
                std::cout << "Quiet mode enabled - minimal output" << std::endl;
            }
            else if (std::strcmp(argv[i], "--verbose") == 0) {
                config.verbosity = VerbosityLevel::Verbose;
                std::cout << "Verbose mode enabled - full educational output" << std::endl;
            }
            else if (std::strcmp(argv[i], "--interactive") == 0) {
                config.interactive_mode = true;
                config.material_type = "cook-torrance";
                config.verbosity = VerbosityLevel::Normal;
                if (!config.scene_explicitly_set) {
                    config.use_scene_file = false;
                }
                std::cout << "Interactive mode enabled - Cook-Torrance material editor (quiet mode default)" << std::endl;
            }
            else if (strncmp(argv[i], "--", 2) == 0) {
                // Check if it's a known camera argument (handled by Camera class)
                if (is_camera_argument(argv[i])) {
                    if (i + 1 < argc) i++;  // Skip parameter if it exists
                } else {
                    // Unknown argument
                    config.has_error = true;
                    config.error_message = std::string("Unknown argument '") + argv[i] + "'";
                    print_suggestion(argv[i]);
                    return false;
                }
            }
        }

        return true;
    }

    // Print help text
    static void print_help() {
        std::cout << "\n=== Educational Ray Tracer - Command Line Help ===" << std::endl;
        std::cout << "\nResolution parameters:" << std::endl;
        std::cout << "--resolution <WxH>    Set image resolution (e.g., --resolution 1024x768)" << std::endl;
        std::cout << "                      Common presets: 256x256, 512x512, 1024x1024, 2048x2048" << std::endl;
        std::cout << "                      Default: 1024x768 (Epic 2 Showcase)" << std::endl;

        std::cout << "\nScene parameters:" << std::endl;
        std::cout << "--scene <filename>    Load scene from file (default: assets/showcase_scene.scene)" << std::endl;
        std::cout << "--no-scene            Use hardcoded single sphere for compatibility" << std::endl;

        std::cout << "\nMaterial parameters (Cook-Torrance BRDF):" << std::endl;
        std::cout << "--material <type>     Material type: lambert, cook-torrance (default: lambert)" << std::endl;
        std::cout << "--roughness <value>   Surface roughness for Cook-Torrance (0.0-1.0, default: 0.5)" << std::endl;
        std::cout << "--metallic <value>    Metallic parameter for Cook-Torrance (0.0-1.0, default: 0.0)" << std::endl;
        std::cout << "--specular <value>    Specular reflectance for dielectrics (0.0-1.0, default: 0.04)" << std::endl;

        std::cout << "\nDebug and verbosity parameters:" << std::endl;
        std::cout << "--quiet               Minimal output (no educational breakdowns, errors only)" << std::endl;
        std::cout << "--verbose             Full educational output (default behavior)" << std::endl;

        std::cout << "\nInteractive mode:" << std::endl;
        std::cout << "--interactive         Enable interactive Cook-Torrance material editor" << std::endl;
        std::cout << "                      Supports real-time parameter adjustment, scene loading" << std::endl;
        std::cout << "                      Material presets, and educational feedback" << std::endl;

        std::cout << "\nQuick presets:" << std::endl;
        std::cout << "--preset showcase     Epic 2 showcase (1024x768, complex scene, optimal camera)" << std::endl;
        std::cout << "--showcase            Shorthand for --preset showcase" << std::endl;
        std::cout << "--preset cook-torrance Cook-Torrance demo (1024x1024, single sphere)" << std::endl;
        std::cout << "--cook-torrance       Shorthand for --preset cook-torrance" << std::endl;
        std::cout << "--preset performance  Fast render (512x512, simple scene, basic camera)" << std::endl;
        std::cout << "--performance         Shorthand for --preset performance" << std::endl;
        std::cout << "--preset quality      High quality (1024x1024, showcase scene, wide FOV)" << std::endl;
        std::cout << "--quality             Shorthand for --preset quality" << std::endl;

        std::cout << "\nCamera parameters:" << std::endl;
        std::cout << "--camera-pos x,y,z    Set camera position (default: 0,0,1)" << std::endl;
        std::cout << "--camera-target x,y,z Set camera target (default: 0,0,-6)" << std::endl;
        std::cout << "--fov degrees         Set field of view (default: 60)" << std::endl;

        std::cout << "\nScene file format (Lambert materials only):" << std::endl;
        std::cout << "material <name> <r> <g> <b>  - Define material with RGB albedo" << std::endl;
        std::cout << "sphere <x> <y> <z> <radius> <material>  - Add sphere to scene" << std::endl;

        std::cout << "\nNOTE: Scene files only support Lambert materials. For Cook-Torrance materials," << std::endl;
        std::cout << "      use --cook-torrance preset (single sphere) or --no-scene with --material cook-torrance" << std::endl;
    }

private:
    // Parse resolution string
    static bool parse_resolution(const char* str, Config& config) {
        try {
            config.resolution = Resolution::parse_from_string(str);
            std::cout << "Resolution override: " << config.resolution.width << "x" << config.resolution.height << std::endl;
            config.resolution.print_memory_analysis();
            return true;
        } catch (const std::exception& e) {
            config.has_error = true;
            config.error_message = std::string("Invalid resolution format '") + str + "'";
            std::cout << "ERROR: " << config.error_message << std::endl;
            std::cout << "Error details: " << e.what() << std::endl;
            std::cout << "Supported formats: WIDTHxHEIGHT (e.g., 512x512)" << std::endl;
            std::cout << "Common presets: 256x256, 512x512, 1024x1024, 2048x2048" << std::endl;
            return false;
        }
    }

    // Apply a preset configuration
    static bool apply_preset(const std::string& preset, Config& config) {
        std::cout << "Using preset: " << preset << std::endl;

        if (preset == "showcase") {
            config.scene_filename = "../assets/showcase_scene.scene";
            config.resolution = Resolution::parse_from_string("1024x768");
            std::cout << "Epic 2 Showcase preset: 1024x768, complex scene, optimal camera" << std::endl;
        }
        else if (preset == "cook-torrance") {
            config.use_scene_file = false;
            config.resolution = Resolution::parse_from_string("1024x1024");
            config.material_type = "cook-torrance";
            config.verbosity = VerbosityLevel::Verbose;
            std::cout << "Cook-Torrance Demo preset: 1024x1024, single sphere with Cook-Torrance material" << std::endl;
        }
        else if (preset == "performance") {
            config.resolution = Resolution::MEDIUM;
            config.scene_filename = "../assets/simple_scene.scene";
            std::cout << "Performance preset: 512x512, simple scene, fast render" << std::endl;
        }
        else if (preset == "quality") {
            config.resolution = Resolution::LARGE;
            config.scene_filename = "../assets/showcase_scene.scene";
            std::cout << "Quality preset: 1024x1024, showcase scene, maximum quality" << std::endl;
        }
        else {
            config.has_error = true;
            config.error_message = std::string("Unknown preset '") + preset + "'";
            std::cout << "ERROR: " << config.error_message << std::endl;
            std::cout << "Available presets: showcase, cook-torrance, performance, quality" << std::endl;
            return false;
        }
        return true;
    }

    // Parse and validate material type
    static bool parse_material(const char* str, Config& config) {
        config.material_type = str;
        std::cout << "Material type override: " << config.material_type << std::endl;

        if (config.material_type != "lambert" && config.material_type != "cook-torrance") {
            config.has_error = true;
            config.error_message = std::string("Unknown material type '") + str + "'";
            std::cout << "ERROR: " << config.error_message << std::endl;
            std::cout << "Supported materials: lambert, cook-torrance" << std::endl;
            return false;
        }
        return true;
    }

    // Parse float and clamp to range
    static float parse_float_clamped(const char* str, float min_val, float max_val) {
        float value = std::stof(str);
        return std::max(min_val, std::min(max_val, value));
    }

    // Check if argument is a camera-related argument
    static bool is_camera_argument(const char* arg) {
        return std::strcmp(arg, "--camera-pos") == 0 ||
               std::strcmp(arg, "--camera-target") == 0 ||
               std::strcmp(arg, "--fov") == 0;
    }

    // Print suggestion for unknown argument
    static void print_suggestion(const char* arg) {
        std::cout << "ERROR: Unknown argument '" << arg << "'" << std::endl;
        std::cout << "Did you mean:" << std::endl;

        std::string arg_str = arg;

        if (arg_str.find("quality") != std::string::npos) {
            std::cout << "  --preset quality     (High quality preset)" << std::endl;
            std::cout << "  --quality            (Shorthand for --preset quality)" << std::endl;
        } else if (arg_str.find("performance") != std::string::npos) {
            std::cout << "  --preset performance (Fast render preset)" << std::endl;
            std::cout << "  --performance        (Shorthand for --preset performance)" << std::endl;
        } else if (arg_str.find("showcase") != std::string::npos) {
            std::cout << "  --preset showcase    (Epic 2 showcase preset)" << std::endl;
            std::cout << "  --showcase           (Shorthand for --preset showcase)" << std::endl;
        } else if (arg_str.find("resolution") != std::string::npos || arg_str.find("res") != std::string::npos) {
            std::cout << "  --resolution WxH     (e.g., --resolution 1024x768)" << std::endl;
        } else if (arg_str.find("scene") != std::string::npos) {
            std::cout << "  --scene <filename>   (Load custom scene file)" << std::endl;
            std::cout << "  --no-scene           (Use hardcoded sphere)" << std::endl;
        } else if (arg_str.find("camera") != std::string::npos || arg_str.find("pos") != std::string::npos) {
            std::cout << "  --camera-pos x,y,z   (Set camera position)" << std::endl;
            std::cout << "  --camera-target x,y,z(Set camera target)" << std::endl;
        } else if (arg_str.find("fov") != std::string::npos) {
            std::cout << "  --fov degrees        (Set field of view)" << std::endl;
        } else {
            std::cout << "  --help               (Show all available options)" << std::endl;
            std::cout << "  --preset showcase    (Epic 2 showcase)" << std::endl;
            std::cout << "  --resolution WxH     (Set custom resolution)" << std::endl;
        }

        std::cout << "\nUse --help to see all available options." << std::endl;
    }
};
