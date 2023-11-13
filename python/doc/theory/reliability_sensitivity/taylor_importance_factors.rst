.. _taylor_importance_factors:

Taylor importance factors
-------------------------

The importance factors derived from a Taylor expansion are
defined to rank the sensitivity of the output to
the inputs for central dispersion analysis.

We consider the Taylor expansion of a function. We use the notations introduced in
:ref:`Taylor Expansion <taylor_expansion>`. Let :math:`\uX` be the input random vector. We assume that
the marginals of :math:`\uX` are independent. Let :math:`Y = h(\ux)` with
:math:`h: \Rset^d \rightarrow \Rset` be a function with a scalar output.

Refer to :ref:`Taylor Expansion <taylor_expansion>` for details on the expressions
of the first-order and second-order Taylor expansions and to
:ref:`Taylor Expansion Moments <taylor_expansion_moments>` for
details on the approximations of the mean and the variance of :math:`Y`.

The importance factor of :math:`X_i` is defined by:

.. math::

    \eta_i = \frac{ \left(\frac{\partial h}{\partial x_i}(\vect{\mu})\right)^2 \sigma_i^2}{\Var Y}.

If the model is linear (i.e. if the model is equal to its first-order Taylor expansion), then the importance factors sum to one:

.. math::

    \eta_1 + \eta_2 + \ldots + \eta_{d} = 1

These importance factors are also called *importance factors derived from perturbation methods*.


.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

