.. _chaos_basis:

Polynomial chaos basis
----------------------

This page introduces *polynomial chaos expansion*.
We consider the notations introduced in :ref:`functional_chaos`.

Let :math:`T : \Rset^{n_X} \rightarrow \Rset^{n_X}` be the isoprobabilistic
transformation such that :math:`\vect{Z} = T(\vect{X})` has independent
marginals.
Therefore, the probability density function of the standard random vector
:math:`\vect{Z}` is the product of the marginal probability density functions:

  .. math::

        \mu_{\vect{Z}}(\vect{z})= \prod_{i=1}^{n_X} \mu_{Z_i}(z_i)

for any :math:`\vect{z} \in \Rset^{n_X}` where :math:`\mu_{\vect{Z}}`
is the joint PDF of the random vector :math:`\vect{Z}` and :math:`\mu_{Z_i}`
is the marginal PDF of the random variable :math:`Z_i`.

For any :math:`i \in \{0, ..., n_X\}`, let
:math:`\left(\pi_k^{(i)}\right)_{k \geq 0}` be the family of univariate
polynomials of the :math:`i`-th marginal orthogonal with respect to the
marginal distribution :math:`\mu_{Z_i}`, where :math:`k` represents the
polynomial degree.
The multivariate polynomial basis can be built using the *tensor product* of
univariate polynomials which are orthonormal with respect to :math:`\mu_{\vect{Z}}`.
The orthonormal polynomials are:

  .. math::

        \Psi_\vect{\alpha}(\vect{z}) = \prod_{i=1}^{n_X} \pi_{\alpha_i}^{(i)}(z_i)

where :math:`\vect{\alpha} = (\alpha_1, \dots, \alpha_{n_X}) \in \Nset^{n_X}` is the multi-index
representing the marginal polynomial degrees and :math:`\pi_{\alpha_i}^{(i)}`
is the :math:`i`-th marginal univariate orthonormal polynomial of degree
:math:`\alpha_i`.
The orthonormal polynomial basis with respect to the marginal :math:`\mu_{Z_i}` is
known for some distributions: see :ref:`orthogonal_polynomials` for more
details on classical orthonormal polynomial families.

For classical polynomials, the :class:`~openturns.StandardDistributionPolynomialFactory`
class implements the three-term recurrence.
If the family is not already known, the polynomials can be represented by
their three-term recurrence, using the adaptive Stieljes algorithm (see
:class:`~openturns.AdaptiveStieltjesAlgorithm`).
Once the sequence of recurrence coefficients is known, the reverse Clenshaw
algorithm enables fast, stable evaluation of the polynomials
at any point (see :class:`~openturns.OrthogonalUniVariatePolynomial`).

.. topic:: API:

    - See :class:`~openturns.StandardDistributionPolynomialFactory`
    - See :class:`~openturns.AdaptiveStieltjesAlgorithm`
    - See :class:`~openturns.OrthogonalUniVariatePolynomial`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFactory`
    - See :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    - See :class:`~openturns.IntegrationExpansion`
    - See :class:`~openturns.LeastSquaresExpansion`


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`
    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_chaos_logNormal`


.. topic:: References:

    - [soizeghanem2004]_
    - [ghanem1991]_
    - [lemaitre2010]_
    - [dahlquist2008]_
    - [rudin1987]_
