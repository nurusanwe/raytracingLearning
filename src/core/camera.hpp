#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include "ray.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

// Camera class implements coordinate transformation from screen space to world space rays
// Provides educational camera system for ray tracing with mathematical transparency
//
// Mathematical Foundation:
// 1. Screen space coordinates: pixel (x,y) in range [0, width-1] × [0, height-1]
// 2. Normalized Device Coordinates (NDC): [-1, +1] × [+1, -1] (Y flipped for image coordinates)
// 3. Camera space coordinates: aspect ratio and field-of-view scaling applied
// 4. World space ray direction: camera coordinate system transformation
//
// Camera Coordinate System:
//   - Camera looks from position toward target
//   - Up vector defines camera roll orientation
//   - Right = forward × up (right-hand rule)
//   - Camera_up = right × forward (orthogonal basis)
//
// ASCII Art Diagram - Camera Coordinate System:
//
//        camera_up
//           ↑
//           |
//    target •←------ camera_position
//          /|      /
//         / |     /
//        /  |    / camera_right
//       ↓   |   ↗
//   forward  |  /
//           | /
//           |/
//        world_up
//
// Field of View Geometry:
//
//    Screen plane at distance d from camera
//         +-------+  ← image_height
//         |       |
//         |   •   |  ← center pixel
//         |       |
//         +-------+  
//           ↑
//       image_width
//         
//    Top edge distance = d * tan(fov/2)
//    NDC scaling factor = tan(fov/2)
//

// Matrix4x4 class for look-at matrix calculations (inline implementation)
class Matrix4x4 {
public:
    float m[4][4];  // Row-major 4x4 matrix
    
    Matrix4x4() {
        // Initialize to identity matrix
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    // Static factory method for look-at matrix creation
    static Matrix4x4 look_at(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 forward = (target - eye).normalize();
        Vector3 right = forward.cross(up).normalize();
        Vector3 camera_up = right.cross(forward);
        
        Matrix4x4 result;
        
        // Build look-at matrix following standard computer graphics convention
        result.m[0][0] = right.x;    result.m[0][1] = camera_up.x;  result.m[0][2] = -forward.x;  result.m[0][3] = 0.0f;
        result.m[1][0] = right.y;    result.m[1][1] = camera_up.y;  result.m[1][2] = -forward.y;  result.m[1][3] = 0.0f;
        result.m[2][0] = right.z;    result.m[2][1] = camera_up.z;  result.m[2][2] = -forward.z;  result.m[2][3] = 0.0f;
        result.m[3][0] = -right.dot(Vector3(eye.x, eye.y, eye.z));  
        result.m[3][1] = -camera_up.dot(Vector3(eye.x, eye.y, eye.z));  
        result.m[3][2] = forward.dot(Vector3(eye.x, eye.y, eye.z));  
        result.m[3][3] = 1.0f;
        
        return result;
    }
    
    // Educational method to explain matrix construction
    void explain_look_at_construction() const {
        std::cout << "=== Look-At Matrix Construction ===" << std::endl;
        std::cout << "Matrix layout (row-major):" << std::endl;
        for(int i = 0; i < 4; i++) {
            std::cout << "  [" << std::fixed << std::setprecision(3) 
                     << m[i][0] << " " << m[i][1] << " " << m[i][2] << " " << m[i][3] << "]" << std::endl;
        }
        std::cout << "First 3 columns: camera basis vectors (right, up, -forward)" << std::endl;
        std::cout << "Last column: translation (camera position projected onto basis)" << std::endl;
    }
};
class Camera {
public:
    // Core camera parameters
    Point3 position;              // Camera position in world space
    Point3 target;               // Point camera is looking at
    Vector3 up;                  // Up vector for camera orientation (world space)
    float field_of_view_degrees; // Vertical field of view in degrees
    float aspect_ratio;          // Width/height ratio of image
    
    // Derived camera properties (calculated from above)
    Vector3 forward;              // Camera forward direction (normalized)
    Vector3 right;                // Camera right direction (normalized)  
    Vector3 camera_up;            // Camera up direction (normalized)
    float focal_length;           // 35mm equivalent focal length
    
