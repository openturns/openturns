.. _orthonormal_polynomials:

Univariate orthonormal polynomials
----------------------------------

This page provides mathematical details on sequences of univariate
polynomials which are orthonornal with respect to a distribution :math:`\mu`.

These sequences are used to
build multivariate polynomial basis by tensorization in the polynomial chaos expansion (refer to
:ref:`functional_chaos` and :ref:`chaos_basis`).

A sequence of univariate orthonormal polynomials is defined by a three-term recurrence
formula. Let by :math:`P_n` the polynomial of degree :math:`n`.
Then the sequence :math:`(P_n)_{n \in \Nset}` verifies the relation:

.. math::

    P_{n+1}\ =\ (a_nx+b_n)\ P_n\ +\ c_n\ P_{n-1}

where for all :math:`n`, :math:`a_n \geq 0` and :math:`c_n < 0`.

We detail:

- some well-known sequences of univariate orthonormal polynomials,
- how to compute any sequence of univariate orthonormal polynomials.

Orthonormal polynomials with respect to usual probability distributions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Below is a table showing some usual sequences of
polynomials orthonormal with respect to continuous distribution :math:`\mu`.

The distributions with respect to which these classical families are
orthonormal are the **standard representatives** of the corresponding
parametric distribution families (see
:meth:`~openturns.Distribution.getStandardRepresentative`), eg the Hermite
polynomials are orthonormal with respect to :math:`\cN(0, 1)`. For the
continuous families, any other member of the family is mapped onto its
standard representative by an **affine transformation**
:math:`X = \alpha Z + \beta`, and the orthonormal polynomials with respect
to :math:`X` are obtained from those of :math:`Z` by the substitution
:math:`x \mapsto (x - \beta)/\alpha`.

.. list-table::
   :widths: 15 30 30 25
   :header-rows: 1

   * - Ortho. poly.
     - :math:`P_n(x)`
     - Weight :math:`w(x)`
     - Recurrence coefficients :math:`(a_n, b_n, c_n)`
   * - Hermite
     - :math:`{He}_n(x)`
     - :math:`\displaystyle \frac{1}{\sqrt{2 \pi}} e^{-\frac{x^2}{2}}`
     - :math:`\begin{array}{ccc} a_n & = & \frac{1}{\sqrt{n+1}} \\ b_n & = & 0 \\ c_n & = &  - \sqrt{\frac{n}{n+1}} \end{array}`
   * - Legendre
     - :math:`\begin{array}{c} {Le}_n(x) \\ \alpha>-1 \end{array}`
     - :math:`\displaystyle \frac{1}{2} \times \mathbb{I}_{[-1,1]}(x)`
     - :math:`\begin{array}{ccc} a_n & = & \frac{\sqrt{(2n+1)(2n+3)}}{n+1} \\ b_n & = & 0 \\ c_n & = &  -\frac{ n \sqrt{2n+3} }{ (n+1)\sqrt{2n-1} } \end{array}`
   * - Laguerre
     - :math:`L_n^{(\alpha)}(x)`
     - :math:`\displaystyle \frac{x^{k-1}}{\Gamma(k)}~e^{-x} \mathbb{I}_{[0,+\infty[}(x)`
     - :math:`\begin{array}{ccc} \omega_{n} & = & \left((n+1)(n+k+1) \right)^{-1/2} \\ a_n & = & \omega_{n} \\ b_n & = & -(2n+k+1)~\omega_{n} \\ c_n & = &  -\sqrt{(n+k)n}~\omega_{n} \end{array}`
   * - Jacobi
     - :math:`\begin{array}{c} J^{(\alpha,\beta)}_n(x) \\ \alpha,\beta>-1 \end{array}`
     - :math:`\frac{(1-x)^{\alpha}(1+x)^{\beta}}{2^{\alpha + \beta + 1} B(\beta + 1, \alpha + 1)} \mathbb{I}_{[-1,1]}(x)`
     - :math:`\begin{array}{ccc} K_{1,n} & = & \frac{2n+\alpha + \beta + 3}{(n+1)(n+\alpha+1)(n+\beta+1)(n+\alpha+\beta+1)} \\ K_{2,n} & = & \frac{1}{2} \sqrt{(2n + \alpha + \beta + 1) K_{1,n}} \\ a_n & = & K_{2,n}(2n+\alpha + \beta + 2) \\ b_n & = & K_{2,n}\frac{(\alpha - \beta)(\alpha + \beta)}{2n+\alpha+\beta} \\ c_n & = & - \frac{2n+\alpha+\beta + 2}{2n+\alpha+\beta} \Big[(n+\alpha)(n+\beta) \times (n+\alpha+\beta)n\frac{K_{1,n}}{2n+\alpha+\beta-1}\Big]^{1/2} \end{array}`

