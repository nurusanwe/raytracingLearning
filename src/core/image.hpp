#pragma once
#include "vector3.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// STB Image Write implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Resolution management structure for multi-resolution rendering support
struct Resolution {
    int width, height;
    std::string name;
    
    Resolution(int w, int h, const std::string& n = "") 
        : width(w), height(h), name(n) {}
    
    float aspect_ratio() const { return (float)width / height; }
    size_t pixel_count() const { return width * height; }
    size_t memory_estimate_bytes() const { return pixel_count() * sizeof(Vector3); }
    
    // Common presets for educational ray tracing
    static const Resolution SMALL;    // 256x256
    static const Resolution MEDIUM;   // 512x512  
    static const Resolution LARGE;    // 1024x1024
    static const Resolution XLARGE;   // 2048x2048
    
    static Resolution parse_from_string(const std::string& resolution_str);
    static std::vector<Resolution> get_common_presets();
    
    // Educational memory analysis
    void print_memory_analysis() const {
        float mb = memory_estimate_bytes() / (1024.0f * 1024.0f);
        std::cout << "Resolution " << width << "x" << height << " (" << name << "):" << std::endl;
        std::cout << "  Pixels: " << pixel_count() << std::endl;
        std::cout << "  Memory: " << mb << " MB" << std::endl;
        std::cout << "  Aspect ratio: " << aspect_ratio() << ":1" << std::endl;
        
        if (mb > 100.0f) {
            std::cout << "  WARNING: Large memory usage! Consider smaller resolution for educational use." << std::endl;
        }
    }
};

// Define common resolution presets
const Resolution Resolution::SMALL(256, 256, "Small");
const Resolution Resolution::MEDIUM(512, 512, "Medium");
const Resolution Resolution::LARGE(1024, 1024, "Large");
const Resolution Resolution::XLARGE(2048, 2048, "X-Large");

// Parse resolution from command line string (e.g., "1024x768")
Resolution Resolution::parse_from_string(const std::string& resolution_str) {
    // Find the 'x' separator
    size_t x_pos = resolution_str.find('x');
    if (x_pos == std::string::npos) {
        x_pos = resolution_str.find('X');  // Also accept uppercase X
    }
    
    if (x_pos == std::string::npos) {
        throw std::invalid_argument("Resolution format must be WIDTHxHEIGHT (e.g., 512x512)");
    }
    
    try {
        std::string width_str = resolution_str.substr(0, x_pos);
        std::string height_str = resolution_str.substr(x_pos + 1);
        
        int width = std::stoi(width_str);
        int height = std::stoi(height_str);
        
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Resolution dimensions must be positive integers");
        }
        
        if (width > 4096 || height > 4096) {
            throw std::invalid_argument("Resolution too large (maximum 4096x4096 for educational use)");
        }
        
        return Resolution(width, height, "Custom");
    } catch (const std::invalid_argument& e) {
        throw std::invalid_argument("Invalid resolution format: " + std::string(e.what()));
    }
}