    // Constructor with explicit camera parameters
    // position: Camera location in world space
    // target: Point camera is looking toward
    // up_vector: World space up direction (typically (0,1,0))
    // fov_degrees: Vertical field of view in degrees (typically 45-90)
    // aspect: Image width/height ratio (e.g., 16:9 = 1.778)
    Camera(const Point3& pos, const Point3& tgt, const Vector3& up_vec, 
           float fov_degrees, float aspect = 1.0f)
        : position(pos), target(tgt), up(up_vec), 
          field_of_view_degrees(fov_degrees), aspect_ratio(aspect) {
        
        // Clamp parameters to safe ranges
        clamp_to_safe_ranges();
        
        // Calculate derived camera properties
        calculate_camera_basis_vectors();
        
        // Calculate focal length
        focal_length = fov_to_focal_length(field_of_view_degrees);
        
        // Educational output about aspect ratio setup
        std::cout << "\n=== Camera Aspect Ratio Setup ===" << std::endl;
        std::cout << "Aspect ratio: " << aspect_ratio << ":1" << std::endl;
        explain_aspect_ratio_effects();
    }
    
    // Generate world space ray for given pixel coordinates
    // pixel_x, pixel_y: Pixel coordinates in range [0, image_width-1] × [0, image_height-1]
    // image_width, image_height: Image resolution in pixels
    // Returns: Ray from camera position through specified pixel
    //
    // Mathematical Process:
    // 1. Convert pixel coordinates to Normalized Device Coordinates (NDC)
    // 2. Apply field of view and aspect ratio scaling to get camera space coordinates
    // 3. Transform camera space direction to world space using camera basis vectors
    // 4. Create ray with camera position as origin and world space direction
    Ray generate_ray(float pixel_x, float pixel_y, int image_width, int image_height) const {
        // Step 1: Convert pixel coordinates to Normalized Device Coordinates [-1, +1]
        // NDC space: center of screen is (0,0), edges are ±1
        // Note: Y coordinate is flipped for image coordinates (Y increases downward)
        float ndc_x = (2.0f * pixel_x / image_width) - 1.0f;   // Map [0, width-1] → [-1, +1]
        float ndc_y = 1.0f - (2.0f * pixel_y / image_height);  // Map [0, height-1] → [+1, -1] (flip Y)
        
        // Step 2: Apply field of view scaling
        // Convert vertical field of view from degrees to radians
        float fov_radians = field_of_view_degrees * M_PI / 180.0f;
        float fov_scale = std::tan(fov_radians * 0.5f);  // Half-angle tangent for scaling
        
        // Apply FOV and aspect ratio to get camera space coordinates
        // Camera space: center at origin, scaled by FOV and aspect ratio
        float camera_x = ndc_x * aspect_ratio * fov_scale;  // Apply aspect ratio to horizontal
        float camera_y = ndc_y * fov_scale;                 // Apply FOV scaling to vertical
        float camera_z = 1.0f;                              // Screen plane at unit distance (positive Z points forward in camera space)
        
        // Step 3: Use pre-calculated camera coordinate system basis vectors
        // (These are computed during construction and kept up-to-date)
        
        // Step 4: Transform camera space direction to world space
        // Camera space direction vector (from origin to screen plane point)
        Vector3 camera_direction = Vector3(camera_x, camera_y, camera_z);
        
        // Transform to world space using pre-calculated camera basis vectors
        // world_direction = camera_x * right + camera_y * camera_up + camera_z * forward
        Vector3 world_direction = (camera_x * right) + (camera_y * camera_up) + (camera_z * forward);
        
        // Normalize direction vector for consistent ray parameterization
        world_direction = world_direction.normalize();
        
        // Step 5: Create ray with camera position and world space direction
        return Ray(position, world_direction);
    }
    
