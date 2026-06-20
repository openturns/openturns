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

    P_{n+1} & = (a_nx+b_n)\ P_n\ +\ c_n\ P_{n-1}, \quad n \geq  \\
    P_{-1}  & = 0 \\
    P_{0}  & = 1

where :math:`c_n < 0` for all :math:`n\geq 0`.

We detail:

- some well-known sequences of univariate orthonormal polynomials,
- how to compute any sequence of univariate orthonormal polynomials.

Orthonormal polynomials with respect to usual probability distributions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Below is a table showing some usual sequences of
polynomials orthonormal with respect to continuous distribution :math:`\mu`.

.. csv-table::
   :header: "Ortho. poly.", "Orthonormal distribution", "Recurrence coefficients :math:`(a_n,b_n,c_n)`"
   :widths: auto

   "Hermite", "Normal :math:`\cN(0,1)`", ":math:`\left\{\begin{array}{lcl} a_n & = & \frac{1}{\sqrt{n+1}} \\     b_n & = & 0 \\ c_n & = &  - \sqrt{\frac{n}{n+1}} \end{array}\right.`"
   "Legendre", "Uniform :math:`\cU(-1,1)`", ":math:`\left\{\begin{array}{lcl} a_n & = & \frac{\sqrt{(2n+1)(2n+3)}}{n+1} \\     b_n & = & 0 \\ c_n & = &  -\frac{ n \sqrt{2n+3} }{ (n+1)\sqrt{2n-1} } \end{array}\right.`"
   "Laguerre (k)", "Gamma :math:`Gamma(k, 1, 0)`", ":math:`\left\{\begin{array}{lcl}  \omega_{n} & = & \left((n+1)(n+k+1) \right)^{-1/2} \\ a_n & = & \omega_{n} \\     b_n & = & -(2n+k+1)~\omega_{n} \\ c_n & = &  -\sqrt{(n+k)n}~\omega_{n} \end{array}\right.`"
   "Jacobi(:math:`\alpha, \beta`)", "Beta :math:`B(\alpha, \beta, -1, 1)`", ":math:`\left\{\begin{array}{lcl}  K_{1,n} & = & \frac{2n+\alpha + \beta + 3}{(n+1)(n+\alpha+1)(n+\beta+1)(n+\alpha+\beta+1)} \\ \\ K_{2,n} & = & \frac{1}{2} \sqrt{(2n + \alpha + \beta + 1) K_{1,n}} \\ \\a_n & = & K_{2,n}(2n+\alpha + \beta + 2)  \\   \\  b_n & = & K_{2,n}\frac{(\alpha - \beta)(\alpha + \beta)}{2n+\alpha+\beta} \\ \\ c_n & = & - \frac{2n+\alpha+\beta + 2}{2n+\alpha+\beta} \Big[(n+\alpha)(n+\beta) \times (n+\alpha+\beta)n\frac{K_{1,n}}{2n+\alpha+\beta-1}\Big]^{1/2}  \end{array}\right.`"
   "Chebychev",  "Beta :math:`B(1/2, 1/2, -1, 1)`", ":math:`\left\{\begin{array}{l} a_0 = \sqrt{2}, \, a_n = 2\quad \forall n \geq 1 \\  b_n = 0 \quad \forall n \geq 0 \\  c_0 = 0, \, c_1 = -\sqrt{2}, \, c_n = -1 \quad \forall n \geq 2 \end{array}\right.`"

Furthermore, two families of orthonormal polynomials with respect to
discrete  distribution are well-known and detailed in the Table below.

.. csv-table::
   :header: "Ortho. poly.", "Probability mass function", "Recurrence coefficients :math:`(a_n,b_n,c_n)`"
   :widths: auto

   "Charlier :math:`(\lambda)`",  "Poisson :math:`\cP(\lambda)`", ":math:`\left\{\begin{array}{lcl} a_n & = & - \frac{1}{\sqrt{\lambda (n+1)}} \\   \\  b_n & = & \frac{n+\lambda}{\sqrt{\lambda (n+1)}} \\ \\ c_n & = &  - \sqrt{1 - \frac{1}{n+1}} \end{array}\right.`"
   "Krawtchouk",  "Binomial :math:`\cB(m,p)`", ":math:`\left\{\begin{array}{lcl} a_n & = & - \frac{1}{\sqrt{(n+1)(m-n)p(1-p)}} \\   \\  b_n & = & \frac{p(m-n)+n(1-p)}{\sqrt{(n+1)(m-n)p(1-p)}} \\ \\ c_n & = &  - \sqrt{(1 - \frac{1}{n+1})(1+\frac{1}{m-n})} \end{array}\right.`"
   "Meixner",  "Polya :math:`\cP(r,p)`", ":math:`\left\{\begin{array}{lcl} a_n & = & - \frac{p-1}{\sqrt{p (n + 1) (n + r)}} \\   \\  b_n & = & \frac{p(n+r)+n}{\sqrt{p(n+1)(n+r)}} \\ \\ c_n & = &  - \frac{\sqrt{p n (n + r - 1)}}{\sqrt{p (n + 1) (n + r)}}\end{array}\right.`"


Notice that the Krawtchouk polynomials are only defined up to the
degree :math:`m-1`. Indeed, for the degree :math:`n=m`, some
factors of the denominators of the recurrence coefficients are
equal to zero.

The Table below sums up the available polynomials sequences and the distribution with respect to which they are orthonormal.

.. csv-table::
   :header: "Distribution", "Support", "Polynomial family", "In the library"
   :widths: auto

   "Normal :math:`\cN(0,1)`", ":math:`\Rset`", "Hermite", ":class:`~openturns.HermiteFactory`"
   "Uniform :math:`\cU(-1,1)`", ":math:`[-1,1]`", "Legendre", ":class:`~openturns.LegendreFactory`"
   "Gamma :math:`\Gamma(k,1,0)`", ":math:`(0,+\infty)`", "Laguerre", ":class:`~openturns.LaguerreFactory`"
   "Beta :math:`B(\alpha,\beta,-1,1)`", ":math:`(-1,1)`", "Jacobi", ":class:`~openturns.JacobiFactory`"
   "Beta :math:`B(1/2,1/2,-1,1)`", ":math:`(-1,1)`", "Chebychev", ":class:`~openturns.ChebychevFactory`"
   "Poisson :math:`\cP(\lambda)`", ":math:`\Nset`", "Charlier", ":class:`~openturns.CharlierFactory`"
   "Binomial :math:`\cB(m,p)`", ":math:`\{0,\dots,m\}`", "Krawtchouk\ :math:`^{\dagger}`", ":class:`~openturns.KrawtchoukFactory`"
   "Polya :math:`\cP(r,p)`", ":math:`\Nset`", "Meixner", ":class:`~openturns.MeixnerFactory`"

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

