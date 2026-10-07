%feature("docstring") OT::FiniteOrthonormalFunctionFactory
"Finite orthonormal function factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
This class computes a finite set of orthonormal functions with respect to a
given probability distribution, obtained by QR factorization of the weighted
design matrix. It uses a :class:`~openturns.experimental.FiniteOrthonormalizationAlgorithm`
to compute the orthonormal basis and a :class:`~openturns.experimental.GaussLPQuadrature`
to build minimal quadrature rules.

The following :class:`~openturns.ResourceMap` keys are used:

- ``FiniteOrthonormalizationAlgorithm-DefaultDiscretization`` (``UnsignedInteger``, default: ``128``): number of 1D nodes in the Gauss product experiment.
- ``FiniteOrthonormalizationAlgorithm-Epsilon`` (``Scalar``, default: ``1.0e-11``): threshold for zeroing coefficients.
- ``GaussLPQuadrature-AlphaS`` (``UnsignedInteger``, default: ``100``): multiplier for the number of LP candidate points.
- ``GaussLPQuadrature-Ngauss`` (``UnsignedInteger``, default: ``100``): number of 1D Gauss-Legendre nodes for moment computation.
- ``GaussLPQuadrature-Epsilon`` (``Scalar``, default: ``1.0e-5``): tolerance for quadrature residual.
- ``GaussLPQuadrature-Solver`` (``String``, default: ``choose``): HiGHS solver used for the linear programming solve.
- ``GaussLPQuadrature-FallbackSolver`` (``String``, default: ``ipm``): HiGHS solver used when the first linear programming solve fails; an empty value disables the retry.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Initial set of functions to orthonormalize.
distribution : :class:`~openturns.Distribution`
    The probability measure with respect to which the functions are orthonormal.
experiment : :class:`~openturns.WeightedExperiment`, optional
    The weighted experiment used to compute the Gram matrix. It either
    targets the measure or lives on the reference cube; default builds a
    Gauss product rule adapted to the measure. Use
    `setIntegrationAlgorithm` to select any other
    :class:`~openturns.IntegrationAlgorithm`.
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`, optional
    Alternative to `experiment`: the integration algorithm used to compute
    the Gram matrix.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.Normal(dim)
>>> refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
>>> basis = [refBasis.build(i) for i in range(6)]
>>> factory = otexp.FiniteOrthonormalFunctionFactory(basis, distribution)
>>> f0 = factory.build(0)
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::build
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

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::buildQuadrature
"Build a minimal quadrature rule.

Build a quadrature rule that exactly integrates the first n orthonormal
functions against the measure. Uses the Gauss-LP approach: linear programming
followed by clustering and local refinement.

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

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::setFunctionsCollection
"Accessor to the initial functions.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Initial functions.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::setMeasureAndFunctions
"Set the measure and the initial functions simultaneously.

Sets both the probability measure and the initial set of functions to
orthonormalize. The input dimension of each function must match the
measure dimension.

Parameters
----------
measure : :class:`~openturns.Distribution`
    The probability measure.
functions : sequence of :class:`~openturns.Function`
    Initial functions.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::getFunctionsCollection
"Accessor to the initial functions.

Returns
-------
functions : :class:`~openturns.FunctionCollection`
    Initial functions.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::setExperiment
"Accessor to the weighted experiment.

Setting an experiment resets the integration algorithm.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    Weighted experiment.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::getExperiment
"Accessor to the weighted experiment.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    Weighted experiment. It is empty when an integration algorithm was set.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::setIntegrationAlgorithm
"Accessor to the integration algorithm.

Setting an integration algorithm resets the experiment. Any
:class:`~openturns.IntegrationAlgorithm` can be used, including an
:class:`~openturns.ExperimentIntegration` built from any weighted experiment.

Parameters
----------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::getIntegrationAlgorithm
"Accessor to the integration algorithm.

Returns
-------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::getCoefficients
"Accessor to the coefficient matrix.

Returns
-------
coefficients : :class:`~openturns.SquareMatrix`
    Coefficient matrix of the orthonormal functions.
"

%feature("docstring") OT::FiniteOrthonormalFunctionFactory::getOrthonormalizationAlgorithm
"Accessor to the orthonormalization algorithm.

Returns
-------
algo : :class:`~openturns.experimental.FiniteOrthonormalizationAlgorithm`
    The orthonormalization algorithm holding the measure, the initial
    functions and the integration rule.
"
