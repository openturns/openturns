.. _polynomial_least_squares:

Least squares polynomial response surface
-----------------------------------------

| Instead of replacing the model response :math:`h(\underline{x})` for a
  *local* approximation around a given set :math:`\underline{x}_0` of
  input parameters, one may seek a *global* approximation of
  :math:`h(\underline{x})` over its whole domain of definition. A common
  choice to this end is global polynomial approximation. For the sake of
  simplicity, a *scalar* model response :math:`y=h(\underline{x})` will
  be considered from now on. Nonetheless, the following derivations hold
  for a vector-valued response.
| In the sequel, one considers global approximations of the model
  response using:

-  a linear function, i.e. a polynomial of degree one;

-  a quadratic function, i.e. a polynomial of degree two.

  .. math::

      y \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, a_0 \, + \,  \sum_{i=1}^{n_{X}} \; a_{i} \; x_i

  where :math:`(a_j  \, , \, j=0,\dots,n_X)` is a set of unknown
  coefficients.

  .. math::

     \begin{aligned}
         \underline{y} \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, a_0 \, + \,  \sum_{i=1}^{n_{X}} \; a_{i} \; x_i \, + \,
         \sum_{i=1}^{n_{X}} \; \sum_{j=1}^{n_{X}} \; a_{i,j} \; x_i \; x_j
       \end{aligned}

| The two previous equations may be recast using a unique formalism as
  follows:

  .. math::

      \underline{y} \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, \sum_{j=0}^{P-1} \; a_j \; \psi_j(\underline{x})

  where :math:`P` denotes the number of terms, which is equal to
  :math:`(n_X + 1)` (resp. to :math:`(1 + 2n_X + n_X (n_X - 1)/2)`) when
  using a linear (resp. a quadratic) approximation, and the family
  :math:`(\psi_j,j=0,\dots,P-1)` gathers the constant monomial
  :math:`1`, the monomials of degree one :math:`x_i` and possibly the
  cross-terms :math:`x_i x_j` as well as the monomials of degree two
  :math:`x_i^2`. Using the vector notation
  :math:`\underline{a} \, \, = \, \, (a_{0} , \dots , a_{P-1} )^{\textsf{T}}`
  and
  :math:`\underline{\psi}(\underline{x}) \, \, = \, \, (\psi_{0}(\underline{x}) , \dots , \psi_{P-1}(\underline{x}) )^{\textsf{T}}`,
  this rewrites:

  .. math::

      \underline{y} \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, \underline{a}^{\textsf{T}} \; \underline{\psi}(\underline{x})

| A *global* approximation of the model response over its whole
  definition domain is sought. To this end, the coefficients :math:`a_j`
  may be computed using a least squares regression approach. In this
  context, an experimental design
  :math:`\underline{\cX} =(x^{(1)},\dots,x^{(N)})`, i.e. a set of
  realizations of input parameters is required, as well as the
  corresponding model evaluations
  :math:`\underline{\cY} =(y^{(1)},\dots,y^{(N)})`.

| The following minimization problem has to be solved:

  .. math::

     \mbox{Find} \quad \widehat{\underline{a}} \quad \mbox{that minimizes} \quad \cJ(\underline{a}) \, \, = \, \, \sum_{i=1}^N \; \left( y^{(i)} \; - \; \underline{a}^{\textsf{T}}  \underline{\psi}(\underline{x}^{(i)}) \right)^2

  The solution is given by:

  .. math::

      \widehat{\underline{a}} \, \, = \, \, \left( \underline{\underline{\Psi}}^{\textsf{T}} \underline{\underline{\Psi}}  \right)^{-1} \; \underline{\underline{\Psi}}^{\textsf{T}}  \; \underline{\cY}

  where:

  .. math::

      \underline{\underline{\Psi}} \, \, = \, \, (\psi_{j}(\underline{x}^{(i)}) \; , \; i=1,\dots,N \; , \; j = 0,\dots,P-1)

  It is clear that the above equation is only valid for a full rank
  information matrix. A necessary condition is that the size :math:`N`
  of the experimental design is not less than the number :math:`P` of PC
  coefficients to estimate. In practice, it is not recommended to
  directly invert
  :math:`\underline{\underline{\Psi}}^{\textsf{T}} \underline{\underline{\Psi}}`
  since the solution may be particularly sensitive to an
  ill-conditioning of the matrix. The least-square problem is rather
  solved using more robust numerical methods such as *singular value
  decomposition* (SVD) or *QR-decomposition*.


.. topic:: API:

    - See :class:`~openturns.FunctionalChaosAlgorithm`

.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/functional_chaos`


.. topic:: References:

    - A. Bjorck, 1996, "Numerical methods for least squares problems", SIAM Press, Philadelphia, PA.

