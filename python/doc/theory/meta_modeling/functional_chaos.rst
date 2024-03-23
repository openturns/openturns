.. _functional_chaos:

Functional Chaos Expansion
--------------------------

Introduction
~~~~~~~~~~~~

Accounting for the joint probability density function (PDF)
:math:`\mu_{\vect{X}}(\vect{x})` of the input random vector
:math:`\vect{X}`, one seeks the joint PDF of output random vector
:math:`\vect{Y} = g(\vect{X})`. This may be achieved using
Monte Carlo (MC) simulation (see :ref:`monte_carlo_simulation`). However, the MC
method may require a large number of model evaluations, i.e. a great
computational cost, in order to obtain accurate results.

A possible solution to overcome this problem is to project the model
:math:`g` in a suitable functional space, such as
the Hilbert space :math:`L^2(\mu_{\vect{X}})` of square-integrable functions with
respect to :math:`\mu_{\vect{X}}`.
More precisely, we may consider an expansion of the model onto an orthonormal
basis of :math:`L^2(\mu_{\vect{X}})`.
As an example of this type of expansion, one can mention expansions by
wavelets, polynomials, etc.

The principles of the building of a functional chaos expansion are described in the sequel.

Model
~~~~~

We consider the output random vector:

.. math::

    \vect{Y} = g(\vect{X})

where :math:`g: \Rset^{n_X} \rightarrow \Rset^{n_Y}` is the model,
:math:`\vect{X}` is the input random vector which distribution is
:math:`\mu_{\vect{X}}`,
:math:`n_X \in \Nset` is the input dimension,
:math:`n_Y \in \Nset` is the output dimension.
We assume that :math:`\vect{Y}` has finite variance i.e.
:math:`g\in L^2(\mu_{\vect{X}})`.

When :math:`n_Y > 1`, the functional chaos algorithm is used on each marginal
of :math:`\vect{Y}`, using the same multivariate orthonormal basis for
all the marginals.
Thus, the method is detailed here for a scalar output :math:`Y` and
:math:`g: \Rset^{n_X} \rightarrow \Rset`.

Iso-probabilistic transformation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`T: \Rset^{n_X} \rightarrow \Rset^{n_X}` be an *isoprobabilistic transformation*
(see :ref:`isoprobabilistic_transformation`) such that :math:`\vect{Z} = T(\vect{X}) \sim \mu`
where :math:`\mu` is the distribution of the standardized random vector :math:`\vect{Z}`.
The distribution is called the *measure* below.
As we will see soon, this distribution defines the scalar product that defines
the orthogonality property of the functional basis.
Let :math:`h` be the function defined by the equation:

.. math::
    h = g \circ T^{-1}.

Therefore :math:`h \in L^2\left(\mu_{\vect{Z}}\right)`.

Hilbert space
~~~~~~~~~~~~~

We introduce the *scalar product*:

  .. math::

        \scalarproduct{h_1}{h_2} _{L^2\left(\mu_{\vect{Z}}\right)}
        = \Expect{h_1(\vect{Z}) h_2(\vect{Z})}

for any :math:`(h_1,h_2) \in L^2\left(\mu_{\vect{Z}}\right)`.
For a continuous random variable, the scalar product is:

  .. math::
        \scalarproduct{h_1}{h_2} _{L^2\left(\mu_{\vect{Z}}\right)}
        & =  \int h_1(\vect{z}) h_2(\vect{z})\, \mu_{\vect{Z}}(\vect{z}) d\vect{z}.

For a discrete random variable, the scalar product is:

  .. math::
        \scalarproduct{h_1}{h_2} _{L^2\left(\mu_{\vect{Z}}\right)}
        & = \sum_\vect{z} h_1(\vect{z}) h_2(\vect{z})\, \Prob{\vect{Z} = \vect{z}}.

The associated norm is defined by:

  .. math::

        \|h\|^2_{L^2(\mu_{\vect{Z}})}
        = \Expect{\left(h(\vect{Z})\right)^2}

for any :math:`h \in L^2\left(\mu_{\vect{Z}}\right)`.
Based on this scalar product, the functional space
:math:`L^2\left(\mu_{\vect{Z}}\right)` is a Hilbert space.

