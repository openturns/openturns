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
- Docstring API documentation is located in python/src/*_doc.i files with SWIG directives
- New classes documentation docstring must contain the preamble warning: "This class is experimental...", see other existing classes in experimental_module.i
- Use R"RAW(...)RAW" verbatim delimiters in docstrings when backslashes are needed (ie latex formulas)
- Use latex macros from python/doc/math_notations.sty wherever possible to uniformize notations
- Use :any: instead of :ref: directives inside docstrings only, to ensure valid links in third-party Sphinx documentations.
- Docstrings follow numpydoc conventions (Parameters/Notes/Examples sections, formatting, etc).
- The "Notes" numpydoc section will appear after the methods list,
  so only use it for specific elements like ResourceMap keys, corner cases etc
  to avoid important elements to be separated from the main documentation body.
- The ResourceMap entries of a class (if any) must be listed in the Notes numpydoc section of its docstring with type and value matching the contents of lib/src/Base/Common/ResourceMap.cxx and lib/etc/openturns.conf.in

## Sphinx documentation
- The HTML documentation is built using Sphinx-doc generator from python/doc
- Sphinx documentation is located in python/doc
- For new classes API entries need to be added in python/doc/user_manual
- It is recommended to add a dedicated gallery example in `python/doc/examples` when introducing a new feature that does not already follow a well-known concept (for example it would not be mandatory for a new Distribution class)
