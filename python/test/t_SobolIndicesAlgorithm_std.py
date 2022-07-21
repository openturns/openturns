#! /usr/bin/env python

import openturns as ot
from openturns.usecases import ishigami_function

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

# Test the different sampling options
samplings = ["MonteCarlo", "LHS", "QMC"]

# Generate input/output designs
computeSO = True
# Case 1 : Estimation of sensitivity using estimator and no bootstrap
for method in methods:
    for sampling in samplings:
        ot.ResourceMap.SetAsString(
            "SobolIndicesExperiment-SamplingMethod", sampling)
        sensitivity_algorithm = eval(
            'ot.' + method + "SensitivityAlgorithm(distribution, size, model, computeSO)")
        print("Method of evaluation=", method)
        print("Method of sampling=", sampling)
        # Get first order indices
        fo = sensitivity_algorithm.getFirstOrderIndices()
        print("First order indices = ", fo)
        # Get total order indices
        to = sensitivity_algorithm.getTotalOrderIndices()
        print("Total order indices = ", to)

        # Get the confidence interval thanks to Bootstrap
        nr_bootstrap = 100
        confidence_level = 0.95
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
        interval_fo_asymptotic = sensitivity_algorithm.getFirstOrderIndicesInterval(
        )
        interval_to_asymptotic = sensitivity_algorithm.getTotalOrderIndicesInterval(
        )
        print("asymptotic intervals:")
        print("First order indices distribution = ",
              sensitivity_algorithm.getFirstOrderIndicesDistribution())
        print("Total order indices distribution = ",
              sensitivity_algorithm.getTotalOrderIndicesDistribution())
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
distribution_aggregated = ot.ComposedDistribution([ot.Uniform()] * 3)
inputDesign = ot.SobolIndicesExperiment(
    distribution_aggregated, size).generate()
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
    # sensitivity_algorithm = ot.MartinezSensitivityAlgorithm(
    # inputDesign, outputDesign, size)
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
    interval_fo_asymptotic = sensitivity_algorithm.getFirstOrderIndicesInterval(
    )
    interval_to_asymptotic = sensitivity_algorithm.getTotalOrderIndicesInterval(
    )
    print("asymptotic intervals:")
    print("Aggregated first order indices interval = ", interval_fo_asymptotic)
    print("Aggregated total order indices interval = ", interval_to_asymptotic)


# special case in dim=2
ot.ResourceMap.SetAsString(
    'SobolIndicesExperiment-SamplingMethod', 'MonteCarlo')
ot.RandomGenerator.SetSeed(0)
distribution = ot.ComposedDistribution([ot.Uniform()] * 2)
size = 1000
model = ot.SymbolicFunction(['X1', 'X2'], ['2*X1 + X2'])
sensitivity_algorithm = ot.SaltelliSensitivityAlgorithm(
    distribution, size, model, True)
print(sensitivity_algorithm.getSecondOrderIndices())
ot.RandomGenerator.SetSeed(0)
experiment = ot.MonteCarloExperiment(distribution, size)
sensitivity_algorithm = ot.SaltelliSensitivityAlgorithm(
    experiment, model, True)
print(sensitivity_algorithm.getSecondOrderIndices())
ot.RandomGenerator.SetSeed(0)
x = ot.SobolIndicesExperiment(distribution, size, True).generate()
y = model(x)
sensitivity_algorithm = ot.SaltelliSensitivityAlgorithm(x, y, size)
print(sensitivity_algorithm.getSecondOrderIndices())


# null contribution case: X3 not in output formula
model = ot.SymbolicFunction(['X1', 'X2', 'X3'], ['10+3*X1+X2'])
distribution = ot.ComposedDistribution(
    [ot.Uniform(-1.0, 1.0)] * input_dimension)
size = 10000
for method in methods:
    sensitivity_algorithm = eval(
        'ot.' + method + "SensitivityAlgorithm(distribution, size, model, False)")
    sensitivity_algorithm.setUseAsymptoticDistribution(True)
    fo = sensitivity_algorithm.getFirstOrderIndices()
    print("First order indices = ", fo)
    to = sensitivity_algorithm.getTotalOrderIndices()
    #print("Total order indices = ", to)
    interval_fo = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to = sensitivity_algorithm.getTotalOrderIndicesInterval()
    print("Aggregated first order indices interval = ", interval_fo)
    #print("Aggregated total order indices interval = ", interval_to)

# setDesign must reset results across runs
ot.Log.Show(ot.Log.NONE)
im = ishigami_function.IshigamiModel()
exact_first_order = ot.Point([im.S1, im.S2, im.S3])
exact_total_order = ot.Point([im.ST1, im.ST2, im.ST3])
sobolIndicesAlgorithmB = ot.SaltelliSensitivityAlgorithm()
for sample_size in [100, 1000, 10000]:
    print("Size:", sample_size)

    # Method A : classical
    X = ot.SobolIndicesExperiment(im.distributionX, sample_size).generate()
    Y = im.model(X)
    sobolIndicesAlgorithmA = ot.SaltelliSensitivityAlgorithm(X, Y, sample_size)
    computed_first_orderA = sobolIndicesAlgorithmA.getFirstOrderIndices()
    computed_total_orderA = sobolIndicesAlgorithmA.getTotalOrderIndices()
    first_error = computed_first_orderA - exact_first_order
    total_error = computed_total_orderA - exact_total_order
    print("Method A :", first_error, total_error)

    # Method B : setDesign
    sobolIndicesAlgorithmB.setDesign(X, Y, sample_size)
    computed_first_orderB = sobolIndicesAlgorithmB.getFirstOrderIndices()
    computed_total_orderB = sobolIndicesAlgorithmB.getTotalOrderIndices()

    first_error = computed_first_orderB - exact_first_order
    total_error = computed_total_orderB - exact_total_order
    print("Method B :", first_error, total_error)
    assert computed_first_orderB == computed_first_orderA, "wrong first"
    assert computed_total_orderB == computed_total_orderA, "wrong total"
