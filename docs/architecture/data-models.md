# Data Models

The core data models represent the mathematical and scene entities shared between the rendering engine and educational interface, **enhanced with educational debugging infrastructure and real-time parameter binding** to support the interactive learning experience.

## Vector3 (Enhanced with Educational Features)

**Purpose:** Fundamental 3D mathematical operations with **calculation history tracking** and **educational validation**

**Key Attributes:**
- x, y, z: float - Components optimized for SIMD operations
- Clean, minimal interface suitable for Apple Silicon NEON vectorization

### TypeScript Interface
```cpp
class Vector3 {
public:
    float x, y, z;
    
    // Core mathematical operations - optimized and clean
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;  
    Vector3 normalized() const;
    float length() const;
    float length_squared() const;
    
    // Standard arithmetic operators
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;  
    Vector3 operator*(float scalar) const;
    
    // Production validation (minimal overhead)
    bool is_finite() const;
    bool is_normalized(float tolerance = 0.001f) const;
};
```

### Educational Inspection Layer (Separate)
```cpp
class Vector3Inspector {
public:
    static std::string explain_dot_product(const Vector3& a, const Vector3& b, float result);
    static std::string explain_cross_product(const Vector3& a, const Vector3& b, const Vector3& result);
    static std::string explain_normalization(const Vector3& input, const Vector3& result);
    
    // Optional calculation tracking (enabled only when needed)
    static void enable_calculation_tracking(bool enable);
    static std::vector<std::string> get_recent_calculations();
    static void clear_calculation_history();
    
private:
    static thread_local std::vector<std::string> calculation_log;
    static std::atomic<bool> tracking_enabled;
};
```

## Material (Clean Core with Parameter Binding Adapter)

**Purpose:** Surface material properties for BRDF evaluation

### TypeScript Interface  
```cpp
enum class MaterialType {
    Lambert,
    CookTorrance, 
    OpenPBR
};

class Material {
public:
    Vector3 base_color;
    float roughness;
    float metallic;
    float specular;
    Vector3 emission;
    MaterialType type;
    
    Material(const Vector3& color = Vector3(0.7f, 0.7f, 0.7f),
             float roughness = 0.5f,
             MaterialType type = MaterialType::Lambert);
    
    // Core BRDF evaluation - clean and fast
    Vector3 evaluate_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const;
    
    // Production validation
    bool validate_parameters() const;
    void clamp_to_valid_ranges();
};
```

## Scene (Clean Core with Educational Monitoring)

**Purpose:** Scene container optimized for rendering performance

### TypeScript Interface
```cpp
class Scene {
public:
    std::vector<Primitive> primitives;
    std::vector<Material> materials;
    std::vector<Light> lights;
    Camera camera;
    Vector3 background;
    
    Scene() = default;
    
    struct Intersection {
        bool hit;
        float t;
        Vector3 point;
        Vector3 normal;  
        const Material* material;
        const Primitive* primitive;
    };
    
    Intersection intersect(const Ray& ray) const;
    
    // Standard scene management
    int add_material(const Material& material);
    int add_primitive(const Primitive& primitive);
    void set_camera(const Camera& camera);
};
```