Furthermore, two families of orthonormal polynomials with respect to
discrete  distribution are well-known and detailed in the Table below.

.. list-table::
   :widths: 20 30 30 20
   :header-rows: 1

   * - Ortho. poly.
     - :math:`P_n(x)`
     - Probability mass function
     - Recurrence coefficients :math:`(a_n, b_n, c_n)`
   * - Charlier
     - :math:`Ch^{(\lambda)}_n(x), \ \lambda>0`
     - :math:`\displaystyle{\frac{\lambda^k}{k!}~e^{-\lambda}}, \ k=0,1,2,\dots`
     - :math:`\begin{array}{ccc} a_n & = & - \frac{1}{\sqrt{\lambda (n+1)}} \\ b_n & = & \frac{n+\lambda}{\sqrt{\lambda (n+1)}} \\ c_n & = &  - \sqrt{1 - \frac{1}{n+1}} \end{array}`
   * - Krawtchouk\ :math:`^{\dagger}`
     - :math:`Kr^{(m,p)}_n(x), \ m \in \Nset, \ p \in [0,1]`
     - :math:`\displaystyle{\binom{m}{k}p^k (1-p)^{m-k}}, \ k=0,1,2,\dots`
     - :math:`\begin{array}{ccc} a_n & = & - \frac{1}{\sqrt{(n+1)(m-n)p(1-p)}} \\ b_n & = & \frac{p(m-n)+n(1-p)}{\sqrt{(n+1)(m-n)p(1-p)}} \\ c_n & = &  - \sqrt{(1 - \frac{1}{n+1})(1+\frac{1}{m-n})} \end{array}`

Notice that the Krawtchouk polynomials are only defined up to the
degree :math:`m-1`. Indeed, for the degree :math:`n=m`, some
factors of the denominators of the recurrence coefficients would be
equal to zero.

The Table below sums up the available polynomials sequences and the distribution family with respect to which they are orthonormal (through its standard representative, see above).

.. list-table::
   :widths: 20 15 25 20
   :header-rows: 1

   * - Distribution family
     - Support
     - Polynomial family
     - In the library
   * - Normal :math:`\cN(\mu,\sigma)`
     - :math:`\Rset`
     - Hermite
     - :class:`~openturns.HermiteFactory`
   * - Uniform :math:`\cU(a,b)`
     - :math:`[a,b]`
     - Legendre
     - :class:`~openturns.LegendreFactory`
   * - Gamma :math:`\Gamma(k,\lambda,\gamma)`
     - :math:`(\gamma,+\infty)`
     - Laguerre
     - :class:`~openturns.LaguerreFactory`
   * - Beta :math:`B(\alpha,\beta,a,b)`
     - :math:`(a,b)`
     - Jacobi
     - :class:`~openturns.JacobiFactory`
   * - Poisson :math:`\cP(\lambda)`
     - :math:`\Nset`
     - Charlier
     - :class:`~openturns.CharlierFactory`
   * - Binomial :math:`\cB(m,p)`
     - :math:`\{0,\dots,m\}`
     - Krawtchouk\ :math:`^{\dagger}`
     - :class:`~openturns.KrawtchoukFactory`
   * - Negative Binomial :math:`\cN \cB(m,p)`
     - :math:`\Nset`
     - Meixner
     - :class:`~openturns.MeixnerFactory`

Orthonormal polynomials with respect to arbitrary probability distributions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to generate a family of orthonormal polynomials
with respect to an arbitrary probability distribution :math:`\mu`.

The *Gram-Schmidt* algorithm can be used to this end. Note
that this algorithm gives a constructive proof of the existence of
orthonormal bases. However it is known to be numerically unstable, so alternative
procedures are often used in practice.

For all arbitrary distributions, the three-term recurrence is computed (see :class:`~openturns.AdaptiveStieltjesAlgorithm`)
and used to build its orthonormal polynomial family (see :class:`~openturns.StandardDistributionPolynomialFactory`).

.. topic:: API:

    - See the available :ref:`orthogonal basis <orthogonal_basis>`.


.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_functional_chaos`


.. topic:: References:

    - [gautschi2004]_
    - [chihara1978]_
    - [sullivan2015]_ chapter 8 page 133

