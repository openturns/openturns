Taylor variance decomposition
-----------------------------

| The quadratic cumul is a probabilistic approach designed to propagate
  the uncertainties of the input variables :math:`\uX` through the model
  :math:`h` towards the output variables :math:`\uY`. It enables to
  access the central dispersion (expectation and variance) of the output
  variables.
| This method is based on a Taylor decomposition of the output variable
  :math:`\uY` towards the :math:`\uX` random vectors around the mean
  point :math:`\muX`. Depending on the order of the Taylor decomposition
  (classically first order or second order), one can obtain different
  formulas. For easiness of the reading, we first present the formulas
  with :math:`n_Y=1` before the ones obtained for :math:`n_Y > 1`.
| As :math:`Y=h(\uX)`, the Taylor decomposition around
  :math:`\ux = \muX` at the second order yields to:

| **Case** :math:`n_Y=1`

.. math::

    Y = h(\muX) + < \underline{\nabla} h(\muX) , \: \uX - \muX> + \frac{1}{2}<<\underline{\underline{\nabla }}^2 h(\muX,\: \underline{\mu}_{\:X}),\: \uX - \muX>,\: \uX - \muX> + o(\Cov \uX)

where:

-  :math:`\muX = \Expect{\uX}` is the vector of the input variables at
   the mean values of each component.

-  :math:`\Cov \uX` is the covariance matrix of the random vector
   :math:`\uX`. The elements are the followings :
   :math:`(\Cov \uX)_{ij} = \Expect{\left(X^i - \Expect{X^i} \right)\times\left(X^j - \Expect{X^j} \right)}`

-  :math:`\underline{\nabla} h(\muX) = \: ^t \left( \frac{\partial y}{\partial x^j}\right)_{\ux\: =\: \muX} = \: ^t \left( \frac{\partial h(\ux)}{\partial x^j}\right)_{\ux\: =\: \muX}`
   is the gradient vector taken at the value :math:`\ux = \muX` and
   :math:`j=1,\ldots,n_X`.

-  :math:`\underline{\underline{\nabla}}^2 h(\ux,\ux)` is a matrix. It
   is composed by the second order derivative of the output variable
   towards the :math:`i^\textrm{th}` and :math:`j^\textrm{th}`
   components of :math:`\ux` taken around :math:`\ux = \muX`. It yields
   to:
   :math:`\left( \nabla^2 h(\muX,\muX) \right)_{ij} =\left( \frac{\partial^2 h(\ux,\ux)}{\partial x^i \partial x^j}\right)_{\ux\: =\: \muX}`

-  :math:`<,>` is a scalar product between two vectors.

| **Approximation at the order 1 - Case** :math:`n_Y=1`

  .. math::

      \Expect{Y} = h(\muX)


  .. math::

      \Var{Y} = \sum_{i,j=1}^{n_X} \frac{\partial h(\muX)}{\partial X^i} . \frac{\partial h(\muX)}{\partial X^j}.(\Cov \uX)_{ij}

| **Approximation at the order 2 - Case** :math:`n_Y=1`

  .. math::

     \begin{aligned}
         \begin{split}
           \Expect{Y} = h(\muX) + \frac{1}{2}. \sum_{i,j=1}^{n_X} \frac{\partial^2 h(\muX,\muX)}{\partial x^i \partial x^j} . (\Cov \uX)_{ij}
         \end{split}
       \end{aligned}

| The decomposition of the variance at the order 2 is not implemented.
  It requires both the knowledge of higher order derivatives of the model
  and the knowledge of moments of order strictly greater than 2 of the pdf.

| **Case** :math:`n_Y>1`


| The quadratic cumul approach can be developed at different orders
  from the Taylor decomposition of the random vector :math:`\uY`. As
  :math:`\uY=h(\uX)`, the Taylor decomposition around :math:`\ux = \muX`
  at the second order yields to:

  .. math::

      \uY = h(\muX) + <\underline{\underline{\nabla}}h(\muX) , \: \uX - \muX> + \frac{1}{2}<<\underline{\underline{\underline{\nabla }}}^2 h(\muX,\: \underline{\mu}_{\:X}),\: \uX - \muX>,\: \uX - \muX> + o(\Cov \uX)

