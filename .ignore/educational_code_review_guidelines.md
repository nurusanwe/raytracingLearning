# Educational Code Review Framework

**Philosophy**: Code reviews in educational contexts prioritize learning and understanding over production efficiency. Reviews should reinforce mathematical concepts and encourage experimentation.

## Review Focus Areas (Priority Order)

### 1. Mathematical Correctness & Understanding

**Primary Questions:**
- Is the mathematical implementation physically accurate?
- Do variable names reflect the mathematical concepts they represent?
- Are intermediate calculations preserved for educational visibility?
- Does the code demonstrate understanding of the underlying physics?

**Review Examples:**
```cpp
// ✅ Good: Educational clarity
float dot_product = normal.dot(light_direction);
float lambertian_term = std::max(0.0f, dot_product);  // n·l clamping
Vector3 diffuse_color = material_albedo * lambertian_term;

// ❌ Avoid: Compressed but opaque
Vector3 color = albedo * std::max(0.0f, n.dot(l));
```

### 2. Educational Transparency

**Review Questions:**
- Are complex calculations broken into educational steps?
- Do comments explain the "why" not just the "what"?
- Can a student follow the mathematical derivation from the code?
- Are educational debugging features preserved?

**Example Educational Comments:**
```cpp
// Ray-sphere intersection using quadratic formula
// Quadratic equation: t² + 2bt + c = 0
Vector3 oc = ray.origin - sphere.center;
float b = oc.dot(ray.direction);  // Linear coefficient
float c = oc.dot(oc) - radius * radius;  // Constant term
float discriminant = b * b - c;  // Determines intersection existence
```

### 3. Progressive Complexity Appropriateness

**Epic-Specific Guidelines:**

**Epic 1 Reviews:**
- Prioritize mathematical transparency over efficiency
- Encourage detailed step-by-step calculations
- Validate educational console output clarity
- Focus on conceptual understanding over optimization

**Epic 2 Reviews:**
- Balance clarity with basic performance awareness
- Ensure UI integration doesn't obscure mathematics
- Validate real-time parameter feedback works correctly
- Check that visual results match mathematical expectations

**Epic 3 Reviews:**
- Ensure Cook-Torrance terms (D, G, F) are clearly separated
- Validate energy conservation through code structure
- Check that microfacet theory is properly implemented
- Balance mathematical accuracy with reasonable performance

**Epic 4 Reviews:**
- Focus on OpenPBR specification compliance
- Ensure production patterns don't sacrifice educational value
- Validate comprehensive material parameter support
- Check educational graduation criteria are met

### 4. Conceptual Code Organization

**Review Focus:**
- Do class and function boundaries match mathematical concepts?
- Is the progression from simple to complex concepts clear?
- Are abstractions introduced at appropriate learning moments?

**Good Conceptual Organization:**
```cpp
class Material {
    // Epic 1: Simple Lambert implementation
    Vector3 evaluate_lambert_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const;
    
    // Epic 3: Added Cook-Torrance implementation  
    Vector3 evaluate_cook_torrance_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const;
    
    // Epic 4: OpenPBR implementation
    Vector3 evaluate_openpbr_brdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const;
};
```

## Code Review Process for Educational Projects

### Student Self-Review Checklist

Before submitting code for review, students should verify:

**Mathematical Validation:**
- [ ] All calculations produce expected results for known test cases
- [ ] Variable names clearly indicate mathematical purpose
- [ ] Energy conservation is maintained (for BRDF implementations)
- [ ] Edge cases are handled with educational explanations

**Educational Clarity:**
- [ ] Complex calculations are broken into understandable steps
- [ ] Comments explain mathematical reasoning and physics concepts
- [ ] Console output (when enabled) shows calculation breakdown
- [ ] Code structure matches conceptual understanding

**Epic-Appropriate Complexity:**
- [ ] Implementation complexity matches epic learning objectives
- [ ] No premature optimization obscures educational goals
- [ ] Progressive features build on previous epic foundations
- [ ] New concepts are introduced with sufficient explanation

### Instructor Review Guidelines

**Review Priorities:**
1. **Conceptual Understanding** (40%): Does the code demonstrate deep understanding?
2. **Mathematical Accuracy** (30%): Are calculations physically and mathematically correct?
3. **Educational Clarity** (20%): Can other students learn from this code?
4. **Code Quality** (10%): Is the code well-organized and maintainable?

**Common Educational Issues to Flag:**

