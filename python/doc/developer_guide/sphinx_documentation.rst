.. _sphinx_doc:

Sphinx documentation
====================

This section deals with the sphinx documentation included in the library repository
and built on top the Python interface.

Sphinx 101
----------

This documentation is powered by the `Sphinx documentation system <http://sphinx-doc.org>`_.
Hence, reading `Sphinx's own documentation <http://sphinx-doc.org/contents.html>`_
might be a good idea for starters! You will also need to learn some basics
about its main featured language: `reStructuredText <http://sphinx-doc.org/rest.html>`_.

Sphinx extensions
~~~~~~~~~~~~~~~~~

numpydoc
    The `numpydoc <https://numpydoc.readthedocs.io/en/latest/format.html#docstring-standard>`_
    Sphinx extension is used for a structured semi-automatic generation of the
    :ref:`User Manual <user_manual>` from the :ref:`docstrings <contribute_docstrings>`
    of OpenTURNS objects.

matplotlib.sphinxext.plot_directive
    The `plot_directive <http://matplotlib.org/sampledoc/extensions.html>`_
    Matplotlib/Sphinx extension is used for *executing* and *testing* the
    code blocks featured in the pages of this documentation, especially in the
    :ref:`examples section <examples>`.

sphinx_gallery.gen_gallery
    The `gen_gallery <https://sphinx-gallery.github.io/stable/gen_modules/sphinx_gallery.gen_gallery.html>`_
    extension from Sphinx-Gallery is used to generate pages from the examples directory.

.. _contribute_docstrings:

Docstrings (in separate SWIG header files)
------------------------------------------

OpenTURNS main featured language is C++. We then use `SWIG <http://swig.org/>`_
in order to generate the Python interface. Hence, docstrings are defined
within **dedicated SWIG header files** (:file:`$OT_SOURCE_DIR/python/src/*_doc.i.in`)
and are then included in the main SWIG header files
(:file:`$OT_SOURCE_DIR/python/src/*.i`).

For instance, the docstrings for the :class:`~openturns.Arcsine` distribution
are defined in :file:`Arcsine_doc.i.in`, and this docstring file is then
included in :file:`Arcsine.i` using a `%include Arcsine_doc.i`.

.. note::

    Note the difference between the name of the docstring file in the source
    tree (:file:`Arcsine_doc.i.in`) and its reference in :file:`Arcsine.i`.
    The :file:`.in` suffix disappeared because the docstring files are
    preprocessed by CMake in order to escape LaTeX backslashes for SWIG and
    Python.

    Note also that the use of double quotes (`"`) in docstrings is forbidden.
    This is because SWIG uses them to delimit the docstrings.

Here are a few recommendations you'd better read in order to help us enhancing
the docstrings coverage.

Docstring conventions
~~~~~~~~~~~~~~~~~~~~~

Please follow `PEP257 <https://www.python.org/dev/peps/pep-0257>`_ and
`numpydoc <https://numpydoc.readthedocs.io/en/latest/format.html#docstring-standard>`_
guidelines for writing the docstrings as well as `PEP8 <http://legacy.python.org/dev/peps/pep-0008/>`_
recommendations for the Examples section (for instance, please don't
``from openturns import *``, indent with 4 spaces, etc. ...).

LaTeX
~~~~~

Using maths is highly recommended for illustrating the mathematical concepts
featured in OpenTURNS. Mathematical expression must use Sphinx ``:math:``
roles for inline maths, and ``.. math::`` directives for equations. These
equations will appear as plain LaTeX at prompt (using the ``help`` command in
Python or the ``?`` suffix in IPython) but Sphinx will render them as PNG images
in the :ref:`User Manual <user_manual>`.

.. note::

    Please use the math commands defined in our :file:`math_notations.sty`
    LaTeX package.

Docstrings & inheritance
~~~~~~~~~~~~~~~~~~~~~~~~

Good news! Docstrings are inherited so that we only need to document the
methods of the parent objects (until we want to make them more specific).

Bridge pattern
~~~~~~~~~~~~~~

An important number of objects use the :ref:`bridge pattern <bridge_pattern>`.
For instance, the :class:`~openturns.Distribution`
object which is the interface class for all probability distributions
has an implementation class **DistributionImplementation** (that we don't
need to expose). And the trick is that the interface class does not inherit from
its implementation object but the children do, so we need to
document them both.

In order to avoid docstrings duplicates though we decided to document the
implementation class with defined blocks. Since we load the
implementation first, we can then refer to the same defined
blocks for documenting the object itself.

For instance the main docstring of the :class:`~openturns.Distribution`
object is defined and referred to in the :file:`DistributionImplementation_doc.i.in`
SWIG header file::

    ...
    %define OT_Distribution_doc
    "Base class for probability distributions."
    %enddef
    %feature("docstring") OT::DistributionImplementation
    OT_Distribution_doc
    ...

and it is then only being referred to in the :file:`Distribution_doc.i.in`
SWIG header file::

    ...
    %feature("docstring") OT::Distribution
    OT_Distribution_doc
    ...

Integration to the build system
-------------------------------

The separate docstring SWIG header files are included in the SWIG header files
of the `openturns repos <https://github.com/openturns>`_, so this does
not need any further integration steps (out of the backslashes escaper CMake
script). A docstring test (`python/test/t_docstring.py`) has been added to the
Python tests.

We added the following CMake variables:

SPHINX_EXECUTABLE
    Path to the *sphinx-build* command.

SPHINX_FLAGS
    This is passed as the options of the sphinx-build command (see
    `sphinx-build invocation <https://www.sphinx-doc.org/en/master/man/sphinx-build.html>`_).

All these targets depend on the rst files located in the sources
(:file:`$OT_SOURCE_DIR/python/doc/*.rst`).

Example gallery
---------------

Example pages are generated from Python scripts in the examples directory
(:file:`$OT_SOURCE_DIR/python/doc/examples/*.py`).

Each page allows one to display notebook-like code or text cells.
Each cell is delimited by a specific marker and the text cells are written in
rst format, for example::

    # %%
    # First cell (text) in *rst* format inside comments
    # Some formula :math:`\lambda = 4`

    # %%
    import openturns as ot
    print("Second cell (code)")
    dist = ot.Normal()

Output cells are rendered at compilation time, so the script must
not be too long to run.

Figures are automatically generated from the matplotlib handles,
for example using the openturns.viewer module::

    # %%
    from openturns.viewer import View
    graph = dist.drawPDF()
    view = View(graph)

Note that sphinx runs the examples in the same Python process so you might want
to reset the random generator at the beginning of your example for consistent
results::

    # %%
    import openturns as ot
    ot.RandomGenerator.SetSeed(0)

For the same reason you might want to reset the ResourceMap default values at
the end if your example sets specific settings::

   # %%
   # Reset default settings
   ot.ResourceMap.Reload()

The special variable `sphinx_gallery_thumbnail_number <https://sphinx-gallery.github.io/stable/configuration.html#choosing-thumbnail>`_
can be used to determine which figure is used as thumbnail in the examples gallery::

    # %%
    # sphinx_gallery_thumbnail_number = 3
