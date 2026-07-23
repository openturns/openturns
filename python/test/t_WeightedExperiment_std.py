#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Normal(4)
distribution2 = ot.Normal(2)
size = 10

# MonteCarloExperiment
assert ot.MonteCarloExperiment(distribution, size) == ot.MonteCarloExperiment(distribution, size), "MCE==MCE"
assert ot.MonteCarloExperiment(distribution, size) != ot.MonteCarloExperiment(distribution2, size), "MCE!=MCE dist"
assert ot.MonteCarloExperiment(distribution, size) != ot.MonteCarloExperiment(distribution, size + 1), "MCE!=MCE size"

# LHSExperiment
assert ot.LHSExperiment(distribution, size) == ot.LHSExperiment(distribution, size), "LHS==LHS"
assert ot.LHSExperiment(distribution, size, True) != ot.LHSExperiment(distribution, size, False), "LHS!=LHS shuffle"

# LowDiscrepancyExperiment
seq = ot.SobolSequence(4)
assert ot.LowDiscrepancyExperiment(seq, distribution, size) == ot.LowDiscrepancyExperiment(seq, distribution, size), "LDE==LDE"
assert ot.LowDiscrepancyExperiment(seq, distribution, size) != ot.LowDiscrepancyExperiment(ot.HaltonSequence(4), distribution, size), "LDE!=LDE seq"

# FixedExperiment
sample = ot.Normal(4).getSample(5)
w = ot.Point(5, 1.0)
assert ot.FixedExperiment(sample, w) == ot.FixedExperiment(sample, w), "FE==FE"
assert ot.FixedExperiment(sample, w) != ot.FixedExperiment(sample * 2.0, w), "FE!=FE sample"

# ImportanceSamplingExperiment
impDist = ot.Normal(4)
assert ot.ImportanceSamplingExperiment(distribution, impDist, size) == ot.ImportanceSamplingExperiment(distribution, impDist, size), "ISE==ISE"
assert ot.ImportanceSamplingExperiment(distribution, impDist, size) != ot.ImportanceSamplingExperiment(distribution, impDist, size + 1), "ISE!=ISE size"

# GaussProductExperiment
assert ot.GaussProductExperiment(distribution) == ot.GaussProductExperiment(distribution), "GPE==GPE"
assert ot.GaussProductExperiment(distribution) != ot.GaussProductExperiment(distribution2), "GPE!=GPE dist"

# Handle equality
w1 = ot.WeightedExperiment(ot.MonteCarloExperiment(distribution, size))
w2 = ot.WeightedExperiment(ot.MonteCarloExperiment(distribution, size))
assert w1 == w2, "WE handle =="
assert w1 != ot.WeightedExperiment(), "WE handle !="

# Self equality
mce = ot.MonteCarloExperiment(distribution, size)
assert mce == mce, "self =="
assert not mce != mce, "self !="
