.. _uncertainty_quantification:

=====================================
Quantification of uncertainty sources
=====================================

This section is organized in three parts.
The first one gives the list of probabilistic uncertainty models proposed.
The second part gives an overview of the content of the statistical toolbox that may be used to build these uncertainty models if data are available.
The last part is dedicated to the mathematical description of each method.

Available probabilistic models
==============================

Non-parametric models
---------------------
- Kernel smoothing :class:`~openturns.KernelSmoothing`
- Empirical cumulative distribution function

Parametric models
-----------------
- :ref:`Usual distributions <parametric_distributions>`
- :ref:`Copulas - a mathematical tool for multi-dimensional distirbutions <copulas>`
- Random mixture - affine combination of independent univariate distributions :class:`~openturns.RandomMixture`

Methods description
===================

.. toctree::
    :maxdepth: 2

    kolmogorov_test
