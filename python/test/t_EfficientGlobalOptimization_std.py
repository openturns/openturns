#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m
import openturns.testing

# ot.Log.Show(ot.Log.INFO)
ot.TBB.Disable()


#
# branin

dim = 2

# model
branin = ot.SymbolicFunction(
    ['x1', 'x2'], ['((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/8*pi_)*cos(x1)+10-54.8104)/51.9496', '0.96'])
transfo = ot.SymbolicFunction(['u1', 'u2'], ['15*u1-5', '15*u2'])
model = ot.ComposedFunction(branin, transfo)

# problem
problem = ot.OptimizationProblem()
problem.setObjective(model)
bounds = ot.Interval([0.0] * dim, [1.0] * dim)
problem.setBounds(bounds)

# design
experiment = ot.Box([1, 1])
inputSample = experiment.generate()
modelEval = model(inputSample)
outputSample = modelEval.getMarginal(0)


# first kriging model
covarianceModel = ot.SquaredExponential([0.3007, 0.2483], [0.981959])
basis = ot.ConstantBasisFactory(dim).build()
kriging = ot.KrigingAlgorithm(
    inputSample, outputSample, covarianceModel, basis)
noise = [x[1] for x in modelEval]
kriging.setNoise(noise)
kriging.run()

# algo
algo = ot.EfficientGlobalOptimization(problem, kriging.getResult())
algo.setNoiseModel(ot.SymbolicFunction(
    ['x1', 'x2'], ['0.96']))  # assume constant noise var
algo.setMaximumEvaluationNumber(20)
algo.setImprovementFactor(
    0.05)  # stop whe improvement is < a% the current optimum
algo.setAEITradeoff(0.66744898)
algo.run()
result = algo.getResult()
# print('1st pass result=', result)
# print('iteration=', result.getEvaluationNumber())
assert result.getEvaluationNumber(
) > 3 and result.getEvaluationNumber() < 15, 'Too few/much iterations'
# print(result.getInputSample())
# print(result.getOutputSample())

# openturns.testing.assert_almost_equal(result.getOptimalPoint(), [0.5, 0.0], 1e-5, 1e-5)
# openturns.testing.assert_almost_equal(result.getOptimalValue(),
# [-0.802223], 1e-5, 1e-5)

# local refinement eventhough the model is noisy (we still want to check
# we're not too far from optimum)
problem.setObjective(model.getMarginal(0))
algo2 = ot.NLopt(problem, 'LD_LBFGS')
algo2.setStartingPoint(result.getOptimalPoint())
algo2.run()
result = algo2.getResult()
# print(result)
# openturns.testing.assert_almost_equal(result.getOptimalPoint(), [0.542773, 0.151666], 1e-5, 1e-5)
# openturns.testing.assert_almost_equal(result.getOptimalPoint(),
# [0.123895, 0.818329], 1e-5, 1e-5)
openturns.testing.assert_almost_equal(
    result.getOptimalPoint(), [0.961652, 0.165000], 1e-5, 1e-5)
openturns.testing.assert_almost_equal(
    result.getOptimalValue(), [-0.979476], 1e-5, 1e-5)


#
# ackley 2-d
ot.RandomGenerator.SetSeed(0)
dim = 2


# model
def ackley(X):
    a = 20.0
    b = 0.2
    c = 2.0 * m.pi
    d = len(X)
    f = -a * m.exp(-b * m.sqrt(sum(x**2 for x in X) / d)) - \
        m.exp(sum(m.cos(c * x) for x in X) / d) + a + m.exp(1.0)
    # print(X, f)
    return [f]


model = ot.PythonFunction(dim, 1, ackley)

# problem
problem = ot.OptimizationProblem()
problem.setObjective(model)
bounds = ot.Interval([-15.0] * dim, [15.0] * dim)
problem.setBounds(bounds)

# design
center = [0.5] * dim
levels = [2.0, 4.0, 8.0, 14.0]
experiment = ot.Factorial(center, levels)
inputSample = experiment.generate()
outputSample = model(inputSample)

# first kriging model
covarianceModel = ot.SquaredExponential([2.50057] * dim, [0.1])
basis = ot.ConstantBasisFactory(dim).build()
kriging = ot.KrigingAlgorithm(
    inputSample, outputSample, covarianceModel, basis)
kriging.run()

# algo
algo = ot.EfficientGlobalOptimization(problem, kriging.getResult())
# solver = ot.NLopt('GN_ESCH')
# solver = ot.NLopt('GN_MLSL')
algo.setMaximumEvaluationNumber(15)
algo.setMaximumAbsoluteError(1e-10)
algo.setMaximumRelativeError(1e-10)
algo.setMaximumResidualError(1e-10)
algo.setMaximumConstraintError(1e-10)
algo.setMultiStartExperimentSize(
    100)  # number of multistart candidates improvement optim
algo.setMultiStartNumber(
    20)  # number of multistart points for improvement optim
algo.setParameterEstimationPeriod(
    1)  # relearn kriging parameters every X iteration
algo.setImprovementFactor(1.0)  # improvement stopping criterion factor
algo.setCorrelationLengthFactor(
    1.0)  # correlation length stopping criterion factor
algo.run()
result = algo.getResult()

# print('1st pass result=', result)
assert result.getEvaluationNumber(
) > 0 and result.getEvaluationNumber() < 16, 'Too few/much iterations'
# print('iteration=', result.getEvaluationNumber())
# print(result.getInputSample())
# print(result.getOutputSample())

# local refinement
algo2 = ot.NLopt(problem, 'LD_LBFGS')
algo2.setStartingPoint(result.getOptimalPoint())
algo2.run()
result = algo2.getResult()

openturns.testing.assert_almost_equal(
    result.getOptimalPoint(), [0.0] * dim, 1e-7, 1e-5)
openturns.testing.assert_almost_equal(
    result.getOptimalValue(), [0.0], 1e-15, 1e-5)
# ei = algo.getExpectedImprovement()
# print(ei)
print('OK')


# Cobyla out of bound test
ot.RandomGenerator.SetSeed(0)
dim = 4
model = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4'], ['x1*x1+x2^3*x1+x3+x4'])
model = ot.MemoizeFunction(model)
bounds = ot.Interval([-5.0] * dim, [5.0] * dim)
problem = ot.OptimizationProblem()
problem.setObjective(model)
problem.setBounds(bounds)
experiment = ot.Composite([0.0] * dim, [1.0, 2.0, 4.0])
inputSample = experiment.generate()
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([2.0] * dim, [0.1])
basis = ot.ConstantBasisFactory(dim).build()
kriging = ot.KrigingAlgorithm(
    inputSample, outputSample, covarianceModel, basis)
kriging.run()
algo = ot.EfficientGlobalOptimization(problem, kriging.getResult())
algo.setMaximumEvaluationNumber(2)
algo.run()
result = algo.getResult()
