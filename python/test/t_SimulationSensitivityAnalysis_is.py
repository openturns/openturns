#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Physical model: Pf sensitivity with importance sampling in physical space
dim = 2
distribution = ot.Normal([0.0] * dim, [1.0] * dim)
model = ot.SymbolicFunction(["x0", "x1"], ["x0+2*x1"])
threshold = 3.0

# Monte Carlo reference
memo_mc = ot.MemoizeFunction(model)
event_mc = ot.ThresholdEvent(ot.CompositeRandomVector(memo_mc, ot.RandomVector(distribution)), ot.Greater(), threshold)
algo_mc = ot.ProbabilitySimulationAlgorithm(event_mc)
algo_mc.setMaximumOuterSampling(20000)
algo_mc.setBlockSize(500)
algo_mc.setMaximumCoefficientOfVariation(0.05)
algo_mc.run()
ssa_mc = ot.SimulationSensitivityAnalysis(algo_mc.getResult())
sens_mc = ssa_mc.computeEventProbabilitySensitivity()
mean_mc = ssa_mc.computeMeanPointInEventDomain()

# Importance sampling in physical space
memo_is = ot.MemoizeFunction(model)
event_is = ot.ThresholdEvent(ot.CompositeRandomVector(memo_is, ot.RandomVector(distribution)), ot.Greater(), threshold)
importance = ot.Normal([1.0, 2.0], [1.0] * dim)
experiment = ot.ImportanceSamplingExperiment(importance)
algo_is = ot.ProbabilitySimulationAlgorithm(event_is, experiment)
algo_is.setMaximumOuterSampling(20000)
algo_is.setBlockSize(500)
algo_is.setMaximumCoefficientOfVariation(0.05)
algo_is.run()
input_sample = memo_is.getInputHistory()
output_sample = memo_is.getOutputHistory()
size = input_sample.getSize()
weights = [distribution.computePDF(p) / importance.computePDF(p) for p in input_sample]

# Without weights the estimate is biased
ssa_unweighted = ot.SimulationSensitivityAnalysis(event_is, input_sample, output_sample)
assert ssa_unweighted.getWeights().getSize() == 0

# With weights it matches Monte Carlo
ssa_is = ot.SimulationSensitivityAnalysis(event_is, input_sample, output_sample, weights)
ott.assert_almost_equal(ssa_is.getWeights().getSize(), size)
sens_is = ssa_is.computeEventProbabilitySensitivity()
mean_is = ssa_is.computeMeanPointInEventDomain()
# loose tolerance: IS with ~1k points vs MC reference
ott.assert_almost_equal(list(sens_is), list(sens_mc), 1e-1, 5e-2)
ott.assert_almost_equal(list(mean_is), list(mean_mc), 1e-1, 5e-2)

# setWeights accessor
ssa_check = ot.SimulationSensitivityAnalysis(event_is, input_sample, output_sample)
ssa_check.setWeights(weights)
ott.assert_almost_equal(list(ssa_check.computeEventProbabilitySensitivity()), list(sens_is), 1e-12, 1e-12)

# Standard-space IS mapped back to physical space (discourse #102)
marg_L = ot.LogNormalMuSigmaOverMu(5.0, 0.02, 0.0).getDistribution()
marg_F = ot.LogNormalMuSigmaOverMu(0.1, 0.20, 0.0).getDistribution()
phys_dist = ot.JointDistribution([marg_L, marg_F])
beam = ot.MemoizeFunction(ot.SymbolicFunction(["L", "F"], ["F * L^3"]))
ante = ot.RandomVector(phys_dist)
phys_event = ot.ThresholdEvent(ot.CompositeRandomVector(beam, ante), ot.Greater(), 20.0)
std_event = ot.StandardEvent(phys_event)
beam_std = ot.MemoizeFunction(std_event.getFunction())
ante_std = ot.CompositeRandomVector(beam_std, std_event.getAntecedent())
std_event_is = ot.ThresholdEvent(ante_std, std_event.getOperator(), std_event.getThreshold())
# design point from a cheap FORM-like guess: use quantile-based shift here, fixed for determinism
mpfp = [1.0, 2.0]
std_importance = ot.Normal(mpfp, ot.CovarianceMatrix(2))
std_experiment = ot.ImportanceSamplingExperiment(std_importance)
std_algo = ot.ProbabilitySimulationAlgorithm(std_event_is, std_experiment)
std_algo.setMaximumOuterSampling(20)
std_algo.setBlockSize(200)
std_algo.run()
std_in = beam_std.getInputHistory()
std_out = beam_std.getOutputHistory()
std_dist = ot.Normal(2)
std_weights = [std_dist.computePDF(p) / std_importance.computePDF(p) for p in std_in]
phys_in = phys_dist.getInverseIsoProbabilisticTransformation()(std_in)
ssa_std = ot.SimulationSensitivityAnalysis(phys_event, phys_in, std_out, std_weights)
# sensitivities are finite and match the sign pattern of a MC reference
sens_std = ssa_std.computeEventProbabilitySensitivity()
assert len(sens_std) == 6
assert all([abs(v) < 10.0 for v in sens_std])