// Get list of common preset resolutions
std::vector<Resolution> Resolution::get_common_presets() {
    return {Resolution::SMALL, Resolution::MEDIUM, Resolution::LARGE, Resolution::XLARGE};
}

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
    float aspect_ratio;                   // Computed aspect ratio (width/height)
    std::vector<Vector3> pixels;          // RGB data as Vector3 for mathematical consistency
    
    // Constructor: Creates image with specified dimensions
    // All pixels initialized to black (0,0,0) for consistent starting state
    Image(int w, int h) : width(w), height(h), pixels(w * h, Vector3(0, 0, 0)) {
        if (w <= 0 || h <= 0) {
            width = height = 1;  // Default to 1x1 for safety
            pixels.resize(1, Vector3(0, 0, 0));
        }
        aspect_ratio = static_cast<float>(width) / height;
        print_memory_statistics();
    }
    
    // Constructor from Resolution struct
    Image(const Resolution& resolution) : Image(resolution.width, resolution.height) {
        std::cout << "\n=== Image Created from Resolution ===" << std::endl;
        resolution.print_memory_analysis();
        explain_memory_layout();
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
    
    // Memory and performance monitoring methods
    size_t memory_usage_bytes() const {
        return pixels.size() * sizeof(Vector3);
    }
    
    void print_memory_statistics() const {
        size_t bytes = memory_usage_bytes();
        float mb = bytes / (1024.0f * 1024.0f);
        
        std::cout << "\n=== Image Memory Statistics ===" << std::endl;
        std::cout << "Resolution: " << width << "x" << height << " pixels" << std::endl;
        std::cout << "Total pixels: " << pixels.size() << std::endl;
        std::cout << "Memory usage: " << bytes << " bytes (" << mb << " MB)" << std::endl;
        std::cout << "Bytes per pixel: " << sizeof(Vector3) << " (Vector3: 3 × float)" << std::endl;
        
        if (mb > 100.0f) {
            std::cout << "WARNING: Large memory allocation detected!" << std::endl;
            std::cout << "Educational note: Memory scales quadratically with resolution." << std::endl;
            std::cout << "Doubling width AND height quadruples memory usage." << std::endl;
        }
        
        std::cout << "=== End Memory Statistics ===" << std::endl;
    }
    
    void explain_memory_layout() const {
        std::cout << "\n=== Educational Memory Layout Explanation ===" << std::endl;
        std::cout << "Memory Layout: Row-major order" << std::endl;
        std::cout << "  - Pixels stored left-to-right, then top-to-bottom" << std::endl;
        std::cout << "  - Index calculation: pixel[y * width + x]" << std::endl;
        std::cout << "  - Cache-friendly for scanline rendering" << std::endl;
        
        std::cout << "Memory Scaling:" << std::endl;
        std::cout << "  - Linear with pixel count: O(width × height)" << std::endl;
        std::cout << "  - Quadratic with resolution: doubling dimensions = 4× memory" << std::endl;
        std::cout << "  - Example scaling from 512x512 to 1024x1024:" << std::endl;
        std::cout << "    512×512 = 262,144 pixels = 3.0 MB" << std::endl;
        std::cout << "   1024×1024 = 1,048,576 pixels = 12.0 MB (4× increase)" << std::endl;
        
        std::cout << "Memory Efficiency:" << std::endl;
        std::cout << "  - Vector3 per pixel: 12 bytes (3 × 4-byte float)" << std::endl;
        std::cout << "  - No padding or alignment overhead" << std::endl;
        std::cout << "  - Direct mathematical operations on pixel data" << std::endl;
        std::cout << "=== End Memory Layout Explanation ===" << std::endl;
    }
    
    void print_resolution_statistics() const {
        std::cout << "\n=== Resolution Statistics ===" << std::endl;
        std::cout << "Dimensions: " << width << " × " << height << " pixels" << std::endl;
        std::cout << "Aspect ratio: " << aspect_ratio << ":1" << std::endl;
        
        // Classify aspect ratio
        if (std::abs(aspect_ratio - 1.0f) < 0.01f) {
            std::cout << "Format: Square (1:1)" << std::endl;
        } else if (std::abs(aspect_ratio - 4.0f/3.0f) < 0.01f) {
            std::cout << "Format: Classic TV (4:3)" << std::endl;
        } else if (std::abs(aspect_ratio - 16.0f/9.0f) < 0.01f) {
            std::cout << "Format: Widescreen (16:9)" << std::endl;
        } else {
            std::cout << "Format: Custom aspect ratio" << std::endl;
        }
        
        std::cout << "Pixel count: " << (width * height) << std::endl;
        std::cout << "Memory estimate: " << (memory_usage_bytes() / (1024.0f * 1024.0f)) << " MB" << std::endl;
        
        // Memory warnings based on resolution
        check_resolution_memory_warnings();
        
        std::cout << "=== End Resolution Statistics ===" << std::endl;
    }
    
    // Resolution-specific memory warnings for educational guidance
    void check_resolution_memory_warnings() const {
        size_t bytes = memory_usage_bytes();
        float mb = bytes / (1024.0f * 1024.0f);
        int pixel_count = width * height;
        
        std::cout << "\nResolution Memory Analysis:" << std::endl;
        
        // Educational warnings based on resolution
        if (pixel_count > 4000000) { // > 2048x2048
            std::cout << "⚠️  WARNING: Very high resolution detected!" << std::endl;
            std::cout << "  - Resolution: " << width << "x" << height << " = " << pixel_count << " pixels" << std::endl;
            std::cout << "  - Memory usage: " << mb << " MB" << std::endl;
            std::cout << "  - Educational guidance: Consider smaller resolutions for learning" << std::endl;
            std::cout << "  - High resolution may cause slow rendering and high memory usage" << std::endl;
        } else if (pixel_count > 1000000) { // > 1024x1024  
            std::cout << "🔶 NOTICE: High resolution" << std::endl;
            std::cout << "  - Resolution: " << width << "x" << height << " = " << pixel_count << " pixels" << std::endl;
            std::cout << "  - Memory usage: " << mb << " MB" << std::endl;
            std::cout << "  - Educational note: Good for detailed results, but will take longer to render" << std::endl;
        } else if (pixel_count > 250000) { // > 512x512
            std::cout << "✅ Moderate resolution - good balance for educational use" << std::endl;
            std::cout << "  - Resolution: " << width << "x" << height << " = " << pixel_count << " pixels" << std::endl;
            std::cout << "  - Memory usage: " << mb << " MB" << std::endl;
        } else {
            std::cout << "✅ Low resolution - optimal for fast educational experiments" << std::endl;
            std::cout << "  - Resolution: " << width << "x" << height << " = " << pixel_count << " pixels" << std::endl;
            std::cout << "  - Memory usage: " << mb << " MB" << std::endl;
        }
        
        // Demonstrate quadratic scaling
        std::cout << "\nQuadratic Scaling Demonstration:" << std::endl;
        if (width == height) { // Square resolution
            int double_res = width * 2;
            size_t double_pixels = double_res * double_res;
            float double_mb = (double_pixels * sizeof(Vector3)) / (1024.0f * 1024.0f);
            
            std::cout << "  Current (" << width << "x" << height << "): " 
                      << pixel_count << " pixels, " << mb << " MB" << std::endl;
            std::cout << "  Doubled (" << double_res << "x" << double_res << "): " 
                      << double_pixels << " pixels, " << double_mb << " MB" << std::endl;
            std::cout << "  Scaling factor: " << (double_pixels / (float)pixel_count) << "× pixels, " 
                      << (double_mb / mb) << "× memory" << std::endl;
            std::cout << "  Educational insight: Doubling dimensions quadruples memory usage!" << std::endl;
        }
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