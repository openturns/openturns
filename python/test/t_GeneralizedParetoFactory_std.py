#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import coles

ot.TESTPREAMBLE()

xi = [-0.75, 0.0, 0.75]
size = 10000
covariance = ot.CovarianceMatrix(2)
factory = ot.GeneralizedParetoFactory()
for i in range(3):
    distribution = ot.GeneralizedPareto(2.5, xi[i], 0.5)
    sample = distribution.getSample(size)
    # Distribution estimatedDistribution(factory.build(sample, covariance))
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    ott.assert_almost_equal(
        estimatedDistribution.getParameter(), distribution.getParameter(), 1e-1, 1e-1
    )
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(sample)
    print("GeneralizedPareto          =", distribution)
    print("Estimated generalizedPareto=", estimatedGeneralizedPareto)
    try:
        estimatedDistribution = factory.buildMethodOfMoments(sample)
        print("GeneralizedPareto from moments=", estimatedDistribution)
        ott.assert_almost_equal(
            estimatedDistribution.getParameter(),
            distribution.getParameter(),
            1e-1,
            1e-1,
        )
    except TypeError:
        # the method is invalid for some range of xsi
        pass


estimatedDistribution = factory.build()
ott.assert_almost_equal(estimatedDistribution.getParameter(), [1.0, 0.0, 0.0])
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto()
print("Default generalizedPareto=", estimatedGeneralizedPareto)
estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(
    distribution.getParameter()
)
print("GeneralizedPareto from parameters=", estimatedGeneralizedPareto)
ott.assert_almost_equal(
    estimatedGeneralizedPareto.getParameter(), distribution.getParameter()
)

# mean residual life
sample = coles.Coles().rain
graph = factory.drawMeanResidualLife(sample)
