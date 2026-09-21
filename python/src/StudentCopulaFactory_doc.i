%feature("docstring") OT::StudentCopulaFactory
R"RAW(Student copula factory.

This class implements the method described in [peng2014]_.
It consists in inferring the correlation matrix and then estimating the
:math:`\nu` parameter via profiled likelihood maximization.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``StudentCopulaFactory-MaximumAbsoluteError`` (``Scalar``, default: ``1e-10``): absolute error of the optimization,
- ``StudentCopulaFactory-MaximumConstraintError`` (``Scalar``, default: ``1e-10``): constraint error of the optimization,
- ``StudentCopulaFactory-MaximumObjectiveError`` (``Scalar``, default: ``1e-10``): objective error of the optimization,
- ``StudentCopulaFactory-MaximumRelativeError`` (``Scalar``, default: ``1e-10``): relative error of the optimization,
- ``StudentCopulaFactory-MaximumCallsNumber`` (``UnsignedInteger``, default: ``1000``): maximum number of calls of the objective function,
- ``StudentCopulaFactory-NuMin`` (``Scalar``, default: ``2.0``): lower bound of the :math:`\nu` parameter,
- ``StudentCopulaFactory-NuMax`` (``Scalar``, default: ``1e2``): upper bound of the :math:`\nu` parameter,
- ``StudentCopulaFactory-NuStart`` (``Scalar``, default: ``5.0``): starting value of the :math:`\nu` parameter,
- ``StudentCopulaFactory-DefaultOptimizationAlgorithm`` (``String``, default: ``Cobyla``): name of the optimization algorithm used, among the ones recognized by :class:`~openturns.OptimizationAlgorithm`.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- ``StudentCopulaFactory-MaximumCallsNumber`` (``UnsignedInteger``, default: ``1000``)

See also
--------
DistributionFactory, StudentCopula
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::StudentCopulaFactory::buildAsStudentCopula
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsStudentCopula()

    buildAsStudentCopula(*sample*)

    buildAsStudentCopula(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`openturns.StudentCopula`.

Returns
-------
dist : :class:`~openturns.Uniform`
    The estimated distribution as a :class:`openturns.StudentCopula`.
    In the first usage, the default :class:`openturns.StudentCopula` distribution is built."

// ---------------------------------------------------------------------

%feature("docstring") OT::StudentCopulaFactory::setOptimizationAlgorithm
"Accessor to the optimization solver.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for the optimization of the log-likelihood.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::StudentCopulaFactory::getOptimizationAlgorithm
"Accessor to the optimization solver.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for the optimization of the log-likelihood."
