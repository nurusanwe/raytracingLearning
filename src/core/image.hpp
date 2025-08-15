#pragma once
#include "vector3.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

// STB Image Write implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Image class manages pixel data for ray traced output with educational color management
// Provides complete color processing pipeline from linear RGB to display-ready output
//
// Color Management Pipeline:
// 1. Linear RGB values from ray tracing calculations
// 2. Color clamping to [0.0, 1.0] range for display compatibility
// 3. Gamma correction for accurate sRGB display representation
// 4. Format conversion for image output (8-bit PNG, etc.)
//
// Educational Features:
// - Detailed color statistics and analysis
// - Mathematical transparency in color transformations
// - Visual debugging through console output
//
// Technical Implementation:
// - Uses Vector3 for RGB color consistency with mathematical framework
// - Supports arbitrary image resolutions
// - Provides both linear and gamma-corrected color access
// - Includes validation and error checking for robust operation
class Image {
public:
    int width, height;                    // Image dimensions in pixels
    std::vector<Vector3> pixels;          // RGB data as Vector3 for mathematical consistency
    
    // Constructor: Creates image with specified dimensions
    // All pixels initialized to black (0,0,0) for consistent starting state
    Image(int w, int h) : width(w), height(h), pixels(w * h, Vector3(0, 0, 0)) {
        if (w <= 0 || h <= 0) {
            width = height = 1;  // Default to 1x1 for safety
            pixels.resize(1, Vector3(0, 0, 0));
        }
    }
    
    // Pixel access with bounds checking - mutable version
    // Returns reference to allow direct pixel modification
    Vector3& pixel(int x, int y) {
        // Bounds checking with clamping for safety
        if (x < 0 || x >= width || y < 0 || y >= height) {
            static Vector3 error_pixel(1.0f, 0.0f, 1.0f);  // Magenta for error visualization
            return error_pixel;
        }
        return pixels[y * width + x];  // Row-major order storage
    }
    
