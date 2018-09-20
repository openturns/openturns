.. _distribution_realization:

Distribution realizations
-------------------------

| The objective is to transform a random realization of the Uniform(0,1)
  distribution onto a random realization of a distribution which
  cumulative density function is :math:`F`.

Several classical techniques exist:

-  The inversion of the CDF: if :math:`U` is distributed according to
   the uniform distribution over :math:`[0, 1]` (the bounds 0 and 1 may
   or may not be included), then :math:`X=F^{-1}(U)` is distributed
   according to the CDF :math:`F`. If :math:`F^{-1}` has a simple
   analytical expression, it provides an efficient way to generate
   realizations of :math:`X`. Two points need to be mentioned:

   #. If the expression of :math:`F^{-1}(U)` involves the quantity
      :math:`1-U` instead of :math:`U`, it can be replaced by :math:`U`
      as :math:`U` and :math:`1-U` are identically distributed;

   #. The numerical range of :math:`F` is always bounded (i.e. the
      interval over which it is invertible) even if its mathematical
      range is unbounded, and the numerical range may not preserve the
      symmetry of its mathematical counterpart. It can lead to biased
      nonuniform generators, even if this bias is usually small. For
      example, using standard double precision, the CDF of the standard
      normal distribution is numerically invertible over :math:`[A,B]`
      with :math:`A\simeq -17.3` and :math:`B\simeq 8.5`.

-  The rejection method: suppose that one want to sample a random
   variable :math:`X` with a continuous distribution with PDF
   :math:`p_X` and that we know how to sample a random variable
   :math:`Y` with a continuous distribution with PDF :math:`p_Y`. We
   suppose that there exist a positive scalar :math:`k` such that
   :math:`\forall t\in\Rset, p_X(t)\leq kp_Y(t)`. The rejection method
   consists of the following steps:

   #. Generate :math:`y` according to :math:`p_Y`;

   #. Generate :math:`u` according to a random variable :math:`U`
      independent of :math:`Y` and uniformly distributed over
      :math:`[0, 1]`;

   #. If :math:`u\leq p_X(u)/(kp_Y(u))`, accept :math:`y` as a
      realization of :math:`X`, else return to step 1.

   The rejection method can be improved in several ways:

   -  If the evaluation of :math:`\rho(u)=p_X(u)/(kp_Y(u))` is costly,
      and if one knows a cheap function :math:`h` such that
      :math:`h(u) \leq \rho(u)`, then one can first check if
      :math:`u\leq h(u)` and directly accept :math:`y` if the test is
      positive (quick acceptance step). This is very effective if
      :math:`h(u)` can be evaluated from quantities that have to be
      computed to evaluate :math:`\rho(u)`: :math:`h` is a kind of cheap
      version of :math:`\rho`. The same trick can be use if one knows a
      cheap function :math:`m` such that :math:`m(u) \geq \rho(u)`: one
      checks if :math:`u\geq m(u)` and directly reject :math:`y` if the
      test is positive (quick rejection test). The combination of quick
      acceptation and quick rejection is called a *squeeze*.

   -  The test :math:`u\leq \rho(u)` can be replaced by an equivalent
      one but much more computationally efficient.

-  The transformation method: suppose that one want to sample a random
   variable that is the image by a simple transformation of another
   random variable (or random vector) that can easily be sampled. It is
   easy to sample this last random variable (or vector) and then
   transform this realization through the transformation to get the
   needed realization. This method can be combined with the rejection
   method for example, to build :math:`h` or :math:`m` implicitly.

-  The sequential search method (discrete distributions): it is a
   particular version of the CDF inversion method, dedicated to discrete
   random variables. One generates a realization :math:`u` of a random
   variable :math:`U` uniformly distributed over :math:`[0, 1]`, then we
   search the smallest integer :math:`k` such that
   :math:`u\leq\sum_{i=0}^kp_i`, where :math:`p_i=\Prob{X=i}`.

-  The stable distribution method (archimedean copulas): to be detailed.

-  The conditional CDF inversion (general copula or general multivariate
   distributions): this method is a general procedure to sample a
   multivariate distribution. One generate in sequence a realization of
   the first marginal distribution, then a realization of the
   distribution of the second component conditionally to the value taken
   by the first component, and so on. Each step is done by inversion of
   the conditional CDF of the component :math:`i` with respect to the
   value taken by the preceding components.

-  The ratio of uniforms method: this is a special combination of the
   rejection method and the transformation method that has gained a
   great popularity due to its concision and its versatility. Let
   :math:`\cA=\{(u,v):\quad 0\leq u\leq \sqrt{f\left(\frac{u}{v}\right)}\}`.
   If :math:`(U,V)` is a random vector uniformly distributed over
   :math:`\cA`, then :math:`\frac{U}{V}` has density :math:`f/\int f`.
   The generation of :math:`(U, V)` is done by a rejection method, using
   a bounded enclosing region of :math:`\cA`. It can be done if and only
   if both :math:`f(x)` and :math:`x^2f(x)` are bounded. This method can
   be enhanced by using quick acceptance and quick rejection steps.

