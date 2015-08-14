#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # this analitycal example is taken from "Bayesian Modeling Using WinBUGS" - Ioannis Ntzoufras
    # 1.5.3: Inference for the mean or normal data with known variance

    # Variable of interest: Y=N(mu, sigma)
    # Prior for mu: Normal(mu0, sigma0), sigma is known
    # Posterior for mu: E(mu|y)=w*y_mean+(1-w)*mu0, and Var(mu|y)=w*(sigmay^2)/n
    # => weighted average of the prior an the sample mean
    # with w = n*sigma0^2 / (n*sigma0^2 + sigma^2)

    # Log::Show(Log::ALL)
    # observations
    size = 10
    realDist = Normal(31., 1.2)

    data = realDist.getSample(size)

    # calibration parameters
    calibrationColl = [CalibrationStrategy()] * 2

    # proposal distribution
    proposalColl = DistributionCollection()
    mean_proposal = Uniform(-2.0, 2.0)
    std_proposal = Uniform(-2.0, 2.0)
    proposalColl.add(mean_proposal)
    proposalColl.add(std_proposal)

    # prior distribution
    mu0 = 25.

    sigma0s = [0.1, 1.0]
    # sigma0s.append(2.0)

    # play with the variance of the prior:
    # if the prior variance is low (information concernig the mu parameter is strong)
    # then the posterior mean will be equal to the prior mean
    # if large, the the posterior distribution is equivalent to the
    # distribution of the sample mean
    for i in range(len(sigma0s)):

        sigma0 = sigma0s[i]
        mean_prior = Normal(mu0, sigma0)
        std_prior = Dirac(2.0)  # standard dev is known
        prior = ComposedDistribution([mean_prior, std_prior])

        # choose the initial state within the prior
        initialState = prior.getRealization()

        # conditional distribution
        conditional = Normal()

        # create a metropolis-hastings sampler
        sampler = RandomWalkMetropolisHastings(
            prior, conditional, data, initialState, proposalColl)
        sampler.setVerbose(True)
        sampler.setThinning(2)
        sampler.setBurnIn(500)
        sampler.setCalibrationStrategyPerComponent(calibrationColl)
        realization = sampler.getRealization()

        sigmay = ConditionalDistribution(
            Normal(), prior).getStandardDeviation()[0]
        w = size * sigma0 ** 2. / (size * sigma0 ** 2. + sigmay ** 2.0)

        print("prior variance= %.12g" % (sigma0 ** 2.))
        print("  realization=", realization)

        print("  w= %.12g" % w)

        # the posterior for mu is analytical
        print("  expected posterior ~N( %.12g" % (w * data.computeMean()
                                                  [0] + (1. - w) * mu0), ",  %.12g" % ((w * sigmay ** 2.0 / size) ** 0.5), ")")

        # try to generate a sample
        sample = sampler.getSample(50)

        print("  obtained posterior ~N( %.12g" % sample.computeMean()[
              0], ",  %.12g" % sample.computeStandardDeviationPerComponent()[0], ")")

        print("  acceptance rate=", sampler.getAcceptanceRate())


except:
    import sys
    import traceback
    traceback.print_exc(file=sys.stdout)
