.. _meta_modeling:

Meta modeling
=============

The current section is dedicated to the building of an analytical approximation of the response of a given model.
Such an approximation is commonly referred to as a response surface in the literature.
This methodology is relevant if each model evaluation is time consuming.
Indeed, once a response surface has been built up, the various propagation steps may be applied at a negligible cost.
A special focus will be given to polynomial response surfaces.


.. toctree::
    :maxdepth: 1

    taylor_expansion
    polynomial_least_squares
    polynomial_sparse_least_squares
    kriging
    kriging_robust

Functional chaos
----------------

.. toctree::
    :maxdepth: 1

    orthogonal_polynomials
    chaos_basis
    enumeration_strategy
    functional_chaos
    cross_validation
