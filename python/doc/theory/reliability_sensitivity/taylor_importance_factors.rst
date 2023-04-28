.. _taylor_importance_factors:

Taylor decomposition importance factors
---------------------------------------

The importance factors derived from a Taylor expansion are
defined to rank the sensitivity of the inputs to
the output for central dispersion analysis.

Let us denote by :math:`\uX` the input random vector.
Assume that the marginals of :math:`\uX` are independent.

We note :math:`Y = h(\ux)` with :math:`h: \Rset^d \rightarrow \Rset`.

Refer to :ref:`taylor_expansion_moments` to get details on the expressions of the approximations:

The first order  Taylor expansion of :math:`h` around :math:`\ux = \mu = \Expect{\uY}` yields to the first order approximation of :math:`\Expect{Y}`:

.. math::
    \Expect{Z} = h(\muX).

The independence of the marginals implies that:

.. math::
    \Var Y = \sum_{i=1}^{d} \left(\frac{\partial h}{\partial x_i} (\mu)\right)^2 \Var X_i

We note :math:`\eta_i` the importance factor of :math:`X_i`, defined by:

.. math::

    \eta_i = \frac{ \left(\frac{\partial h}{\partial x_i} (\mu)\right)^2 \Var X_i}{\Var Z}.

Therefore, the importance factors sum to one:

.. math::

    \eta_1 + \eta_2 + \ldots + \eta_{n_X} = 1

These importance factors are also called *importance factors derived from perturbation methods*.


.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

