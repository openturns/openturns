#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    inDim = 3
    outDim = 1

    formula = [
        'sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)']

    model = NumericalMathFunction(['X1', 'X2', 'X3'], ['Y'], formula)

    distribution = ComposedDistribution(
        [Uniform(-1.0, 1.0)] * inDim, IndependentCopula(inDim))

    size = 10000
    sample1 = distribution.getSample(size)
    sample2 = distribution.getSample(size)

    sensitivityAnalysis = SensitivityAnalysis(sample1, sample2, model)
    sensitivityAnalysis.setBlockSize(17)
    secondOrderIndices = sensitivityAnalysis.getSecondOrderIndices()
    firstOrderIndices = sensitivityAnalysis.getFirstOrderIndices()
    totalOrderIndices = sensitivityAnalysis.getTotalOrderIndices()

    print("First order Sobol indice of Y|X1 = %.6f" % firstOrderIndices[0])
    print("Total order Sobol indice of Y|X3 = %.6f" % totalOrderIndices[2])
    print("Second order Sobol indice of Y|X1,X3 = %.6f" %
          secondOrderIndices[0, 2])

except:
    import sys
    print("t_SensitivityAnalysis_std.py", sys.exc_info()[0], sys.exc_info()[1])
