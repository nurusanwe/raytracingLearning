# Educational Ray Tracer

An educational ray tracer implementation focused on learning graphics programming fundamentals through progressive epic development.

## Learning Objectives

### Epic Progression
1. **Epic 1**: Mathematical Foundation - Vector math, basic ray-sphere intersection
2. **Epic 2**: Visual Foundation - Multi-ray rendering with Dear ImGui integration  
3. **Epic 3**: Cook-Torrance Microfacet Theory Implementation
4. **Epic 4**: OpenPBR Material System (Educational Completion Point)
5. **Epic 5**: MaterialX Integration & USD Pipeline
6. **Epic 6**: Apple Silicon Optimization & Advanced Features

### Core Learning Goals
- Master fundamental vector mathematics for graphics programming
- Understand physically-based rendering (PBR) theory and implementation
- Learn progressive software architecture for complex graphics systems
- Gain experience with Apple Silicon optimization techniques

## Build Instructions

### Prerequisites
- CMake 3.20 or higher
- C++20/C++23 compatible compiler
- macOS (Apple Silicon optimized) or cross-platform compatible

### Quick Start
```bash
# Single command build
./build_simple.sh
```

### Manual Build Process
```bash
# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build project
make

# Run tests
./test_math_correctness

# Run ray tracer
./raytracer
```

## Project Structure

```
raytracer/
├── src/                     # Single evolving codebase
│   ├── main.cpp             # Epic 1 single-ray → Epic 6 production ray tracer
│   ├── core/                # Core mathematical and rendering components
│   ├── materials/           # Material implementations (Epic 1+)
│   ├── ui/                  # Dear ImGui integration (Epic 2+)
│   └── platform/            # Platform abstraction (Epic 6)
├── external/                # Minimal external dependencies
├── epic_checkpoints/        # Git-based learning milestones
├── tests/                   # Mathematical correctness testing
├── tools/                   # Educational tools
├── docs/                    # Educational documentation
└── assets/                  # Minimal test assets
```

## Architectural Decision Rationale

### Educational-First Design
- **Single Command Setup**: `./build_simple.sh` works on any supported system
- **Visible Compilation**: Build process shows exactly what's happening for learning
- **Progressive Complexity**: Build requirements grow with epic progression, not front-loaded
- **Mathematical Validation**: Every epic includes mathematical correctness testing

### Apple Silicon Optimization Strategy
- **Primary Target**: Apple Silicon (M1/M2/M3) with `-mcpu=apple-m1 -O3` optimization
- **Cross-Platform Foundation**: Preprocessor directives maintain compatibility
- **NEON Vectorization**: Preparation for advanced SIMD optimizations in Epic 6
- **Metal Performance Shaders**: Future integration target for GPU acceleration

### Incremental Dependency Management
- **Minimal External Dependencies**: Start with standard library only
- **Progressive Introduction**: Dependencies added only when educational value is clear
- **Educational Justification**: Each dependency includes learning rationale

## Testing Framework

### Mathematical Correctness Testing
Primary focus on validating mathematical precision and energy conservation:

```cpp
// Example: Vector3 mathematical validation
Vector3 a(1, 2, 3);
Vector3 b(4, 5, 6);
assert(abs(a.dot(b) - 32.0f) < 1e-6); // Mathematical precision validation
```

### Testing Hierarchy
1. **Unit Tests**: Individual component mathematical correctness
2. **Mathematical Tests**: Vector operations, ray-surface intersections
3. **Visual Tests**: Rendered output validation (Epic 2+)
4. **Learning Tests**: Educational milestone verification

## Troubleshooting

### Common Build Issues

**CMake Version Error**
```bash
# Ensure CMake 3.20+
cmake --version
# Update via Homebrew: brew upgrade cmake
```

**Apple Silicon Compilation Issues**
```bash
# Verify architecture detection
uname -m  # Should show "arm64"
# Force architecture if needed
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64
```

**Missing C++20 Support**
```bash
# Verify compiler version
g++ --version  # GCC 10+ required
clang++ --version  # Clang 10+ required
```

**Test Execution Failures**
```bash
# Run tests manually for debugging
cd build
./test_math_correctness
# Check mathematical precision issues
```

### Educational Debugging
- Build with `CMAKE_VERBOSE_MAKEFILE=ON` for compilation visibility
- Mathematical tests include precision tolerance validation
- Cross-platform compatibility verified through preprocessor directive testing

## Next Steps

After successful build:
1. Verify mathematical test suite passes (`./test_math_correctness`)
2. Run basic ray tracer (`./raytracer`) to confirm platform detection
3. Begin Epic 1 development: Vector3 mathematics and ray-sphere intersection
4. Set up learning milestone branching strategy in git

## Epic Milestones

Track learning progress through git branches and epic checkpoints:
- `epic-1-foundation`: Mathematical core implementation
- `epic-2-visual`: Multi-ray rendering with GUI
- `epic-3-microfacet`: Cook-Torrance implementation
- `epic-4-openpbr`: Educational completion point
- `epic-5-materialx`: Professional pipeline integration
- `epic-6-optimization`: Apple Silicon advanced features