Orthonormal basis
~~~~~~~~~~~~~~~~~
In this section, we introduce an orthonormal basis of the
previous Hilbert space.
Let :math:`\left(\Psi_k : \Rset^{n_X} \rightarrow \Rset\right)_{k \geq 0}` be
a set of functions.
This set is *orthonormal* with respect to :math:`\mu_{\vect{Z}}` if:

.. math::
   :label: orthonorm

    \scalarproduct{\Psi_k}{\Psi_{\ell}}  =  \delta_{k,\ell}

for any :math:`k, \ell \geq 0` where :math:`\delta_{k, \ell}` is the Kronecker symbol:

.. math::

  \delta_{k, \ell}
  =
  \begin{cases}
  1 & \textrm{ if } k = \ell, \\
  0 & \textrm{otherwise.}
  \end{cases}

See :ref:`orthogonal basis <orthogonal_basis>` for more details on the available
orthonormal bases.

In the library, we choose a basis :math:`\left(\Psi_k\right)_{k \geq 0}` which is orthonormal
with respect to :math:`\mu_{\vect{Z}}`, so that the equation :eq:`orthonorm` is
satisfied.
Furthermore, we require that the first element be:

  .. math::
    :label: defPsi0

      \Psi_0 = 1

The orthogonality of the functions imply:

  .. math::
      \scalarproduct{\psi_{i}}{\psi_{0}}_{L^2\left(\mu_{\vect{Z}}\right)} = 0

for any non-zero :math:`i`.
The equation :eq:`defPsi0` implies:

  .. math::

       \Expect{\psi_{i}(\vect{Z})} = \Expect{\Psi_{i}(\vect{Z})\Psi_{0}(\vect{Z})}
       = 0

for any :math:`i\neq 0`.

Functional chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The *functional chaos expansion* of *h* is (see [lemaitre2010]_ page 39):

.. math::

    h = \sum_{k \geq 0} a_k \Psi_k

where :math:`\left(a_k \in \Rset\right)_{k\geq 0}` is a set of coefficients.

We cannot compute an infinite set of coefficients.
The *truncated functional chaos expansion* is:

.. math::

    \widetilde{h} =  \sum_{k = 0}^{P} a_k \Psi_k

where :math:`P \in \Nset`.
Thus :math:`\widetilde{h}` is represented by a *finite* subset of coefficients :math:`(a_k)_{k = 0, ..., P}` in a *truncated* basis :math:`\left(\Psi_k\right)_{k = 0, ..., P}`.

A specific choice of :math:`P` can be done using one enumeration rule,
as presented in :ref:`enumeration_strategy`.
If the number of coefficients, :math:`P + 1`, is too large,
this can lead to *overfitting*.
This may happen e.g. if the total polynomial order we choose is too large.
In order to limit this effect, one method is to select the coefficients which
best predict the output, as presented in :ref:`polynomial_sparse_least_squares`.


Convergence of the expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In this section, we introduce the conditions which ensures
that the expansion converges to the function.

The orthonormal expansion of any function :math:`h \in L^2\left(\mu_{\vect{Z}}\right)`
converges in norm to :math:`h`, i.e.:

  .. math::
      \lim_{P \rightarrow \infty} \left\|h - \sum_{k = 0}^{P} a_k \Psi_k\right\|_{L^2\left(\mu_{\vect{Z}}\right)} = 0

if and only if the basis :math:`\left(\Psi_k\right)_{k \geq 0}` is a *complete
orthonormal system* (see [sullivan2015]_, page 139, [dahlquist2008]_,
theorem 4.5.16 page 456 and [rudin1987]_, section 4.24 page 85).

In other words:

  .. math::
       :label: fermeturePn

       \operatorname{span}\left(\left(\Psi_k\right)_{k \geq 0}\right) = L^2\left(\mu_{\vect{Z}}\right).

There are known sufficient conditions which ensure this property.
For example, if the support of :math:`\mu_{\vect{Z}}` is bounded, then
the basis is a complete orthonormal system.

There are known examples of infinite set of orthonormal polynomials
which are not complete, e.g. those derived from the log-normal distribution
(see [ernst2012]_).
In this case, the expansion may not converge to the function.
Nevertheless, even without any guarantee, it
is possible that the meta model built using the basis :math:`\left(\Psi_k\right)_{k \in \cK}` may be a good approximation of :math:`h`.

