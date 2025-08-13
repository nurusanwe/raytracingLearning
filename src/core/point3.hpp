#pragma once
#include "vector3.hpp"
#include <cmath>

// Point3 represents a position in 3D space (not a direction/displacement)
// Geometric distinction: Points are locations, Vectors are displacements
// Mathematical foundation: Point operations follow affine geometry rules
class Point3 {
public:
    float x, y, z;
    
    // Default constructor - creates point at origin (0,0,0)
    Point3() : x(0), y(0), z(0) {}
    
    // Explicit coordinate constructor
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Point-to-point vector calculation: displacement from other point to this point
    // Geometric interpretation: creates vector representing direction and distance
    // Mathematical rule: Point - Point = Vector (affine geometry)
    Vector3 operator-(const Point3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    // Point displacement by vector: moves point by vector displacement
    // Geometric interpretation: translates point position by vector amount
    // Mathematical rule: Point + Vector = Point (affine geometry)
    Point3 operator+(const Vector3& displacement) const {
        return Point3(x + displacement.x, y + displacement.y, z + displacement.z);
    }
    
    // Point displacement by negative vector: moves point opposite to vector
    // Geometric interpretation: translates point backwards along vector direction
    // Mathematical rule: Point - Vector = Point (affine geometry)
    Point3 operator-(const Vector3& displacement) const {
        return Point3(x - displacement.x, y - displacement.y, z - displacement.z);
    }
    
    // Distance calculation between two points using Euclidean metric
    // Geometric interpretation: straight-line distance through 3D space
    // Formula: √((x₂-x₁)² + (y₂-y₁)² + (z₂-z₁)²)
    float distance_to(const Point3& other) const {
        Vector3 displacement = *this - other;  // Get displacement vector
        return displacement.length();          // Return vector magnitude
    }
    
    // Squared distance - more efficient for comparisons (avoids sqrt)
    // Geometric interpretation: same as distance² but computationally faster
    // Usage: When only relative distances matter (nearest neighbor, sorting)
    float distance_squared_to(const Point3& other) const {
        Vector3 displacement = *this - other;  // Get displacement vector
        return displacement.length_squared();  // Return squared magnitude
    }
};

// Free function: Vector + Point = Point (commutative displacement)
// Allows writing: vector + point (same as point + vector)
inline Point3 operator+(const Vector3& displacement, const Point3& point) {
    return point + displacement;
}