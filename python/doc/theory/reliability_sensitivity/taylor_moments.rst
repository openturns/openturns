.. _taylor_expansion_moments:

Taylor expansion moments
------------------------

In this page, we consider the Taylor expansion of a function.
One way to evaluate the central dispersion (expectation and variance) of the variable :math:`\uY=h(\uX)`
is to use the Taylor expansion of the function :math:`h: \Rset^d \rightarrow \Rset^q` at
the mean point :math:`\vect{\mu} = \Expect{\uX}`. Depending on the order of the Taylor expansion (classically first or second order), we get different approximations
of the mean and variance of :math:`Y`.

We use the notations introduced in :ref:`Taylor Expansion <taylor_expansion>`.

In the remainder, let :math:`\Cov \uX` be the covariance matrix of :math:`\uX`, defined by:

.. math::

    \Cov \uX = \mat{C}

where :math:`\mat{C} \in \Rset^{n_X \times n_X}` is the input covariance matrix:

.. math::

    c_{ij} = \Expect{\left(X_i - \Expect{X_i}\right)\left(X_j - \Expect{X_j} \right)}

for :math:`i, j \in \{1, ..., n_X\}`.
Notice that each diagonal element of the covariance matrix :math:`c_{ii} = \sigma_i^2`, is
equal to the variance of an input variable (:math:`X_i`).

Case 1: scalar output
~~~~~~~~~~~~~~~~~~~~~

In this section, we analyse the special case where :math:`q = 1` and :math:`Y = h(\vect{X}) \in \Rset`.
The second-order Taylor expansion of :math:`h` at the point :math:`\ux = \vect{\mu}` is:

.. math::

    h(\ux) = h(\vect{\mu}) + \sum_{i = 1}^d \frac{\partial h}{\partial x_i} (\vect{\mu})(x_i - \mu_i)
    + \frac{1}{2} \sum_{i,j = 1}^d \frac{\partial^2 h}{\partial x_i \partial x_j}(\vect{\mu})
    (x_i - \mu_i) (x_j - \mu_j) + o\left(\|\vect{x} - \vect{\mu}\|^2\right)

when :math:`\vect{x} \rightarrow \vect{\mu}`.
The expectation and variance of the first-order expansion are:

.. math::

    \Expect{Y} \approx h(\vect{\mu})

.. math::

    \Var{Y} \approx \sum_{i=1}^{d} \sum_{j=1}^{d}
        c_{ij} \frac{\partial h}{\partial x_i}(\vect{\mu}) \frac{\partial h}{\partial x_j}(\vect{\mu}).

In the special case where the inputs are independent, then
the variance expression is simplified and we get:

.. math::

    \Var{Y} \approx \sum_{i=1}^{d} \sigma_i^2 \left(\frac{\partial h}{\partial x_i}(\vect{\mu}) \right)^2.

The expectation of the second-order expansion is:

.. math::

    \Expect{Y}  \approx h (\vect{\mu}) + \frac{1}{2} \sum_{i,j=1}^{d} c_{ij} \frac{\partial^2 h}
        {\partial x_i \partial x_j}(\vect{\mu}).

The second-order approximation of the variance  is not implemented because it requires both the knowledge of higher
order derivatives of :math:`h` and the knowledge of moments of order strictly greater
than 2 of the distribution of :math:`\uX`.

Case 2: multi-dimensional output
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we present the general case where :math:`q > 1` and :math:`Y =(Y_1, \dots, Y_q) \in \Rset^q`.
The second-order Taylor expansion of :math:`h = (h_1, \dots, h_q)` at the point
:math:`\ux = \vect{\mu}` for each marginal function :math:`h_k` is:

.. math::

    y_k = h_k(\vect{\mu}) + \sum_{i = 1}^d \frac{\partial h_k}{\partial x_i} (\vect{\mu})
    (x_i-\mu_i)+ \frac{1}{2} \sum_{i,j = 1}^d \frac{\partial^2 h_k}{\partial x_i \partial
    x_j}(\vect{\mu}) (x_i - \mu_i) (x_j - \mu_j) + o(\|\vect{x} - \vect{\mu}\|^2)

where :math:`1\leq k \leq q` when :math:`\vect{x} \rightarrow \vect{\mu}`.

The expectation and covariance matrix of the first-order expansion are:

.. math::

     \Expect{\uY} \approx  h(\vect{\mu})

and:

.. math::

     (\Cov{\uY})_{k, \ell} \approx \sum_{i,j=1}^{d} c_{ij}  \frac{\partial h_k}{\partial x_i }
     (\vect{\mu}) \frac{\partial h_\ell}{\partial x_j } (\vect{\mu})

for :math:`k, \ell \in \{1, ..., q\}`.
The expectation of the second-order expansion is:

.. math::

    (\Expect{\uY})_k \approx \Expect{Y_k} \approx h_k(\vect{\mu}) + \frac{1}{2}  \sum_{i,j=1}^{d}  c_{ij}\left(
    \frac{\partial^2 h_k}{\partial x_i \partial x_j}\right)(\vect{\mu})

for :math:`k \in \{1, ..., q\}`.

The second-order approximation of the variance  is not implemented because it requires both the
knowledge of higher order derivatives of :math:`h` and the knowledge of moments of order strictly greater
than 2 of the probability density function.

.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`

.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/central_dispersion/plot_estimate_moments_taylor`