-  The ziggurat method: this method allows for a very fast generation of
   positive random variate with decreasing PDF. The graph of the PDF is
   partitioned into horizontal slices of equal mass, the bottom slice
   covering the whole support of the PDF. All these slices have a
   maximal enclosed rectangle (the top one being empty) and a minimal
   enclosing rectangle (the bottom one not being defined). Then, one
   generate a discrete uniform random variable :math:`d` over the number
   of slice. It selects a slice, and if this slice has both an enclosed
   and an enclosing rectangle, one generates a realization of a
   continuous uniform random variable on :math:`[0,L_i]`, :math:`L_d`
   being the length of the enclosing rectangle of slice :math:`d`. The
   enclosing and the enclosed rectangles define an efficient squeeze for
   a rejection method. If the bottom slice is selected, one has to
   sample the tail distribution conditional to the length of the
   enclosed rectangle: it is the only case where a costly non-uniform
   random number has to be computed. If the number of slices is large
   enough, this case appears only marginally, which is the main reason
   for the method efficiency.

The techniques implemented in each distribution are:

- Arcsine: CDF inversion
- Bernoulli: CDF inversion
- Beta:

   - CDF inversion if :math:`r=1` or :math:`t-r=1`.
   - Rejection (Johnk’s method) for :math:`t\leq 1`.
   - Rejection (Cheng’s method) for :math:`r>1`, :math:`t-r>1`.
   - Rejection (Atkinson and Whittaker method 1) for :math:`t > 1, \max(r, t-r) < 1`.
   - Rejection (Atkinson and Whittaker method 2) for :math:`t > 1, \max(r, t-r) > 1`.

- Binomial: Squeeze and Reject method: See [Hormann1992]_.
- Burr: CDF inversion.
- Chi: Transformation.
- ChiSquare: See the Gamma distribution.
- ClaytonCopula: Conditional CDF inversion.
- ComposedCopula: Simulation of the copula one by one then association.
- ComposedDistribution: Simulation of the copula and the marginal with CDF inversion.
- Dirac: Return the supporting point.
- Dirichlet: Transformation.
- Epanechnikov: CDF inversion.
- Exponential: CDF inversion.
- Fisher-Snedecor: Transformation.
- FrankCopula: Conditional CDF inversion.
- Gamma:  Transformation and rejection, see [Marsaglia1993]_.
- Geometric: CDF inversion.
- Generalized Pareto: CDF inversion.
- GumbelCopula: Stable distribution.
- Gumbel: CDF inversion.
- Histogram: CDF inversion.
- IndependentCopula: Transformation.
- InverseNormal: Transformation.
- KernelMixture: Transformation.
- Kpermutaions: Knuth’s algorithm.
- Laplace: CDF inversion.
- Logistic: CDF inversion.
- LogNormal: Transformation.
- LogUniform: Transformation.
- Meixner: Uniform ratio method.
- MinCopula: Transformation.
- Mixture: Transformation.
- MultiNomial: Conditional CDF inversion.
- Non Central Chi Square: Transformation.
- NegativeBinomial: Conditional simulation (Poisson|Gamma)
- Non Central Student: Transformation.
- NormalCopula: Transformation of independent Normal realizations.
- Normal:

   - 1D: Ziggurat method
   - nD: Transformation of independent Normal realizations

- Poisson:

   - Sequential search for :math:`\mu < 6`
   - Ratio of uniforms for :math:`\mu\geq 6`

- RandomMixture: Transformation
- Rayleigh: CDF inversion
- Rice: Transformation
- Skellam: Transformation
- SklarCopula: Conditional CDF inversion by Gaussian quadrature and numerical inversion
- Student: Transformation
- Trapezoidal: CDF inversion
- Triangular: CDF inversion
- TruncatedDistribution: on :math:`[a,b]` we note :math:`F` the CDF of the non truncated distribution

   - if :math:`F(b)-F(a)<s`: CDF inversion
   - if :math:`F(b)-F(a)>s` : rejection

  By default, :math:`s=0.5` (modifiable)
- TruncatedNormal:

   - small truncation interval: CDF inversion
   - large truncation interval: rejection
- Uniform: Transformation.
- UserDefined: Sequential search.
- Weibull: CDF inversion.
- Zipf-Mandelbrot: Bisection search.


.. topic:: API:

    - See the available :ref:`parametric models <parametric_distributions>`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/distribution_manipulation`

.. topic:: References:

    - [Devroye1986]_
    - [Hormann1992]_
    - [Marsaglia1993]_
    - [Doornik2005]_
    - [Aas2004]_
    - [Stadlober1990]_
