# AGENTS.md

## Configure
```bash
cmake --preset=linux-debug  # only if build/ folder does not exist
```

## Build
```bash
cmake --build build --target install --parallel $(( $(nproc) / 2 ))
```

## Library
- C++ library is located into lib/src
- Mention new classes into the ChangeLog file

## Python bindings
- SWIG files entries must be created into python/src/*.i
- New classes must be attached to experimental_module.i

## Tests
- C++ tests in `lib/test/`, Python tests in `python/test/`
- If a test has a corresponding `.expout` file; then the test output diffs against it
- Testcases must be added in the Python folder preferably, new C++ tests are not necessary but existing ones must keep passing
- Build a specific C++test: `cmake --build --target t_Axial_std $(( $(nproc) / 2 ))`
- Run a specific C++ test: `ctest -R cppcheck_Axial_std -V`
- Run a specific Python test: `ctest -R pyinstallcheck_Axial_std -V`
- Tests use `ott.assert_almost_equal` for floating-point numeric assertions
- Tests use `with ott.assert_raises(...)` for exception checks

## Documentation
- Docstring API documentation is located in python/src/*_doc.i files with SWIG directives
- Use R"RAW(...)RAW" delimiters in docstrings when backslashes are needed (ie latex formulas)
- Use latex macros from python/doc/math_notations.sty wherever possible to uniformize notations
- New classes documentation must contain the preamble warning: "This class is experimental..."
- Sphinx documentation is located in python/doc
- For new classes API entries need to be added in python/doc/user_manual
- The "Notes" numpydoc section will appear after the methods list,
  so only use it for specific elements like ResourceMap keys, corner cases etc
  to avoid important elements to be separated from the main documentation body.

## Conventions
- Use `<Classname|Area>: <Short task overview>` as commit title template if possible
- Commit message can end with `Closes #<issue number>` if it must reference a GH issue
- Avoid unicode characters in source, commit messages
- Add Python testcases for numeric issues
- Python files are linted with flake8, see `utils/lint.sh`
