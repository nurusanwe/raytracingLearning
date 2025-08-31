#pragma once

#include <string>
#include <cstdlib>
#include <iostream>

class ImageViewer {
public:
    static void open_image(const std::string& image_path) {
        std::string command;
        
#ifdef _WIN32
        command = "start \"\" \"" + image_path + "\"";
#elif __APPLE__
        command = "open \"" + image_path + "\"";
#else
        command = "xdg-open \"" + image_path + "\"";
#endif
        
        int result = std::system(command.c_str());
        if (result == 0) {
            std::cout << "✓ Image opened: " << image_path << std::endl;
        } else {
            std::cout << "⚠ Could not automatically open image. Please check: " << image_path << std::endl;
        }
    }
    
    static void open_image_async(const std::string& image_path) {
        std::string command;
        
#ifdef _WIN32
        command = "start \"\" \"" + image_path + "\" >nul 2>&1";
#elif __APPLE__
        command = "open \"" + image_path + "\" >/dev/null 2>&1 &";
#else
        command = "xdg-open \"" + image_path + "\" >/dev/null 2>&1 &";
#endif
        
        int result = std::system(command.c_str());
        if (result == 0) {
            std::cout << "✓ Image opened in background: " << image_path << std::endl;
        }
    }
};