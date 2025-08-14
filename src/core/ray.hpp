#pragma once
#include "point3.hpp"
#include "vector3.hpp"
#include <cmath>

// Ray represents a mathematical ray: infinite line starting at origin, extending in direction
// Ray equation: P(t) = origin + t * direction, where t ≥ 0
// Geometric interpretation: Starting point with infinite extension in one direction
// Ray tracing usage: Models light paths, viewing rays, shadow rays
//
// ASCII Art Diagram:
//
//      P(t) = O + t*D
//        
//   t=0     t=1     t=2
//    O-------•-------•-----> direction D
//  origin   P(1)   P(2)
//
// Mathematical Properties:
//   - t=0: Ray origin point
//   - t>0: Points along ray in forward direction  
//   - t<0: Points behind ray origin (usually not used in ray tracing)
//   - |P(t) - O| = t * |D| when D is unit vector
//
// Physics Context:
//   - Models light ray paths in geometric optics
//   - Represents viewing rays from camera through pixels
//   - Used for shadow ray calculations in lighting
class Ray {
public:
    Point3 origin;      // Starting point of ray in 3D space
    Vector3 direction;  // Direction vector (should be normalized for consistent behavior)
    
    // Default constructor - ray at origin pointing along positive x-axis
    Ray() : origin(Point3(0, 0, 0)), direction(Vector3(1, 0, 0)) {}
    
    // Explicit ray constructor with origin point and direction vector
    // Note: direction vector should ideally be normalized for consistent parameterization
    Ray(const Point3& origin, const Vector3& direction) 
        : origin(origin), direction(direction) {}
    
    // Ray evaluation at parameter t: implements ray equation P(t) = origin + t * direction
    // Geometric interpretation: point along ray at distance t*|direction| from origin
    // Parameter t: 0 = origin, positive values extend in direction, negative go backwards
    // Mathematical formula: P(t) = O + t*D where O=origin, D=direction
    Point3 at(float t) const {
        return origin + (t * direction);
    }
    
    // Ray evaluation with explicit parameterization (same as at() but clearer naming)
    // Usage: when emphasizing the mathematical parameter nature of ray traversal
    Point3 point_at_parameter(float t) const {
        return at(t);
    }
    
    // Validates that direction vector is normalized (unit length)
    // Geometric interpretation: ensures consistent parameterization where t represents actual distance
    // Tolerance allows for floating-point precision errors in normalization
    bool is_direction_normalized(float tolerance = 1e-6f) const {
        float length_diff = std::abs(direction.length() - 1.0f);
        return length_diff <= tolerance;
    }
    
    // Returns ray with normalized direction vector (preserves origin)
    // Geometric interpretation: creates equivalent ray with unit direction for consistent parameterization
    // Usage: when ray direction needs to represent actual distance units
    Ray normalized() const {
        return Ray(origin, direction.normalize());
    }
    
    // Ray-point distance calculation: finds closest approach distance
    // Geometric interpretation: perpendicular distance from point to infinite ray line
    // Mathematical approach: uses vector projection and Pythagorean theorem
    float distance_to_point(const Point3& point) const {
        Vector3 to_point = point - origin;           // Vector from ray origin to point
        
        // Handle degenerate case: zero-length direction vector
        float dir_length_sq = direction.length_squared();
        if (dir_length_sq < 1e-12f) {  // Very small threshold for robustness
            return origin.distance_to(point);  // Distance to origin if no direction
        }
        
        // Normalize direction for consistent parameterization
        Vector3 unit_direction = direction.normalize();
        
        // Project to_point onto ray direction to find closest point parameter
        float t = to_point.dot(unit_direction);
        
        // For rays (not lines), clamp t to non-negative values
        // This ensures we measure distance to ray, not infinite line
        t = std::max(0.0f, t);
        
        // Find closest point on ray to the given point
        Point3 closest_point = origin + (t * unit_direction);
        
        // Return distance from point to closest point on ray
        return closest_point.distance_to(point);
    }
    
    // Check if ray has valid configuration
    // Usage: Defensive programming for ray operations
    bool is_valid() const {
        return direction.is_finite() && 
               direction.length_squared() > 1e-12f;  // Non-zero direction
    }
};