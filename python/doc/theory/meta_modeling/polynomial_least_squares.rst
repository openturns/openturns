.. _polynomial_least_squares:

Least squares polynomial response surface
-----------------------------------------

Instead of replacing the model response :math:`\model(\vect{x})` with a
*local* approximation around a given set :math:`\vect{x}_0` of
input parameters as in :ref:`Taylor expansion <taylor_expansion>`, one may seek a *global* approximation of
:math:`\model(\vect{x})` over its whole domain of definition. A common
choice to this end is global polynomial approximation. For the sake of
simplicity, a *scalar* model response :math:`y=\model(\vect{x})` will
be considered from now on. Nonetheless, the following derivations hold
for a vector-valued response.

In the following, one considers global approximations of the model
response using:

- a linear function, i.e. a polynomial of degree one;

  .. math::

      y  \approx  \widehat{\model}(\vect{x})  =  a_0 \, + \,  \sum_{i=1}^{n_{X}} a_{i} x_i

  where :math:`(a_j  \, , \, j=0,\dots,n_X)` is a set of unknown
  coefficients.

- a quadratic function, i.e. a polynomial of degree two.

  .. math::

         y  \approx  \widehat{\model}(\vect{x})  =  a_0 \, + \,  \sum_{i=1}^{n_{X}} a_{i} x_i \, + \,
         \sum_{i=1}^{n_{X}} \sum_{j=1}^{n_{X}} a_{i,j} x_i x_j

The two previous equations may be recast as:

  .. math::

      y  \approx  \widehat{\model}(\vect{x})  =  \sum_{j=0}^{P-1} a_j \psi_j(\vect{x})

where :math:`P` denotes the number of terms, which is equal to
:math:`n_X + 1` (resp. to :math:`1 + 2n_X + n_X (n_X - 1)/2`) when
using a linear (resp. a quadratic) approximation, and the family
:math:`(\psi_j,j=0,\dots,P-1)` gathers the constant monomial
:math:`1`, the monomials of degree one :math:`x_i` and possibly the
cross-terms :math:`x_i x_j` as well as the monomials of degree two
:math:`x_i^2`. Using the vector notation
:math:`\vect{a}  =  (a_{0} , \dots , a_{P-1} )^{\textsf{T}}`
and
:math:`\vect{\psi}(\vect{x})  =  (\psi_{0}(\vect{x}) , \dots , \psi_{P-1}(\vect{x}) )^{\textsf{T}}`,
this can be rewritten:

  .. math::

      y  \approx  \widehat{\model}(\vect{x})  =  \Tr{\vect{a}}\vect{\psi}(\vect{x})

A *global* approximation of the model response over its whole
definition domain is sought. To this end, the coefficients :math:`a_j`
may be computed using a least squares regression approach. In this
context, an experimental design, that is, a set of observations of
input parameters, is required:

.. math::

    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(N)} \right\},

as well as the corresponding model evaluations:

.. math::

    \cY = \left\{ y^{(1)},\dots,y^{(N)} \right\}.

The least squares problem is to solve:

.. math::

    \widehat{\vect{a}} = \argmin_{\vect{a} \in \Rset^P} \cJ(\vect{a})

where :math:`\cJ(\vect{a})` is the cost function, defined as:

.. math::
    \cJ(\vect{a}) = \sum_{i=1}^N \left( y^{(i)} - \Tr{\vect{a}} \vect{\psi}\left(\vect{x}^{(i)}\right) \right)^2.

Let :math:`\vect{y} = \Tr{(y^{(1)},\dots,y^{(N)})} \in \Rset^{N}` be the
vector of output observations.
If the design matrix :math:`\mat{\Psi}` has full rank,
then the solution is given by the normal equations:

.. math::
    :label: solutionLLS

    \widehat{\vect{a}}  =  \left( \Tr{\mat{\Psi}} \mat{\Psi}  \right)^{-1} \Tr{\mat{\Psi}}  \vect{y}

where:

.. math::

    \mat{\Psi}_{ij}  =  \psi_{j}\left(\vect{x}^{(i)}\right)

for :math:`i = 1, \dots, N` and :math:`j = 0, \dots, P - 1`.
A necessary condition for having a solution is that the size :math:`N`
of the experimental design is not less than the number :math:`P` of
coefficients to estimate.
The Gram matrix :math:`\Tr{\mat{\Psi}} \mat{\Psi}` can be
ill-conditionned.
Hence, the best method is not necessarily to invert the Gram matrix,
because the solution may be particularly sensitive to rounding errors.
The least-squares problem is rather solved using more robust numerical methods
such as the *singular value decomposition* (SVD) or the *QR-decomposition*.

.. topic:: API:

    - See :class:`~openturns.LinearLeastSquares`

.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/linear_regression/plot_create_linear_least_squares_model`


.. topic:: References:

    - [bjork1996]_

