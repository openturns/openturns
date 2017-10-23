Taylor decomposition importance factors
---------------------------------------

The importance factors derived from a quadratic combination method are
defined to discriminate the influence of the different inputs towards
the output variable for central dispersion analysis.

The importance factors are derived from the following expression. It
can be shown by Taylor expansion of the output variable :math:`z`
(:math:`n_Z = 1`) around :math:`\ux = \muX` and computation of the
variance that :

  .. math::

     \Var Z \approx \nabla h(\muX).\Cov \uX .^t\nabla h(\muX)

which can be re written :

  .. math::

     1 \approx \sum_{i=1}^{n_X}\frac{\partial h(\muX)}{\partial X^i}\times\frac{  \sum_{j=1}^{n_X} \frac{\partial h(\muX)}{\partial x^j}.(\Cov \uX)_{ij} }{\Var Y} \approx & \cF_1 + \cF_2 + \ldots + \cF_{n_X}


  .. math::

     \underline{\cF} = \nabla h(\muX) \times \frac{\Cov \uX .^t\nabla h(\muX)}{\Var Z}


  .. math::

     \cF_i = \frac{\partial h(\muX)}{\partial x^i} \times \frac{\sum_{j=1}^{n_X} \frac{\partial h(\muX)}{\partial x^j}.(\Cov \uX)_{ij} }{\Var Y}


where:

-  :math:`\nabla h(\ux) = \left( \frac{\partial h(\ux)}{\partial x^i}\right)_{i=1,\ldots,n_X}`
   is the gradient of the model at the point :math:`\ux`,

-  :math:`\Cov \uX` is the covariance matrix,

-  :math:`\muX` is the mean of the input random vector,

-  :math:`\Var Z` is the variance of the output variable.

| Let us note that this interpretation supposes that :math:`(X^i)_i` are
  independent.
| Each coefficient :math:`\frac{\partial h(\ux)}{\partial x^i}` is a
  linear estimate of the number of units change in the variable
  :math:`y=h(\ux)` as a result of a unit change in the variable
  :math:`x^i`. This first term depends on the physical units of the
  variables and is meaningful only when the units of the model are
  known. In the general case, as the variables have different physical
  units, it is not possible to compare these sensitivities
  :math:`\frac{\partial h(\ux)}{\partial x^i}` the one with the others.
  This is the reason why the importance factor proposed are
  normalized factors. These factors enable to make the results
  comparable independently of the original units of the inputs of the
  model. The second term
  :math:`\frac{\sum_{j=1}^{n_X} \frac{\partial h(\muX)}{\partial x^j}.(\Cov \uX)_{ij} }{\Var Z}`
  is the renormalization factor.
| To summarize, the coefficients :math:`(\cF_i)_{i=1,\ldots,n_X}`
  represent a linear estimate of the percentage change in the variable
  :math:`z = h(\ux)` caused by one percent change in the variable
  :math:`x^i`. The importance factors are independent of the original
  units of the model, and are comparable with each other.


These are also called Importance Factors derived from Perturbation Methods.


.. topic:: API:

    - See :class:`~openturns.TaylorExpansionMoments`


.. topic:: Examples:

    - See :ref:`examples/reliability_sensitivity/estimate_moments_taylor.ipynb`

