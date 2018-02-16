// SWIG file ProbabilitySimulationAlgorithm.i

%{
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
%}

%include ProbabilitySimulationAlgorithm_doc.i

%include openturns/ProbabilitySimulationAlgorithm.hxx

namespace OT {


%extend ProbabilitySimulationAlgorithm {

ProbabilitySimulationAlgorithm(const ProbabilitySimulationAlgorithm & other) { return new OT::ProbabilitySimulationAlgorithm(other); }

} // ProbabilitySimulationAlgorithm
} // OT


%pythoncode %{
def MonteCarlo(event):
    openturns.common.Log.Warn('class MonteCarlo is deprecated in favor of ProbabilitySimulationAlgorithm')
    if event.isComposite():
        experiment = openturns.weightedexperiment.MonteCarloExperiment()
        algo = openturns.simulation.ProbabilitySimulationAlgorithm(event, experiment)
    else:
        algo = openturns.simulation.Simulation(event)
    return algo

def ImportanceSampling(event, importanceDistribution):
    openturns.common.Log.Warn('class ImportanceSampling is deprecated in favor of ProbabilitySimulationAlgorithm')
    experiment = openturns.weightedexperiment.ImportanceSamplingExperiment(importanceDistribution)
    return openturns.simulation.ProbabilitySimulationAlgorithm(event, experiment)

def QuasiMonteCarlo(event, sequence=None):
    openturns.common.Log.Warn('class QuasiMonteCarlo is deprecated in favor of ProbabilitySimulationAlgorithm')
    if sequence is None:
        sequence = openturns.statistics.SobolSequence()
    experiment = openturns.weightedexperiment.LowDiscrepancyExperiment(sequence)
    return openturns.simulation.ProbabilitySimulationAlgorithm(event, experiment)

def RandomizedQuasiMonteCarlo(event, sequence=None):
    openturns.common.Log.Warn('class RandomizedQuasiMonteCarlo is deprecated in favor of ProbabilitySimulationAlgorithm')
    if sequence is None:
        sequence = openturns.statistics.SobolSequence()
    experiment = openturns.weightedexperiment.LowDiscrepancyExperiment(sequence)
    experiment.setRandomize(True)
    return openturns.simulation.ProbabilitySimulationAlgorithm(event, experiment)

def RandomizedLHS(event):
    openturns.common.Log.Warn('class RandomizedLHS is deprecated in favor of ProbabilitySimulationAlgorithm')
    experiment = openturns.weightedexperiment.LHSExperiment()
    experiment.setAlwaysShuffle(True)
    return openturns.simulation.ProbabilitySimulationAlgorithm(event, experiment)
%}