**Mathematical Errors:**
```cpp
// ❌ Common Error: Missing normalization
Vector3 light_dir = light_pos - hit_point;  
float cos_theta = normal.dot(light_dir);  // Wrong! Not normalized

// ✅ Correct Implementation
Vector3 light_dir = (light_pos - hit_point).normalized();  
float cos_theta = std::max(0.0f, normal.dot(light_dir));  // Clamped n·l
```

**Educational Opacity:**
```cpp
// ❌ Avoid: Cryptic implementation
float f = pow(1-abs(n.dot(v)), 5);

// ✅ Prefer: Educational clarity
float cosine_angle = std::abs(normal.dot(view_direction));
float fresnel_power = 5.0f;  // Schlick approximation exponent
float fresnel_factor = std::pow(1.0f - cosine_angle, fresnel_power);
```

### Peer Review Activities

**Learning-Focused Peer Reviews:**

**Code Explanation Sessions:**
- Student explains their ray tracing implementation to classmates
- Focus on mathematical reasoning rather than code syntax
- Encourage questions about physical concepts and derivations
- Document insights and alternative approaches discovered

**Parameter Exploration Reviews:**
- Students demonstrate interactive parameter changes
- Explain visual results in terms of underlying mathematics
- Compare different BRDF implementations side-by-side
- Discuss when different material models are appropriate

**Mathematical Validation Reviews:**
- Students verify each other's energy conservation implementations
- Check mathematical test cases and boundary conditions
- Validate numerical stability for edge cases
- Ensure reference implementations match student code

## Educational Code Review Tools

### Mathematical Validation Helpers

```cpp
// Educational assertion macros for reviews
#define EDUCATIONAL_ASSERT(condition, explanation) \
    if (!(condition)) { \
        if (EducationalMode::is_explanation_enabled()) { \
            std::cout << "Educational Check Failed: " << explanation << std::endl; \
        } \
        assert(condition); \
    }

// Example usage in student code
EDUCATIONAL_ASSERT(std::abs(normalized_vector.length() - 1.0f) < 1e-6, 
                  "Vector normalization failed - length should be 1.0");
```

### Review Documentation Templates

**Student Code Submission Template:**
```markdown
## Epic [X] Code Review Submission

### Mathematical Concepts Implemented:
- [ ] Concept 1: [Brief explanation of understanding]
- [ ] Concept 2: [Brief explanation of understanding]

### Key Implementation Decisions:
1. [Decision]: [Reasoning based on mathematics/physics]
2. [Decision]: [Reasoning based on mathematics/physics]

### Test Results:
- Mathematical validation: [Pass/Fail with details]
- Visual validation: [Expected results achieved? Screenshots if helpful]
- Performance characteristics: [If relevant to epic]

### Questions for Review:
1. [Specific question about implementation approach]
2. [Question about mathematical accuracy or alternative approaches]
```

## Advanced Review Techniques

### Epic Progression Reviews

**Cross-Epic Consistency Checks:**
- Ensure Epic 3 Cook-Torrance builds properly on Epic 1 Lambert foundation
- Validate that Epic 4 OpenPBR maintains educational transparency from earlier epics
- Check that optimization in later epics doesn't sacrifice learning value

**Learning Progression Validation:**
- Can the student explain how each epic builds on previous knowledge?
- Are mathematical concepts properly connected across implementations?
- Does code complexity grow appropriately with student understanding?

### Educational Debugging Reviews

**Debugging as Learning Tool:**
- Review how students diagnose mathematical errors in their implementations
- Ensure debugging process reinforces conceptual understanding
- Validate that educational error messages guide learning rather than just reporting failures

**Performance Learning Reviews:**
- For Epic 4+: Review how students measure and understand performance characteristics
- Ensure optimization discussions include educational context about trade-offs
- Validate that performance improvements don't compromise educational transparency

---

## Key Principles Summary

1. **Mathematical Understanding First**: Code reviews prioritize conceptual mastery over syntactic perfection
2. **Educational Transparency**: Complex calculations should be readable and teachable to other students
3. **Progressive Complexity**: Review standards adapt to epic-appropriate learning objectives  
4. **Collaborative Learning**: Reviews become opportunities for peer teaching and concept reinforcement
5. **Real-World Preparation**: Code quality standards prepare students for industry development while maintaining educational focus

**Remember**: The goal is not perfect code, but perfect understanding expressed through clear, mathematically accurate implementations that demonstrate deep learning of ray tracing concepts.