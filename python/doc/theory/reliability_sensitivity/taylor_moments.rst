.. _taylor_expansion_moments:

Taylor expansion moments
------------------------

One way to evaluate the central dispersion (expectation and variance) of the variable :math:`Y=h(\uX)` is to use the Taylor expansion of the function :math:`h: \Rset^d \rightarrow \Rset^q` at the mean point :math:`\vect{\mu} = \Expect{\uX}`.

Depending on the order of the Taylor expansion (classically first or second order), we get different approximations of the mean and variance of :math:`Y`.


In the remainder, we denote by :math:`\Cov \uX`  the covariance matrix of :math:`\uX`, defined by:

.. math::

    \Cov \uX= (c_{ij})_{ij}
    
where: 

.. math::

    c_{ij} = \Expect{\left(X_i - \Expect{X_i}\right)\left(X_j - \Expect{X_j} \right)}


whith :math:`c_{ii} = \sigma_i^2`.

**Case 1:** :math:`Y \in \Rset` , :math:`q=1`:

The second order Taylor expansion of :math:`h` around :math:`\ux = \vect{\mu}` is written as:

.. math::

    Y = h(\vect{\mu}) + \sum_{i = 1}^d \left( \frac{\partial h}{\partial x_i }\right)_{\ux = \vect{\mu}}(X_i-\mu_i)
    + \frac{1}{2} \sum_{i,j = 1}^d \left(\frac{\partial^2 h}{\partial x_i \partial x_j}\right)_{\ux = \vect{\mu}}(X_i-\mu_i)(X_j-\mu_j) + o(||\uX||^2)


If we use the first order expansion, we get:

  .. math::

       \begin{array}{lcl}
            \Expect{Y} & \simeq & h(\vect{\mu})\\
            \Var{Y} & \simeq & \sum_{i=1}^{d} \sigma_i^2 \left[\left(\frac{\partial h}{\partial x_i}\right)_{\ux = \vect{\mu}} \right]^2
       \end{array}


If we use the second order expansion, we get:

  .. math::

       \Expect{Y}  \simeq h (\vect{\mu}) + \frac{1}{2} \sum_{i,j=1}^{d} c_{ij}\left(\frac{\partial^2 h}{\partial x_i \partial x_j}\right)_{\ux = \vect{\mu}}


The second order approximation of the variance  is not implemented because it requires both the knowledge of higher
order derivatives of :math:`h` and the knowledge of moments of order strictly greater than 2 of the distribution of :math:`\uX`.


**Case 2:** :math:`Y =(Y_1, \dots, Y_q) \in \Rset^q` with :math:`q>1`:


The second order Taylor expansion of :math:`h = (h_1, \dots, h_q)` around :math:`\ux = \vect{\mu}` is written
for each marginal function :math:`h_k` as:

.. math::

      Y_k = h_k(\vect{\mu}) + \sum_{i = 1}^d \left( \frac{\partial h_k}{\partial x_i }\right)_{\ux = \vect{\mu}}(X_i-\mu_i)+ \frac{1}{2} \sum_{i,j = 1}^d \left( \frac{\partial^2 h_k}{\partial x_i \partial
      x_j}\right)_{\ux = \vect{\mu}}(X_i-\mu_i)(X_j-\mu_j) + o(||\uX||^2)


where :math:`1\leq k \leq q`.

If we use the first order expansion, we get:


  .. math::

       \begin{array}{lcl}
         \Expect{\uY} & \simeq &  h(\vect{\mu})\\
         \Cov \uY & \simeq & \left( \sum_{i,j=1}^{d} c_{ij}  \left( \frac{\partial h_k}{\partial x_i }\right)_{\ux = \vect{\mu}}\left( \frac{\partial h_l}{\partial x_j }\right)_{\ux = \vect{\mu}}\right)_{k,l}
       \end{array}


If we use the second order expansion, we get:

.. math::

    (\Expect{\uY})_k = \Expect{Y_k} \simeq h_k(\vect{\mu}) + \frac{1}{2}  \sum_{i,j=1}^{d}  c_{ij}\left( \frac{\partial^2 h_k}
    {\partial x_i \partial x_j}\right)(\vect{\mu})


The second order approximation of the variance  is not implemented because it requires both the knowledge of higher
order derivatives of :math:`h` and the knowledge of moments of order strictly greater than 2 of the probability density
function.



.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

