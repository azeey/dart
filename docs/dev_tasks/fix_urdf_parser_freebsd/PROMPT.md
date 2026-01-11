# Task: Fix FreeBSD CI for PR #2414

## Context

Work on PR #2414 (`fix/urdf-parser-freebsd-rtti` branch) to fix FreeBSD CI failures with **proper root-cause fixes instead of skipping tests**.

- **Repository**: https://github.com/dartsim/dart
- **PR**: https://github.com/dartsim/dart/pull/2414
- **Branch**: `fix/urdf-parser-freebsd-rtti`

## User Requirements

**CRITICAL**: The user explicitly said: "No, DO NOT skip tests. Instead you fix the root causes."

## Completed Work

### 1. Fixed QEMU CPU model (`docker/freebsd/entrypoint.py`)

- Added `-cpu host` when KVM available, `-cpu max` otherwise
- Fixes most "Illegal instruction" crashes caused by QEMU's default CPU lacking SSE4.2/AVX2 instructions

### 2. Removed test exclusions (`.github/workflows/ci_freebsd.yml`)

- Deleted `FREEBSD_VM_TEST_EXCLUDE_REGEX` environment variable
- All tests now run instead of being skipped

### 3. Guarded SDF-dependent tests with CMake (`tests/integration/CMakeLists.txt`)

- Wrapped `INTEGRATION_simulation_MomentumConservation` and `INTEGRATION_simulation_MimicConstraint` with `if(DART_ENABLE_SDFORMAT)`
- Reverted `GTEST_SKIP()` workarounds back to `ASSERT_TRUE(world)` in test files

### 4. Fixed `Read.AutoDetectsSdfWorld` test (`tests/integration/io/test_Read.cpp`)

- Added `#if DART_HAS_SDFORMAT` guard around the assertion
- When SDF is disabled, expects `nullptr` instead of asserting non-null

## Current State

- All changes pushed to remote
- CI running: https://github.com/dartsim/dart/pull/2414
- Local FreeBSD test results: 73/74 tests passed (99% pass rate)
- AltLinux test: 78/78 tests passed (100%)

## Known Issue: BalanceConstraint "Illegal instruction"

One test still fails on FreeBSD with "Illegal instruction":
- Test: `UNIT_constraint_BalanceConstraint`
- Root cause: The test uses geometry operations (support polygon, convex hull) that may trigger SIMD instructions incompatible with QEMU even with KVM

Investigation notes:
- The test was added in PR #2175 (after release-6.16)
- It didn't run on main CI previously because earlier tests failed with `--stop-on-failure`
- Attempted fix with `EIGEN_DONT_VECTORIZE` caused worse issues (Bus error in CollisionGroups)
- The issue is likely with FreeBSD packages (Eigen, FCL) compiled with CPU-specific optimizations

This should be addressed in a follow-up issue as it's not related to the SDF/URDF parsing fixes.

## Commits

```
8083772e59c Guard SDF test in test_Read.cpp for non-SDF builds
ac2f76f64a7 Merge branch 'main' into fix/urdf-parser-freebsd-rtti
38b7ecacd29 Guard SDF-dependent tests with DART_ENABLE_SDFORMAT in CMake
f2fe46119b1 Fix FreeBSD CI: use proper CPU model instead of skipping tests
```

## Key Files Modified

| File                                                         | Change                                  |
| ------------------------------------------------------------ | --------------------------------------- |
| `docker/freebsd/entrypoint.py`                               | QEMU CPU model fix                      |
| `.github/workflows/ci_freebsd.yml`                           | Removed test exclusions                 |
| `tests/integration/CMakeLists.txt`                           | Added `if(DART_ENABLE_SDFORMAT)` guards |
| `tests/integration/simulation/test_MimicConstraint.cpp`      | Reverted GTEST_SKIP to ASSERT_TRUE      |
| `tests/integration/simulation/test_MomentumConservation.cpp` | Reverted GTEST_SKIP to ASSERT_TRUE      |
| `tests/integration/io/test_Read.cpp`                         | Added `#if DART_HAS_SDFORMAT` guard     |

## Relevant Macros

- `DART_HAS_SDFORMAT` - CMake compile definition, set to 1 when SDF enabled
- `DART_IO_HAS_URDF` - CMake compile definition, set to 1 when URDF enabled
- `DART_ENABLE_SDFORMAT` - CMake option to enable/disable SDF support

## Monitor CI

```bash
gh run list --branch fix/urdf-parser-freebsd-rtti --limit 5
gh pr checks 2414
```
