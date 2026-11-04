#! /usr/bin/env python

import os
import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

# same surrogate setup as t_ActiveLearningUFunction_std
model = ot.SymbolicFunction(["x1", "x2"], ["x1 + 2.0 * x2 - 3.0"])
distribution = ot.JointDistribution([ot.Normal(), ot.Normal()])
inputSample = distribution.getSample(12)
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([1.0] * 2, [1.0])
basis = ot.ConstantBasisFactory(2).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
regressor = ot.GaussianProcessRegression(fitter.getResult())
regressor.run()
gprResult = regressor.getResult()


class PythonU(otexp.PythonActiveLearningReliabilityFunction):
    def __init__(self):
        super().__init__(0.0, 2.0)

    def computeAsScalar(self, x):
        cov = ot.GaussianProcessConditionalCovariance(self.getGaussianProcessRegression())
        mx = cov.getConditionalMean(x)[0]
        s2 = cov.getConditionalMarginalVariance(x)
        if not s2 > 0.0:
            return ot.SpecFunc.MaxScalar
        return abs(mx - self.getReliabilityThreshold()) / math.sqrt(s2)

    def checkConvergenceLearning(self, values):
        return values.getMin()[0] >= self.getLearningThreshold()

    def isMaximization(self):
        return False


# wrap a pure Python object into the C++ handle, as Distribution does
pyU = PythonU()
criterion = otexp.ActiveLearningReliabilityFunction(pyU)
assert "ActiveLearningReliabilityFunction" in criterion.__repr__()
assert criterion.getReliabilityThreshold() == 0.0
assert criterion.getLearningThreshold() == 2.0

# GPR set through the handle is pushed to the Python object
criterion.setGaussianProcessRegression(gprResult)
assert pyU.getGaussianProcessRegression() is not None
assert criterion.getGaussianProcessRegression().getMetaModel().getInputDimension() == 2
assert not criterion.isMaximization()

# scores match the C++ U criterion
reference = otexp.ActiveLearningUFunction(0.0, 2.0)
reference.setGaussianProcessRegression(gprResult)
pool = distribution.getSample(20)
ott.assert_almost_equal(criterion(pool, inputSample), reference(pool, inputSample))

# point and sample evaluations agree
ott.assert_almost_equal(
    criterion([pool[1]], inputSample)[0][0], criterion(pool, inputSample)[1][0]
)

# default getInfillSample selects the minimizer
scores = criterion(pool, inputSample)
infill = criterion.getInfillSample(pool, scores)
assert infill.getSize() == 1
ott.assert_almost_equal(infill[0], pool[scores.argsort(True)[0]])

# convergence matches the U rule
assert criterion.checkConvergenceLearning(ot.Sample([[2.5], [3.0]]))
assert not criterion.checkConvergenceLearning(ot.Sample([[2.5], [0.5]]))

# thresholds set through the handle are pushed to the Python object
criterion.setReliabilityThreshold(0.5)
assert pyU.getReliabilityThreshold() == 0.5
assert criterion(pool, inputSample)[0][0] != scores[0][0]
criterion.setReliabilityThreshold(0.0)
criterion.setLearningThreshold(3.0)
assert pyU.getLearningThreshold() == 3.0
assert criterion.checkConvergenceLearning(ot.Sample([[3.5], [4.0]]))


class BadInfillDimension(otexp.PythonActiveLearningReliabilityFunction):
    def __init__(self):
        super().__init__(0.0, 2.0)

    def computeAsScalar(self, x):
        return 0.0

    def checkConvergenceLearning(self, values):
        return True

    def isMaximization(self):
        return False

    def getInfillSample(self, candidates, values):
        return ot.Sample(1, 1)


class EmptyInfill(otexp.PythonActiveLearningReliabilityFunction):
    def __init__(self):
        super().__init__(0.0, 2.0)

    def computeAsScalar(self, x):
        return 0.0

    def checkConvergenceLearning(self, values):
        return True

    def isMaximization(self):
        return False

    def getInfillSample(self, candidates, values):
        return ot.Sample(0, 2)


# invalid infill samples are rejected instead of failing later in the algorithm
badInfill = otexp.ActiveLearningReliabilityFunction(BadInfillDimension())
with ott.assert_raises(ValueError):
    badInfill.getInfillSample(pool, scores)
emptyInfill = otexp.ActiveLearningReliabilityFunction(EmptyInfill())
with ott.assert_raises(TypeError):
    emptyInfill.getInfillSample(pool, scores)


class PythonMax(otexp.PythonActiveLearningReliabilityFunction):
    # vector __call__ override, ignores the design like EFF, maximizes
    def __init__(self):
        super().__init__(0.0, 0.01)

    def __call__(self, candidates, doe):
        return [[point[0] + 2.0 * point[1]] for point in candidates]

    def checkConvergenceLearning(self, values):
        return values.getMax()[0] <= self.getLearningThreshold()

    def isMaximization(self):
        return True


maxCriterion = otexp.ActiveLearningReliabilityFunction(PythonMax())
maxCriterion.setGaussianProcessRegression(gprResult)
assert maxCriterion.isMaximization()
maxScores = maxCriterion(pool, inputSample)
assert maxScores.getSize() == pool.getSize()
maxInfill = maxCriterion.getInfillSample(pool, maxScores)
ott.assert_almost_equal(maxInfill[0], pool[maxScores.argsort(False)[0]])

# usable from C++: run the algorithm with the Python criterion
event = ot.ThresholdEvent(
    ot.CompositeRandomVector(
        model, ot.RandomVector(ot.JointDistribution([ot.Normal(), ot.Normal()]))
    ),
    ot.Greater(),
    3.0,
)
simulation = ot.ProbabilitySimulationAlgorithm(event)
simulation.setMaximumOuterSampling(1000)
simulation.setBlockSize(100)
algoFitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
algo = otexp.ActiveLearningReliabilityAlgorithm(
    algoFitter, simulation, otexp.ActiveLearningReliabilityFunction(PythonU())
)
algo.setMaximumIterations(2)
algo.setSimulationAlgorithmSeed(0)
algo.run()
assert algo.getInputDoE().getSize() == 12 + algo.getFunctionCallNumber()

# persistence round-trip through a Study: a threshold changed on the Python
# side only must not be overwritten by the stale C++ value on save/load
pyU.setLearningThreshold(4.0)
assert criterion.getLearningThreshold() == 4.0
fileName = "t_ActiveLearningReliabilityFunction_python.xml"
study = ot.Study(fileName)
study.add("criterion", criterion)
study.save()
study2 = ot.Study(fileName)
study2.load()
loaded = otexp.ActiveLearningReliabilityFunction(PythonU())
study2.fillObject("criterion", loaded)
assert loaded.getReliabilityThreshold() == 0.0
assert loaded.getLearningThreshold() == 4.0
assert not loaded.checkConvergenceLearning(ot.Sample([[3.5], [4.5]]))
assert loaded.checkConvergenceLearning(ot.Sample([[4.5], [5.0]]))
ott.assert_almost_equal(loaded(pool, inputSample), criterion(pool, inputSample))
os.remove(fileName)
