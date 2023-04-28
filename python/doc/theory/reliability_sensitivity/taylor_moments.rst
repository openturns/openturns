.. _taylor_expansion_moments:

Taylor expansion moments
------------------------

| One way to evaluate the central dispersion (expectation and variance) of the variable :math:`Y=h(\uX)` is to use the Taylor expansion of the function :math:`h: \Rset^d \rightarrow \Rset^q` at the mean point :math:`\mu = \Expect{\uX}`.

Depending on the order of the Taylor expansion (classically first or second order), we get different approximations of the mean and variance of :math:`Y`.


In the remainder, we denote by :math:`\Cov \uX`  the covariance matrix of :math:`\uX`, defined by:

.. math::

    \Cov \uX= \left(\Expect{\left(X_i - \Expect{X_i} \right)\times\left(X_j - \Expect{X_j}\right)_{ij} \right)}


| **Case 1:** :math:`Y \in \Rset`:

The second order Taylor expansion of :math:`h` around :math:`\ux = \mu` is written as:

.. math::

    Y = h(\mu) + \sum_{i = 1}^d \left( \frac{\partial h}{\partial x^i }\right)(\mu)(X_i-\mu_i) + \frac{1}{2} \sum_{i,j = 1}^d \left( \frac{\partial^2 h}{\partial x^i \partial x^j}\right)(\mu)(X_i-\mu_i)+(X_j-\mu_j) o(\Cov \uX)



| If we use the first order expansion, we get:

  .. math::

       \begin{array}{lcl}
            \Expect{Y} & = & h(\muX)\\
            \Var{Y} & = & \sum_{i=1}^{d} \left(\frac{\partial h}{\partial X_i}\right)^2(\muX). \Var  X_i
        \end{array}


| If we use the second order expansion, we get:

  .. math::

           \Expect{Y}  = h (\muX) + \frac{1}{2} \sum_{i,j=1}^{d} \left(\frac{\partial^2 h}{\partial x^i \partial x^j}\right)(\muX,\muX) . (\Cov \uX)_{ij}

| The second order approximation of the variance  is not implemented because it requires both the knowledge of higher order derivatives of :math:`h`
and the knowledge of moments of order strictly greater than 2 of the probability density function.

| **Case 2:** :math:`Y =(Y_1, \dots, Y_q) \in \Rset^q` with :math:`q>1`:


The second order Taylor expansion of :math:`h = (h_1, \dots, h_q)` around :math:`\ux = \mu` is written as:

.. math::

      Y_k = \left(h(\mu)\right)_k + \sum_{i = 1}^d \left( \frac{\partial h_k}{\partial x^i }\right)(\mu)(X_i-\mu_i) + \frac{1}{2} \sum_{i,j = 1}^d \left( \frac{\partial^2 h_k}{\partial x^i \partial x^j}\right)(\mu)(X_i-\mu_i)+(X_j-\mu_j) o(\Cov \uX)

where :math:`1\leq k \leq q`.

| If we use the first order expansion, we get:

  .. math::

       \begin{array}{lcl}
         \Expect{\uY} & = &  h(\mu)\\
         \Cov \uY & = & \left( \sum_{i,j=1}^{d} (\Cov \uX)_{ij}  \left( \frac{\partial h_i}{\partial x^k }\right) \left( \frac{\partial h_l}{\partial x^j }\right)\right)_{k,l}
        \end{array}


| If we use the second order expansion, we get:

.. math::

    (\Expect{\uY})_k \approx h_k(\mu) + \frac{1}{2} \left( \sum_{i=1}^{d} \Var X_i.\left( \frac{\partial^2 h_k}{\partial^2 x^i}\right)(\mu) + \sum_{i=1}^{d} \sum_{j=1}^{i-1} (\Cov X)_{ij}\left( \frac{\partial^2 h_k}{\partial x^i \partial x^j}\right)(\mu)

| The second order approximation of the variance  is not implemented because it requires both the knowledge of higher order derivatives of :math:`h`
and the knowledge of moments of order strictly greater than 2 of the probability density function.



.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

