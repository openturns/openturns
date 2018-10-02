#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.Log.Show(ot.Log.ALL)

dim = 3
model = ot.SymbolicFunction(['x0', 'x1', 'x2'],
                            ['sin(x0)*cos(x1)*exp(x2)+x0*x1*x2', '(-1+x0*x0^2+x0^3)*(2+x1+2*x1^2+3*x1^5)*(15+2*x2+5*x2^7)'])
coll = [ot.Uniform()] * dim


# friedman
# dim = 5
# model = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4', 'x5'],
#['10*sin(pi_*x1*x2)+20*(x3-0.5)^2+10*x4+5*x5'])
# coll = [ot.Uniform(0., 1.)] * dim


# borehole
# dim = 8
# model = ot.SymbolicFunction(['rw', 'r', 'Tu', 'Hu', 'Tl', 'Hl', 'L', 'Kw'],
#['(2*pi_*Tu*(Hu-Hl))/(ln(r/rw)*(1+(2*L*Tu)/(ln(r/rw)*rw^2*Kw)+Tu/Tl))'])
# coll = [ot.Normal(0.1, 0.0161812),
# ot.LogNormal(7.71, 1.0056),
# ot.Uniform(63070.0, 115600.0),
# ot.Uniform(990.0, 1110.0),
# ot.Uniform(63.1, 116.0),
# ot.Uniform(700.0, 820.0),
# ot.Uniform(1120.0, 1680.0),
# ot.Uniform(9855.0, 12045.0)]

distribution = ot.ComposedDistribution(coll)
factoryCollection = [ot.OrthogonalUniVariateFunctionFamily(
    ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.StandardDistributionPolynomialFactory(dist))) for dist in coll]


functionFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)

size = 1000
X = distribution.getSample(size)
Y = model(X)


# ot.ResourceMap.Set('TensorApproximationAlgorithm-Method', 'RankM')

# n-d
nk = [10] * dim
maxRank = 5
algo = ot.TensorApproximationAlgorithm(
    X, Y, distribution, functionFactory, nk, maxRank)
algo.run()
result = algo.getResult()
# print('residuals=', result.getResiduals())
ott.assert_almost_equal(result.getResiduals(), [0.000466643, 0.0])

metamodel = result.getMetaModel()
x = distribution.getMean()
print('x=', ot.Point(x), 'f(x)=', model(x), 'f^(x)=', metamodel(x))
for i in range(model.getOutputDimension()):
    print('rank[', i, ']=', result.getTensor(i).getRank())
