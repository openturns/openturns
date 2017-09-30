#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Defining parameters
dimension = 3
size = 100

# Build standard LHS algorithm
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension)
lhs = ot.LHSExperiment(distribution, size)
lhs.setRandomShift(False)  # centered
lhs.setAlwaysShuffle(True)  # randomized

# print the object
print("lhs=", lhs)
bounds = distribution.getRange()
print("Bounds of uniform distributions=", bounds)

# Generate design without optimization
design = lhs.generate()
print("design=", design)

# Defining space fillings
spaceFillingC2 = ot.SpaceFillingC2()
spaceFillingPhiP = ot.SpaceFillingPhiP()

# print the criteria on this design
print("PhiP=%f, C2=%f" %
      (ot.SpaceFillingPhiP().evaluate(design), ot.SpaceFillingC2().evaluate(design)))

# Parameters for drawing design ==> Number of points are for the "grid"
Nx = 50
Ny = 50

# Show the design
# ot.Show(lhsGraph)

# --------------------------------------------------#
# ------------ MonteCarlo algorithm  ------------- #
# --------------------------------------------------#

# RandomBruteForce MonteCarlo with N designs
N = 100

# 1) LHS with C2 optimization
optimalLHSAlgorithm = ot.MonteCarloLHS(lhs, N, spaceFillingC2)
# print lhs
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print("Generating design with MonteCarlo and C2 space filling=", design)
result = optimalLHSAlgorithm.getResult()
print("History criterion=", result.getAlgoHistory())
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Criterion graph ==> Graph object
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("MC_C2_Criterion")

# 2) LHS with PhiP optimization (=mindist optim)
optimalLHSAlgorithm = ot.MonteCarloLHS(lhs, N, spaceFillingPhiP)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print("Generating design with MonteCarlo and PhiP optimum=", design)
result = optimalLHSAlgorithm.getResult()
print("History criterion=", result.getAlgoHistory())
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Graph of criterion
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("MC_PhiP_Criterion")

# --------------------------------------------------#
# ------------- Simulated annealing  ------------- #
# --------------------------------------------------#
# Defining temperature profil ==> TO, iterations...
T0 = 10.0
iMax = 2000
c = 0.95
# Care, c should be in ]0,1[
# Geometric profil
geomProfile = ot.GeometricProfile(T0, c, iMax)

# 3) Simulated Annealing LHS with geometric temperature, C2 optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, geomProfile, spaceFillingC2)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using SimulatedAnnealing geometric temperature & C2 criterion=", design)
result = optimalLHSAlgorithm.getResult()
history = result.getAlgoHistory()
print("History criterion=", history[:, 0])
print("History temperature=", history[:, 1])
print("History probability=", history[:, 2])
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Criteria drawing
# SA algorithms returns also Probability & temperature
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("SA_Geom_C2_Criterion")
probaGraph = result.drawHistoryProbability()
# probaGraph.draw("SA_Geom_C2_Proba")
tempGraph = result.drawHistoryTemperature()
# tempGraph.draw("SA_Geom_C2_Temperature")

# 4) Simulated Annealing LHS with geometric temperature, PhiP optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, geomProfile, spaceFillingPhiP)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using SimulatedAnnealing geometric temperature & C2 criterion=", design)
result = optimalLHSAlgorithm.getResult()
history = result.getAlgoHistory()
print("History criterion=", history[:, 0])
print("History temperature=", history[:, 1])
print("History probability=", history[:, 2])
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Criteria drawing
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("SA_Geom_PhiP_Criterion")
probaGraph = result.drawHistoryProbability()
# probaGraph.draw("SA_Geom_PhiP_Proba")
tempGraph = result.drawHistoryTemperature()
# tempGraph.draw("SA_Geom_PhiP_Temperature")

# Linear profil
linearProfile = ot.LinearProfile(T0, iMax)

# 5) Simulated Annealing LHS with linear temperature, C2 optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, linearProfile, spaceFillingC2)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using linear temperature SimulatedAnnealing & C2 criterion =", design)
result = optimalLHSAlgorithm.getResult()
history = result.getAlgoHistory()
print("History criterion=", history[:, 0])
print("History temperature=", history[:, 1])
print("History probability=", history[:, 2])
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Criteria drawing
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("SA_Lin_C2_Criterion")
probaGraph = result.drawHistoryProbability()
# probaGraph.draw("SA_Lin_C2_Proba")
tempGraph = result.drawHistoryTemperature()
# tempGraph.draw("SA_Lin_C2_Temperature")

# 6) Simulated Annealing LHS with linear temperature, PhiP optimization
optimalLHSAlgorithm = ot.SimulatedAnnealingLHS(
    lhs, linearProfile, spaceFillingPhiP)
print("lhs=", optimalLHSAlgorithm)
design = optimalLHSAlgorithm.generate()
print(
    "Generating design using linear temperature SimulatedAnnealing & PhiP criterion =", design)
result = optimalLHSAlgorithm.getResult()
history = result.getAlgoHistory()
print("History criterion=", history[:, 0])
print("History temperature=", history[:, 1])
print("History probability=", history[:, 2])
print("Final criteria: C2=%f, PhiP=%f" % (result.getC2(), result.getPhiP()))

# Criteria drawing
criterionGraph = result.drawHistoryCriterion()
# criterionGraph.draw("SA_Lin_PhiP_Criterion")
probaGraph = result.drawHistoryProbability()
# probaGraph.draw("SA_Lin_PhiP_Proba")
tempGraph = result.drawHistoryTemperature()
# tempGraph.draw("SA_Lin_PhiP_Temperature")

# 7) Simulated Annealing LHS with linear temperature, Phi optimization and nStart > 1
#  Result is a collection of LHSResult
nStart = 10
optimalDesign = optimalLHSAlgorithm.generateWithRestart(nStart)
for i in range(nStart):
    result = optimalLHSAlgorithm.getResult()
    design = result.getOptimalDesign(i)
    history = result.getAlgoHistory(i)
    print(
        "Generating design using linear temperature SimulatedAnnealing & PhiP criterion =", design)
    print("History criterion=", history[:, 0])
    # print("History temperature=", history[:,1])
    # print("History probability=", history[:,2])
    print("Final criteria: C2=%f, PhiP=%f" %
          (result.getC2(i), result.getPhiP(i)))

    # decreasing criterion
    criterionGraph = result.drawHistoryCriterion(i)
    # criterionGraph.draw("SA_Lin_PhiP_Criterion_MultiRestart_%d"%(i+1))
    probaGraph = result.drawHistoryProbability(i)
    # probaGraph.draw("SA_Lin_PhiP_Proba_MultiRestart_%d"%(i+1))
    tempGraph = result.drawHistoryTemperature(i)
    # tempGraph.draw("SA_Lin_PhiP_Temperature_MultiRestart_%d"%(i+1))