Compute the coefficients
~~~~~~~~~~~~~~~~~~~~~~~~
The meta model of *h* is the solution of the *linear least-squares optimization
problem*:

  .. math::
    :label: metaModeleh

     \vect{a}^\star  = \argmin_{\vect{a} \in \Rset^{P + 1}} \left\| h - \sum_{k = 0}^{P} a_k \Psi_k \right\|^2_{L^2\left(\mu_{\vect{Z}}\right)}.

Equation :eq:`metaModeleh` means that the coefficients :math:`(a_k)_{k \geq 0}`
minimize the quadratic error between the model and the polynomial approximation.
For more details of the PCE based on regression, see the :class:`~openturns.LeastSquaresStrategy` class.

If the basis :math:`\left(\Psi_k\right)_{k \in \cK}` is orthonormal,
then the least squares problem :eq:`metaModeleh` is well-conditioned.
In particular, the choice of basis has a major influence on the
conditioning of the least-squares problem :eq:`metaModeleh` (for example, using an orthonormal basis makes well-condtioned the
discretized problem).

The problem can be equivalently solved using the equations (see
[dahlquist2008]_ theorem 4.5.13 page 454):

.. math::
    :label: scalProd

    a_k^\star = \scalarproduct{h}{\Psi_k}

for :math:`k = 0, ..., P`.
Since the definition of the scalar product is based on an expectation,
this amounts to evaluate integrals.

Equation :eq:`scalProd` means that each coefficient :math:`a_k` is the scalar product of the
model with the *k-th* element of the orthonormal basis :math:`\left(\Psi_k\right)_{k \geq 0}`.
For more details on the PCE based on quadrature, see the :class:`~openturns.IntegrationStrategy` class.

Several algorithms are available to compute the coefficients :math:`(a_k)_{k \geq 0}`:

- see :class:`~openturns.IntegrationExpansion` for an algorithm based on quadrature,
- see :class:`~openturns.LeastSquaresExpansion` for an algorithm based on the
  least squares problem,
- see :class:`~openturns.FunctionalChaosAlgorithm` for an algorithm that
  can manage both methods.

A step-by-step method
~~~~~~~~~~~~~~~~~~~~~

Three steps are required in order to create a functional chaos algorithm:

- define the multivariate orthonormal basis;
- truncate the multivariate orthonormal basis;
- evaluate the coefficients.

These steps are presented in more detail below.

**Step 1 - Define the multivariate orthonormal basis**: the
multivariate orthonornal basis :math:`\left(\Psi_k\right)_{k \geq 0}` is built
as the tensor product of orthonormal univariate families.

The univariate bases may be:

- *polynomials*: the associated distribution :math:`\mu_i` can be continuous
  or discrete.
  Note that it is possible to build the polynomial family orthonormal to any
  arbitrary univariate distribution :math:`\mu_i` under some conditions.
  For more details on this basis, see :class:`~openturns.StandardDistributionPolynomialFactory`;

- Haar wavelets: they approximate functions with discontinuities.
  For details on this basis, see :class:`~openturns.HaarWaveletFactory`;

- Fourier series: for more details on this basis, see :class:`~openturns.FourierSeriesFactory`.

Furthermore, the numbering of the multivariate orthonormal basis
:math:`\left(\Psi_k\right)_{k \geq 0}` is given by an enumerate function
which defines a way to generate the collection of polynomial degrees used
for the univariate polynomials: an enumerate function
represents a bijection :math:`\Nset \rightarrow \Nset^{n_X}`.
See :class:`~openturns.LinearEnumerateFunction` or
:class:`~openturns.HyperbolicAnisotropicEnumerateFunction` for more details
on this topic.

**Step 2 - Truncate the multivariate orthonormal basis**: a
strategy must be chosen for the selection of the different terms of the
multivariate basis. The selected terms are gathered in the subset *K*.
For information about the possible strategies, see :class:`~openturns.FixedStrategy`
and :class:`~openturns.CleaningStrategy`.

**Step 3 -  Evaluate the coefficients**: a *projectionStrategy* must be chosen
for the estimation of the coefficients :math:`a_k`.

The meta model
~~~~~~~~~~~~~~

The meta model of *g* can be defined using the isoprobabilistic transformation :math:`T`:

.. math::
    :label: metaModeleg

    \widetilde{g} = \widetilde{h} \circ T.

