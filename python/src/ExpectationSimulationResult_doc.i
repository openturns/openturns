%feature("docstring") OT::ExpectationSimulationResult
"Expectation simulation result.

Gathers the results of a :class:`~openturns.ExpectationSimulationAlgorithm`
algorithm.

See also
--------
ExpectationSimulationAlgorithm"

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::setRandomVector
"Accessor to the random variable.

Parameters
----------
randomVector : :class:`~openturns.RandomVector`
    Random vector we want to study."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getRandomVector
"Accessor to the random variable.

Returns
-------
randomVector : :class:`~openturns.RandomVector`
    Random vector we want to study."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getExpectationEstimate
"Accessor to the expectation estimate.

Returns
-------
expectationEstimate : :class:`~openturns.Point`
    Estimate of the expectation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::setExpectationEstimate
"Accessor to the expectation estimate.

Parameters
----------
expectationEstimate : :class:`~openturns.Point`
    Estimate of the expectation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getVarianceEstimate
"Accessor to the variance of the estimator of the expectation.

Returns
-------
varianceEstimate : :class:`~openturns.Point`
    Estimate of the variance of the estimator of the expectation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::setVarianceEstimate
"Accessor to the variance of the estimator of the expectation.

Parameters
----------
varianceEstimate : :class:`~openturns.Point`
    Estimate of the variance."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getExpectationDistribution
"Accessor to the distribution of the estimator of the expectation.

Returns
-------
expectationDistribution : :class:`~openturns.Distribution`
    Distribution of the estimator of the expectation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getStandardDeviation
"Accessor to the standard deviation of the estimator of the expectation.

Returns
-------
standardDeviation : :class:`~openturns.Point`
    Standard deviation of the estimator of the expectation."

// ---------------------------------------------------------------------

%feature("docstring") OT::ExpectationSimulationResult::getCoefficientOfVariation
"Accessor to the coefficient of variation of the estimator of the expectation.

Returns
-------
coefficientOfVariation : :class:`~openturns.Point`
    Coefficient of variation of the estimator of the expectation."
