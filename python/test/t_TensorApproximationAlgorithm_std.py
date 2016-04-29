#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

#ot.Log.Show(ot.Log.ALL)


## friedman
#dim = 5
#model = ot.NumericalMathFunction(['x1', 'x2', 'x3', 'x4', 'x5'], ['u'],
                                    #['10*sin(_pi*x1*x2)+20*(x3-0.5)^2+10*x4+5*x5'])
#distribution = ot.ComposedDistribution([ot.Uniform(0., 1.)] * dim)

dim = 3
model = ot.NumericalMathFunction(['x0', 'x1', 'x2'], ['y'], ['sin(x0)*cos(x1)*exp(x2)'])
distribution = ot.ComposedDistribution([ot.Uniform()] * dim)

factoryCollection = [ot.OrthogonalUniVariateFunctionFamily(ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.LegendreFactory()))] * dim
#factoryCollection = [ot.FourierSeriesFactory()] * dim
functionFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)

size = 1000
X = distribution.getSample(size)
Y = model(X)
#print(X,Y)
nk = [10] * dim
algo = ot.TensorApproximationAlgorithm(X, Y, distribution, functionFactory, nk)
algo.run()
result = algo.getResult()
print('residuals=', result.getResiduals())

