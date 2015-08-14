#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # We create a NumericalMathFunction
    myFunc = NumericalMathFunction(['x1', 'x2', 'x3', 'x4'], ['y1', 'y2'], [
                                   '(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)', 'cos(x2*x2+x4)/(x1*x1+1+x3^4)'])

    # We create a distribution
    dim = myFunc.getInputDimension()
    meanPoint = [0.2] * dim
    sigma = [0.] * dim
    for i in range(dim):
        sigma[i] = 0.1 * (i + 1)
    R = CorrelationMatrix(IdentityMatrix(dim))
    for i in range(1, dim):
        R[i, i - 1] = 0.25

    distribution = Normal(meanPoint, sigma, R)

    # We create a distribution-based RandomVector
    X = RandomVector(distribution)

    # We create a composite RandomVector Y from X and myFunction
    Y = RandomVector(myFunc, X)

    # We create a quadraticCumul algorithm
    myQuadraticCumul = QuadraticCumul(Y)

    # We test the attributes here
    print("myQuadraticCumul=", myQuadraticCumul)

    # We compute the several elements provided by the quadratic cumul algorithm
    print("First order mean=", repr(myQuadraticCumul.getMeanFirstOrder()))
    print("Second order mean=", repr(myQuadraticCumul.getMeanSecondOrder()))
    print("Covariance=", repr(myQuadraticCumul.getCovariance()))
    print("Value at mean=", repr(myQuadraticCumul.getValueAtMean()))
    print("Gradient at mean=", repr(myQuadraticCumul.getGradientAtMean()))
    print("Hessian at mean=", repr(myQuadraticCumul.getHessianAtMean()))

    myQuadraticCumul_1 = QuadraticCumul(Y.getMarginal(0))
    print("Importance factors=", repr(
        myQuadraticCumul_1.getImportanceFactors()))

except:
    import sys
    print("t_QuadraticCumul_std.py", sys.exc_info()[0], sys.exc_info()[1])
