# Risk-Mitigated Project Structure - Simplified Educational Focus

**Core Architectural Decision:** Single evolving codebase with git-based epic checkpoints, eliminating code duplication and reducing project management complexity while maintaining clear learning progression.

```
raytracer/
├── src/                               # Single evolving codebase - students work here throughout all epics
│   ├── main.cpp                       # Evolution: Epic 1 single-ray → Epic 6 production ray tracer
│   ├── core/                          # Core mathematical and rendering components
│   │   ├── vector3.hpp                # Students implement once, optimize progressively
│   │   ├── ray.hpp                    # Single Ray class, features added per epic
│   │   ├── material.hpp               # Single Material class, BRDF methods added progressively
│   │   ├── scene.hpp                  # Single Scene class, complexity grows with epics
│   │   ├── camera.hpp                 # Camera implementation added in Epic 2
│   │   └── educational_mode.hpp       # Unified educational control - single system
│   ├── materials/                     # Material implementations added as students progress
│   │   ├── lambert.cpp                # Epic 1 - students implement first
│   │   ├── cook_torrance.cpp          # Epic 3 - students add to existing Material class
│   │   └── openpbr.cpp                # Epic 4 - students extend further (optional)
│   ├── ui/                            # Dear ImGui integration (Epic 2+)
│   │   ├── main_window.cpp            # Simple main interface
│   │   └── educational_inspector.cpp  # Mathematical transparency controls
│   └── platform/                      # Platform abstraction (Epic 6)
│       ├── simd.hpp                   # SIMD abstraction: NEON + fallback
│       └── threading.cpp              # Multi-threading (Epic 6, optional)
├── external/                          # Minimal external dependencies
│   └── imgui/                         # Dear ImGui submodule (Epic 2+)
├── epic_checkpoints/                  # Git-based learning milestones
│   ├── epic1_foundation.md            # Epic 1 completion criteria and validation
│   ├── epic2_visual_foundation.md     # Epic 2 refactoring guidance
│   ├── epic3_cook_torrance.md         # Epic 3 microfacet theory implementation
│   ├── epic4_advanced_materials.md    # Epic 4 optional advanced features
│   ├── epic5_industry_exploration.md  # Epic 5 simplified industry concepts
│   └── epic6_apple_optimization.md    # Epic 6 optional performance learning
├── tests/                             # Streamlined testing focused on mathematical correctness
│   ├── test_math_correctness.cpp      # Core mathematical validation (energy conservation, etc.)
│   ├── test_visual_regression.cpp     # Simple reference image comparison
│   └── reference_images/              # Expected outputs for validation
├── tools/                             # Simplified educational tools
│   ├── validate_epic.sh               # Single epic validation script
│   ├── build_simple.sh                # One-command build for all platforms
│   └── educational_report.cpp         # Learning progress analysis
├── docs/                              # Streamlined educational documentation
│   ├── README.md                      # Getting started guide
│   ├── learning_path.md               # Epic progression guide
│   └── mathematical_reference.md      # Ray tracing math reference
├── assets/                            # Minimal test assets
│   ├── simple_scene.scene             # Basic scene for all epics
│   └── reference_materials.mtl        # Material examples
├── CMakeLists.txt                     # Single, simple build configuration
├── .gitignore                         # Standard C++ gitignore
└── LICENSE                            # Educational license
```
