#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include "ray.hpp"
#include <cmath>

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
class Camera {
public:
    Point3 position;              // Camera position in world space
    Point3 target;               // Point camera is looking at
    Vector3 up;                  // Up vector for camera orientation (world space)
    float field_of_view_degrees; // Vertical field of view in degrees
    float aspect_ratio;          // Width/height ratio of image
    
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
        
        // Validate camera parameters during construction
        if (fov_degrees <= 0.0f || fov_degrees >= 180.0f) {
            field_of_view_degrees = 45.0f;  // Default to reasonable FOV
        }
        if (aspect_ratio <= 0.0f) {
            aspect_ratio = 16.0f / 9.0f;   // Default to 16:9 aspect ratio
        }
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
        
        // Step 3: Build camera coordinate system basis vectors
        // Forward vector: from camera position toward target (normalized)
        Vector3 forward = (target - position).normalize();
        
        // Right vector: perpendicular to forward and up (right-hand rule)
        // right = forward × up (cross product gives perpendicular vector)
        Vector3 right = forward.cross(up).normalize();
        
        // Camera up vector: perpendicular to both forward and right
        // camera_up = right × forward (completes orthonormal basis)
        Vector3 camera_up = right.cross(forward).normalize();
        
        // Step 4: Transform camera space direction to world space
        // Camera space direction vector (from origin to screen plane point)
        Vector3 camera_direction = Vector3(camera_x, camera_y, camera_z);
        
        // Transform to world space using camera basis vectors
        // world_direction = camera_x * right + camera_y * camera_up + camera_z * forward
        Vector3 world_direction = (camera_x * right) + (camera_y * camera_up) + (camera_z * forward);
        
        // Normalize direction vector for consistent ray parameterization
        world_direction = world_direction.normalize();
        
        // Step 5: Create ray with camera position and world space direction
        return Ray(position, world_direction);
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
        
        // Calculate and display camera basis vectors
        Vector3 forward = (target - position).normalize();
        Vector3 right = forward.cross(up).normalize();
        Vector3 camera_up = right.cross(forward).normalize();
        
        std::cout << "\nCamera Coordinate System Basis Vectors:" << std::endl;
        std::cout << "Forward (camera → target): (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
        std::cout << "Right (cross forward×up): (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
        std::cout << "Camera Up (cross right×forward): (" << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << ")" << std::endl;
        
        // Display mathematical transformations
        float fov_radians = field_of_view_degrees * M_PI / 180.0f;
        float fov_scale = std::tan(fov_radians * 0.5f);
        
        std::cout << "\nMathematical Transformation Parameters:" << std::endl;
        std::cout << "FOV in radians: " << fov_radians << std::endl;
        std::cout << "FOV scale factor (tan(fov/2)): " << fov_scale << std::endl;
        std::cout << "Horizontal scale (aspect × fov_scale): " << (aspect_ratio * fov_scale) << std::endl;
        
        std::cout << "\nCoordinate Transformation Process:" << std::endl;
        std::cout << "1. Pixel (x,y) → NDC: (2x/width-1, 1-2y/height)" << std::endl;
        std::cout << "2. NDC → Camera space: (ndc_x×aspect×fov_scale, ndc_y×fov_scale, -1)" << std::endl;
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
        
        // Calculate camera distance to target for reference
        float distance_to_target = (target - position).length();
        std::cout << "Distance to target: " << distance_to_target << std::endl;
    }
    
    // Validate camera configuration for proper ray generation
    // Returns true if camera parameters are valid for rendering
    bool validate_camera() const {
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
        if (field_of_view_degrees <= 0.0f || field_of_view_degrees >= 180.0f) {
            return false;
        }
        
        // Check for positive aspect ratio
        if (aspect_ratio <= 0.0f) {
            return false;
        }
        
        // Check that camera is not at same position as target
        Vector3 forward_direction = target - position;
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
    
private:
    // Helper method: Calculate camera right vector (forward × up)
    Vector3 calculate_camera_right() const {
        Vector3 forward = (target - position).normalize();
        return forward.cross(up).normalize();
    }
    
    // Helper method: Calculate camera up vector (right × forward) 
    Vector3 calculate_camera_up() const {
        Vector3 forward = (target - position).normalize();
        Vector3 right = forward.cross(up).normalize();
        return right.cross(forward).normalize();
    }
    
    // Helper method: Calculate camera forward vector (toward target)
    Vector3 calculate_camera_forward() const {
        return (target - position).normalize();
    }
};