# Scene Data Format Specifications

## Educational Scene File Format (.scene)

The scene file format uses simple text format for educational clarity and supports progressive complexity from Epic 2 through Epic 4.

## Multi-Material Scene Format (Epic 3+)

### Format Overview
The extended scene format supports both Lambert and Cook-Torrance materials in the same scene file, enabling comprehensive educational demonstrations of different BRDF models.

### Material Definitions

#### Lambert Material Format
```
material_lambert <name> <red> <green> <blue>
```

**Parameters:**
- `name`: Unique material identifier (string, no spaces)
- `red`, `green`, `blue`: Albedo color components [0.0, 1.0]

**Example:**
```
material_lambert diffuse_red 0.8 0.3 0.3
material_lambert matte_white 0.9 0.9 0.9
```

#### Cook-Torrance Material Format
```
material_cook_torrance <name> <base_r> <base_g> <base_b> <roughness> <metallic> <specular>
```

**Parameters:**
- `name`: Unique material identifier (string, no spaces)
- `base_r`, `base_g`, `base_b`: Base color components [0.0, 1.0]
- `roughness`: Surface roughness [0.01, 1.0] (0.01=mirror, 1.0=diffuse)
- `metallic`: Metallic parameter [0.0, 1.0] (0.0=dielectric, 1.0=conductor)
- `specular`: Dielectric F0 reflectance [0.0, 1.0] (typical: 0.04)

**Example:**
```
material_cook_torrance gold_mirror 1.0 0.8 0.3 0.02 1.0 0.04
material_cook_torrance plastic_rough 0.2 0.8 0.4 0.9 0.0 0.04
```

### Primitive Definitions

#### Sphere Format
```
sphere <center_x> <center_y> <center_z> <radius> <material_name>
```

**Parameters:**
- `center_x`, `center_y`, `center_z`: Sphere center coordinates
- `radius`: Sphere radius (positive value)
- `material_name`: Reference to previously defined material

**Example:**
```
sphere 0.0 0.0 -5.0 1.0 diffuse_red
sphere 2.0 0.0 -5.0 1.0 gold_mirror
```

## Complete Scene File Example

### Cook-Torrance Material Showcase
```
# Cook-Torrance Material Showcase Scene
# Educational demonstration of microfacet theory with multiple material configurations
# Demonstrates roughness, metallic, and specular parameter effects

# Lambert materials for comparison
material_lambert matte_white 0.9 0.9 0.9
material_lambert matte_red 0.8 0.3 0.3
material_lambert matte_blue 0.3 0.3 0.8

# Cook-Torrance materials demonstrating parameter ranges
material_cook_torrance gold_mirror 1.0 0.8 0.3 0.02 1.0 0.04
material_cook_torrance gold_brushed 1.0 0.8 0.3 0.3 1.0 0.04
material_cook_torrance gold_rough 1.0 0.8 0.3 0.8 1.0 0.04
material_cook_torrance plastic_smooth 0.2 0.8 0.4 0.1 0.0 0.04
material_cook_torrance plastic_rough 0.2 0.8 0.4 0.9 0.0 0.04

# Scene layout: 3x3 grid demonstrating material progression
# Top row: Metal progression (smooth to rough)
sphere -2.0 1.0 -5.0 0.8 gold_mirror
sphere 0.0 1.0 -5.0 0.8 gold_brushed  
sphere 2.0 1.0 -5.0 0.8 gold_rough

# Middle row: Dielectric Cook-Torrance materials
sphere -1.0 0.0 -5.0 0.8 plastic_smooth
sphere 1.0 0.0 -5.0 0.8 plastic_rough

# Bottom row: Lambert reference materials for comparison
sphere -1.0 -1.0 -5.0 0.8 matte_red
sphere 0.0 -1.0 -5.0 0.8 matte_white
sphere 1.0 -1.0 -5.0 0.8 matte_blue

# Educational Notes:
# - Gold spheres show roughness progression in metallic materials
# - Plastic spheres demonstrate dielectric Cook-Torrance behavior  
# - Lambert spheres provide educational reference for comparison
# - Scene layout enables side-by-side visual comparison of material theory
```

## Backward Compatibility

### Legacy Lambert-Only Format (Epic 2)
The original format continues to work for backward compatibility:
```
material red_sphere 0.8 0.3 0.3
sphere 0.0 0.0 -5.0 1.0 red_sphere
```

**Compatibility Rules:**
1. Lines starting with `material ` (single space) are interpreted as Lambert materials
2. Lines starting with `material_lambert ` are explicit Lambert materials  
3. Lines starting with `material_cook_torrance ` are Cook-Torrance materials
4. Mixed format files are supported - both legacy and new syntax in same file

### Error Handling

**Invalid Material References:**
- Sphere referencing undefined material → Error with helpful message
- Material name conflicts → Later definition overrides earlier one with warning

**Invalid Parameters:**
- Parameter out of range → Automatic clamping with educational console output
- Insufficient parameters → Error with expected format example
- Invalid number format → Error with line number and expected type

**Educational Error Messages:**
```
Error: Sphere at line 15 references undefined material 'unknown_material'
Available materials: matte_white, gold_mirror, plastic_rough

Warning: Roughness 1.5 at line 8 exceeds valid range [0.01, 1.0], clamped to 1.0
Educational note: Roughness > 1.0 would represent unphysical surface behavior

Error: Expected 7 parameters for Cook-Torrance material at line 12, got 5
Format: material_cook_torrance <name> <r> <g> <b> <roughness> <metallic> <specular>
Example: material_cook_torrance gold 1.0 0.8 0.3 0.1 1.0 0.04
```

## Implementation Guidelines

### Parser Requirements
1. **Line-based parsing**: Each material/primitive definition on separate line
2. **Comment support**: Lines starting with `#` are ignored
3. **Whitespace tolerance**: Multiple spaces/tabs between parameters allowed
4. **Case sensitivity**: Material names and keywords are case-sensitive
5. **UTF-8 encoding**: Support for educational comments in multiple languages

### Memory Management
1. **Polymorphic storage**: `std::vector<std::unique_ptr<Material>>` for materials
2. **Reference validation**: Ensure all material references resolve before geometry creation
3. **Resource cleanup**: Automatic memory management through smart pointers

### Educational Integration
1. **Parsing feedback**: Console output showing materials loaded and validated
2. **Parameter clamping**: Educational explanations when values are adjusted
3. **Material statistics**: Summary of material types and parameter ranges loaded
4. **Cross-reference validation**: Report which materials are used/unused