#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing

# branin
dim = 2
formula = "((x2-(5.1/(4*pi_^2))*x1^2+5*x1/pi_-6)^2+10*(1-1/8*pi_)*cos(x1)+10-54.8104)/51.9496"
branin = ot.SymbolicFunction(["x1", "x2"], [formula])
transfo = ot.SymbolicFunction(["u1", "u2"], ["15*u1-5", "15*u2"])
model = ot.ComposedFunction(branin, transfo)

# problem
problem = ot.OptimizationProblem()
problem.setObjective(model)
bounds = ot.Interval([0.0] * dim, [1.0] * dim)
problem.setBounds(bounds)

# design
experiment = ot.Box([1, 1])
inputSample = experiment.generate()
outputSample = model(inputSample)

# with kriging
covarianceModel = ot.SquaredExponential([0.3, 0.25], [1.0])
covarianceModel.setNuggetFactor(0.96)  # assume constant noise var
basis = ot.ConstantBasisFactory(dim).build()
kriging = ot.KrigingAlgorithm(inputSample, outputSample, covarianceModel, basis)
kriging.run()

# EGO
algo = otexp.EfficientGlobalOptimization(problem, kriging.getResult())
algo.setMaximumCallsNumber(14)
algo.setAEITradeoff(0.66)
algo.run()
result = algo.getResult()
print("iteration=", result.getIterationNumber())
iters = result.getIterationNumber()
assert iters > 3 and iters < 15, "Too few/much iterations"
print(result.getInputSample())
print(result.getOutputSample())
print("x*=", result.getOptimalPoint(), "y*=", result.getOptimalValue())
openturns.testing.assert_almost_equal(result.getOptimalValue(), [-0.967], 1e-2, 1e-2)

# local refinement despite noise (we still want to check we're not too far from optimum)
problem.setObjective(model.getMarginal(0))
algo2 = ot.TNC(problem)
algo2.setStartingPoint(result.getOptimalPoint())
algo2.run()
result = algo2.getResult()
openturns.testing.assert_almost_equal(
    result.getOptimalPoint(), [0.961652, 0.165000], 1e-5, 1e-5
)
openturns.testing.assert_almost_equal(result.getOptimalValue(), [-0.979476], 1e-5, 1e-5)


# with GPR
covarianceModel = ot.SquaredExponential([2.50057] * dim, [0.1])
basis = ot.ConstantBasisFactory(dim).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()

# EGO
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(15)
algo.setMultiStartExperimentSize(100)  # n multistart candidates
algo.setMultiStartNumber(20)  # number of multistart points for improvement optim
algo.setParameterEstimationPeriod(1)  # relearn kriging parameters every X iteration
algo.setCorrelationLengthFactor(1.0)  # correlation length stopping criterion factor
algo.run()
result = algo.getResult()

print("x*=", result.getOptimalPoint(), "y*=", result.getOptimalValue())
openturns.testing.assert_almost_equal(result.getOptimalValue(), [-0.979476], 1e-4, 1e-4)

print("iteration=", result.getIterationNumber())
iters = result.getIterationNumber()
assert iters > 3 and iters < 16, "Too few/much iterations"
print(result.getInputSample())
print(result.getOutputSample())

# local refinement
algo2 = ot.TNC(problem)
algo2.setStartingPoint(result.getOptimalPoint())
algo2.run()
result = algo2.getResult()
print("x*=", result.getOptimalPoint(), "y*=", result.getOptimalValue())

# openturns.testing.assert_almost_equal(result.getOptimalPoint(), [0.0] * dim, 1e-7, 1e-5)
openturns.testing.assert_almost_equal(result.getOptimalValue(), [-0.979476], 1e-4, 1e-4)
ei = algo.getExpectedImprovement()
print("ei=", ei)


# Cobyla out of bound test
ot.RandomGenerator.SetSeed(0)
dim = 4
model = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1*x1+x2^3*x1+x3+x4"])
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
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(10)
algo.run()
result = algo.getResult()

# check maximization
problem.setMinimization(False)
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(10)
algo.run()
result = algo.getResult()
assert result.getOptimalValue()[0] >= 650.0

print("OK")
