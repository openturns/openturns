.. _chaos_basis:

Orthonormal basis
-----------------

In that file, we detail the construction of an orthonormal basis with respect to a multivariate
measure denoted by :math:`\mu`, addressing both the independent and dependent marginal settings.

In the functional expansion setting (refer to :ref:`functional_chaos`), the distribution
:math:`\mu` may be the input distribution :math:`\mu_{\inputRV}`, the distribution
:math:`\mu_{\vect{U}}` which is the push-forward distribution of :math:`\mu_{\inputRV}` through
the isoprobabilistic transformation :math:`T`, or the instrumental distribution
:math:`\tilde{p}` of the domination method.

The set of functions :math:`(\psi_k)_{k \geq 0}` with :math:`\psi_k : \Rset^{\inputDim} \rightarrow \Rset`
is *orthonormal* with respect to :math:`\mu` if:

.. math::
   :label: orthonorm

    \scalarproduct{\psi_k}{\psi_{\ell}}_{L^2\left(\mu\right)}  =  \delta_{k,\ell}

for any :math:`k, \ell \geq 0` where :math:`\delta_{k, \ell}` is the Kronecker symbol:

.. math::
  \delta_{k, \ell}
  =
  \begin{cases}
  1 & \textrm{ if } k = \ell, \\
  0 & \textrm{otherwise.}
  \end{cases}

In the library, we require that the first element be:

  .. math::
    :label: defPsi0

      \psi_0 = 1

The orthogonality of the functions implies that:

  .. math::
      \mathbf{E}_{\mu}\left[\psi_{i}(\vect{X})\right] = \scalarproduct{\psi_{i}}{\psi_{0}}_{L^2\left(\mu\right)} = 0

for any non-zero :math:`i\neq 0`.


Case of independent marginals: Tensorized univariate chaos basis
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We assume here that :math:`\mu` is a multivariate distribution with independent marginals
denoted by :math:`\mu_i`:

.. math::
   \mu = \otimes_{i=1}^{\inputDim} \mu_i

In that case, a multivariate basis orthonormal with respect to :math:`\mu` can be built as the
tensorization of univariate basis orthonormal with respect to its marginals.

For any :math:`1 \leq i \leq \inputDim`, let
:math:`\left(\varphi_k^{(i)}\right)_{k \geq 0}` be the univariate
basis orthonormal with respect to the marginal distribution :math:`\mu_i`.
The tensorized multivariate basis is defined by:

  .. math::
        \Psi_\vect{\alpha}(\vect{x}) = \prod_{i=1}^{\inputDim} \varphi_{\alpha_i}^{(i)}(x_i)

where :math:`\vect{\alpha} = (\alpha_1, \dots, \alpha_{\inputDim}) \in \Nset^{\inputDim}` is a
multi-index that enables to define each element of the multivariate basis from the elements of
the univariate marginal basis (see :class:`~openturns.EnumerateFunction` to get details on that
bijection).

The univariate bases may be:

- *polynomials*: the associated distribution :math:`\mu_i` can be continuous
  or discrete.
  The orthonormal polynomial basis is represented by its three-term recurrence and
  the reverse Clenshaw algorithm enables fast, stable evaluation of the polynomials
  at any point (see :class:`~openturns.OrthogonalUniVariatePolynomial`).
  For some distributions, the orthonormal polynomial basis is
  known (see :ref:`orthonormal_polynomials`).
  For all the other arbitrary distributions, the three-term recurrence is computed (see
  :class:`~openturns.AdaptiveStieltjesAlgorithm`) and used to build its orthonormal
  polynomial family (see :class:`~openturns.StandardDistributionPolynomialFactory`).

- Haar wavelets: the Haar wavelets basis is orthonormal with respect to the  :math:`\cU(0,1)`
  measure. This basis is used to approximate functions with discontinuities.
  For details on this basis, see :class:`~openturns.HaarWaveletFactory`.

- Fourier series: the Fourier series is orthonormal with respect to the :math:`\cU(-\pi, \pi)`
  measure. For more details on this basis, see :class:`~openturns.FourierSeriesFactory`.


Case of dependent marginals
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When :math:`\mu` is a multivariate distribution with dependent marginals, several methods
enable to build a basis orthonormal with respect to :math:`\mu`.

One method is to use an :ref:`isoprobabilistic_transformation` denoted by
:math:`T: \Rset^{\inputDim} \rightarrow \Rset^{\inputDim}`
that maps the measure :math:`\mu` into a measure :math:`\tilde{\mu}` with independent marginals.
Therefore, if  :math:`(\varphi_k)_k` is a basis orthonormal with respect to :math:`\tilde{\mu}`,
then :math:`(\varphi_k \circ T)_k` is a basis orthonormal with respect to :math:`\mu`. It is
important to note that if :math:`(\varphi_k)_k` is a polynomial basis, then
:math:`(\varphi_k \circ T)_k` is not a polynomial basis as soon as :math:`T` is not affine.

Another method consists in the following functional basis, which was first introduced by
Soize-Ghanem in [soizeghanem2004]_ defined by:

.. math::

    \psi_{\vect{\alpha}}(\vect{x})
    =  \dfrac{1}{\sqrt{c(F_1(x_1), \dots, F_\inputDim(x_\inputDim))}}
    \prod_{i=1}^{\inputDim} \varphi^{(i)}_{\alpha_{i}}(x_{i})

where :math:`F_i` is the cumulative distribution function of the :math:`i`-th marginal of
:math:`\mu`, :math:`c` the probability density function of its copula and
:math:`(\varphi^{(i)}_{\alpha_i})_{\alpha_i}` the polynomial basis orthonormal with respect to :math:`\mu_i`.

It is important to note that although :math:`(\varphi_{\alpha_i})_{\alpha_i}` is a polynomial basis,
:math:`(\psi_{\vect{\alpha}})_\vect{\alpha}` is not a polynomial basis.

Furthermore, this basis generates approximation subspaces of poor quality.
Indeed, for most copulas, the density :math:`c` tends to infinity at :math:`\vect{0}` or :math:`\vect{1}`
(and possibly at other points).
As a consequence, all the basis functions :math:`(\psi_{\vect{\alpha}})_\vect{\alpha}`
vanish at these points and take small values in their neighborhoods.
Therefore, the coefficients in the expansion of a function that reaches
large values at these points must necessarily be of large magnitude.
This leads to numerical stability issues, both for the computation of the expansion coefficients
and for the evaluation of the metamodel.

That is the reason why the use of such a multivariate orthonormal basis is not recommended.

.. topic:: API:

    - See :class:`~openturns.StandardDistributionPolynomialFactory`
    - See :class:`~openturns.AdaptiveStieltjesAlgorithm`
    - See :class:`~openturns.OrthogonalUniVariatePolynomial`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFactory`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    - See :class:`~openturns.IntegrationExpansion`
    - See :class:`~openturns.LeastSquaresExpansion`


.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_functional_chaos`
    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_chaos_lognormal`

.. topic:: References:

    - [soizeghanem2004]_
    - [ghanem1991]_
    - [lemaitre2010]_
    - [dahlquist2008]_
    - [rudin1987]_
