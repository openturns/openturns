---
name: openturns-test
description: 'OpenTURNS testing guidelines: ctest syntax, expout files. Read before writing or editing tests in lib/test or python/test, or running tests.'
---

# OpenTURNS testing guidelines

## When to use
- Before editing C++ tests code under `lib/test/`
- Before editing Python code under `python/test/`
- Before launching C++ or Python tests

## General
- C++ tests in `lib/test/`, Python tests in `python/test/`
- Each test is enabled from the macro entry `ot_*_test(...)` in the corresponding CMakeLists.txt
- If a test has a corresponding `.expout` file and is not marked IGNOREOUT in CMakeLists; then this test output is tested against the content of the `.expout` file
- New tests should use IGNOREOUT and no .expout files (cmake errors if `IGNOREOUT` is missing and the `.expout` file does not exist)
- New testcases must be added in the Python folder
- No new C++ tests (except if the API to test is not exposed in Python) but existing ones must keep passing
- Python coverage must be typically above 90%
- Keep tests short, typically <10s
- When the implementation is mathematically correct, adapt the tests (expected values, formulas), never weaken the code to match stale expectations.

## How to run
- Run tests through ctest utility from the build folder
- Always use `-j` parallelism option when launching several tests: `ctest -j $(( $(nproc) / 2 ))`
- The `-R` option can be used with a regex to filter tests to run: `ctest -R "pyinstallcheck_(Axial|Box)_std"`
- The `-E` option can be used with a regex to filter out tests: `ctest -E cppcheck`
- Use `--output-on-failure --timeout 100 --schedule-random` options as done on CI

## C++ tests
- C++ tests are executables linking against the main library
- Build a specific C++ test: `cmake --build build --target t_Axial_std --parallel $(( $(nproc) / 2 ))`
- Run a specific C++ test: `ctest -R cppcheck_Axial_std --output-on-failure -V`
- C++ tests are not built by default, but if you need to run all first build them all with `cmake --build build --target tests --parallel $(( $(nproc) / 2 ))`
- Make sure to rebuild C++ test targets prior to running the corresponding tests
- If you ever need to rebuild all C++ tests: `cmake --build build --target tests --parallel $(( $(nproc) / 2 ))`

## Python tests
- Python tests need the SWIG bindings to be fully built: `cmake --build build --target install --parallel $(( $(nproc) / 2 ))`
- Run a specific Python test: `ctest -R pyinstallcheck_Axial_std --output-on-failure -V`
- Built-in random generator seed is initialized in a deterministic manner by default
- Python tests use `ott.assert_almost_equal` for floating-point numeric assertions
- Python tests use `with ott.assert_raises(<ExceptionType>)` exception contexts for exception checks, but note that Exception mapping does not follow C++ exceptions
- Distribution tests should use `ott.DistributionValidation`
- Run the whole Python testsuite: `OPENTURNS_NUM_THREADS=1 OMP_NUM_THREADS=1 ctest -R pyinstallcheck -j $(( $(nproc) / 2 )) --output-on-failure`
