#! /usr/bin/env python

from __future__ import print_function
import sys
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.INFO)


def progress(percent):
    sys.stderr.write('-- progress=' + str(percent) + '%\n')


def stop():
    sys.stderr.write('-- stop?\n')
    return False


# We create a numerical math function
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], [
                            '-F*L^3/(3*E*I)', '-F*L^4/(3*E*I)'])

dim = model.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [50.0, 1.0, 10.0, 5.0]  # E, F, L, I
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
composite = ot.CompositeRandomVector(model, vect)

ot.RandomGenerator.SetSeed(42)
algo = ot.ExpectationSimulationAlgorithm(composite)
algo.setMaximumOuterSampling(250000)
algo.setBlockSize(2)
# algo.setMaximumCoefficientOfVariation(1e-6)
algo.setStandardDeviationCriterionType('MAX')
algo.setCoefficientOfVariationCriterionType('NONE')
# algo.setMaximumStandardDeviation(1.6)
# print(algo.getMaximumStandardDeviation())
# algo.setProgressCallback(progress)
# algo.setStopCallback(stop)

print('algo=', algo)

# Perform the simulation
algo.run()

# Stream out the result
result = algo.getResult()
print('result=', result)

ref_mu = composite.getSample(1000000).computeMean()
ref_var = composite.getSample(1000000).computeVariance()
print('mu=', ref_mu, 'var=', ref_var)
ott.assert_almost_equal(result.getExpectationEstimate(), ref_mu, 1e-2, 1e-5)
#ott.assert_almost_equal(result.getVarianceEstimate(), ref_var, 1e-2, 1e-5)

expectationDistribution = result.getExpectationDistribution()
print(expectationDistribution)

convergenceGraph = algo.drawExpectationConvergence()

#from openturns.viewer import View
# View(convergenceGraph).ShowAll()