    // Pixel access with bounds checking - const version
    // Returns const reference for read-only access
    const Vector3& pixel(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            static const Vector3 error_pixel(1.0f, 0.0f, 1.0f);  // Magenta for error visualization
            return error_pixel;
        }
        return pixels[y * width + x];
    }
    
    // Color clamping for display compatibility
    // Clamps linear RGB values to [0.0, 1.0] range for standard display
    // Mathematical operation: clamp(x, 0, 1) = max(0, min(x, 1))
    Vector3 clamp_color(const Vector3& color) const {
        return Vector3(
            std::max(0.0f, std::min(color.x, 1.0f)),  // Red channel clamping
            std::max(0.0f, std::min(color.y, 1.0f)),  // Green channel clamping
            std::max(0.0f, std::min(color.z, 1.0f))   // Blue channel clamping
        );
    }
    
    // Gamma correction for accurate sRGB display
    // Converts linear RGB to gamma-corrected sRGB for standard monitors
    // Mathematical formula: sRGB = linear^(1/gamma) where gamma ≈ 2.2
    //
    // Physics background:
    // - Linear RGB represents physical light intensity
    // - Human vision and CRT monitors have non-linear response
    // - sRGB standard uses gamma ≈ 2.2 for perceptual uniformity
    // - Gamma correction compensates for display characteristics
    //
    // Educational note: Without gamma correction, images appear too dark
    // because monitors assume gamma-corrected input
    Vector3 gamma_correct(const Vector3& linear_color, float gamma = 2.2f) const {
        if (gamma <= 0.0f) {
            return linear_color;  // Invalid gamma, return unchanged
        }
        
        float inv_gamma = 1.0f / gamma;  // Compute 1/gamma once for efficiency
        
        // Apply gamma correction: sRGB = linear^(1/gamma)
        return Vector3(
            std::pow(std::max(0.0f, linear_color.x), inv_gamma),  // Red channel
            std::pow(std::max(0.0f, linear_color.y), inv_gamma),  // Green channel
            std::pow(std::max(0.0f, linear_color.z), inv_gamma)   // Blue channel
        );
    }
    
    // Set pixel with automatic color management
    // Applies clamping and stores in linear RGB space
    // Gamma correction applied during output/display
    void set_pixel(int x, int y, const Vector3& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = clamp_color(color);
        }
    }
    
    // Add color to existing pixel (for accumulation/sampling)
    // Useful for multi-sampling and progressive rendering
    void add_pixel(int x, int y, const Vector3& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] += color;  // Accumulate linear values
        }
    }
    
    // Clear image to specified color (default black)
    void clear(const Vector3& clear_color = Vector3(0, 0, 0)) {
        std::fill(pixels.begin(), pixels.end(), clear_color);
    }
    
    // Image statistics and analysis for educational insight
    void print_image_statistics() const {
        if (pixels.empty()) {
            std::cout << "Empty image - no statistics available" << std::endl;
            return;
        }
        
        std::cout << "\n=== Image Statistics ===" << std::endl;
        std::cout << "Resolution: " << width << " × " << height << " pixels" << std::endl;
        std::cout << "Total pixels: " << (width * height) << std::endl;
        
        // Calculate luminance and color statistics
        Vector3 min_color(1e6f, 1e6f, 1e6f);
        Vector3 max_color(-1e6f, -1e6f, -1e6f);
        Vector3 avg_color(0, 0, 0);
        
        float min_luminance = 1e6f;
        float max_luminance = -1e6f;
        float avg_luminance = 0.0f;
        
        int non_black_pixels = 0;
        
        for (const auto& pixel : pixels) {
            // Update color statistics
            min_color.x = std::min(min_color.x, pixel.x);
            min_color.y = std::min(min_color.y, pixel.y);
            min_color.z = std::min(min_color.z, pixel.z);
            
            max_color.x = std::max(max_color.x, pixel.x);
            max_color.y = std::max(max_color.y, pixel.y);
            max_color.z = std::max(max_color.z, pixel.z);
            
            avg_color += pixel;
            
            // Calculate luminance using CIE 1931 standard: Y = 0.299R + 0.587G + 0.114B
            float luminance = 0.299f * pixel.x + 0.587f * pixel.y + 0.114f * pixel.z;
            min_luminance = std::min(min_luminance, luminance);
            max_luminance = std::max(max_luminance, luminance);
            avg_luminance += luminance;
            
            // Count non-black pixels
            if (pixel.x > 1e-6f || pixel.y > 1e-6f || pixel.z > 1e-6f) {
                non_black_pixels++;
            }
        }
        
        // Calculate averages
        int total_pixels = width * height;
        avg_color *= (1.0f / total_pixels);
        avg_luminance /= total_pixels;
        
        // Display statistics
        std::cout << "Color Range:" << std::endl;
        std::cout << "  Minimum RGB: (" << min_color.x << ", " << min_color.y << ", " << min_color.z << ")" << std::endl;
        std::cout << "  Maximum RGB: (" << max_color.x << ", " << max_color.y << ", " << max_color.z << ")" << std::endl;
        std::cout << "  Average RGB: (" << avg_color.x << ", " << avg_color.y << ", " << avg_color.z << ")" << std::endl;
        
        std::cout << "Luminance Analysis:" << std::endl;
        std::cout << "  Minimum luminance: " << min_luminance << std::endl;
        std::cout << "  Maximum luminance: " << max_luminance << std::endl;
        std::cout << "  Average luminance: " << avg_luminance << std::endl;
        
        std::cout << "Content Analysis:" << std::endl;
        std::cout << "  Non-black pixels: " << non_black_pixels << " (" << 
                     (100.0f * non_black_pixels / total_pixels) << "%)" << std::endl;
        std::cout << "  Black pixels: " << (total_pixels - non_black_pixels) << " (" << 
                     (100.0f * (total_pixels - non_black_pixels) / total_pixels) << "%)" << std::endl;
        
        // Gamma correction example for one pixel
        if (max_luminance > 0.01f) {
            Vector3 example_linear = max_color;
            Vector3 example_gamma = gamma_correct(example_linear);
            std::cout << "Gamma Correction Example:" << std::endl;
            std::cout << "  Linear RGB (brightest): (" << example_linear.x << ", " << example_linear.y << ", " << example_linear.z << ")" << std::endl;
            std::cout << "  Gamma-corrected (γ=2.2): (" << example_gamma.x << ", " << example_gamma.y << ", " << example_gamma.z << ")" << std::endl;
        }
        
        std::cout << "=== End Image Statistics ===" << std::endl;
    }
    
    // Convert to 8-bit RGB values for PNG output
    // Returns vector of bytes in RGB format (3 bytes per pixel)
    std::vector<unsigned char> to_8bit_rgb(bool apply_gamma_correction = true) const {
        std::vector<unsigned char> rgb_data;
        rgb_data.reserve(width * height * 3);  // 3 channels per pixel
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Vector3 linear_color = pixel(x, y);
                
                // Apply gamma correction if requested
                Vector3 display_color = apply_gamma_correction ? 
                    gamma_correct(clamp_color(linear_color)) : 
                    clamp_color(linear_color);
                
                // Convert to 8-bit range [0, 255]
                unsigned char r = (unsigned char)(display_color.x * 255.0f + 0.5f);
                unsigned char g = (unsigned char)(display_color.y * 255.0f + 0.5f);
                unsigned char b = (unsigned char)(display_color.z * 255.0f + 0.5f);
                
                rgb_data.push_back(r);
                rgb_data.push_back(g);
                rgb_data.push_back(b);
            }
        }
        
        return rgb_data;
    }
    
    // Save image to PNG file with proper color management
    // filename: Output PNG file path (e.g., "output.png")
    // apply_gamma_correction: Whether to apply gamma correction (default: true)
    // Returns: true if save successful, false otherwise
    bool save_to_png(const std::string& filename, bool apply_gamma_correction = true) const {
        if (!validate_image()) {
            std::cout << "ERROR: Cannot save invalid image to PNG" << std::endl;
            return false;
        }
        
        std::cout << "\n=== PNG Output Generation ===" << std::endl;
        std::cout << "Saving image to: " << filename << std::endl;
        std::cout << "Resolution: " << width << " × " << height << " pixels" << std::endl;
        std::cout << "Gamma correction: " << (apply_gamma_correction ? "enabled" : "disabled") << std::endl;
        
        // Convert to 8-bit RGB data
        std::vector<unsigned char> rgb_data = to_8bit_rgb(apply_gamma_correction);
        
        // Write PNG using stb_image_write
        // Parameters: filename, width, height, components (3=RGB), data, stride_bytes (0=automatic)
        int result = stbi_write_png(filename.c_str(), width, height, 3, rgb_data.data(), width * 3);
        
        if (result != 0) {
            std::cout << "✓ PNG file saved successfully: " << filename << std::endl;
            std::cout << "✓ File size: " << rgb_data.size() << " bytes (raw RGB data)" << std::endl;
            std::cout << "✓ Color channels: RGB (3 components per pixel)" << std::endl;
            
            // Educational file format information
            std::cout << "\nPNG Technical Details:" << std::endl;
            std::cout << "  - Format: PNG (Portable Network Graphics)" << std::endl;
            std::cout << "  - Color depth: 8-bit per channel (24-bit total)" << std::endl;
            std::cout << "  - Color space: sRGB" << (apply_gamma_correction ? " (gamma corrected)" : " (linear)") << std::endl;
            std::cout << "  - Compression: lossless PNG compression" << std::endl;
            std::cout << "  - Pixel layout: row-major order, RGB interleaved" << std::endl;
            
            return true;
        } else {
            std::cout << "✗ ERROR: Failed to save PNG file: " << filename << std::endl;
            std::cout << "✗ Check file path permissions and disk space" << std::endl;
            return false;
        }
    }
    
    // Validate image consistency and detect issues
    bool validate_image() const {
        // Check dimensions
        if (width <= 0 || height <= 0) {
            return false;
        }
        
        // Check pixel array size
        if (pixels.size() != static_cast<size_t>(width * height)) {
            return false;
        }
        
        // Check for invalid pixel values (NaN, infinity)
        for (const auto& pixel : pixels) {
            if (!pixel.is_finite()) {
                return false;
            }
        }
        
        return true;
    }
    
    // Educational method: explain color management principles
    void explain_color_management() const {
        std::cout << "\n=== Color Management in Ray Tracing ===" << std::endl;
        std::cout << "Linear RGB Space:" << std::endl;
        std::cout << "  - Ray tracing calculations use linear RGB values" << std::endl;
        std::cout << "  - Linear space: doubling light intensity doubles RGB values" << std::endl;
        std::cout << "  - Mathematical operations (addition, multiplication) work correctly" << std::endl;
        std::cout << "  - Example: (0.5, 0.5, 0.5) represents half intensity of (1.0, 1.0, 1.0)" << std::endl;
        
        std::cout << "\nGamma Correction:" << std::endl;
        std::cout << "  - Converts linear RGB to display RGB for accurate viewing" << std::endl;
        std::cout << "  - Formula: display_value = linear_value^(1/2.2)" << std::endl;
        std::cout << "  - Compensates for non-linear monitor response" << std::endl;
        std::cout << "  - Without gamma correction, images appear too dark" << std::endl;
        
        std::cout << "\nColor Clamping:" << std::endl;
        std::cout << "  - Limits RGB values to [0.0, 1.0] range for display compatibility" << std::endl;
        std::cout << "  - Values > 1.0 represent 'over-exposed' light (clipped to white)" << std::endl;
        std::cout << "  - Values < 0.0 are unphysical (clipped to black)" << std::endl;
        std::cout << "  - Preserves color ratios while ensuring valid display values" << std::endl;
        
        std::cout << "\nDisplay Pipeline:" << std::endl;
        std::cout << "  1. Ray tracing → Linear RGB values" << std::endl;
        std::cout << "  2. Color clamping → [0.0, 1.0] range" << std::endl;
        std::cout << "  3. Gamma correction → sRGB space" << std::endl;
        std::cout << "  4. Quantization → 8-bit values [0, 255]" << std::endl;
        std::cout << "  5. Image format → PNG, JPEG, etc." << std::endl;
        std::cout << "=== End Color Management Explanation ===" << std::endl;
    }
};