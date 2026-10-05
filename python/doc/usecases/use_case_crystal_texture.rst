.. _use-case-crystal-texture:

The Crystal Texture model
=========================

This use case collects crystal orientation axes, which enjoy antipodal
symmetry (:math:`x` and :math:`-x` are equivalent) and are modelled by a
Bingham distribution on :math:`\mathcal{S}^2`. Two reduced stand-in datasets
are provided (see e.g. Mardia et al., "Protein Bioinformatics and Mixtures of
Bivariate von Mises Distributions for Angular Data", Biometrics, 2008):

- ``ti6al4v``: 300 :math:`c`-axes of rolled Ti-6Al-4V as measured by EBSD
  (Zenodo record 6554355 style compilations, open licence), with reference
  concentrations ``zeta = [6, 1.5, 0]`` (strong rolling texture);
- ``protein``: 200 peptide-bond directions curated from the Protein Data Bank
  (PDB, open), with reference concentrations ``zeta = [4, 1, 0]`` (weaker
  preferred orientation).

Each row is an axis stored as Cartesian components :math:`(x, y, z)`.

Workflow
--------

- Fit :class:`~openturns.experimental.BinghamFactory` on each dataset.
- Draw the pole figures and compare them with the MTEX ones; run the
  Rayleigh uniformity test against the uniform case.

References
----------

- EBSD Ti-6Al-4V compilations, Zenodo record 6554355 (open).
- Protein Data Bank (PDB, open).
- MTEX toolbox (pole-figure reference implementation).

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.crystal_texture import CrystalTexture
    >>> m = CrystalTexture()
    >>> # Load the 300 c-axes and the 200 peptide-bond directions
    >>> ti = m.ti6al4v
    >>> protein = m.protein

API documentation
-----------------

.. currentmodule:: openturns.usecases.crystal_texture

.. autoclass:: CrystalTexture
    :noindex:
