%feature("docstring") OT::BasisOrthonormalizationFactory
R"RAW(Basis orthonormalization factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class computes a finite set of orthonormal functions from the first
:math:`s` elements of an :class:`~openturns.OrthogonalBasis`, with respect to
the basis measure. It uses a
:class:`~openturns.experimental.BasisOrthonormalizationAlgorithm` to compute
the orthonormal basis and a
:class:`~openturns.experimental.GaussLPQuadrature` built from the orthonormal
functions and the basis measure to construct quadrature rules. The basis
comes from the QR factorization of the weighted design matrix when the
integration rule exposes its nodes, and from the Cholesky factorization of
the Gram matrix for opaque rules.

The initial functions do not need to be orthonormal with respect to the
basis measure: the factory computes an orthonormal basis of the vector space
they span. An
:class:`~openturns.experimental.FiniteOrthogonalFunctionFactory` can be used
to expose an arbitrary collection of functions and its measure as an
:class:`~openturns.OrthogonalBasis`.

The integration algorithm is not a constructor argument: the default
measure-adapted rule is used unless :meth:`setIntegrationAlgorithm` replaces
it, so that the basis, its measure and the rule are always set consistently.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis whose first elements are orthonormalized against its
    measure.
size : int
    Number of first elements of the basis to orthonormalize. It must be
    positive.

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
>>> functions = [legendreBasis.build(i) for i in range(6)]
>>> initialBasis = ot.OrthogonalBasis(
...     otexp.FiniteOrthogonalFunctionFactory(functions, distribution))
>>> factory = otexp.BasisOrthonormalizationFactory(initialBasis, len(functions))
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

Build a quadrature rule that exactly integrates the first n orthonormal
functions against the basis measure. Uses the Gauss-LP approach: linear
programming followed by clustering and local refinement. The returned number
of nodes is at most ``n``: a natural linear-programming support smaller than
``n`` is preserved.

In Python, the two outputs are returned as a tuple and can be unpacked as
``nodes, weights = factory.buildQuadrature(n)``.

.. note::
    This method requires HiGHS support and raises
    *NotYetImplementedException* when OpenTURNS was built without HiGHS.

Parameters
----------
n : int
    Number of orthonormal functions to integrate exactly.

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
the orthonormal functions and the basis measure, and returns it as a
:class:`~openturns.WeightedExperiment` whose implementation is the immutable
:class:`~openturns.experimental.GaussLPQuadratureRule`. The rule stores the
nodes, the weights and the basis measure; its size is the actual number of
nodes, which is at most ``n``.

.. note::
    Generating the rule requires HiGHS support and raises
    *NotYetImplementedException* when OpenTURNS was built without HiGHS.

Parameters
----------
n : int
    Number of orthonormal functions to integrate exactly.

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
For an experiment, a continuous experiment distribution must be the basis
measure, and its instrumental distribution must contain the support of the
basis measure, otherwise an exception is raised; a discrete experiment such
as :class:`~openturns.FixedExperiment` is exempt because its distribution is
the empirical measure supported by its nodes.

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
    The orthonormalization algorithm holding the orthogonal basis, the size
    and the integration rule.
"
