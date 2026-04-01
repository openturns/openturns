.. _enumeration_multivariate_basis:

Tensorized multivariate basis enumeration functions
---------------------------------------------------

Enumeration functions (refer to :ref:`enumeration_strategy`) help to enumerate a multivariate basis
built as the tensorization of univariate basis, using the indexation of each marginal basis.

Such a  multivariate basis is used, for example, in the :ref:`functional chaos expansion setting <functional_chaos>`.

Let consider some :math:`\inputDim` univariate basis, denoted by
:math:`(\pi_{k}^{(i)})_{k \geq 0}` for :math:`1 \leq i \leq \inputDim`, where each
:math:`\pi_{k}^{(i)}: \Rset \rightarrow \Rset`.

Let denote by :math:`\{\psi_{\vect{\alpha}},\vect{\alpha} \in \Nset^\inputDim\}` a multivariate basis
built as the tensorization of the univariate basis. The multivariate basis term
:math:`\psi_{\vect{\alpha}}` is defined by the product:

.. math::

    \psi_{\vect{\alpha}} (\vect{\xi}) = \pi_{\alpha_1}^{(1)}(\xi_1) \times \dots \times
    \pi_{\alpha_{\inputDim}}^{({\inputDim})}(\xi_{\inputDim})

for any :math:`\vect{\xi} \in \mathbb{R}^{\inputDim}`.

When the univariate basis are polynomials such that :math:`\alpha_i`
is the degree of :math:`\pi_{\alpha_i}^{(i)}`, then the multi-index represents
the marginal degrees of the polynomial :math:`\psi_{\vect{\alpha}}`. In that case,
the length of the multi-index is the total degree of the polynomial.

Several enumeration functions can be used:

- the linear enumeration function,
- the hyperbolic enumeration function,
- the anisotropic hyperbolic enumeration function,
- the infinity norm enumeration function.

We detail the interest of each one in the functional chaos expansion setting.

Linear enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The linear enumeration function builds the multivariate polynomials by increasing total degrees and for a
given value of the total degree, by  "graded reverse-lexicographic ordering" (see [sullivan2015]_).

Hyperbolic enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The hyperbolic enumeration function is inspired by the so-called
*sparsity-of-effects principle*, which states that most models are
principally governed by main effects and low-order interactions.
Accordingly, one wishes to define an enumeration function which first
selects those multi-indices related to main effects, i.e. with a
reasonably small number of nonzero components, prior to selecting
those associated with higher-order interactions.

The hyperbolic enumeration functions are based on the q-norm  defined in :ref:`enumeration_strategy`,
equation :eq:`qnorm_def`.
The smaller :math:`q` is, the more the indices containing high-order
interactions are penalized.

The indices are ordered by increasing  q-norm and for a same  q-norm,
according to the graded reverse-lexicographic ordering.

Anisotropic hyperbolic enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The anisotropic hyperbolic enumeration functions is a weighted version
of the hyperbolic enumeration function, using a weighted q-norm
defined in  :ref:`enumeration_strategy`,
equation :eq:`anisotropEnumFct`. The components with large
anisotropic coefficients will have marginal degrees lower than the components with small anisotropic coefficients.

Therefore, when the model is governed by main effects of specific inputs, the associated weights
should be small compared to the other ones.

The indices are ordered by increasing  weighted q-norm and for a same
weighted q-norm, according to the graded reverse-lexicographic ordering.

Infinity norm enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The infinity norm enumeration function is based on the infinity-norm
defined in :ref:`enumeration_strategy`, equation :eq:`def_infNorm`.

This function allows one to define the largest space of polynomials with given maximum marginal degree.
When a tensorized Gaussian quadrature formula (:class:`~openturns.GaussProductExperiment`)
is obtained by tensorization of univariate formulas, this space (with the appropriate marginal degrees)
is the space of polynomials whose integrals are computed exactly by the quadrature rule.

.. topic:: API:

    - See :class:`~openturns.LinearEnumerateFunction`
    - See :class:`~openturns.HyperbolicAnisotropicEnumerateFunction`
    - See :class:`~openturns.NormInfEnumerateFunction`

.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_functional_chaos`
    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_enumeratefunction`
    - See :doc:`/auto_surrogate_modeling/fields_surrogate_models/plot_fieldfunction_metamodel`

.. topic:: References:

    - [blatman2009]_
