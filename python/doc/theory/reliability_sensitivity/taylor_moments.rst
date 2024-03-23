.. _taylor_expansion_moments:

Taylor expansion moments
------------------------

In this page, we consider the Taylor expansion of a function.
One way to evaluate the central dispersion (expectation and variance) of the variable :math:`\uY=\model(\uX)`
is to use the Taylor expansion of the function :math:`\model: \Rset^\inputDim \rightarrow \Rset^q` at
the mean point :math:`\vect{\mu} = \Expect{\uX}`. Depending on the order of the Taylor expansion (classically first or second order), we get different approximations
of the mean and variance of :math:`Y`.

We use the notations introduced in :ref:`Taylor Expansion <taylor_expansion>`.

In the remainder, let :math:`\Cov \uX` be the covariance matrix of :math:`\uX`, defined by:

.. math::

    \Cov \uX = \mat{C}


where :math:`\mat{C} \in \Rset^{\inputDim \times \inputDim}` is the input covariance matrix:

.. math::

    c_{ij} = \Expect{\left(X_i - \Expect{X_i}\right)\left(X_j - \Expect{X_j} \right)}


with :math:`c_{ii} = \sigma_i^2`.


Case 1: :math:`q=1`, :math:`Y = \model (\inputRV) \in \Rset`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The second-order Taylor expansion of :math:`\model` at the point :math:`\ux = \vect{\mu}` is:

.. math::

    y = \model (\vect{\mu}) + \sum_{i = 1}^\inputDim \left( \frac{\partial \model }{\partial x_i }\right)(\vect{\mu})(x_i-\mu_i)
    + \frac{1}{2} \sum_{i,j = 1}^\inputDim \left(\frac{\partial^2 \model }{\partial x_i \partial x_j}\right)(\vect{\mu})
    (x_i-\mu_i)(x_j-\mu_j) + o\left(\|\vect{x}\|^2\right).


The expectation and variance of the first-order expansion are:

  .. math::

            \Expect{Y} \approx \model (\vect{\mu})\\
            \Var{Y} \approx \sum_{i=1}^\inputDim \sigma_i^2 \left(\left(\frac{\partial \model }{\partial x_i}
            \right)(\vect{\mu}) \right)^2


The expectation of the second-order expansion is:

  .. math::

       \Expect{Y}  \approx \model (\vect{\mu}) + \frac{1}{2} \sum_{i,j=1}^\inputDim c_{ij}\left(\frac{\partial^2 \model}
       {\partial x_i \partial x_j}\right)(\vect{\mu}).


The second-order approximation of the variance  is not implemented because it requires both the knowledge of higher
order derivatives of :math:`\model` and the knowledge of moments of order strictly greater
than 2 of the distribution of :math:`\uX`.


Case 2: :math:`q>1`, :math:`Y =(Y_1, \dots, Y_\outputDim) \in \Rset^\outputDim`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The second-order Taylor expansion of :math:`\model = (\model_1, \dots, \model_q)` at the point
:math:`\ux = \vect{\mu}` for each marginal function :math:`h_k` is:

.. math::

      y_k = \model_k(\vect{\mu}) + \sum_{i = 1}^\inputDim \left( \frac{\partial \model_k}{\partial x_i }\right)(\vect{\mu})
      (x_i-\mu_i)+ \frac{1}{2} \sum_{i,j = 1}^\inputDim \left( \frac{\partial^2 \model_k}{\partial x_i \partial
      x_j}\right)(\vect{\mu})(x_i-\mu_i)(x_j-\mu_j) + o(\|\vect{x}\|^2).


where :math:`1\leq k \leq \outputDim`.

The expectation and covariance matrix of the first-order expansion are:


  .. math::

         \Expect{\uY} \approx  \model(\vect{\mu})\\
         \Cov \uY \approx \left( \sum_{i,j=1}^\inputDim c_{ij}  \left( \frac{\partial h_k}{\partial x_i }
         \right)(\vect{\mu})\left( \frac{\partial \model_\ell}{\partial x_j }\right)(\vect{\mu})\right)_{k,
         \ell}


The expectation of the second-order expansion is:

.. math::

    (\Expect{\uY})_k \approx \Expect{Y_k} \approx \model_k(\vect{\mu}) + \frac{1}{2}  \sum_{i,j=1}^\inputDim  c_{ij}\left(
    \frac{\partial^2  \model_k}{\partial x_i \partial x_j}\right)(\vect{\mu})


The second-order approximation of the variance  is not implemented because it requires both the
knowledge of higher order derivatives of :math:`\model` and the knowledge of moments of order strictly greater
than 2 of the probability density function.



.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

