#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


input_dimension = 3
output_dimension = 1

formula = [
    'sin(pi_*X1)+7*sin(pi_*X2)*sin(pi_*X2)+0.1*((pi_*X3)*(pi_*X3)*(pi_*X3)*(pi_*X3))*sin(pi_*X1)']

model = ot.SymbolicFunction(['X1', 'X2', 'X3'], formula)

distribution = ot.ComposedDistribution(
    [ot.Uniform(-1.0, 1.0)] * input_dimension)

# Size of simulation
size = 10000

# Test with the various implementation methods
methods = ["Saltelli", "Jansen", "MauntzKucherenko", "Martinez"]

# Generate input/output designs
computeSO = True
inputDesign = ot.SobolIndicesExperiment(distribution, size, computeSO).generate()
outputDesign = model(inputDesign)
# Case 1 : Estimation of sensitivity using estimator and no bootstrap
for method in methods:
    sensitivity_algorithm = eval(
        'ot.' + method + "SensitivityAlgorithm(inputDesign, outputDesign, size)")
    print("Method of evaluation=", method)

    # Get first order indices
    fo = sensitivity_algorithm.getFirstOrderIndices()
    print("First order indices = ", fo)
    # Get total order indices
    to = sensitivity_algorithm.getTotalOrderIndices()
    print("Total order indices = ", to)

    # Get the confidence interval thanks to Bootstrap
    nr_bootstrap = 100
    confidence_level = 0.95
    #sensitivity_algorithm = ot.MartinezSensitivityAlgorithm(
        #inputDesign, outputDesign, size)
    sensitivity_algorithm.setBootstrapSize(nr_bootstrap)
    sensitivity_algorithm.setConfidenceLevel(confidence_level)
    sensitivity_algorithm.setUseAsymptoticDistribution(False)
    interval_fo = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to = sensitivity_algorithm.getTotalOrderIndicesInterval()
    print("bootstrap intervals")
    print("First order indices interval = ", interval_fo)
    print("Total order indices interval = ", interval_to)

    # Asymptotic confidence interval
    sensitivity_algorithm.setUseAsymptoticDistribution(True)
    interval_fo_asymptotic = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to_asymptotic = sensitivity_algorithm.getTotalOrderIndicesInterval()
    print("asymptotic intervals:")
    print("First order indices distribution = ", sensitivity_algorithm.getFirstOrderIndicesDistribution())
    print("Total order indices distribution = ", sensitivity_algorithm.getTotalOrderIndicesDistribution())
    print("First order indices interval = ", interval_fo_asymptotic)
    print("Total order indices interval = ", interval_to_asymptotic)

# with experiment
sequence = ot.SobolSequence(input_dimension)
experiment = ot.LowDiscrepancyExperiment(sequence, ot.ComposedDistribution(
    [ot.Uniform(0.0, 1.0)] * input_dimension), size)
sensitivity_algorithm = ot.SaltelliSensitivityAlgorithm(experiment, model)
print(sensitivity_algorithm.getFirstOrderIndices())

# multi variate model
model_aggregated = ot.SymbolicFunction(['X1', 'X2', 'X3'],
                                    ['2*X1 + X2 - 3*X3 + 0.3*X1*X2', 
                                    '-5*X1 + 4*X2 - 0.8*X2*X3 + 2*X3'])
distribution_aggregated = ot.ComposedDistribution([ot.Uniform()]*3)
inputDesign = ot.SobolIndicesExperiment(distribution_aggregated, size).generate()
outputDesign = model_aggregated(inputDesign)
# Case 1 : Estimation of sensitivity using estimator and no bootstrap
for method in methods:
    sensitivity_algorithm = eval(
        'ot.' + method + "SensitivityAlgorithm(inputDesign, outputDesign, size)")
    print("Method of evaluation=", method)

    # Get first order indices
    fo = sensitivity_algorithm.getAggregatedFirstOrderIndices()
    print("Aggregated first order indices = ", fo)
    # Get total order indices
    to = sensitivity_algorithm.getAggregatedTotalOrderIndices()
    print("Aggregated total order indices = ", to)

    # Get the confidence interval thanks to Bootstrap
    nr_bootstrap = 100
    confidence_level = 0.95
    #sensitivity_algorithm = ot.MartinezSensitivityAlgorithm(
        #inputDesign, outputDesign, size)
    sensitivity_algorithm.setBootstrapSize(nr_bootstrap)
    sensitivity_algorithm.setConfidenceLevel(confidence_level)
    sensitivity_algorithm.setUseAsymptoticDistribution(False)
    interval_fo = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to = sensitivity_algorithm.getTotalOrderIndicesInterval()
    print("bootstrap intervals")
    print("Aggregated first order indices interval = ", interval_fo)
    print("Aggregated total order indices interval = ", interval_to)

    # Asymptotic confidence interval
    sensitivity_algorithm.setUseAsymptoticDistribution(True)
    interval_fo_asymptotic = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to_asymptotic = sensitivity_algorithm.getTotalOrderIndicesInterval()
    print("asymptotic intervals:")
    print("Aggregated first order indices interval = ", interval_fo_asymptotic)
    print("Aggregated total order indices interval = ", interval_to_asymptotic)
