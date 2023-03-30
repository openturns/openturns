.. _taylor_importance_factors:

Taylor decomposition importance factors
---------------------------------------

The importance factors derived from a Taylor expansion are
defined to rank the sensitivity of the inputs to
the output for central dispersion analysis.

Let us denote by :math:`\uX` the input random vector.
Assume that the marginals of :math:`\uX` are independent.
Suppose that :math:`Z = h(\ux)` is a real function of the
input, i.e. :math:`n_Z = 1`.
Assume that the order 1 Taylor expansion of the function :math:`h`
at the point :math:`\ux = \muX` is exact, i.e.:

.. math::
    h(\ux) = h(\muX)
    + \sum_{i=1}^{n_X} \frac{\partial h}{\partial x_i} (\muX) (x_i - \mu_i)

where:

-  :math:`\muX` is the mean of the input random vector,

-  :math:`\frac{\partial h}{\partial x_i} (\muX)`
   is the partial derivative of the model :math:`h`
   with respect to the i-th input variable,
   evaluated at the point :math:`\ux`.

Therefore the expectation of :math:`Z` is:

.. math::
    \Expect{Z} = h(\muX).

The independence of the marginals implies:

.. math::
    \Var Z = \sum_{i=1}^{n_X} \left(\frac{\partial h}{\partial x_i} (\muX)\right)^2 \sigma_i^2

where:

-  :math:`\Var Z` is the variance of the output variable,

-  :math:`\sigma_i^2 = \Var X_i` is the variance of the i-th input variable.

Let :math:`\cF_i` be the importance factor of the i-th input
variable, defined by:

.. math::
    \cF_i = \frac{\left(\frac{\partial h}{\partial x_i} (\muX)\right)^2 \sigma_i^2}{\Var Z}.

Therefore, the importance factors sum to one:

.. math::
    \cF_1 + \cF_2 + \ldots + \cF_{n_X} = 1

| Each coefficient :math:`\frac{\partial h(\ux)}{\partial x^i}` is a
  linear estimate of the number of units change in the variable
  :math:`y=h(\ux)` as a result of a unit change in the variable
  :math:`x^i`. This first term depends on the physical units of the
  variables and is meaningful only when the units of the model are
  known. In the general case, as the variables have different physical
  units, if :math:`i\neq j`, it is not possible to compare
  :math:`\frac{\partial h(\ux)}{\partial x_i}` with
  :math:`\frac{\partial h(\ux)}{\partial x_j}`.
  This is the reason why the importance factor are
  normalized. These factors enable to make the results
  comparable independently of the original units of the inputs of the
  model.
| To summarize, the coefficients :math:`\cF_1, ..., \cF_{n_X}`
  represent a linear estimate of the change in the output variable
  :math:`z = h(\ux)` caused by a small change in the input variable
  :math:`x_i`. The importance factors are independent of the original
  units of the model, and are comparable with each other.

These are also called importance factors derived from perturbation methods.


.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

