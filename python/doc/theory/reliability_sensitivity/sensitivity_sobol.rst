Sensivity analysis using Sobol indices
--------------------------------------

This method deals with analysing the influence the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` has on a random variable
:math:`Y^k` which is being studied for uncertainty. Here we attempt to evaluate the
part of variance of :math:`Y^k` due to the different components :math:`X^i`.

The estimators for the mean of  :math:`m_{Y^j}` and the standard deviation
:math:`\sigma` of :math:`Y^k` can be obtained from a first sample, as Sobol
indices estimation requires two samples of the input variables : :math:`(X^1,\ldots,X^{n_X})`,
that is two sets of *N* vectors of dimension :math:`n_X`
:math:`(x_{11}^{(1)},\ldots,x_{1n_X})^{(1)},\ldots,(x_{N^1}^{(1)},\ldots,x_{Nn_X}^{(1)})`
and :math:`(x_{11}^{(2)},\ldots,x_{1n_X})^{(2)},\ldots,(x_{N^1}^{(2)},\ldots,x_{Nn_X}^{(2)})`

The estimation of sensivity indices for first order consists in estimating the quantity

.. math::

    V_i = \Var{\Expect{ Y^k \vert X_i}} = \Expect{ \Expect{Y^k \vert X_i}^2}  - \Expect{\Expect{ Y^k \vert X_i }} ^2 = U_i - \Expect{Y^k} ^2

Sobol proposes to estimate the quantity :math:`U_i = \Expect{\Expect{ Y^k \vert X_i}^2}`
by swapping every variables in the two samples except the variable :math:`X_i` between the two calls of the function:

.. math::

    \hat U_i = \frac{1}{N}\sum_{k=1}^N{ Y^k\left( x_{k1}^{(1)},\dots, x_{k(i-1)}^{(1)},x_{ki}^{(1)},x_{k(i+1)}^{(1)},\dots,x_{kn_X}^{(1)}\right) \times Y^k\left( x_{k1}^{(2)},\dots,x_{k(i-1)}^{(2)},x_{ki}^{(1)},x_{k(i+1)}^{(2)},\dots,x_{kn_X}^{(2)}\right)}

Then the :math:`n_X` first order indices are estimated by

.. math::

    \hat S_i = \frac{\hat V_i}{\hat \sigma^2} = \frac{\hat U_i - m_{Y^k}^2}{\hat \sigma^2}

For the second order, the two variables :math:`X_i` and :math:`X_j` are not swapped to estimate :math:`U_{ij}`,
and so on for higher orders, assuming that order :math:`< n_X`.
Then the :math:`\binom {n_X}{2}` second order indices are estimated by

.. math::

    \hat S_{ij} = \frac{\hat V_{ij}}{\hat \sigma^2} = \frac{\hat U_{ij} - m_{Y^k}^2 - \hat V_i - \hat V_j}{\hat \sigma^2}

For the :math:`n_X` total order indices :math:`T_i`, we only swap the variable :math:`X_i` between the two samples.

.. topic:: API:

    - See :class:`~openturns.SobolIndicesAlgorithm` for indices based on sampling
    - See :class:`~openturns.FunctionalChaosRandomVector` for indices based on chaos expansion

.. topic:: Examples:

    - See :ref:`examples/statistical_estimation/estimate_sobol.ipynb`

.. topic:: References:

    - [Saltelli2002]_
