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
           float fov_degrees, float aspect)
        : position(pos), target(tgt), up(up_vec), 
          field_of_view_degrees(fov_degrees), aspect_ratio(aspect) {
        
        // Clamp parameters to safe ranges
        clamp_to_safe_ranges();
        
        // Calculate derived camera properties
        calculate_camera_basis_vectors();
        
        // Calculate focal length
        focal_length = fov_to_focal_length(field_of_view_degrees);
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