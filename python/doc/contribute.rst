.. _contribute:

==================
How to contribute?
==================

If you are willing to contribute to the OpenTURNS project, we need you! There
are several ways to contribute.

Library enhancement
===================

Feedback
--------

Providing us with feedback on your own experience using OpenTURNS is extremely
valuable for the other users as well as our developers. There exists many
channels for you to contact us:

  - We host two mailing lists at sourceforge, one for `users <http://openturns.org/mailman/listinfo/users>`_
    and the other one for `developers <http://openturns.org/mailman/listinfo/developers>`_.
  - Bugs can be reported on our `trac server <http://trac.openturns.org>`_.
  - Once a year, one of the partners hosts the OpenTURNS Users day which is a
    unique occasion for users to meet up and share their experience using the
    library.

.. warning::

    Please consider using the mailing list first if you have any doubt whether
    you have bumped into a real bug or not.

Code
----

If you are willing to contribute the actual development of the library, please
consider reading our coding guidelines in the OpenTURNS developer's guide and
contact us on the developer's mailing list.

Proposing a pull request
------------------------

Patches are welcome to our library repository: https://github.com/openturns/openturns.

Here are the steps required to propose a modification with git:

1. Setup git for first time use::

    $ git config --global user.name "John Doe"
    $ git config --global user.email "johndoe@example.com"
    $ git config --global branch.autosetuprebase remote

2. Register on `GitHub <https://github.com>`_ and add a new ssh key to https://github.com/settings/ssh::

    $ ssh-keygen -t rsa -b 4096 -f ~/.ssh/id_rsa-github -C "johndoe@example.com"
    $ xclip ~/.ssh/id_rsa-github.pub # copy the public key to clipboard

3. Fork the library repository https://github.com/openturns/openturns and clone it via ssh::

    $ git clone git@github.com:doe/openturns.git


4. Create a branch from master with a relevant topic name::

    $ git checkout -b branch_name master


5. Commit your changes, split your work in as much commits as needed to ease reviewing::

    $ git add <files> # Add current state of files to commit
    $ git commit -m "Comment on commit 1/2" # Commit added files
    ...
    $ git add <files>
    $ git commit -m "Comment on commit 2/2"


6. Push the new branch your personal repository::

    $ git push --set-upstream origin branch_name


7. Create a pull request on https://github.com/openturns/openturns

That's it, your code is ready for reviewing, but it may not be accepted as is.

Checkout the comments made after your pull request and the automated test result.

You may want to push more commits to that branch as required.

Propose a separate pull request for each topic you want to address.

Here are more resources on `using pull requests <https://help.github.com/articles/using-pull-requests/>`_
and `Git <https://git-scm.com/book/en/v2>`_.


Keep your local repository in sync
----------------------------------

You may want to keep your personal repository in sync with the main repository
before starting new developments.

1. Add upstream remote repository::

    $ git remote add upstream https://github.com/openturns/openturns.git


2. Retrieve usptream::

    $ git fetch upstream


3. Merge upstream master branch to your local personal master branch::

    $ git checkout master
    $ git merge upstream/master


4. Push to your remote personal repository::

    $ git push


Delete a branch
---------------

Once your pull-request is merged, you may want to delete the branch.

1. Delete local branch::

    $ git branch -d branch_name

2. Delete remote branch::

    $ git push origin :branch_name


Tagging a release
-----------------

0. List existing tags::

    $ git tag


1. Get the master branch::

    $ git checkout master


2. Create the tag on local repository::

    $ git tag -a v2.0 -m 'version 2.0'


3. Push the tag on the remote repository::

    $ git push origin v2.0


Documentation
=============

The documentation of OpenTURNS is mainly 3-folds. It contains:

  - a :ref:`User Manual <user_manual>`,
  - an :ref:`Examples Guide <examples>`,
  - a :ref:`Reference Guide <theory>`, which is more literate and deals with the
    theoretical background of the methods featured in the library.

You may also want to enhance the other satellite pages such as this one or
theme-up the site.

Sphinx powered
--------------

Sphinx 101
~~~~~~~~~~

This documentation is powered by the `Sphinx documentation system <http://sphinx-doc.org>`_.
Hence, reading `Sphinx's own documentation <http://sphinx-doc.org/contents.html>`_
might be a good idea for starters! You will also need to learn some basics
about its main featured language: `reStructuredText <http://sphinx-doc.org/rest.html>`_.

Sphinx extensions
~~~~~~~~~~~~~~~~~

numpydoc
    The `numpydoc <https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt>`_
    Sphinx extension is used for a structured semi-automatic generation of the
    :ref:`User Manual <user_manual>` from the :ref:`docstrings <contribute_docstrings>`
    of OpenTURNS objects.

matplotlib.sphinxext.plot_directive
    The `plot_directive <http://matplotlib.org/sampledoc/extensions.html>`_
    Matplotlib/Sphinx extension is used for *executing* and *testing* the
    code blocks featured in the pages of this documentation, especially in the
    :ref:`examples section <examples>`.

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

.. warning::

    Note also that the use of double quotes (`"`) in docstrings is forbidden.
    This is because SWIG uses them to delimit the docstrings.

Here are a few recommendations you'd better read in order to help us enhancing
the docstrings coverage.

Docstring conventions
~~~~~~~~~~~~~~~~~~~~~

Please follow `PEP257 <https://www.python.org/dev/peps/pep-0257>`_ and
`numpydoc <https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt>`_
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

Implementation patterns
~~~~~~~~~~~~~~~~~~~~~~~

The OpenTURNS library counts an important number of parent objects with an
:class:`Implementation` pattern. For instance, the :class:`~openturns.Distribution`
object which is the base class for all probability distributions in OpenTURNS
has a :class:`~openturns.DistributionImplementation` pattern (that we don't
need to expose). And the trick is that the base object does not inherit from
its :class:`Implementation` pattern but the children do, so we need to
document them both.

In order to avoid docstrings duplicates though we decided to document the
:class:`Implementation` pattern with defined blocks. Since we load the
:class:`Implementation` patterns first, we can then refer to the same defined
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

Integration to the building suite
---------------------------------

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
    `sphinx-build invocation <http://sphinx-doc.org/invocation.html#invocation-of-sphinx-build>`_).

All these targets depend on the rst files located in the sources 
(:file:`$OT_SOURCE_DIR/python/doc/*.rst`).

Todo list
---------

Here a list of things to do in order to complete this documentation:

.. todolist::
