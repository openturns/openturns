.. _functional_chaos:

Functional Chaos Expansion
--------------------------

Accounting for the joint probability density function (PDF)
:math:`\mu_{\inputRV}(\vect{x})` of the input random vector
:math:`\inputRV`, one seeks the joint PDF of output random vector
:math:`\outputRV = \model(\inputRV)`. This may be achieved using
Monte Carlo (MC) simulation (see :ref:`monte_carlo_simulation`). However, the MC
method may require a large number of model evaluations, i.e. a great
computational cost, in order to obtain accurate results.

A possible solution to overcome this problem is to project the model
:math:`\model` in a suitable functional space, such as
the Hilbert space :math:`L^2\left(\mu_{\inputRV}\right)` of square-integrable functions with
respect to :math:`\mu_{\inputRV}`. This projection is called a *functional chaos expansion* of
the model :math:`\model`.

The principles of the building of a functional chaos expansion are described in the sequel.

Methodology: principles
~~~~~~~~~~~~~~~~~~~~~~~

We consider the output random vector:

.. math::

    \outputRV = \model(\inputRV)

where :math:`\model: \Rset^{\inputDim} \rightarrow \Rset^{\outputDim}` is the model,
:math:`\inputRV` is the input random vector which distribution is
:math:`\mu_{\inputRV}`.

We assume that :math:`\outputRV` has finite variance i.e.
:math:`\model \in L^2\left(\mu_{\inputRV}\right)`.

When :math:`\outputDim > 1`, the functional chaos algorithm is used on each marginal
of :math:`\outputRV`, using the same multivariate orthonormal basis for
all the marginals.
Thus, the method is detailed here for a scalar output :math:`Y` and
:math:`\model: \Rset^{\inputDim} \rightarrow \Rset`.

The functional space
:math:`L^2\left(\mu_{\inputRV}\right)` is a Hilbert space equipped with an inner
product defined by:

.. math::

    \scalarproduct{h_1}{h_2}_{L^2\left(\mu_{\inputRV}\right)}
    = \Expect{h_1(\inputRV) h_2(\inputRV)}

for any :math:`(h_1,h_2) \in L^2\left(\mu_{\inputRV}\right)`.
For a continuous random variable, the inner product is defined by:

.. math::

    \scalarproduct{h_1}{h_2}_{L^2\left(\mu_{\inputRV}\right)}
    =  \int h_1(\vect{x}) h_2(\vect{x})\, \mu_{\inputRV}(\vect{x}) d\vect{x}.

For a discrete random variable, the scalar product is:

.. math::

    \scalarproduct{h_1}{h_2}_{L^2\left(\mu_{\inputRV}\right)}
    = \sum_\vect{x} h_1(\vect{x}) h_2(\vect{x})\, \Prob{\inputRV = \vect{x}}.

The associated norm is defined by:

.. math::

    \|h\|^2_{L^2\left(\mu_{\inputRV}\right)}
    = \Expect{\left(h(\inputRV)\right)^2}

for any :math:`h \in L^2\left(\mu_{\inputRV}\right)`.

The Hilbert space :math:`L^2\left(\mu_{\inputRV}\right)` admits an orthonormal basis denoted by
:math:`(\psi_k)_{k \geq 0}` with :math:`\psi_k : \Rset^{\inputDim} \rightarrow \Rset`. Therefore, any
:math:`h \in L^2\left(\mu_{\inputRV}\right)` can be written as a *functional chaos expansion*
(see [lemaitre2010]_ page 39):

.. math::

    h = \sum_{k \geq 0} a_k \psi_k

where :math:`\left(a_k \in \Rset\right)_{k\geq 0}` is a sequence of coefficients.
The meta model is the truncation of the expansion to a *finite* subset :math:`I` of coefficients:

.. math::

    \metaModel =  \sum_{k \in I} a_k \psi_k


Methodology: step by step
~~~~~~~~~~~~~~~~~~~~~~~~~

The construction of a meta model as a truncated functional expansion consists in choosing:

- Choice 1: an approximation space :math:`\cP` such that:

.. math::
   :label: fermetureP

   \overline{\cP} = L^2\left(\mu_{\inputRV}\right).

- Choice 2: a sequence of nested approximation spaces :math:`(\cP_n)_{n \in \Nset}` such that:

.. math::
   :label: UnionPn

   \cP = \bigcup_{n \in \Nset} \cP_n

which implies that any function in :math:`L^2\left(\mu_{\inputRV}\right)` is the limit with
respect to :math:`L^2\left(\mu_{\inputRV}\right)` of a sequence :math:`(h_n)_{n \in \Nset}` such
that :math:`h_n \in \cP_n`:

