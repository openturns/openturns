#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# We create a Function
myFunc = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4'],
                             ['(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)',
                              'cos(x2*x2+x4)/(x1*x1+1+x3^4)'])

# We create a distribution
dim = myFunc.getInputDimension()
meanPoint = [0.2] * dim
sigma = [0.] * dim
for i in range(dim):
    sigma[i] = 0.1 * (i + 1)
R = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    R[i, i - 1] = 0.25

distribution = ot.Normal(meanPoint, sigma, R)

# We create a distribution-based RandomVector
X = ot.RandomVector(distribution)

# We create a composite RandomVector Y from X and myFunction
Y = ot.CompositeRandomVector(myFunc, X)

# We create a quadraticCumul algorithm
algo = ot.TaylorExpansionMoments(Y)

# We test the attributes here
print('algo=', algo)

# We compute the several elements provided by the quadratic cumul algorithm
print('First order mean=', repr(algo.getMeanFirstOrder()))
print('Second order mean=', repr(algo.getMeanSecondOrder()))
print('Covariance=', repr(algo.getCovariance()))
print('Value at mean=', repr(algo.getValueAtMean()))
print('Gradient at mean=', repr(algo.getGradientAtMean()))
print('Hessian at mean=', repr(algo.getHessianAtMean()))

algo_1 = ot.TaylorExpansionMoments(Y.getMarginal(0))
print('Importance factors=', repr(
    algo_1.getImportanceFactors()))
