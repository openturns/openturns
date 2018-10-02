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


dimension = 3

# gsobol
#meanTh = 1.0
#a = [0.0] * dimension
#inputVariables = [''] * dimension
#formula = '1.0'
#covTh = 1.0
# for i in range(dimension):
#a[i] = 0.5 * i
#covTh = covTh * (1.0 + 1.0 / (3.0 * (1.0 + a[i]) ** 2))
#inputVariables[i] = "xi" + str(i)
#formula += " * ((abs(4.0 * xi" + str(i) + " - 2.0) + " + str(a[i]) + ") / (1.0 + " + str(a[i]) + "))"
#covTh = covTh - 1.0
#model = ot.SymbolicFunction(inputVariables, [formula])
#distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * dimension)

# ishigami
formula = 'sin(pi_*X1)+7*sin(pi_*X2)*sin(pi_*X2)+0.1*((pi_*X3)*(pi_*X3)*(pi_*X3)*(pi_*X3))*sin(pi_*X1)'
model = ot.SymbolicFunction(['X1', 'X2', 'X3'], [formula])
distribution = ot.ComposedDistribution([ot.Uniform(-1.0, 1.0)] * dimension)

estimator = ot.SaltelliSensitivityAlgorithm()
estimator.setUseAsymptoticDistribution(True)
algo = ot.SobolSimulationAlgorithm(distribution, model, estimator)
algo.setMaximumOuterSampling(250)  # number of iterations
# size N of Sobol experiment at each iteration, total size is N*(d+2)
algo.setBlockSize(10000)
algo.setBatchSize(97)  # number of points evaluated simultaneously
algo.setIndexQuantileLevel(0.05)
algo.setIndexQuantileEpsilon(1e-2)
# algo.setProgressCallback(progress)
# algo.setStopCallback(stop)

print('algo=', algo)

# Perform the simulation
algo.run()

# Stream out the result
result = algo.getResult()
print('result=', result)
print('FO=', result.getFirstOrderIndicesEstimate())
print('TO=', result.getTotalOrderIndicesEstimate())

foDist = result.getFirstOrderIndicesDistribution()
print(foDist)
toDist = result.getTotalOrderIndicesDistribution()
print(toDist)
convergenceGraph = algo.drawFirstOrderIndexConvergence()
convergenceGraph = algo.drawTotalOrderIndexConvergence()

#from openturns.viewer import View
# View(convergenceGraph).ShowAll()
