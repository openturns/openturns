#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    RandomGenerator.SetSeed(0)
    inputDimension = 3
    outputDimension = 1

    inputName = ["X1", "X2", "X3"]
    outputName = ["Y"]

    # Test with Ishigami function
    formulaIshigami = Description(outputDimension)
    formulaIshigami[
        0] = "sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)"

    modelIshigami = NumericalMathFunction(
        inputName, outputName, formulaIshigami)

    distributions = ComposedDistribution([Uniform(-1.0, 1.0)] * inputDimension)

    sensitivityAnalysis = FAST(modelIshigami, distributions, 400)

    firstOrderIndices = sensitivityAnalysis.getFirstOrderIndices()
    totalIndices = sensitivityAnalysis.getTotalIndices()

    # Comparaison with reference analytical values
    a = 7.0
    b = 0.1
    covTh = (b ** 2 * pi ** 8) / 18.0 + \
        (b * pi ** 4) / 5.0 + (a ** 2) / 8.0 + 1.0 / 2.0
    sob_1 = [(b * pi ** 4 / 5.0 + b ** 2 * pi ** 8 / 50.0 + 1.0 / 2.0)
             / covTh, (a ** 2 / 8.0) / covTh, 0.0]
    sob_2 = [
        0.0, (b ** 2 * pi ** 8 / 18.0 - b ** 2 * pi ** 8 / 50.0) / covTh, 0.0]
    sob_3 = [0.0]
    sob_T1 = [sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0], sob_1[1] + sob_2[0]
              + sob_2[2] + sob_3[0], sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0]]

    for i in range(inputDimension):
        value = firstOrderIndices[i]
        print("Ishigami first order FAST indice ", i, "= %.8f" %
              value, "absolute error=%.8f" % fabs(value - sob_1[i]))
    print("\n")
    for i in range(inputDimension):
        value = totalIndices[i]
        print("Ishigami total FAST indice", i, "= %.8f" %
              value, "absolute error=%.8f" % fabs(value - sob_T1[i]))

    # Test with G-Sobol function
    formulaGSobol = ["1.0"]
    covTh = 1.0
    a = NumericalPoint(inputDimension)
    for i in range(inputDimension):
        a[i] = 0.5 * i
        covTh = covTh * (1.0 + 1.0 / (3.0 * (1.0 + a[i]) ** 2))
        formulaGSobol[0] = formulaGSobol[0] + " * ((abs(4.0 * X" + str(i + 1) + " - 2.0) + " + \
                                              str(a[i]) + ") / (1.0 + " + \
            str(a[i]) + "))"

    covTh = covTh - 1.0
    modelGSobol = NumericalMathFunction(inputName, outputName, formulaGSobol)

    distributions = ComposedDistribution([Uniform(0.0, 1.0)] * inputDimension)

    sensitivityAnalysis = FAST(modelGSobol, distributions, 400)

    # Comparaison with reference analytical values
    firstOrderIndices = sensitivityAnalysis.getFirstOrderIndices()
    totalIndices = sensitivityAnalysis.getTotalIndices()
    # First-order indices
    V_i = NumericalPoint(inputDimension)
    Vtot_i = NumericalPoint(inputDimension)
    prod_V_i = 1.0
    for i in range(inputDimension):
        V_i[i] = 1.0 / (3.0 * (1.0 + a[i]) ** 2.0)
        prod_V_i *= V_i[i]
    # Total indices
    Vtot_i[0] = V_i[0] + V_i[0] * V_i[1] + V_i[0] * V_i[2] + prod_V_i
    Vtot_i[1] = V_i[1] + V_i[0] * V_i[1] + V_i[1] * V_i[2] + prod_V_i
    Vtot_i[2] = V_i[2] + V_i[0] * V_i[2] + V_i[1] * V_i[2] + prod_V_i
    # Results
    print("\n")
    for i in range(inputDimension):
        value = firstOrderIndices[i]
        print("G-Sobol first order FAST indice ", i, "= %.8f" %
              value, "absolute error=%.8f" % fabs(value - V_i[i] / covTh))

    print("\n")
    for i in range(inputDimension):
        value = totalIndices[i]
        print("G-Sobol total FAST indices ", i, "= %.8f" %
              value, "absolute error=%.8f" % fabs(value - Vtot_i[i] / covTh))

except:
    import sys
    print("t_FAST_std.py", sys.exc_info()[0], sys.exc_info()[1])
