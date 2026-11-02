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
- Commit message body can end with a `Closes #<issue number>` line if it must reference a GH issue
- Add ChangeLog entries for new classes
- Add ChangeLog and TODO files entries for public API deprecations
- Deprecated methods docstrings entries are removed in favor of the new method
- Deprecated classes Sphinx API entries are removed in favor of the new class
- Never break the public API without asking first
- Any new type or exotic construction vs the current codebase must be justified with maximum details: dimensional/mathematical analysis, verification evidence

## C++
- C++ code must follow rules in `python/doc/developer_guide/coding_rules.rst`
- C++ tests are not necessary for APIs that can be tested in Python
- Keep strict compiler warnings enabled as they will be turned into errors on the CI with `-Werror`,
  for example with GCC (see also CMakePresets.json):
  * CMAKE_C_FLAGS: "-Wall -Wextra -Wpedantic -Wshadow"
  * CMAKE_CXX_FLAGS: "-Wall -Wextra -Wpedantic -Wshadow -Wsuggest-override"
- Use const keyword wherever possible
- Use assignment operator for simple types initialization (bool, int, double) but copy constructor for class types initialization.
- Use explicit keyword for class constructors that reduce to a single argument (with or without optional arguments), except when inheriting TypedInterfaceObject.
- Initialize simple types class attributes from header files (with assignment operator)
- Any new `ResourceMap` entry must have its counterpart in `openturns.conf.in`.
- Contiguous `Scalar` storage of a `Point`/`Collection<Scalar>` is passed as `x.data()`, never `&a[0]`
- Avoid hard-coded numerical values: prefer constants from `SpecFunc` (like `SpecFunc::Precision`) or `ResourceMap` entries.
- Enforce const correctness for local variables, methods, etc
- For variable initialization: complex types must use copy-construction but simple types (Scalar, UnsignedInteger, Bool, ...) must use operator=
- Prefer prefix increment (`++i`) rather than postfix increment for loop counters, iterators, etc
- Integer to float type promotion uses the `1.0 * x` idiom, never `static_cast<Scalar>` but `static_cast` stays mandatory for narrowing conversions.
- Validate floating point input values with negated form to handle nan/inf values
  ie use `if (!(epsilon > 0.0)) throw Invalid...` instead of `if (epsilon <= 0.0) throw Invalid...`
- Explicitly use `std::` prefix for math operators (`std::sqrt`, etc)
- Use std::copy if possible to copy continuous data instead of copying item by item with a for-loop
- For branches touching HMAT paths like GaussianProcessFitter/GaussianProcess make sure to configure with `-DUSE_HMAT=ON` as the CI coverage job will
- In hot per-point paths (PDF/logPDF/evaluation loops) hoist transcendentals out of inner loops, stay in log-domain with max
  factoring, cache factor tables at `update()` time, prefer vectorized  `Sample` evaluations, avoid per-evaluation heap allocations
- Keep `RandomGenerator` RNG draws sequential (out of `TBB` regions) for reproducibility.
- Never use `l` (lowercase L) as a variable name. Fix occurrences when encountered.
- While variable and method names use camelCase (`computeLogPDF`), static methods/functions/variables use PascalCase (`GetClassName`).
- No gradient with respect to non-continuous parameters: integer, bool and string parameters throw `NotYetImplemented` instead of finite differences or analytical forms.
- Every new significant/complex algorithm must cite its scientific references in the C++ comments at the relevant place (function or class
  header), with the same level of description as a `python/doc/bibliography.rst` entry: author, title, journal/book,
  volume, pages, year, plus what the reference covers relative to the code. Write the full citation text in the comment, not a bare key.
- Do not re-implement special functions/constants that may already exist in SpecFunc.hxx, or DistFunc.hxx for distribution-related special functions.

## Python
- Add Python testcases for numeric issues
- Python files are linted with specific flake8 flags, see `utils/lint.sh`
- Tests use `ott.assert_almost_equal` for floating-point numeric assertions
- Tests use `with ott.assert_raises(...)` for exception checks
- New classes must be attached to the `python/src/experimental_module.i` SWIG module
- Avoid any SWIG warning as they will be turned into errors on the CI with `-DCMAKE_SWIG_FLAGS="-Werror"`
- New classes land in the experimental module first: `%include` in `python/src/experimental_module.i`, docstring in `python/src/<Class>_doc.i`,
  manual entry as `experimental.<Class>`, Python access via `openturns.experimental`, ChangeLog entry tagged `(openturns.experimental)`.

## ChangeLog
- ChangeLog contains one-liners, no free-prose.
- Use `New classes` alphabetically-ordered entries only for Python-exposed classes  (mention `openturns.experimental` module)
