.. _meta_modeling:

Meta modeling
=============

The current section is dedicated to the building of an analytical approximation of the model of a given model.
Such an approximation is commonly referred to as a response surface in the literature.
This methodology is relevant if each model evaluation is time consuming.
Indeed, once a surrogate model has been built up, the various propagation steps may be applied at a negligible cost.

Local approximation
-------------------

.. toctree::
    :maxdepth: 1

    taylor_expansion


Least square surrogate models
-----------------------------

.. toctree::
    :maxdepth: 1

    polynomial_least_squares
    polynomial_sparse_least_squares


Gaussian Process Regression
---------------------------

.. toctree::
    :maxdepth: 1

    gaussian_process_regression


Functional chaos
----------------

.. toctree::
    :maxdepth: 1

    orthogonal_polynomials
    chaos_basis
    enumeration_strategy
    functional_chaos
    pce_cross_validation


Surrogate validation
--------------------

.. toctree::
    :maxdepth: 1

    cross_validation
