#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
#from openturns.viewer import PlotSensitivity

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

try:
    input_dimension = 3
    output_dimension = 1

    formula = [
        'sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)']

    model = ot.NumericalMathFunction(['X1', 'X2', 'X3'], ['Y'], formula)

    distribution = ot.ComposedDistribution(
        [ot.Uniform(-1.0, 1.0)] * input_dimension)

    # Size of simulation
    size = 10000

    # Test with the various implementation methods
    methods = ["Saltelli", "Jansen", "MauntzKucherenko", "Martinez"]

    # Use of Generate method to build input/output designs
    computeSO = True
    inputDesign = ot.SaltelliSensitivityAlgorithm.Generate(
        distribution, size, computeSO)
    outputDesign = model(inputDesign)
    # Case 1 : Estimation of sensitivity using estimator and no bootstrap
    for method in methods:
        sensitivity_algorithm = eval(
            'ot.' + method + "SensitivityAlgorithm(inputDesign, outputDesign, size)")
        # Get first order indices
        fo = sensitivity_algorithm.getFirstOrderIndices()
        print("Method of evaluation=", method)
        print("First order indices = ", fo)
        # Get total order indices
        to = sensitivity_algorithm.getTotalIndices()
        print("Total indices = ", to)

    # Case 2 : Estimation of sensitivity using Martinez estimator and bootstrap
    nr_bootstrap = 100
    confidence_level = 0.95
    sensitivity_algorithm = ot.MartinezSensitivityAlgorithm(
        inputDesign, outputDesign, size)
    sensitivity_algorithm.setBootstrapSize(nr_bootstrap)
    sensitivity_algorithm.setBootstrapConfidenceLevel(confidence_level)
    # Get first order indices
    fo = sensitivity_algorithm.getFirstOrderIndices()
    to = sensitivity_algorithm.getTotalIndices()
    print("Martinez method : indices")
    print("First order indices = ", fo)
    print("Total indices = ", to)
    # Get the confidence interval thanks to Bootstrap
    interval_fo = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to = sensitivity_algorithm.getTotalIndicesInterval()
    print("Martinez method : bootstrap intervals")
    print("First order indices interval = ", interval_fo)
    print("Total indices interval = ", interval_to)
    # In the case of Martinez, if output is Gaussian, we may use the Fisher
    # transform and get an asymptotic confidence interval
    ot.ResourceMap.SetAsBool(
        "MartinezSensitivityAlgorithm-UseAsymptoticInterval", True)
    interval_fo_asymptotic = sensitivity_algorithm.getFirstOrderIndicesInterval()
    interval_to_asymptotic = sensitivity_algorithm.getTotalIndicesInterval()
    print("Martinez method : asymptotic intervals")
    print("First order indices interval = ", interval_fo_asymptotic)
    print("Total indices interval = ", interval_to_asymptotic)

    # with experiment
    sequence = ot.SobolSequence(input_dimension)
    experiment = ot.LowDiscrepancyExperiment(sequence, ot.ComposedDistribution(
        [ot.Uniform(0.0, 1.0)] * input_dimension), size)
    sensitivity_algorithm = ot.SaltelliSensitivityAlgorithm(experiment, model)
    print(sensitivity_algorithm.getFirstOrderIndices())
except:
    import sys
    print("t_SensitivityAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
