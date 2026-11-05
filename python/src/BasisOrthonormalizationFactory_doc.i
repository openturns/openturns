%feature("docstring") OT::BasisOrthonormalizationFactory
R"RAW(Basis orthonormalization factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class computes a finite set of orthonormal functions from the first
:math:`s` elements of a source :class:`~openturns.OrthogonalBasis`, with
respect to a target measure. It uses a
:class:`~openturns.experimental.BasisOrthonormalizationAlgorithm` to compute
the orthonormal basis and a
:class:`~openturns.experimental.GaussLPQuadrature` built from the orthonormal
functions and the target measure to construct quadrature rules. The basis
comes from the QR factorization of the weighted design matrix when the
integration rule exposes its nodes, and from the Cholesky factorization of
the Gram matrix for opaque rules.

The source basis carries an instrumental measure, against which its functions
are naturally orthogonal. The target measure is the measure against which the
orthonormal basis is built. The two measures must have the same dimension and
the support of the target measure must be contained in the support of the
instrumental measure; an exception is raised otherwise. A discrete
instrumental measure is exempt from the range check.

The initial functions do not need to be orthonormal with respect to the
target measure: the factory computes an orthonormal basis of the vector space
they span.

The integration algorithm is not a constructor argument: the default
target-measure-adapted rule is used unless :meth:`setIntegrationAlgorithm`
replaces it, so that the basis, the target measure and the rule are always
set consistently.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Source orthogonal basis whose first elements are orthonormalized. Its
    measure is the instrumental measure.
targetMeasure : :class:`~openturns.Distribution`
    Target measure against which the orthonormal basis is built.
size : int
    Number of first elements of the source basis to orthonormalize. It must
    be positive.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

.. list-table::
   :widths: 30 15 15 40
   :header-rows: 1

   * - Key
     - Type
     - Default value
     - Description
   * - ``BasisOrthonormalizationAlgorithm-DefaultDiscretization``
     - ``UnsignedInteger``
     - ``128``
     - Requested number of 1D nodes in the Gauss product experiment.
   * - ``BasisOrthonormalizationAlgorithm-MaximumTensorNodes``
     - ``UnsignedInteger``
     - ``1000000``
     - Maximum number of nodes of the default tensor rule, the marginal order being reduced above this budget and the rule being rejected when the budget cannot even hold the order-2 tensor product.
   * - ``BasisOrthonormalizationAlgorithm-Epsilon``
     - ``Scalar``
     - ``1.0e-11``
     - Relative threshold for zeroing coefficients and detecting linearly dependent functions.
   * - ``GaussLPQuadrature-AlphaS``
     - ``UnsignedInteger``
     - ``100``
     - Multiplier for the number of LP candidate points.
   * - ``GaussLPQuadrature-Margin``
     - ``UnsignedInteger``
     - ``10``
     - Minimum number of candidate points beyond the number of functions.
   * - ``GaussLPQuadrature-Ngauss``
     - ``UnsignedInteger``
     - ``100``
     - Requested order of the measure-adapted rule used for the moments.
   * - ``GaussLPQuadrature-MaximumTensorNodes``
     - ``UnsignedInteger``
     - ``1000000``
     - Maximum number of nodes of the tensor rule used for the moments, the marginal order being reduced above this budget and the rule being rejected when the budget cannot even hold the order-2 tensor product.
   * - ``GaussLPQuadrature-Epsilon``
     - ``Scalar``
     - ``1.0e-5``
     - Tolerance for quadrature residual.
   * - ``GaussLPQuadrature-ClusterPath``
     - ``Bool``
     - ``True``
     - Refine the rules along the greedy clustering path and stop at the first rule meeting the tolerance.
   * - ``GaussLPQuadrature-MaximumAbsoluteError``
     - ``Scalar``
     - ``1.0e-12``
     - Absolute error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumAbsoluteError``.
   * - ``GaussLPQuadrature-MaximumRelativeError``
     - ``Scalar``
     - ``1.0e-12``
     - Relative error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumRelativeError``.
   * - ``GaussLPQuadrature-MaximumObjectiveError``
     - ``Scalar``
     - ``1.0e-12``
     - Objective error tolerance of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumResidualError``.
   * - ``GaussLPQuadrature-MaximumConstraintError``
     - ``Scalar``
     - ``1.0e-12``
     - Constraint error tolerance of the linear programming solve and of the local refinement, superseding ``OptimizationAlgorithm-DefaultMaximumConstraintError``.
   * - ``GaussLPQuadrature-Solver``
     - ``String``
     - ``choose``
     - HiGHS solver used for the linear programming solve, see :meth:`~openturns.experimental.HiGHS.GetAlgorithmNames`.
   * - ``GaussLPQuadrature-FallbackSolver``
     - ``String``
     - ``ipm``
     - HiGHS solver used when the first linear programming solve fails, see :meth:`~openturns.experimental.HiGHS.GetAlgorithmNames`; an empty value disables the retry.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.JointDistribution(
...     [ot.Uniform(-1.0, 1.0)] * dim, ot.FrankCopula(4.0))
>>> legendreBasis = ot.OrthogonalProductPolynomialFactory(
...     [ot.LegendreFactory()] * dim)
>>> factory = otexp.BasisOrthonormalizationFactory(
...     legendreBasis, distribution, 6)
>>> factory.getSize()
6
>>> f0 = factory.build(0)
)RAW"

