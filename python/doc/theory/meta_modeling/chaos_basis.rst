.. _chaos_basis:

Polynomial chaos basis
----------------------

This page introduces *polynomial chaos expansion*.
We consider the notations introduced in :ref:`functional_chaos`.

If the marginals of :math:`\mu_{\vect{X}}` are independent,
then the isoprobabilistic transformation :math:`T` is so that
:math:`\mu_{\vect{Z}}` has independent components too.
Let us assume that the standard random vector :math:`\vect{Z}` has
independent components:

  .. math::

     \mu_{\vect{Z}}(\vect{z})= \prod_{i=1}^{n_X} \mu_i(z_i)

for any :math:`\vect{z} \in \Rset^{n_X}`.

The multivariate polynomial basis can be built using the *tensor product* of
univariate polynomials which are orthonormal with respect to :math:`\mu_{\vect{Z}}`.
The orthonormal polynomials are:

  .. math::

        \Psi_\vect{\alpha}(\vect{z}) = \prod_{i=1}^{n_X} \Psi_{\alpha_i}^{(i)}(z_i)

where :math:`\vect{\alpha} = (\alpha_1, \dots, \alpha_d)` is the multi-index representing the marginal polynomial degrees and
:math:`\Psi_{\alpha_i}^{(i)}` is the :math:`i`-th marginal univariate
orthonormal polynomial of degree :math:`\alpha_i`.
The orthonormal polynomial basis with respect to the marginal :math:`\mu_i` is known for some distributions: see
:ref:`orthogonal_polynomial`
to get some usual orthonornal polynomial families.

For classical polynomials, the :class:`~openturns.StandardDistributionPolynomialFactory` class
implements the three-term recurrence.
If the family is not already known, the polynomials can be represented by their three-term
recurrence, using the adaptive Stieljes algorithm (see :class:`~openturns.AdaptiveStieltjesAlgorithm`).
Once the sequence of recurrence coefficients
is known, the Reverse Clenshaw algorithm enables fast, stable evaluation of the polynomials
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


.. topic:: References:

    - [soizeghanem2004]_
    - [ghanem1991]_
    - [lemaitre2010]_
    - [dahlquist2008]_
    - [dudin1987]_
