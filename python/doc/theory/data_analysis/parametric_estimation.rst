.. _parametric_estimation:

Parametric Estimation
---------------------

The objective is to estimate the value of the parameters based on a
sample of an unknown distribution, supposed to be a member of a
parametric family of distributions. We describes here the estimators
implemented for the estimation of the several parametric
models. They are all derived from either the Maximum Likelihood method
or from the method of moments, excepted for the bound parameters that
are systematically modified to strictly include the extreme realizations
of the underlying sample :math:`(x_1,\dots,x_n)`.

Each estimator is detailed its corresponding :ref:`factory <parametric_distibution_factories>`.

.. topic:: API:

    - See the available :ref:`parametric distribution factories <parametric_distibution_factories>`.

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/estimate_normal`