where:

-  :math:`\muX = \Expect{\uX}` is the vector of the input variables at
   the mean values of each component.

-  :math:`\Cov \uX` is the covariance matrix of the random vector
   :math:`\uX`. The elements are the followings :
   :math:`(\Cov \uX)_{ij} = \Expect{\left(X^i - \Expect{X^i} \right)^2}`

-  :math:`\underline{\underline{\nabla}} h(\muX) = \: ^t \left( \frac{\partial y^i}{\partial x^j}\right)_{\ux\: =\: \muX} = \: ^t \left( \frac{\partial h^i(\ux)}{\partial x^j}\right)_{\ux\: =\: \muX}`
   is the transposed Jacobian matrix with :math:`i=1,\ldots,n_Y` and
   :math:`j=1,\ldots,n_X`.

-  :math:`\underline{\underline{\underline{\nabla^2}}} h(\ux\:,\ux)` is
   a tensor of order 3. It is composed by the second order derivative
   towards the :math:`i^\textrm{th}` and :math:`j^\textrm{th}`
   components of :math:`\ux` of the :math:`k^\textrm{th}` component of
   the output vector :math:`h(\ux)`. It yields to:
   :math:`\left( \nabla^2 h(\ux) \right)_{ijk} = \frac{\partial^2 (h^k(\ux))}{\partial x^i \partial x^j}`

-  :math:`<\underline{\underline{\nabla}}h(\muX) , \: \uX - \muX> = \sum_{j=1}^{n_X} \left( \frac{\partial {\uy}}{\partial {x^j}}\right)_{\ux = \muX} . \left( X^j-\muX^j \right)`

-  :math:`<<\underline{\underline{\underline{\nabla }}}^2 h(\muX,\: \underline{\mu}_{X}),\: \uX - \muX>,\: \uX - \muX> = \left( ^t (\uX^i - \muX^i). \left(\frac{\partial^2 y^k}{\partial x^i \partial x^k}\right)_{\ux = \muX}. (\uX^j - \muX^j) \right)_{ijk}`

| **Approximation at the order 1 - Case** :math:`n_Y>1`

  .. math::

       \Expect{\uY} \approx \underline{h}(\muX)

  Pay attention that :math:`\Expect{\uY}` is a vector. The
  :math:`k^\textrm{th}` component of this vector is equal to the
  :math:`k^\textrm{th}` component of the output vector computed by the
  model :math:`h` at the mean value. :math:`\Expect{\uY}` is thus the
  computation of the model at mean.

.. math::

    \Cov \uY \approx ^t\underline{\underline{\nabla}}\:\underline{h}(\muX).\Cov \uX.\underline{\underline{\nabla}}\:\underline{h}(\muX)

| **Approximation at the order 2 - Case** :math:`n_Y>1`

.. math::

    \Expect{\uY} \approx \underline{h}(\muX) + \frac{1}{2}.\underline{\underline{\underline{\nabla}}}^2 \underline{\underline{h}}(\muX,\muX) \odot \Cov \uX

This last formulation is the reduced writing of the following
expression:

.. math::

    (\Expect{\uY})_k \approx (\underline{h}(\muX))_k + \left( \sum_{i=1}^{n_X}\frac{1}{2} (\Cov \uX)_{ii}.{(\nabla^2\:h(\uX))}_{iik} + \sum_{i=1}^{n_X} \sum_{j=1}^{i-1} (\Cov X)_{ij}.{(\nabla^2\:h(\uX))}_{ijk}  \right)_k

The decomposition of the variance at the order 2 is not implemented.
It requires both the knowledge of
higher order derivatives of the model and the knowledge of moments of
order strictly greater than 2 of the pdf.


.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_moments_taylor`

