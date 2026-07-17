%define OT_SimulationAlgorithmImplementation_doc
"Base class for simulation algorithms.

See also
--------
ProbabilitySimulationAlgorithm, ExpectationSimulationAlgorithm"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation
OT_SimulationAlgorithmImplementation_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getBlockSize_doc
"Accessor to the block size.

Returns
-------
blockSize : int
    Number of simultaneous evaluations of the limit-state function.
    It is set by default to 1."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getBlockSize
OT_SimulationAlgorithmImplementation_getBlockSize_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setBlockSize_doc
R"RAW(Accessor to the block size.

Parameters
----------
blockSize : int, :math:`blockSize \geq 1`
    Number of simultaneous evaluations of the limit-state function.
    It is set by default to 1.

Notes
-----
If the function supports batch evaluations this parameter can be set to the number
of available CPUs to benefit from parallel execution (multithreading, multiprocessing, ...);
except for the Directional Sampling, where we recommend to set it to 1.
It also decides the frequency of the stopping criteria and progress callbacks update
as they are called once the whole block of function evaluations is completed.
)RAW"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setBlockSize
OT_SimulationAlgorithmImplementation_setBlockSize_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getMaximumCoefficientOfVariation_doc
"Accessor to the maximum coefficient of variation.

Returns
-------
coefficient : float
    Maximum coefficient of variation of the simulated sample."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getMaximumCoefficientOfVariation
OT_SimulationAlgorithmImplementation_getMaximumCoefficientOfVariation_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setMaximumCoefficientOfVariation_doc
"Accessor to the maximum coefficient of variation.

Parameters
----------
coefficient : float
    Maximum coefficient of variation of the simulated sample."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setMaximumCoefficientOfVariation
OT_SimulationAlgorithmImplementation_setMaximumCoefficientOfVariation_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getMaximumOuterSampling_doc
"Accessor to the maximum iterations number.

Returns
-------
outerSampling : int
    Maximum number of iterations, each iteration performing a block of evaluations."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getMaximumOuterSampling
OT_SimulationAlgorithmImplementation_getMaximumOuterSampling_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setMaximumOuterSampling_doc
"Accessor to the maximum iterations number.

Parameters
----------
outerSampling : int
    Maximum number of iterations, each iteration performing a block of evaluations."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setMaximumOuterSampling
OT_SimulationAlgorithmImplementation_setMaximumOuterSampling_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getMaximumStandardDeviation_doc
R"RAW(Accessor to the maximum standard deviation.

Returns
-------
sigma : float, :math:`\sigma > 0`
    Maximum standard deviation of the estimator.)RAW"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getMaximumStandardDeviation
OT_SimulationAlgorithmImplementation_getMaximumStandardDeviation_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setMaximumStandardDeviation_doc
R"RAW(Accessor to the maximum standard deviation.

Parameters
----------
sigma : float, :math:`\sigma > 0`
    Maximum standard deviation of the estimator.)RAW"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setMaximumStandardDeviation
OT_SimulationAlgorithmImplementation_setMaximumStandardDeviation_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_run_doc
"Launch simulation.

Notes
-----
It launches the simulation on a sample of size at most *outerSampling * blockSize*,
this sample being built by blocks of size *blockSize*. It allows one to use
efficiently the distribution of the computation as well as it allows one to deal
with a sample size :math:`> 2^{32}` by a combination of *blockSize* and
*outerSampling*.

see also
--------
setBlockSize, setMaximumOuterSampling, ResourceMap, SimulationResult"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::run
OT_SimulationAlgorithmImplementation_run_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setProgressCallback_doc
R"RAW(Set up a progress callback.

Can be used to programmatically report the progress of a simulation.

Parameters
----------
callback : callable
    Takes a float as argument as percentage of progress.

Examples
--------
>>> import sys
>>> import openturns as ot
>>> experiment = ot.MonteCarloExperiment()
>>> X = ot.RandomVector(ot.Normal())
>>> Y = ot.CompositeRandomVector(ot.SymbolicFunction(['X'], ['1.1*X']), X)
>>> event = ot.ThresholdEvent(Y, ot.Less(), -2.0)
>>> algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
>>> algo.setMaximumOuterSampling(100)
>>> algo.setMaximumCoefficientOfVariation(-1.0)
>>> def report_progress(progress):
...     sys.stderr.write('-- progress=' + str(progress) + '%\n')
>>> algo.setProgressCallback(report_progress)
>>> algo.run()
)RAW"
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setProgressCallback
OT_SimulationAlgorithmImplementation_setProgressCallback_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setStopCallback_doc
"Set up a stop callback.

Can be used to programmatically stop a simulation.

Parameters
----------
callback : callable
    Returns an int deciding whether to stop or continue."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setStopCallback
OT_SimulationAlgorithmImplementation_setStopCallback_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getConvergenceStrategy_doc
"Accessor to the convergence strategy.

Returns
-------
storage_strategy : :class:`~openturns.HistoryStrategy`
    Storage strategy used to store the values of the probability estimator
    and its variance during the simulation algorithm."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getConvergenceStrategy
OT_SimulationAlgorithmImplementation_getConvergenceStrategy_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setConvergenceStrategy_doc
"Accessor to the convergence strategy.

Parameters
----------
storage_strategy : :class:`~openturns.HistoryStrategy`
    Storage strategy used to store the values of the probability estimator
    and its variance during the simulation algorithm."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setConvergenceStrategy
OT_SimulationAlgorithmImplementation_setConvergenceStrategy_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_setMaximumTimeDuration_doc
"Accessor to the maximum duration.

Parameters
----------
maximumTimeDuration : float
    Maximum optimization duration in seconds."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::setMaximumTimeDuration
OT_SimulationAlgorithmImplementation_setMaximumTimeDuration_doc

// ---------------------------------------------------------------------

%define OT_SimulationAlgorithmImplementation_getMaximumTimeDuration_doc
"Accessor to the maximum duration.

Returns
-------
maximumTimeDuration : float
    Maximum optimization duration in seconds."
%enddef
%feature("docstring") OT::SimulationAlgorithmImplementation::getMaximumTimeDuration
OT_SimulationAlgorithmImplementation_getMaximumTimeDuration_doc