See  :ref:`response_surface` to get more details on:

- the available constructions of the truncated multivariate orthogonal basis,

- the computation of the coefficients.

The approximation :math:`\widetilde{\model}` can be used to build an efficient
random generator of :math:`Y` based on the random vector :math:`\vect{X}`,
using the equation:

.. math::

    \widetilde{Y} = \widetilde{\model}(\vect{X}).

This equation can be used to simulate independent random observations
from the PCE.
This can be done by first simulating independent observations from
the distribution of the standardized random vector :math:`\vect{Z}`,
then by pushing forward these observations through the expansion.
See the :class:`~openturns.FunctionalChaosRandomVector` class
for more details on this topic.


Sensitivity analysis
~~~~~~~~~~~~~~~~~~~~
Assume that the input random vector has independent marginals and
that the basis :math:`\left(\Psi_k\right)_{k \geq 0}` is computed using
the tensor product of univariate orthonormal functions.
In that case, the Sobol' indices can easily be deduced from the coefficients
:math:`a_k`.
Please see :class:`~openturns.FunctionalChaosSobolIndices` for more details on this topic.


Polynomial chaos expansion for independent variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The library enables one to build the meta model called *polynomial chaos
expansion* based on an orthonormal basis of polynomials.
See :ref:`chaos_basis` for more details on polynomial chaos expansion.

Other chaos expansions for independent variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The library enables one to use the Haar wavelet functions or the Fourier series
as orthonormal basis with respect to each margin :math:`\mu_i`.
The Haar wavelets basis is orthonormal with respect to the the :math:`\cU(0,1)` measure (see
:class:`~openturns.HaarWaveletFactory`) and the Fourier series basis is orthonormal with respect to
the :math:`\cU(-\pi, \pi)` measure (see :class:`~openturns.FourierSeriesFactory`).


Some functional chaos expansions for dependent variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the components of the input random vector :math:`\vect{X}` are not
independent, we can use an iso-probabilistic transformation to map :math:`\vect{X}`
into :math:`\vect{Z}` with independent components.

Whatever the dependency in the standardized random vector :math:`\vect{Z}`,
it is also possible to build up a multivariate orthonormal basis with respect to
:math:`\mu_{\vect{Z}}`:

  .. math::

      \Psi_{\idx}(\vect{z})
      = \dfrac{\mu_{Z_1}(z_1) \cdots \mu_{Z_{n_X}}(z_{n_X})}{\mu_{\vect{X}}(\vect{z})} \;
      \prod_{i=1}^{n_X} \pi^{(i)}_{\alpha_{i}}(z_{i})


where :math:`\mu_i` is the :math:`i` -th marginal of :math:`\mu_{\vect{X}}`
and :math:`\pi^{(i)}_{\alpha_{i}}` is the degree :math:`\alpha_i` orthonormal
family of polynomial for the :math:`i`-th marginal.
Notice that:

  .. math::
    :label: soizeghanem

     \dfrac{\mu_{Z_1}(z_1) \cdots \mu_{Z_{n_X}}(z_{n_X})}{\mu_{\vect{Z}}(\vect{z})}
     = \dfrac{1}{\sqrt{c(\vect{z})}}


where :math:`c` is the density of the copula of :math:`\vect{Z}`.

Link with classical deterministic polynomial approximation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a deterministic setting (i.e. when the input parameters are
considered to be deterministic), it is of common practice to substitute
the model function :math:`h` by a polynomial approximation over its
whole domain of definition. Actually this approach is
equivalent to:

#. regarding the input parameters as random uniform random variables,

#. expanding any quantity of interest provided by the model onto a PC
   expansion made of Legendre polynomials.

.. topic:: API:

    - See :class:`~openturns.FunctionalChaosAlgorithm`
    - See :class:`~openturns.HaarWaveletFactory`
    - See :class:`~openturns.FourierSeriesFactory`
    - See :class:`~openturns.SoizeGhanemFactory`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFactory`


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`
    - See :doc:`/auto_functional_modeling/univariate_functions/plot_createUnivariateFunction`


.. topic:: References:

    - [lemaitre2010]_
    - [sullivan2015]_
    - [xiu2010]_
    - [soizeghanem2004]_
    - [dahlquist2008]_
    - [rudin1987]_
    - [ghanem1991]_
