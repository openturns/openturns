#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

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

    sigma0 = 0.1
    mean_prior = Normal(mu0, sigma0)
    std_prior = Dirac(2.0)  # standard dev is known
    priorColl = DistributionCollection()
    priorColl.add(mean_prior)
    priorColl.add(std_prior)
    prior = ComposedDistribution(priorColl)

    # choose the initial state within the prior
    initialState = prior.getRealization()

    # conditional distribution
    conditional = Normal()

    # create a metropolis-hastings sampler
    sampler = RandomWalkMetropolisHastings(
        prior, conditional, data, initialState, proposalColl)
    sampler.setVerbose(True)
    sampler.setThinning(100)
    sampler.setBurnIn(1000)
    sampler.setCalibrationStrategyPerComponent(calibrationColl)

    # create a PosteriorRandomVector
    randomVector = PosteriorRandomVector(sampler)
    print("randomVector=", randomVector)

    # get the dimension
    dimension = randomVector.getDimension()
    print("dimension=", dimension)

    # get a realization
    realization = randomVector.getRealization()
    print("realization=", realization)

    # get a sample
    sample = randomVector.getSample(10)
    print("sample=", sample)

except:
    import sys
    import traceback
    traceback.print_exc(file=sys.stdout)
