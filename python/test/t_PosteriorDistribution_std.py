#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.ResourceMap.SetAsUnsignedInteger("DeconditionedDistribution-MarginalIntegrationNodesNumber", 24)

observationsSize = 5
# Create a collection of distribution
conditionedDistribution = ot.Normal()
conditioningDistributionCollection = []
# First conditioning distribution: continuous/continuous
atoms = [ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0)]
conditioningDistributionCollection.append(ot.JointDistribution(atoms))
# Second conditioning distribution: discrete/continuous
atoms = [ot.Binomial(3, 0.5), ot.Uniform(1.0, 2.0)]
# conditioningDistributionCollection.append(ot.JointDistribution(atoms))
# Third conditioning distribution: dirac/continuous
atoms = [ot.Dirac(0.0), ot.Uniform(1.0, 2.0)]
# conditioningDistributionCollection.append(ot.JointDistribution(atoms))


for conditioning in conditioningDistributionCollection:
    print("\n" + "=" * 50 + "\n")
    print("conditioning distribution=", conditioning)
    observationsDistribution = ot.Distribution(conditionedDistribution)
    observationsDistribution.setParameter(conditioning.getMean())
    observations = observationsDistribution.getSample(observationsSize)
    distribution = otexp.PosteriorDistribution(ot.DeconditionedDistribution(conditionedDistribution, conditioning), observations)
    dim = distribution.getDimension()
    print("Distribution ", distribution)
    print("range=", distribution.getRange())
    mean = distribution.getMean()
    print("Mean ", mean)
    print("Covariance ", distribution.getCovariance())
    # Is this distribution an elliptical distribution?
    print("Elliptical distribution= ", distribution.isElliptical())

    # Has this distribution an elliptical copula?
    print("Elliptical copula= ", distribution.hasEllipticalCopula())

    # Has this distribution an independent copula?
    print("Independent copula= ", distribution.hasIndependentCopula())

    # Show PDF and CDF of mean point
    meanPDF = distribution.computePDF(mean)
    meanCDF = distribution.computeCDF(mean)
    print("Mean point= ", mean, " pdf=%.5g" % meanPDF, " cdf=%.5g" % meanCDF)
    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    print("CDF(quantile)=%.6g" % distribution.computeCDF(quantile))
    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        print("margin=", margin)
        print("margin PDF=", ot.Point(1, margin.computePDF(mean[i])))
        print("margin CDF=", ot.Point(1, margin.computeCDF(mean[i])))
        print("margin quantile=", margin.computeQuantile(0.95))

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.setMomentsSamplingSize(10000)
    validation.skipCorrelation()  # slow
    validation.skipParameters()  # not yet implemented
    validation.skipGradient()  # not yet implemented
    validation.run()

# Test the constructors
conditioned = ot.Normal()
link = ot.SymbolicFunction(["y"], ["0.0", "y"])
conditioning = ot.Uniform(0.5, 1.5)
observations = conditioned.getSample(10)

# First constructor
posterior = otexp.PosteriorDistribution()

# Second constructor
posterior = otexp.PosteriorDistribution(ot.DeconditionedDistribution(conditioned, conditioning, link), observations)

# Third constructor
posterior = otexp.PosteriorDistribution(conditioned, ot.IndependentCopula(2), observations)

# Fourth constructor
posterior = otexp.PosteriorDistribution(conditioned, conditioning, link, observations)

# Accessors to the arguments of the constructor
print("conditioned=", posterior.getConditionedDistribution())
print("conditioning=", posterior.getConditioningDistribution())
print("link=", posterior.getLinkFunction())
