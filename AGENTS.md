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
(distribution algebra, copulas dependence modelling with copulas, non-parametric distributions, distribution transformations, etc),
and with a second layer of statistical and uncertainty quantification capabilities
(functional modelling, sensitivity analysis, reliability analysis, surrogate modelling, etc).

## Build
The library is built using CMake build system, to configure and build:
```bash
cmake --preset=linux-debug  # only if build/ folder does not exist
cmake --build build --target install --parallel $(( $(nproc) / 2 ))
```

## C++ library
- C++ library is located into lib/src/
- The subfolders lib/src/Base/ and lib/src/Uncertainty/ mark the distinction between non-stochastic and stochastic layers

See **`.agents/skills/openturns-conventions/SKILL.md`** for more details.

## Python bindings
- The C++ library is exposed into Python via SWIG generation tool
- SWIG files entries for each class live in python/src/*.i
- Docstring documentation files python/src/*_doc.i are part of the Python bindings
- Python tests are located in python/test/

See **`.agents/skills/openturns-conventions/SKILL.md`** for more details.

## Tests
Tests are run from the build folder with CTest utility

See **`.agents/skills/openturns-test/SKILL.md`** for more details.

## Documentation
The HTML documentation is built using Sphinx-doc generator from python/doc

See **`.agents/skills/openturns-doc/SKILL.md`** for more details.