.. math::
   :label: convergence

   \lim_{n \rightarrow \infty} \left\|\model - h_n\right\|_{L^2\left(\mu_{\inputRV}\right)} = 0


- Choice 3: a basis :math:`\left(\psi_k\right)_{k \in I_n}` of :math:`\cP_n`:

.. math::
   :label: Pn

   \cP_n = \mathrm{span}(\psi_k)_{k \in I_n}

where :math:`I_n` is a finite index set.

- Choice 4: a specific space :math:`\cP_n` in which :math:`\model` is approximated by
  :math:`\metaModel \in \cP_n` as follows:

.. math::
   :label: metaModelPn

   \metaModel =  \sum_{k \in I_n} a_k \psi_k

which amounts to solving:

.. math::
   :label: metaModeleF

   \metaModel = \arg\min_{h_n \in \cP_n} | \model - h_n |^2_{L^2\left(\mu_{\inputRV}\right)}

This problem is a linear least-squares problem.

Which approximation space and sequence of nested approximation spaces?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many choices are possible for :math:`\cP` and the associated sequence :math:`(\cP_n)_{n \in \Nset}`.

For instance, one may choose :math:`\cP` to be **the set of multivariate polynomials**,
provided that  :math:`\cP` verifies the condition :eq:`fermetureP` (see also [sullivan2015]_,
page 139, [dahlquist2008]_, theorem 4.5.16 page 456 and
[rudin1987]_, section 4.24 page 85).

We then consider a sequence of nested spaces :math:`(\cP_n)_{n \in \Nset}` defined as follows: we
construct a complete family using graded polynomials by introducing a bijection :math:`\tau`
from :math:`\Nset` into :math:`\Nset^\inputDim`. The mapping  :math:`\tau(k)` specifies the
multi-index of marginal degrees (this bijection ensures that all polynomials are covered and
that any finite family is linearly independent). Then, :math:`\cP_n` is the space spanned by
polynomials with marginal degrees :math:`\tau(0), \dots, \tau(n)`. Depending on the choice of
:math:`\tau`, :math:`\cP_n` may correspond to the set of polynomials of total degree less than
:math:`n`. See :ref:`enumeration_multivariate_basis` and :ref:`enumeration_strategy` for more details on this topic.

Note that, depending on the distribution :math:`\mu_{\inputRV}`, the space :math:`\cP` of
multivariate polynomials does not always satisfy
:math:`\cP \subset L^2\left(\mu_{\inputRV}\right)`, and even when this inclusion holds, the
density condition :eq:`fermetureP` must also be satisfied:

- First, for all polynomial spaces :math:`\cP_n` to be subspaces of
  :math:`L^2\left(\mu_{\inputRV}\right)`, it is required that
  :math:`\langle X^n, 1 \rangle_{L^2\left(\mu_{\inputRV}\right)} = \Expect{X^n}` for
  :math:`n \geq 0` be finite in the scalar case, and
  :math:`\left\langle \prod_{i \in A} X_i^{n_i}, 1 \right\rangle_{L^2\left(\mu_{\inputRV}\right)} =
  \Expect{\prod_{i \in A} X_i^{n_i}}` with :math:`A \subset \llbracket 1, d \rrbracket` in
  dimension :math:`\inputDim`.
  This implies that the measure :math:`\mu_{\inputRV}` must have finite moments of all orders.
  This is the case, for instance,  when the support of :math:`\mu_{\inputRV}` is bounded.
  However,this is not the case, for instance, when :math:`\mu_{\inputRV}` is the Cauchy distribution: for
  this distribution, the only polynomial space contained in :math:`L^2\left(\mu_{\inputRV}\right)`
  is the space of constant polynomials, since the Cauchy distribution has an infinite expectation.

- Moreover, among probability measures :math:`\mu_{\inputRV}` that admit moments of all orders,
  the closure property :eq:`fermetureP` is not always guaranteed.  For instance, the log-normal
  distribution (see [ernst2012]_). In this case, the expansion may not converge to the function.
  Nevertheless, even without any guarantee, it
  is possible that the meta model built using the basis
  :math:`\left(\psi_k\right)_{k \in I_n\}}` may be a good approximation of :math:`\model`.

One may also consider **non-polynomial approximation spaces** :math:`\cP`. For instance, the space
spanned by trigonometric polynomials (the so-called Fourier space) or spanned by the Haar
wavelets.

