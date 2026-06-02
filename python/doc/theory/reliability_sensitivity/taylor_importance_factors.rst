.. _taylor_importance_factors:

Taylor importance factors
-------------------------

The importance factors derived from a Taylor expansion are
defined to rank the sensitivity of the output to
the inputs for central dispersion analysis.

We consider the Taylor expansion of a function. We use the notations introduced in
:ref:`Taylor Expansion <taylor_expansion>`. Let :math:`\inputRV = (X_1, \dots, X_\inputDim)` be the
input random vector of dimension :math:`\inputDim`.
Let :math:`Y = \model(\inputRV)` with
:math:`\model: \Rset^\inputDim \rightarrow \Rset` be a model with a scalar output.

Refer to :ref:`Taylor Expansion <taylor_expansion>` for details on the expressions
of the first-order and second-order Taylor expansions and to
:ref:`Taylor Expansion Moments <taylor_expansion_moments>` for
details on the approximations of the mean and the variance of the output random variable :math:`Y`.

The importance factor of :math:`X_i` is defined by:

.. math::
    :label: eta_i

    \eta_i = \frac{ \frac{\partial \model}{\partial x_i}(\vect{\mu}) }{\Var Y} \sum_{j=1}^\inputDim
    \frac{\partial \model}{\partial x_j}(\vect{\mu})\Cov(X_i, X_j).


If the model is linear (i.e. if the model is equal to its first-order Taylor expansion),
then the importance factors sum to one:

.. math::

    \eta_1 + \eta_2 + \ldots + \eta_\inputDim = 1.


Note that in the general case, :math:`\eta_i \in \Rset` and can be negative.

If we assume that the marginals of :math:`\inputRV` are independent, then relation :eq:`eta_i`
simplifies into:

.. math::

    \eta_i = \frac{ \left(\frac{\partial \model}{\partial x_i}(\vect{\mu})\right)^2 \sigma_i^2}{\Var Y}.


for :math:`i \in \{1, ..., \inputDim\}` where :math:`\sigma_i^2 = \Var{X_i}`.
In that case, we have :math:`0 \leq \eta_i \leq 1`. These importance factors are also called
*importance factors derived from perturbation methods* and defined in
[daveiga2022]_ (see eq. 2.6 page 38).

.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`

.. topic:: Examples:

    - See :doc:`/auto_reliability/central_dispersion/plot_estimate_moments_taylor`
