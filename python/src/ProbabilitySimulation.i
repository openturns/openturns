// SWIG file ProbabilitySimulation.i

%{
#include "openturns/ProbabilitySimulation.hxx"
%}

%include ProbabilitySimulation_doc.i

%include openturns/ProbabilitySimulation.hxx

namespace OT {


%extend ProbabilitySimulation {

ProbabilitySimulation(const ProbabilitySimulation & other) { return new OT::ProbabilitySimulation(other); }

} // ProbabilitySimulation
} // OT


%pythoncode %{
def MonteCarlo(event):
    openturns.common.Log.Warn('class MonteCarlo is deprecated in favor of ProbabilitySimulation')
    if event.isComposite():
        experiment = openturns.weightedexperiment.MonteCarloExperiment()
        algo = openturns.simulation.ProbabilitySimulation(event, experiment)
    else:
        algo = openturns.simulation.Simulation(event)
    return algo

def ImportanceSampling(event, importanceDistribution):
    openturns.common.Log.Warn('class ImportanceSampling is deprecated in favor of ProbabilitySimulation')
    experiment = openturns.weightedexperiment.ImportanceSamplingExperiment(importanceDistribution)
    return openturns.simulation.ProbabilitySimulation(event, experiment)

def QuasiMonteCarlo(event, sequence=None):
    openturns.common.Log.Warn('class QuasiMonteCarlo is deprecated in favor of ProbabilitySimulation')
    if sequence is None:
        sequence = openturns.statistics.SobolSequence()
    experiment = openturns.weightedexperiment.LowDiscrepancyExperiment(sequence)
    return openturns.simulation.ProbabilitySimulation(event, experiment)

def RandomizedQuasiMonteCarlo(event, sequence=None):
    openturns.common.Log.Warn('class RandomizedQuasiMonteCarlo is deprecated in favor of ProbabilitySimulation')
    if sequence is None:
        sequence = openturns.statistics.SobolSequence()
    experiment = openturns.weightedexperiment.LowDiscrepancyExperiment(sequence)
    experiment.setRandomize(True)
    return openturns.simulation.ProbabilitySimulation(event, experiment)

def RandomizedLHS(event):
    openturns.common.Log.Warn('class RandomizedLHS is deprecated in favor of ProbabilitySimulation')
    experiment = openturns.weightedexperiment.LHSExperiment()
    experiment.setAlwaysShuffle(True)
    return openturns.simulation.ProbabilitySimulation(event, experiment)

%}
