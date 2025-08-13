#pragma once
#include <cmath>

class Vector3 {
public:
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Dot product
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // Cross product
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    // Length
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // Normalization
    Vector3 normalize() const {
        float len = length();
        if (len > 1e-6f) {
            return Vector3(x / len, y / len, z / len);
        }
        return Vector3(0, 0, 0);
    }
    
    // Squared length (more efficient for comparisons)
    float length_squared() const {
        return x * x + y * y + z * z;
    }
    
    // Basic operators
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
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