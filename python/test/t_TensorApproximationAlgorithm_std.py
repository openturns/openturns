#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

#ot.Log.Show(ot.Log.ALL)

dim = 3
model = ot.NumericalMathFunction(['x0', 'x1', 'x2'], ['y1', 'y2'],
  ['sin(x0)*cos(x1)*exp(x2)', '(-1+x0*x0^2+x0^3)*(2+x1+2*x1^2+3*x1^5)*(15+2*x2+5*x2^7)'])
coll = [ot.Uniform()] * dim


## friedman
#dim = 5
#model = ot.NumericalMathFunction(['x1', 'x2', 'x3', 'x4', 'x5'], ['u'],
                                    #['10*sin(_pi*x1*x2)+20*(x3-0.5)^2+10*x4+5*x5'])
#coll = [ot.Uniform(0., 1.)] * dim


# borehole
#dim = 8
#model = ot.NumericalMathFunction(['rw', 'r', 'Tu', 'Hu', 'Tl', 'Hl', 'L', 'Kw'], ['u'],
  #['(2*_pi*Tu*(Hu-Hl))/(ln(r/rw)*(1+(2*L*Tu)/(ln(r/rw)*rw^2*Kw)+Tu/Tl))'])
#coll = [ot.Normal(0.1, 0.0161812),
        #ot.LogNormal(7.71, 1.0056),
        #ot.Uniform(63070.0, 115600.0),
        #ot.Uniform(990.0, 1110.0),
        #ot.Uniform(63.1, 116.0),
        #ot.Uniform(700.0, 820.0),
        #ot.Uniform(1120.0, 1680.0),
        #ot.Uniform(9855.0, 12045.0)]

distribution = ot.ComposedDistribution(coll)
factoryCollection = list(map(lambda dist: ot.OrthogonalUniVariateFunctionFamily(ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.StandardDistributionPolynomialFactory(dist))), coll))


functionFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)

size = 1000
X = distribution.getSample(size)
Y = model(X)

# n-d
nk = [10] * dim
maxRank = 2
algo = ot.TensorApproximationAlgorithm(X, Y, distribution, functionFactory, nk, maxRank)
#algo.setApproximationAlgorithmFactory(ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), ot.CorrectedLeaveOneOut()))
algo.run()
result = algo.getResult()
print('residuals=', result.getResiduals())
metamodel = result.getMetaModel()
x = distribution.getMean()
print('x=', ot.NumericalPoint(x), 'f(x)=', model(x), 'f^(x)=', metamodel(x))

# selection
algo = ot.TensorApproximationAlgorithm(X, Y[:,0], distribution, functionFactory, nk, maxRank)
algo.setRankSelection(True)
algo.run()
result = algo.getResult()
print('residuals=', result.getResiduals())
metamodel = result.getMetaModel()
x = distribution.getMean()
print('x=', ot.NumericalPoint(x), 'f(x)=', model(x)[:1], 'f^(x)=', metamodel(x))