    // Camera coordinate system calculations
    void calculate_camera_basis_vectors() {
        // Calculate forward vector (from position to target)
        forward = (target - position).normalize();
        
        // Calculate right vector (cross product of forward and world up)
        right = forward.cross(up).normalize();
        
        // Calculate camera up vector (orthogonal to forward and right)
        camera_up = right.cross(forward);
        
        // Educational console output
        std::cout << "=== Camera Coordinate System ===" << std::endl;
        std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Target:   (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
        std::cout << "Forward:  (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "Right:    (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "Up:       (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
    }
    
    Matrix4x4 calculate_look_at_matrix() const {
        // Educational mathematical explanation printed to console
        std::cout << "Look-at matrix calculation:" << std::endl;
        std::cout << "  Forward: (" << forward.x << "," << forward.y << "," << forward.z << ")" << std::endl;
        std::cout << "  Right:   (" << right.x << "," << right.y << "," << right.z << ")" << std::endl;
        std::cout << "  Up:      (" << camera_up.x << "," << camera_up.y << "," << camera_up.z << ")" << std::endl;
        
        return Matrix4x4::look_at(Vector3(position.x, position.y, position.z), Vector3(target.x, target.y, target.z), up);
    }
    
    // Update aspect ratio when resolution changes (CRITICAL for AC 5)
    // This ensures FOV correctness across different resolutions
    void set_aspect_ratio(float new_aspect_ratio) {
        float old_aspect = aspect_ratio;
        aspect_ratio = new_aspect_ratio;
        
        // Clamp to safe range
        if (aspect_ratio <= 0.1f) {
            aspect_ratio = 0.1f;
            std::cout << "Warning: Aspect ratio clamped to minimum 0.1" << std::endl;
        } else if (aspect_ratio >= 10.0f) {
            aspect_ratio = 10.0f;
            std::cout << "Warning: Aspect ratio clamped to maximum 10.0" << std::endl;
        }
        
        std::cout << "\n=== Aspect Ratio Update ===" << std::endl;
        std::cout << "Old aspect ratio: " << old_aspect << ":1" << std::endl;
        std::cout << "New aspect ratio: " << aspect_ratio << ":1" << std::endl;
        explain_aspect_ratio_effects();
    }
    
    // Set aspect ratio from image resolution (width, height)
    void set_aspect_ratio_from_resolution(int width, int height) {
        if (height <= 0) {
            std::cout << "ERROR: Invalid height for aspect ratio calculation" << std::endl;
            return;
        }
        
        float new_aspect = static_cast<float>(width) / static_cast<float>(height);
        
        std::cout << "\n=== Resolution-Based Aspect Ratio Update ===" << std::endl;
        std::cout << "Image resolution: " << width << " × " << height << " pixels" << std::endl;
        std::cout << "Calculated aspect ratio: " << new_aspect << ":1" << std::endl;
        
        // Classify common aspect ratios for educational insight
        if (std::abs(new_aspect - 1.0f) < 0.01f) {
            std::cout << "Format: Square (1:1)" << std::endl;
        } else if (std::abs(new_aspect - 4.0f/3.0f) < 0.01f) {
            std::cout << "Format: Classic TV/Monitor (4:3)" << std::endl;
        } else if (std::abs(new_aspect - 16.0f/9.0f) < 0.01f) {
            std::cout << "Format: Widescreen (16:9)" << std::endl;
        } else if (std::abs(new_aspect - 21.0f/9.0f) < 0.01f) {
            std::cout << "Format: Ultra-wide (21:9)" << std::endl;
        } else {
            std::cout << "Format: Custom aspect ratio" << std::endl;
        }
        
        set_aspect_ratio(new_aspect);
    }
    
    // Validate camera ray generation for arbitrary resolutions (AC 5 requirement)
    bool validate_ray_generation(int image_width, int image_height) const {
        std::cout << "\n=== Ray Generation Validation ===" << std::endl;
        std::cout << "Testing image resolution: " << image_width << " × " << image_height << std::endl;
        
        float expected_aspect = static_cast<float>(image_width) / static_cast<float>(image_height);
        std::cout << "Expected aspect ratio: " << expected_aspect << ":1" << std::endl;
        std::cout << "Current camera aspect ratio: " << aspect_ratio << ":1" << std::endl;
        
        // Check aspect ratio consistency
        float aspect_difference = std::abs(expected_aspect - aspect_ratio);
        if (aspect_difference > 0.01f) {
            std::cout << "WARNING: Aspect ratio mismatch detected!" << std::endl;
            std::cout << "  Expected: " << expected_aspect << ":1" << std::endl;
            std::cout << "  Camera: " << aspect_ratio << ":1" << std::endl;
            std::cout << "  Difference: " << aspect_difference << std::endl;
            std::cout << "  This will cause distorted images. Use set_aspect_ratio_from_resolution()." << std::endl;
            return false;
        }
        
        // Test corner rays to ensure proper scaling
        std::cout << "\nTesting corner ray generation..." << std::endl;
        
        // Test corner pixels for ray direction validation
        Ray top_left = generate_ray(0, 0, image_width, image_height);
        Ray top_right = generate_ray(image_width - 1, 0, image_width, image_height);
        Ray bottom_left = generate_ray(0, image_height - 1, image_width, image_height);
        Ray bottom_right = generate_ray(image_width - 1, image_height - 1, image_width, image_height);
        
        // Check that rays are properly normalized
        float tl_len = top_left.direction.length();
        float tr_len = top_right.direction.length();
        float bl_len = bottom_left.direction.length();
        float br_len = bottom_right.direction.length();
        
        std::cout << "Ray direction lengths (should be ~1.0):" << std::endl;
        std::cout << "  Top-left: " << tl_len << std::endl;
        std::cout << "  Top-right: " << tr_len << std::endl;
        std::cout << "  Bottom-left: " << bl_len << std::endl;
        std::cout << "  Bottom-right: " << br_len << std::endl;
        
        // Check normalization (within tolerance)
        if (std::abs(tl_len - 1.0f) > 0.001f || std::abs(tr_len - 1.0f) > 0.001f ||
            std::abs(bl_len - 1.0f) > 0.001f || std::abs(br_len - 1.0f) > 0.001f) {
            std::cout << "ERROR: Ray directions are not properly normalized!" << std::endl;
            return false;
        }
        
        // Test center ray for reference
        Ray center = generate_ray(image_width / 2.0f, image_height / 2.0f, image_width, image_height);
        std::cout << "\nCenter ray direction: (" << center.direction.x << ", " << center.direction.y << ", " << center.direction.z << ")" << std::endl;
        
        // For educational purposes, show horizontal and vertical FOV coverage
        float horizontal_fov = calculate_horizontal_fov();
        std::cout << "\nField-of-View Coverage:" << std::endl;
        std::cout << "  Vertical FOV: " << field_of_view_degrees << "°" << std::endl;
        std::cout << "  Horizontal FOV: " << horizontal_fov << "°" << std::endl;
        
        std::cout << "✓ Ray generation validation successful!" << std::endl;
        return true;
    }
    
    // Calculate horizontal FOV from vertical FOV and aspect ratio
    float calculate_horizontal_fov() const {
        float vfov_radians = field_of_view_degrees * (M_PI / 180.0f);
        float hfov_radians = 2.0f * std::atan(std::tan(vfov_radians * 0.5f) * aspect_ratio);
        return hfov_radians * (180.0f / M_PI);
    }
    
    // Educational explanation of aspect ratio effects on FOV and rendering
    void explain_aspect_ratio_effects() const {
        std::cout << "\nAspect Ratio Effects on Field-of-View:" << std::endl;
        
        float horizontal_fov = calculate_horizontal_fov();
        std::cout << "  Vertical FOV (specified): " << field_of_view_degrees << "°" << std::endl;
        std::cout << "  Horizontal FOV (calculated): " << horizontal_fov << "°" << std::endl;
        std::cout << "  Aspect ratio: " << aspect_ratio << ":1" << std::endl;
        
        // Explain the mathematical relationship
        std::cout << "\nMathematical Relationship:" << std::endl;
        std::cout << "  horizontal_fov = 2 × atan(tan(vertical_fov/2) × aspect_ratio)" << std::endl;
        std::cout << "  For aspect > 1.0: wider horizontal view (landscape)" << std::endl;
        std::cout << "  For aspect < 1.0: narrower horizontal view (portrait)" << std::endl;
        std::cout << "  For aspect = 1.0: equal horizontal and vertical FOV (square)" << std::endl;
        
        // Practical implications
        std::cout << "\nPractical Implications:" << std::endl;
        if (aspect_ratio > 1.5f) {
            std::cout << "  Wide aspect ratio: Good for landscape scenes and panoramic views" << std::endl;
        } else if (aspect_ratio < 0.75f) {
            std::cout << "  Tall aspect ratio: Good for portrait scenes and vertical subjects" << std::endl;
        } else {
            std::cout << "  Balanced aspect ratio: Good general-purpose viewing" << std::endl;
        }
    }
    
    // Explain camera mathematics for educational purposes
    void explain_fov_calculation() const {
        std::cout << "\n=== Field-of-View Calculation Mathematics ===" << std::endl;
        std::cout << "Vertical FOV: " << field_of_view_degrees << "° (user-specified)" << std::endl;
        std::cout << "Aspect Ratio: " << aspect_ratio << ":1 (width:height)" << std::endl;
        
        float vfov_rad = field_of_view_degrees * (M_PI / 180.0f);
        float vfov_scale = std::tan(vfov_rad * 0.5f);
        float hfov_scale = vfov_scale * aspect_ratio;
        float hfov_rad = 2.0f * std::atan(hfov_scale);
        float hfov_deg = hfov_rad * (180.0f / M_PI);
        
        std::cout << "\nStep-by-step calculation:" << std::endl;
        std::cout << "  1. Convert vertical FOV to radians: " << vfov_rad << " rad" << std::endl;
        std::cout << "  2. Calculate vertical scale: tan(vfov/2) = " << vfov_scale << std::endl;
        std::cout << "  3. Apply aspect ratio: hscale = vscale × aspect = " << hfov_scale << std::endl;
        std::cout << "  4. Calculate horizontal FOV: 2 × atan(hscale) = " << hfov_rad << " rad" << std::endl;
        std::cout << "  5. Convert to degrees: " << hfov_deg << "°" << std::endl;
        
        std::cout << "\nCritical Insight: The vertical FOV remains constant regardless of aspect ratio." << std::endl;
        std::cout << "The horizontal FOV scales proportionally with the aspect ratio." << std::endl;
        std::cout << "This ensures that vertical objects maintain their apparent size across resolutions." << std::endl;
    }
    
    // Print camera mathematics for debugging and validation
    void print_camera_mathematics() const {
        std::cout << "\n=== Complete Camera Mathematics ===" << std::endl;
        
        // Basic parameters
        std::cout << "Camera Parameters:" << std::endl;
        std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "  Target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
        std::cout << "  Up vector: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
        std::cout << "  Vertical FOV: " << field_of_view_degrees << "°" << std::endl;
        std::cout << "  Aspect ratio: " << aspect_ratio << ":1" << std::endl;
        
        // Calculated values
        float horizontal_fov = calculate_horizontal_fov();
        std::cout << "\nCalculated Values:" << std::endl;
        std::cout << "  Horizontal FOV: " << horizontal_fov << "°" << std::endl;
        std::cout << "  35mm focal length: " << focal_length << "mm" << std::endl;
        
        // Coordinate system
        std::cout << "\nCamera Coordinate System:" << std::endl;
        std::cout << "  Forward: (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "  Right: (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "  Up: (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
        
        // Validation
        std::cout << "\nValidation Checks:" << std::endl;
        bool is_valid = validate_parameters();
        std::cout << "  Camera parameters valid: " << (is_valid ? "✓ Yes" : "✗ No") << std::endl;
        
        // Educational notes
        std::cout << "\nEducational Notes:" << std::endl;
        std::cout << "  - Vertical FOV controls perspective 'zoom'" << std::endl;
        std::cout << "  - Aspect ratio affects horizontal coverage only" << std::endl;
        std::cout << "  - Camera coordinate system is orthonormal (perpendicular unit vectors)" << std::endl;
        std::cout << "  - Ray direction = screen_point transformed to world space" << std::endl;
    }
    
    // Field-of-view and optics calculations
    float fov_to_focal_length(float fov_degrees, float sensor_width = 36.0f) const {
        // Convert FOV to focal length using standard camera optics formula
        float fov_radians = fov_degrees * (M_PI / 180.0f);
        float focal_length = sensor_width / (2.0f * std::tan(fov_radians * 0.5f));
        
        // Educational explanation
        std::cout << "FOV Conversion: " << fov_degrees << "° = " << focal_length << "mm focal length (35mm equivalent)" << std::endl;
        
        return focal_length;
    }
    
    float focal_length_to_fov(float focal_length, float sensor_width = 36.0f) const {
        // Convert focal length to FOV
        float fov_radians = 2.0f * std::atan(sensor_width / (2.0f * focal_length));
        float fov_degrees = fov_radians * (180.0f / M_PI);
        return fov_degrees;
    }
    
    // Educational mathematical explanations
    void explain_camera_coordinate_system() const {
        std::cout << "\n=== Camera Coordinate System Explanation ===" << std::endl;
        std::cout << "Camera Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Camera Target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
        std::cout << "World Up Vector: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
        std::cout << "\nCalculated Basis Vectors (Orthonormal):" << std::endl;
        std::cout << "  Forward (toward target): (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "  Right (forward × up): (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "  Camera Up (right × forward): (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
        
        // Show orthogonality
        float forward_right_dot = forward.dot(right);
        float right_up_dot = right.dot(camera_up);
        float up_forward_dot = camera_up.dot(forward);
        std::cout << "\nOrthogonality Check (should be ~0):" << std::endl;
        std::cout << "  Forward·Right = " << forward_right_dot << std::endl;
        std::cout << "  Right·Up = " << right_up_dot << std::endl;
        std::cout << "  Up·Forward = " << up_forward_dot << std::endl;
    }
    
    void explain_fov_calculations() const {
        std::cout << "\n=== Field of View Calculations ===" << std::endl;
        std::cout << "Vertical FOV: " << field_of_view_degrees << "°" << std::endl;
        std::cout << "Horizontal FOV: " << focal_length_to_fov(focal_length / aspect_ratio) << "°" << std::endl;
        std::cout << "35mm equivalent focal length: " << focal_length << "mm" << std::endl;
        std::cout << "Aspect ratio: " << aspect_ratio << " (width/height)" << std::endl;
        
        float fov_radians = field_of_view_degrees * (M_PI / 180.0f);
        float fov_scale = std::tan(fov_radians * 0.5f);
        std::cout << "\nTrigonometric Values:" << std::endl;
        std::cout << "  FOV in radians: " << fov_radians << std::endl;
        std::cout << "  tan(FOV/2): " << fov_scale << std::endl;
        std::cout << "  Screen plane scaling factor: " << fov_scale << std::endl;
    }
    
    // Parameter validation and clamping
    bool validate_parameters() const {
        // Check for finite position and target coordinates
        if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z) ||
            !std::isfinite(target.x) || !std::isfinite(target.y) || !std::isfinite(target.z)) {
            return false;
        }
        
        // Check for valid up vector
        if (!up.is_finite() || up.length_squared() < 1e-12f) {
            return false;
        }
        
        // Check for reasonable field of view
        if (field_of_view_degrees <= 1.0f || field_of_view_degrees >= 179.0f) {
            return false;
        }
        
        // Check for positive aspect ratio
        if (aspect_ratio <= 0.0f) {
            return false;
        }
        
        // Check that camera is not at same position as target
        Vector3 forward_direction = Vector3(target.x - position.x, target.y - position.y, target.z - position.z);
        if (forward_direction.length_squared() < 1e-12f) {
            return false;  // Camera and target are at same position
        }
        
        // Check that up vector is not parallel to forward direction
        Vector3 forward_normalized = forward_direction.normalize();
        Vector3 up_normalized = up.normalize();
        float dot_product = std::abs(forward_normalized.dot(up_normalized));
        if (dot_product > 0.999f) {  // Nearly parallel (within ~2.5 degrees)
            return false;
        }
        
        return true;
    }
    
    void clamp_to_safe_ranges() {
        // Clamp field of view to safe range [1°, 179°]
        if (field_of_view_degrees <= 1.0f) {
            field_of_view_degrees = 1.0f;
            std::cout << "Warning: FOV clamped to minimum 1°" << std::endl;
        } else if (field_of_view_degrees >= 179.0f) {
            field_of_view_degrees = 179.0f;
            std::cout << "Warning: FOV clamped to maximum 179°" << std::endl;
        }
        
        // Clamp aspect ratio to reasonable range
        if (aspect_ratio <= 0.1f) {
            aspect_ratio = 0.1f;
            std::cout << "Warning: Aspect ratio clamped to minimum 0.1" << std::endl;
        } else if (aspect_ratio >= 10.0f) {
            aspect_ratio = 10.0f;
            std::cout << "Warning: Aspect ratio clamped to maximum 10.0" << std::endl;
        }
        
        // Normalize up vector if needed
        if (up.length_squared() > 0.0f) {
            up = up.normalize();
        } else {
            up = Vector3(0, 1, 0);  // Default world up
            std::cout << "Warning: Invalid up vector, using default (0,1,0)" << std::endl;
        }
    }
    
    // Command-line parameter setting
    void set_from_command_line_args(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--camera-pos") == 0 && i + 1 < argc) {
                // Parse "x,y,z" format
                parse_vector3_from_string(argv[i + 1], position);
                i++; // Skip next argument
            } else if (std::strcmp(argv[i], "--camera-target") == 0 && i + 1 < argc) {
                parse_vector3_from_string(argv[i + 1], target);
                i++; // Skip next argument
            } else if (std::strcmp(argv[i], "--fov") == 0 && i + 1 < argc) {
                field_of_view_degrees = std::atof(argv[i + 1]);
                i++; // Skip next argument
            }
        }
        
        // Validate and clamp parameters after parsing
        clamp_to_safe_ranges();
        calculate_camera_basis_vectors();
        focal_length = fov_to_focal_length(field_of_view_degrees);
    }
    
