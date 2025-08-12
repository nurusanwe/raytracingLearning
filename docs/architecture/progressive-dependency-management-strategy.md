# Progressive Dependency Management Strategy

The project follows a **progressive dependency introduction** approach, aligning with the epic-based learning progression to prevent overwhelming students with complex external libraries while building towards industry-standard tools.

## Epic-Based Dependency Introduction

**Epic 1 (Foundation): Zero External Dependencies**
- **Philosophy**: Pure C++ implementation for mathematical transparency
- **Dependencies**: None - completely self-contained educational code
- **Build Requirements**: C++20 compiler, CMake 3.25+
- **Rationale**: Students focus on ray tracing mathematics without library complexity

**Epic 2 (Visual Foundation): Minimal UI Dependencies**
- **New Dependencies**: 
  - Dear ImGui 1.90+ (git submodule)
  - GLFW (system package: `brew install glfw` / `apt install libglfw3-dev`)
  - OpenGL (system-provided)
  - STB libraries (single-header: stb_image.h, stb_image_write.h)
- **Integration Strategy**: Git submodules for Dear ImGui, direct download for STB headers
- **Rationale**: Lightweight UI for real-time parameter manipulation without complex frameworks

**Epic 3-4 (Advanced Materials): Mathematical Validation Dependencies**
- **New Dependencies**:
  - GLM 0.9.9+ (optional for mathematical validation)
- **Integration Strategy**: Header-only library via package manager or git submodule
- **Rationale**: Industry reference for validating student's mathematical implementations

**Epic 5-6 (Optional Advanced): Industry Integration Dependencies**
- **New Dependencies** (Optional Advanced Modules):
  - MaterialX 1.38+ (Academy Software Foundation standard)
  - USD Core 23.05+ (Pixar Universal Scene Description)
  - OpenEXR 3.1+ (High dynamic range imaging)
- **Integration Strategy**: vcpkg or manual building for complex libraries
- **Rationale**: Industry exposure without overwhelming core educational objectives

## Cross-Platform Setup Scripts

**Automated Dependency Setup:**

`tools/setup_dependencies.sh`:
```bash
#!/bin/bash