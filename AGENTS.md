# AGENTS.md

## Configure (only if build/ folder does not exist yet)
```bash
cmake --preset=linux-debug
```

## Build
```bash
cmake --build build --target install --parallel $(( $(nproc) / 2 ))
```

## Tests
- C++ tests in `lib/test/`, Python tests in `python/test/`
- If a test has a corresponding `.expout` file; then the test output diffs against it
- Testcases must be added in the Python folder preferably, new C++ tests are not necessary but existing ones must keep passing
- Build a specific C++test: `cmake --build --target t_Axial_std $(( $(nproc) / 2 ))`
- Run a specific C++ test: `ctest -R cppcheck_Axial_std -V`
- Run a specific Python test: `ctest -R pyinstallcheck_Axial_std -V`
- Tests use `ott.assert_almost_equal` for numeric assertions, `with ott.assert_raises(...)` for exception checks

## Documentation
- Docstring API documentation is located in python/src/*_doc.i files with SWIG directives
- Use R"RAW(...)RAW" delimiters in docstrings when backslashes are needed (ie latex formulas)
- Sphinx documentation is located in python/doc
- For new classes API entries need to be added in python/doc/user_manual

## Conventions
- Use `<Classname|Area>: <Short task overview>` as commit title template if possible
- Commit message can end with `Closes #<issue number>` if it must reference a GH issue
- Avoid unicode characters in source, commit messages
- Add Python testcases for numeric issues
- Python files are linted with flake8, see `utils/lint.sh`
