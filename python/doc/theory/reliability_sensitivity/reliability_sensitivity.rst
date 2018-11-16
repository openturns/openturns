.. _reliability_sensitivity:

Reliability, sensitivity
========================

Sensitivity analysis methods can be used to analyze the respective importance of each
uncertainty source with respect to a probabilistic criterion.
This covers ranking methods for probabilistic criterion on central dispersion (expectation and variance)
and probability of exceeding a threshold / failure probability.


Central dispersion
------------------

.. toctree::
    :maxdepth: 1

    low_discrepancy_sequence
    design_experiment
    optimal_lhs
    taylor_moments
    monte_carlo_moments

Sensitivity analysis
--------------------

.. toctree::
    :maxdepth: 1

    taylor_importance_factors
    ranking_pearson
    ranking_spearman
    ranking_src
    ranking_srrc
    ranking_pcc
    ranking_prcc
    sensitivity_sobol
    sensitivity_ancova
    sensitivity_fast
    importance_form
    sensitivity_form


Probability of exceeding a threshold
------------------------------------

Approximation methods
~~~~~~~~~~~~~~~~~~~~~

.. toctree::
    :maxdepth: 1

    form_approximation
    sorm_approximation
    reliability_index
    strong_maximum_test

Sampling methods
~~~~~~~~~~~~~~~~

.. toctree::
    :maxdepth: 1

    monte_carlo_simulation
    importance_simulation
    directional_simulation
    lhs_simulation
    qmc_simulation
