#pragma once
#include <cmath>

class Vector3 {
public:
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Dot product - measures projection of one vector onto another
    // Result: scalar indicating how much vectors point in same direction
    // Geometric interpretation: |a||b|cos(θ) where θ is angle between vectors
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // Cross product - creates vector perpendicular to both input vectors
    // Result: Vector3 orthogonal to both inputs, follows right-hand rule
    // Geometric interpretation: |a||b|sin(θ) magnitude, direction given by right-hand rule
    // Formula: a × b = (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,  // i component: eliminates x components
            z * other.x - x * other.z,  // j component: eliminates y components  
            x * other.y - y * other.x   // k component: eliminates z components
        );
    }
    
    // Vector magnitude - Euclidean distance from origin to point (x,y,z)
    // Geometric interpretation: length of vector in 3D space using Pythagorean theorem
    // Formula: √(x² + y² + z²)
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // Vector normalization - creates unit vector with same direction
    // Geometric interpretation: scales vector to length 1 while preserving direction
    // Formula: v/|v| where |v| is vector magnitude
    // Returns zero vector for near-zero length vectors to avoid division by zero
    Vector3 normalize() const {
        float len = length();
        if (len > 1e-6f) {  // Numerical tolerance for stability
            float inv_len = 1.0f / len;  // Single division, more efficient
            return Vector3(x * inv_len, y * inv_len, z * inv_len);
        }
        return Vector3(0, 0, 0);  // Degenerate case: zero-length vector
    }
    
    // Check if vector is valid (finite components)
    // Usage: Defensive programming to catch NaN/infinity values
    bool is_finite() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    
    // Check if vector is normalized (unit length) within tolerance
    // Usage: Validation for direction vectors in ray tracing calculations
    bool is_normalized(float tolerance = 1e-6f) const {
        float len_sq = length_squared();
        return std::abs(len_sq - 1.0f) <= tolerance;
    }
    
    // Squared magnitude - avoids expensive square root calculation
    // Geometric interpretation: same as length² but more efficient for comparisons
    // Usage: When only relative magnitudes matter (sorting, nearest neighbor)
    float length_squared() const {
        return x * x + y * y + z * z;
    }
    
    // Vector addition - geometric head-to-tail addition
    // Geometric interpretation: placing vectors head-to-tail, resultant vector from origin to final point
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    // Vector subtraction - finds displacement vector from other to this
    // Geometric interpretation: vector from point 'other' to point 'this'
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    // Scalar multiplication - scales vector magnitude by scalar factor
    // Geometric interpretation: changes length by factor, preserves direction (if scalar > 0)
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y; 
        z += other.z;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
};

// Free function for scalar * vector
inline Vector3 operator*(float scalar, const Vector3& v) {
    return v * scalar;
}