---
name: openturns-conventions
description: 'OpenTURNS source and contribution conventions: Python code formatting, C/C++ style, code rules, commit message style, ChangeLog entries for user visible release notes. Read before writing or editing in lib/ or python/ code, comments, or commit messages.'
---

# OpenTURNS coding and contribution conventions

## When to use
- Before editing C/C++ code under `lib/`
- Before editing Python code under `python/`

# General
- Avoid unicode characters in source, commit messages
- Use `<Classname|Area>: <Short task overview>` as commit title template if possible
- Commit message can end with `Closes #<issue number>` if it must reference a GH issue
- Add ChangeLog entries for new classes
- Add ChangeLog and TODO files entries for public API deprecations
- Deprecated methods docstrings entries are removed in favor of the new method
- Deprecated classes Sphinx API entries are removed in favor of the new class

## C++
- C++ code must follow rules in `python/doc/developer_guide/coding_rules.rst`
- C++ tests are not necessary for APIs that can be tested in Python
- Use const keyword wherever possible
- Use assigment operator for simple types initizalization (bool, int, double) but copy ctor for class types initialization.
- Use explicit keyword for class ctors that reduces to a single argument (With or without optional arguments).
- Initizalize simple types class attributes from header files (with assigment operator)

## Python
- Add Python testcases for numeric issues
- Python files are linted with flake8, see `utils/lint.sh`
- Run a specific Python test: `ctest -R pyinstallcheck_Axial_std -V`
- Tests use `ott.assert_almost_equal` for floating-point numeric assertions
- Tests use `with ott.assert_raises(...)` for exception checks
- New classes must be attached to experimental_module.i SWIG module