If the approximation space :math:`\cP` does not satisfy condition :eq:`fermetureP`, i.e., if
:math:`\overline{\cP} \nsubseteq L^2\left(\mu_{\inputRV}\right)`, then one may use an
:ref:`isoprobabilistic transformation <isoprobabilistic_transformation>` :math:`T: \Rset^{\inputDim} \rightarrow \Rset^{\inputDim}`
such that :math:`\vect{U} = T(\inputRV)` is a random
vector distributed according to a density :math:`\mu_{\vect{U}}` satisfying
:math:`\overline{\cP} = L^2\left(\mu_{\vect{U}}\right)`.

The model :math:`\model` is then transformed into
:math:`h = \model \circ T^{-1} \in L^2\left(\mu_{\vect{U}}\right)`.
One seeks :math:`\widetilde{h}`, the orthogonal projection of :math:`h`
with respect to :math:`\mu_{\vect{U}}` onto the polynomial space :math:`\cP_n`.
The metamodel of :math:`\model` is then given by:

.. math::
   \metaModel = \widetilde{h} \circ T

Projecting :math:`h` onto the basis :math:`(\varphi_k)_{k \in I_n}` of :math:`\cP_n` orthogonally
with respect to :math:`\mu_{\vect{U}}` is equivalent to projecting :math:`\model` onto the
space spanned by :math:`(\varphi_k \circ T)_{k \in I_n}` orthogonally with respect to
:math:`\mu_{\inputRV}`.

Which basis of the approximation space?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The sub-spaces :math:`(\cP_n)_{n \in \Nset}` are nested which means that the basis of
:math:`\cP_{n+1}` is constructed from that of :math:`\cP_{n}` by enrichment. Several choices
of basis are possible.

We can choose **a basis which is orthonormal with respect to**
:math:`\boldsymbol{\mu_{\inputRV}}` (see
:ref:`chaos_basis` for more details):

- If  :math:`\mu_{\inputRV}` has independent components,
  this multivariate orthonormal basis can be built as the tensor product of the univariate basis
  orthonormal to each component of :math:`\mu_{\inputRV}`.
- If :math:`\mu_{\inputRV}` has dependent components, we can use an
  :ref:`isoprobabilistic transformation <isoprobabilistic_transformation>`
  :math:`T: \Rset^{\inputDim} \rightarrow \Rset^{\inputDim}` that maps the measure
  :math:`\mu_{\inputRV}` into a measure :math:`\mu_{\vect{U}}` with independent marginals.
  We can also use the Soize-Ghanem basis but this usage is not recommended.

We can also choose **a basis which is not orthonormal with respect to**
:math:`\boldsymbol{\mu_{\inputRV}}` but
which is orthonormal with respect to an *instrumental measure* :math:`\tilde{p} \neq \mu_{\inputRV}`.
This instrumental measure is such that:

- the support of :math:`\tilde{p}` is larger than the support of :math:`\mu_{\inputRV}`,
- :math:`\tilde{p}` has independent components.

This choice is done in the **domination** strategy.

Which sub-space of approximation?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the dimension of :math:`\cP_n` (that is to say if the number of coefficients to be
computed) is too large, this can lead to *overfitting*. This may happen for instance if the
total polynomial order we choose is too large.

In order to limit this effect, one method is to define a strategy for exploring the basis (see
:ref:`sparse_least_squares`) as well as a strategy to select the coefficients which
best predict the output (see  :class:`~openturns.FixedStrategy` and :class:`~openturns.CleaningStrategy`).

Estimate the coefficients
~~~~~~~~~~~~~~~~~~~~~~~~~
In this section, we give some elements to estimate the coefficients of the expansion.

The vector of coefficients is the solution of the linear least-squares problem defined in
:ref:`functional_chaos` by equation :eq:`metaModeleF`. Refer to :ref:`least_squares`
for more details on the resolution of the least-squares problem.

The choice of basis of :math:`\cP_n` has a major impact on the conditioning of the
least-squares problem.
Indeed, if the basis :math:`\left(\psi_k\right)_{k \in I_n}` is
orthonormal with respect to :math:`\mu_{\inputRV}`, then the design matrix of
the least squares problem is well-conditioned.
In the other case, the design matrix might be ill-conditioned, leading to numerical instabilities.

If the chosen basis :math:`\left(\psi_k\right)_{k \in I_n}` is orthonormal with respect to
:math:`\mu_{\inputRV}`, then the coefficients :math:`(a_k)_{k \in I_n}` can be computed
using the inner product (see [dahlquist2008]_ theorem 4.5.13 page 454) as follows:

.. math::
    :label: scalProd

    a_k = \scalarproduct{h}{\psi_k}_{L^2\left(\mu_{\inputRV}\right)}

