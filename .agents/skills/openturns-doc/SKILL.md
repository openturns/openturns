---
name: openturns-doc
description: 'OpenTURNS documentation guidelines: Docstrings, RST structure.'
---

# OpenTURNS documentation guidelines

## When to use
- Before editing docstrings documentation in `python/src/*_doc.i`
- Before editing Sphinx code under `python/doc`

## Structure
- The docstring documentation in `python/src/*_doc.i` constitutes both the inline Python help and the content of the Sphinx API documentation and is part of the SWIG bindings
- The Sphinx HTML documentation in `python/doc` covers all other parts of the generated HTML documentation

## Docstring documentation
- Docstring API documentation source is located in `python/src/*_doc.i` files with SWIG `docstring` directives and lands in generated `.py` wrapper files via SWIG generation
- New classes documentation docstring must contain the preamble warning: "This class is experimental...", see any class from `python/src/experimental_module.i`
- Use R"RAW(...)RAW" verbatim delimiters in docstrings when backslashes are needed (ie latex formulas)
- Always put the closing `)RAW"` delimiter on its own line: never let docstring content ending in `)` share the terminator line (ie `...))RAW"`)
- Use latex macros from python/doc/math_notations.sty wherever possible to uniformize notations
- Use :any: instead of :ref: directives inside docstrings only, to ensure valid links in third-party Sphinx documentations.
- Docstrings follow numpydoc (sections Parameters/Returns/See Also/Notes/Examples in that order, formatting syntax, etc).
- SWIG typemaps allows for sequence-like object parameters when container classes `Point`, `Description`, `Sample` C++ types are expected
  so `Parameters` can declare argument type as `sequence of ...`, while `Returns` section will keep the explicit return type
- The "Notes" numpydoc section will appear after the methods list,
  so only use it for specific elements like ResourceMap keys, corner cases etc
  to avoid important elements to be separated from the main documentation body.
- Numpydoc `ClassDoc` silently collapses blank lines between a `.. math::` marker and its content (and before a following paragraph) when a
  `Parameters` section precedes the math: directives then parse as literal text and citations stay unresolved, failing sphinx `-W`.
  Keep citation-bearing prose in a named section when `Parameters` precedes math.
- Cross-reference roles with an explicit `openturns.` prefix use the tilde
  shorthand (`:class:`~openturns.Point``); bare roles are unaffected.
- Every numpydoc section header must be preceded by a blank line, else
  section parsing silently breaks.
- No docstrings or manual entries for deprecated classes/methods.
- Deprecating a method: first body line `LOGWARN("<Class>::<method> is
  deprecated, use <replacement>")` then delegate; docstring gets `..
  deprecated:: <next-version>` + replacement right after the summary;
  remove it from `python/doc/user_manual/*.rst` autosummary lists.
- Mathematical explanations go before constructor parameters (below the
  experimental warning if present); heavy content moves to the theory
  documentation with a short linked summary in the API docs.

### ResourceMap entries
- The ResourceMap entries of a class (if any) must be listed in the `Notes` numpydoc section of its docstring with type and value matching
  the contents of lib/src/Base/Common/ResourceMap.cxx and lib/etc/openturns.conf.in
- A class with associated `ResourceMap` keys must document them in a single "Notes" section as a bullet list (9 or fewer keys) or a `.. list-table::`
  (10 or more). Merge into an existing "Notes" section; duplicate "Notes" headings break doctest extraction and the doc build. The section stays
  before "Examples" (numpydoc order: "See Also" precedes "Notes").
- Each entry gives key type, purpose, and default value, and ends with a period. Key names use double backticks;
  types are mandatory (`Scalar`, `Bool`, `String`, `Integer`, `UnsignedInteger`); enumerated `String` values are listed when 5 or
  fewer, else link to the full list or a listing method. `Bool` defaults use Python syntax (`True`/`False`).
- Introductory sentence cites `:class:`~openturns.ResourceMap`` once: `The following :class:`~openturns.ResourceMap` keys are used:`
  (singular `key is used:` for one key).
- Table form uses `.. list-table::` with `:header-rows: 1` and four columns (key, type, default value, description).

## Sphinx documentation
- The HTML documentation is built using Sphinx-doc generator from python/doc
- Sphinx documentation is located in python/doc
- For new classes API entries need to be added in python/doc/user_manual
- It is recommended to add a dedicated gallery example in `python/doc/examples` when introducing a new feature that does not already follow a well-known concept
  (for example it would not be mandatory for a new Distribution class)
- Avoid any Sphinx warning as CI will error (`-W` option), like unreferenced citations in bibliography.rst, too short title underline, unresolvable cross-references, etc.
- Tables use `.. list-table::` syntax with `:header-rows: 1` and `:widths:` instead of ASCII grid tables syntax.