%feature("docstring") OT::BasisOrthonormalizationFactory::build
"Build the orthonormal function of given index.

Parameters
----------
index : int
    Index of the function to build (0-based).

Returns
-------
func : :class:`~openturns.Function`
    The orthonormal function.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::getSize
"Accessor to the number of orthonormal functions.

Returns
-------
size : int
    Number of first elements of the orthogonal basis which are
    orthonormalized.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::buildQuadrature
"Build a quadrature rule.

Build a quadrature rule integrating the first n orthonormal functions against
the target measure. Uses the Gauss-LP approach: linear programming followed by
clustering and local refinement. The returned number of nodes is at most
``n``: a natural linear-programming support smaller than ``n`` is preserved.
The moments are reproduced up to the refinement tolerance; when the residual
exceeds it, a warning is logged and the best rule found is returned.

In Python, the two outputs are returned as a tuple and can be unpacked as
``nodes, weights = factory.buildQuadrature(n)``.

.. note::
    The linear program requires HiGHS support. When OpenTURNS is built
    without HiGHS, the builder falls back to a refined measure-adapted tensor
    rule and emits a warning; it raises an exception when that rule has fewer
    than ``n`` nodes.

Parameters
----------
n : int
    Number of orthonormal functions whose moments are reproduced.

Returns
-------
nodes : :class:`~openturns.Sample`
    The quadrature nodes.
weights : :class:`~openturns.Point`
    The quadrature weights.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::buildQuadratureRule
"Build a quadrature rule as a weighted experiment.

Builds the rule with :class:`~openturns.experimental.GaussLPQuadrature` from
the orthonormal functions and the target measure, and returns it as a
:class:`~openturns.WeightedExperiment` whose implementation is the immutable
:class:`~openturns.experimental.GaussLPQuadratureRule`. The rule stores the
nodes, the weights and the target measure; its size is the actual number of
nodes, which is at most ``n``.

.. note::
    The linear program requires HiGHS support. When OpenTURNS is built
    without HiGHS, the builder falls back to a refined measure-adapted tensor
    rule and emits a warning; it raises an exception when that rule has fewer
    than ``n`` nodes.

Parameters
----------
n : int
    Number of orthonormal functions whose moments are reproduced.

Returns
-------
rule : :class:`~openturns.WeightedExperiment`
    The quadrature rule as a weighted experiment, whose implementation is a
    :class:`~openturns.experimental.GaussLPQuadratureRule`.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::setIntegrationAlgorithm
"Accessor to the integration algorithm.

Any :class:`~openturns.IntegrationAlgorithm` can be used, including an
:class:`~openturns.ExperimentIntegration` built from any weighted experiment.
The experiment is validated when the orthonormal basis is computed. For an
experiment, a continuous experiment distribution must be the target measure,
and its instrumental distribution must contain the support of the target
measure, otherwise an exception is raised. If the experiment distribution
does not implement equality, the equality check is skipped and the caller is
responsible for its compatibility with the target measure. A discrete
experiment such as :class:`~openturns.FixedExperiment` is exempt from the
equality and range checks because its distribution is the empirical measure
supported by its nodes.

Parameters
----------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::getIntegrationAlgorithm
"Accessor to the integration algorithm.

Returns
-------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::getCoefficients
"Accessor to the coefficient matrix.

Column :math:`j` of the matrix contains the coefficients of the
:math:`j`-th orthonormal function in the first elements of the orthogonal
basis.

Returns
-------
coefficients : :class:`~openturns.SquareMatrix`
    Coefficient matrix of the orthonormal functions.
"

%feature("docstring") OT::BasisOrthonormalizationFactory::getOrthonormalizationAlgorithm
"Accessor to the orthonormalization algorithm.

Returns
-------
algo : :class:`~openturns.experimental.BasisOrthonormalizationAlgorithm`
    The orthonormalization algorithm holding the source basis, the target
    measure, the size and the integration rule.
"