for :math:`k\in I_n`.
This integral can computed using a quadrature rule (see the documentation
of the :class:`~openturns.IntegrationStrategy` class).
This is the case for instance when an :ref:`isoprobabilistic_transformation` :math:`T` is used
and the model
:math:`h = \model \circ T^{-1}` is expanded onto the basis
:math:`\left(\psi_k\right)_{k \in I_n}` which is orthonormal with respect to
:math:`\mu_{\vect{U}}`. Thus, we get:

.. math::
   :label: scalProdTransf

   a_k = \scalarproduct{h}{\psi_k}_{L^2\left(\mu_{\vect{U}}\right)} = \scalarproduct{\model}
   {\psi_k \circ T}_{L^2\left(\mu_{\inputRV}\right)}


Generally speaking, choosing a basis that is orthonormal with respect to  :math:`\mu_{\inputRV}`
simplifies the computation of the coefficients :math:`(a_k)_{k \in I_n}` by turning the
least-squares problem into the computation of inner products: in this case, solving the
least-squares problem amounts to evaluating inner products, resulting in a significantly lower
computational cost.

However, the optimal estimator of the coefficient vector is the one obtained by solving the
least-squares problem. Therefore, for estimating the coefficients, it is preferable to solve
the least-squares problem even when the basis of the approximation space is orthonormal with
respect to the distribution :math:`\mu_{\inputRV}`.

In a probabilistic setting of independent and identically distributed samples,
the two estimates (from least-squares problem and from the inner product) are consistent
but have different asymptotic variance, resulting in two qualities of approximation
(see [lemaitre2010]_ eq. 3.48 and eq. 3.49 page 66).

Several algorithms are available to compute the coefficients
:math:`(a_k)_{k \in I_n}`:

- see :class:`~openturns.IntegrationExpansion` for an algorithm based on
  quadrature,
- see :class:`~openturns.LeastSquaresExpansion` for an algorithm based on the
  least squares problem,
- see :class:`~openturns.FunctionalChaosAlgorithm` for an algorithm that
  can manage both methods.

Cross Validation of the functional chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cross-validation of a polynomial chaos expansion
uses the theory presented in :ref:`cross_validation`.
In [blatman2009]_ page 84, the author applies the LOO equation to polynomial
chaos expansion
(see appendix D page 203 for a proof).
If the coefficients are estimated from integration, the same derivation cannot,
in theory, be applied.

The fast methods presented in :ref:`cross_validation` can be applied:

- the fast leave-one-out cross-validation,
- the fast K-Fold cross-validation.

Refer to :class:`~openturns.FunctionalChaosValidation`.

Usual exploitation of the functional chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
There are many ways to use the functional chaos expansion. We present two usual exploitations:

- using the expansion as a random vector generator,
- performing the sensitivity analysis of the expansion.

The first usage is to create a random vector :math:`\widetilde{Y}` defined by:

.. math::

    \widetilde{Y} = \metaModel(\inputRV).

This equation can be used to simulate independent random observations
from the functional chaos expansion: see the :class:`~openturns.FunctionalChaosRandomVector`
class for more details on this topic.

The second usage assumes that the input distribution :math:`\mu_{\inputRV}` has independent
marginals and that the basis :math:`\left(\psi_k\right)_{k \in I_n}` is orthonormal with respect
to  :math:`\mu_{\inputRV}` and that the first element be:

  .. math::
    :label: defPsi0

      \psi_0 = 1

The orthogonality of the functions implies that:

  .. math::
      \mathbf{E}_{\mu}\left[\psi_{i}(\vect{X})\right] = \scalarproduct{\psi_{i}}{\psi_{0}}_{L^2\left(\mu\right)} = 0

for any non-zero :math:`i\neq 0`.

In that case, the Sobol' indices can easily be deduced from the coefficients
:math:`(a_k)_{k \in I_n, k\neq 0}`: see :class:`~openturns.FunctionalChaosSobolIndices` for
more details on this topic.

.. topic:: API:

    - See :class:`~openturns.FunctionalChaosAlgorithm`
    - See :class:`~openturns.HaarWaveletFactory`
    - See :class:`~openturns.FourierSeriesFactory`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFactory`
    - See :class:`~openturns.experimental.FiniteOrthogonalFunctionFactory`
    - See :class:`~openturns.FunctionalChaosValidation`.


.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_functional_chaos`
    - See :doc:`/auto_functional_modeling/univariate_functions/plot_create_univariate_function`


.. topic:: References:

    - [lemaitre2010]_
    - [sullivan2015]_
    - [xiu2010]_
    - [soizeghanem2004]_
    - [dahlquist2008]_
    - [rudin1987]_
    - [ghanem1991]_
    - [blatman2009]_
