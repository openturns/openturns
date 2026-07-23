#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# WhiteNoise
dist = ot.Distribution(ot.Uniform())
timeGrid = ot.RegularGrid(0.0, 1.0, 11)
assert ot.WhiteNoise(dist, timeGrid) == ot.WhiteNoise(dist, timeGrid), "WN==WN"
assert ot.WhiteNoise(dist, timeGrid) != ot.WhiteNoise(ot.Normal(), timeGrid), "WN!=WN dist"
assert ot.WhiteNoise(dist, timeGrid) != ot.WhiteNoise(dist, ot.RegularGrid(0.0, 0.5, 11)), "WN!=WN mesh"

# GaussianProcess
cov = ot.SquaredExponential([1.0], [2.0])
assert ot.GaussianProcess(cov, timeGrid) == ot.GaussianProcess(cov, timeGrid), "GP==GP"
assert ot.GaussianProcess(cov, timeGrid) != ot.GaussianProcess(ot.SquaredExponential([2.0], [2.0]), timeGrid), "GP!=GP cov"

# ARMA
ar = ot.ARMACoefficients([0.5])
ma = ot.ARMACoefficients([0.3])
wn = ot.WhiteNoise(ot.Normal())
assert ot.ARMA(ar, ma, wn) == ot.ARMA(ar, ma, wn), "ARMA==ARMA"
assert ot.ARMA(ar, ma, wn) != ot.ARMA(ot.ARMACoefficients([0.7]), ma, wn), "ARMA!=ARMA ar"

# RandomWalk
origin = [0.0]
dist = ot.Distribution(ot.Normal())
assert ot.RandomWalk(origin, dist, ot.RegularGrid(0.0, 1.0, 10)) == ot.RandomWalk(origin, dist, ot.RegularGrid(0.0, 1.0, 10)), "RW==RW"
assert ot.RandomWalk(origin, dist, ot.RegularGrid(0.0, 1.0, 10)) != ot.RandomWalk([1.0], dist, ot.RegularGrid(0.0, 1.0, 10)), "RW!=RW origin"

# DiscreteMarkovChain
transition = ot.SquareMatrix(2, [0.5, 0.3, 0.5, 0.7])
tg = ot.RegularGrid(0.0, 1.0, 10)
assert ot.DiscreteMarkovChain(0, transition, tg) == ot.DiscreteMarkovChain(0, transition, tg), "DMC==DMC"
assert ot.DiscreteMarkovChain(0, transition, tg) != ot.DiscreteMarkovChain(1, transition, tg), "DMC!=DMC origin"

# FunctionalBasisProcess
basis = [ot.SymbolicFunction(["t"], ["t"])]
coefDist = ot.Distribution(ot.Normal())
assert ot.FunctionalBasisProcess(coefDist, basis, timeGrid) == ot.FunctionalBasisProcess(coefDist, basis, timeGrid), "FBP==FBP"

# AggregatedProcess
p1 = ot.WhiteNoise(ot.Normal(), timeGrid)
p2 = ot.WhiteNoise(ot.Uniform(), timeGrid)
assert ot.AggregatedProcess([p1, p2]) == ot.AggregatedProcess([p1, p2]), "AP==AP"
assert ot.AggregatedProcess([p1, p2]) != ot.AggregatedProcess([p2, p1]), "AP!=AP"

# CompositeProcess
mesh = ot.IntervalMesher([5]).build(ot.Interval([0.0], [1.0]))
f = ot.FieldFunction(ot.VertexValueFunction(ot.SymbolicFunction(["t", "x"], ["t+x"]), mesh))
antecedent = ot.WhiteNoise(ot.Normal(), mesh)
assert ot.CompositeProcess(f, antecedent) == ot.CompositeProcess(f, antecedent), "CP==CP"

# Handle equality
p1 = ot.Process(ot.WhiteNoise(dist, timeGrid))
p2 = ot.Process(ot.WhiteNoise(dist, timeGrid))
assert p1 == p2, "Process handle =="
assert p1 != ot.Process(), "Process handle !="

# Self equality
wn = ot.WhiteNoise(dist, timeGrid)
assert wn == wn, "self =="
assert not wn != wn, "self !="
