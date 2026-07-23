%feature("docstring") OT::MulticollinearityAnalysis
"Multicollinearity analysis methods.

Provides sensitivity analysis methods to determine the influence of each
component of a random vector over a single one-dimensional variable,
in the case where the components are correlated.

Available constructors:
    MulticollinearityAnalysis(*firstSample*)

    MulticollinearityAnalysis(*firstSample, secondSample*)

Parameters
----------
firstSample : 2-d list of float
    Values taken by a random vector.
secondSample : 2-d list of float
    Values taken by a single one-dimensional random variable.
    This sample is used to compute the LMG, PMVD and Johnson indices; the VIF metric ignores it.

See also
--------
CorrelationAnalysis

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> corMatrix = ot.CorrelationMatrix(3, [1.0, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0])
>>> distribution = ot.Normal([0.0] * 3, [1.0] * 3, corMatrix)
>>> firstSample = distribution.getSample(100)
>>> model = ot.SymbolicFunction(['x', 'y', 'z'], ['x + 3 * y - 5 * z'])
>>> secondSample = model(firstSample)
>>> analysis = ot.MulticollinearityAnalysis(firstSample, secondSample)
>>> lmg, pmvd = analysis.computeLmgPmvd()
>>> johnson = analysis.computeJohnson()
"

// ---------------------------------------------------------------------

%feature("docstring") OT::MulticollinearityAnalysis::computeJohnson
"Johnson index.

Returns
-------
johnson : :class:`~openturns.PointWithDescription`
    The Johnson index for each component of *firstSample*.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::MulticollinearityAnalysis::computeLmgPmvd
"LMG and PMVD indices.

Returns
-------
lmg : :class:`~openturns.PointWithDescription`
    The LMG index for each component of *firstSample*.
pmvd : :class:`~openturns.PointWithDescription`
    The PMVD index for each component of *firstSample*.

Notes
-----
This function uses the `MulticollinearityAnalysis-MaximumInputDimensionForLmgPmvd` and
`MulticollinearityAnalysis-DimensionThresholdForLmgPmvdParallelization` keys of the
:class:`~openturns.ResourceMap` class.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::MulticollinearityAnalysis::computeVif
"VIF metric.

Returns
-------
vif : :class:`~openturns.PointWithDescription`
    The VIF metric for each component of *firstSample*.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::MulticollinearityAnalysis::estimateLmgPmvdMonteCarlo
"Estimated LMG and PMVD indices.

Parameters
----------
iterations : int
    The number of Monte Carlo iterations to use.

Returns
-------
lmg : :class:`~openturns.PointWithDescription`
    The estimated LMG index for each component of *firstSample*.
pmvd : :class:`~openturns.PointWithDescription`
    The estimated PMVD index for each component of *firstSample*.
"
