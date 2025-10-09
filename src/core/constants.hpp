#pragma once

#include "vector3.hpp"
#include "point3.hpp"

// Educational Ray Tracer - Common Constants
// Centralized location for magic numbers and default values

namespace Constants {
    // Default colors
    inline const Vector3 BACKGROUND_COLOR(0.1f, 0.1f, 0.15f);  // Dark blue background
    inline const Vector3 DEFAULT_MATERIAL_COLOR(0.7f, 0.3f, 0.3f);  // Reddish default for demos

    // Default geometry positions
    inline const Point3 DEFAULT_SPHERE_POSITION(0.0f, 0.0f, -3.0f);  // Default sphere position
    inline const Point3 TEST_SPHERE_POSITION(0.0f, 0.0f, -5.0f);  // Test sphere position
    inline const float DEFAULT_SPHERE_RADIUS = 1.0f;

    // Default light parameters
    inline const float DEFAULT_LIGHT_INTENSITY = 100.0f;
}
