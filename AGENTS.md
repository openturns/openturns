# AGENTS.md

This file provides guidance to AI agents such as OpenCode when working with code in this repository.

## Project skill files - read these first

A number of short `SKILL.md` files under `.agents/skills/` are the canonical references for the most common tasks an AI agent performs in this repo. Open the matching file **before** improvising; each is one or two pages and contains the specific commands, flags, and conventions that AGENTS.md only summarises:

- **`.agents/skills/openturns-conventions/SKILL.md`** — source and contribution conventions: code formatting, C++/Python style, commit message style.  Read before writing or editing `lib/` or `python/` code, comments, or commit messages.
- **`.agents/skills/openturns-test/SKILL.md`** — the canonical way to run the test suite and individual test cases. Read before running any test.
- **`.agents/skills/openturns-doc/SKILL.md`** — editing Python docstrings or HTML Sphinx documentation. Read before any change to `python/doc/` or `python/src/*_doc.i`.
- **`.agents/skills/openturns-debug/SKILL.md`** — debugging the C++ or SWIG layer, debug flags, gdb commands.
- **`.agents/skills/openturns-review/SKILL.md`** — the elements necessary for code review when reviewing pull requests and code changes.
- **`.agents/skills/openturns-ci-repro/SKILL.md`** — reproducing a GitHub Actions Linux CI failure locally when it does not happen on your machine. Read before chasing a failure that only shows up in CI.

## What is OpenTURNS

At its core it is an object-oriented C++ and Python library dedicated to advanced probabilistic modelling
(distribution algebra, dependence modelling with copulas, non-parametric distributions, distribution transformations, etc),
and with a second layer of statistical and uncertainty quantification capabilities
(functional modelling, sensitivity analysis, reliability analysis, surrogate modelling, etc).

## Build
The library is built using the CMake build system, to configure and build:
```bash
cmake --preset=linux-debug  # only if the build/ folder does not exist; to switch preset, remove build/ first
cmake --build build --target install --parallel $(( $(nproc) / 2 ))
```

## C++ library
- C++ library is located in `lib/src/`
- The subfolders `lib/src/Base/` and `lib/src/Uncertainty/` mark the distinction between non-stochastic and stochastic layers
- Follow the rules in **`.agents/skills/openturns-conventions/SKILL.md`** before editing.

## Python bindings
- The C++ library is exposed to Python via SWIG
- SWIG interface entries for each class live in `python/src/*.i`
- Docstring documentation files `python/src/*_doc.i` are part of the Python bindings
- Python tests are located in `python/test/`
- Follow the rules in **`.agents/skills/openturns-conventions/SKILL.md`** before editing.
- Always rebuild before testing changes: `cmake --build build --target install --parallel $(( $(nproc) / 2 ))`
- Test bindings from the install dir: `PYTHONPATH=$PWD/build/install/lib/python3.*/site-packages python3 -c "import openturns as ot; print(ot.__version__)"`

## Tests
- Tests run from the `build/` folder with CTest, e.g. `ctest -R pyinstallcheck_Axial_std --output-on-failure` from `build/`
- Read **`.agents/skills/openturns-test/SKILL.md`** for `-R`/`-E` filtering, `-j` parallelism, and C++ vs Python specifics before running or editing tests.

## Documentation
- HTML documentation is built with Sphinx from `python/doc/`; API docstrings live in `python/src/*_doc.i`
- Read **`.agents/skills/openturns-doc/SKILL.md`** before any change to `python/doc/` or `python/src/*_doc.i`.