    static void print_command_line_help() {
        std::cout << "\n=== Camera Command-Line Parameters ===" << std::endl;
        std::cout << "--camera-pos x,y,z    Set camera position (e.g., --camera-pos 0,0,5)" << std::endl;
        std::cout << "--camera-target x,y,z Set camera target point (e.g., --camera-target 0,0,0)" << std::endl;
        std::cout << "--fov degrees         Set field of view in degrees (e.g., --fov 45)" << std::endl;
        std::cout << "\nExample: ./raytracer --camera-pos 0,0,5 --camera-target 0,0,0 --fov 45" << std::endl;
    }
    
    // Helper method to parse "x,y,z" string into Point3
    void parse_vector3_from_string(const char* str, Point3& point) {
        float x, y, z;
        if (std::sscanf(str, "%f,%f,%f", &x, &y, &z) == 3) {
            point = Point3(x, y, z);
            std::cout << "Parsed position: (" << x << ", " << y << ", " << z << ")" << std::endl;
        } else {
            std::cout << "Warning: Could not parse '" << str << "' as x,y,z coordinates" << std::endl;
        }
    }
    
    // Educational output: explain camera coordinate transformation mathematics
    // Prints detailed explanation of coordinate system transformations
    void explain_coordinate_transformation() const {
        std::cout << "\n=== Camera Coordinate Transformation Mathematics ===" << std::endl;
        std::cout << "Camera Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Camera Target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
        std::cout << "World Up Vector: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
        std::cout << "Field of View: " << field_of_view_degrees << " degrees" << std::endl;
        std::cout << "Aspect Ratio: " << aspect_ratio << " (width/height)" << std::endl;
        
        std::cout << "\nPre-computed Camera Coordinate System Basis Vectors:" << std::endl;
        std::cout << "Forward (camera → target): (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "Right (forward × up): (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "Camera Up (right × forward): (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
        
        // Display mathematical transformations
        float fov_radians = field_of_view_degrees * M_PI / 180.0f;
        float fov_scale = std::tan(fov_radians * 0.5f);
        
        std::cout << "\nMathematical Transformation Parameters:" << std::endl;
        std::cout << "FOV in radians: " << fov_radians << std::endl;
        std::cout << "FOV scale factor (tan(fov/2)): " << fov_scale << std::endl;
        std::cout << "Horizontal scale (aspect × fov_scale): " << (aspect_ratio * fov_scale) << std::endl;
        std::cout << "35mm equivalent focal length: " << focal_length << "mm" << std::endl;
        
        std::cout << "\nCoordinate Transformation Process:" << std::endl;
        std::cout << "1. Pixel (x,y) → NDC: (2x/width-1, 1-2y/height)" << std::endl;
        std::cout << "2. NDC → Camera space: (ndc_x×aspect×fov_scale, ndc_y×fov_scale, 1)" << std::endl;
        std::cout << "3. Camera space → World space: camera_basis_transformation" << std::endl;
        std::cout << "4. Result: Ray(camera_position, normalized_world_direction)" << std::endl;
    }
    
    // Print current camera parameters for debugging and validation
    void print_camera_parameters() const {
        std::cout << "\n=== Camera Parameters ===" << std::endl;
        std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
        std::cout << "Up vector: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
        std::cout << "Field of view: " << field_of_view_degrees << "°" << std::endl;
        std::cout << "Aspect ratio: " << aspect_ratio << std::endl;
        std::cout << "35mm focal length: " << focal_length << "mm" << std::endl;
        
        // Calculate camera distance to target for reference
        Vector3 distance_vector = Vector3(target.x - position.x, target.y - position.y, target.z - position.z);
        float distance_to_target = distance_vector.length();
        std::cout << "Distance to target: " << distance_to_target << std::endl;
        
        std::cout << "\nDerived Basis Vectors:" << std::endl;
        std::cout << "Forward: (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "Right: (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "Camera Up: (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
    }
    
    // Validate camera configuration for proper ray generation
    // Returns true if camera parameters are valid for rendering
    bool validate_camera() const {
        return validate_parameters();
    }
    
};