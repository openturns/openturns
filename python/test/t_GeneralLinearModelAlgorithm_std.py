#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Set Numerical precision to 4
ot.PlatformInfo.SetNumericalPrecision(4)
sampleSize = 40
inputDimension = 1

# Create the function to estimate
model = ot.SymbolicFunction(["x0"], ["x0"])

X = ot.Sample(sampleSize, inputDimension)
for i in range(sampleSize):
    X[i, 0] = 3.0 + (8.0 * i) / sampleSize
Y = model(X)

# Add a small noise to data
Y += (
    ot.GaussianProcess(ot.AbsoluteExponential([0.1], [0.2]), ot.Mesh(X))
    .getRealization()
    .getValues()
)

basis = ot.LinearBasisFactory(inputDimension).build()
# Case of a misspecified covariance model
covarianceModel = ot.DiracCovarianceModel(inputDimension)
print("===================================================\n")
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
algo.run()

result = algo.getResult()
print("\ncovariance (dirac, optimized)=", result.getCovarianceModel())
print("trend (dirac, optimized)=", result.getTrendCoefficients())
print("===================================================\n")

# Now without estimating covariance parameters
basis = ot.LinearBasisFactory(inputDimension).build()
covarianceModel = ot.DiracCovarianceModel(inputDimension)
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, True)
algo.setOptimizeParameters(False)
algo.run()
result = algo.getResult()
print("\ncovariance (dirac, not optimized)=", result.getCovarianceModel())
print("trend (dirac, not optimized)=", result.getTrendCoefficients())
print("===================================================\n")
# Now without estimating covariance parameters
basis = ot.LinearBasisFactory(inputDimension).build()
covarianceModel = ot.DiracCovarianceModel(inputDimension)
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, True)
algo.setOptimizeParameters(False)
algo.run()
result = algo.getResult()
print("\ncovariance (dirac, not optimized)=", result.getCovarianceModel())
print("trend (dirac, not optimized)=", result.getTrendCoefficients())
print("===================================================\n")

# Case of a well specified covariance model
# Test the optimization when the amplitude is deduced analytically from
# the scale
covarianceModel = ot.AbsoluteExponential(inputDimension)
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
algo.run()
result = algo.getResult()
print("\ncovariance (reduced, unbiased)=", result.getCovarianceModel())
print("trend (reduced, unbiased)=", result.getTrendCoefficients())
print("===================================================\n")
ot.ResourceMap.SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", False)
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
algo.run()
result = algo.getResult()
print("\ncovariance (reduced, biased)=", result.getCovarianceModel())
print("trend (reduced, biased)=", result.getTrendCoefficients())
print("===================================================\n")
ot.ResourceMap.SetAsBool(
    "GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", False
)
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis)
# Define interval
bounds = ot.Interval([1e-2] * 2, [100] * 2)
algo.setOptimizationBounds(bounds)
algo.run()
result = algo.getResult()
print("\ncovariance (full optim)=", result.getCovarianceModel())
print("trend (full optim)=", result.getTrendCoefficients())
print("===================================================\n")

# --- Coverage: 3-arg ctor, validation, getters, noise paths ---
ot.RandomGenerator.SetSeed(0)
X_cov = ot.Sample(20, 1)
for i in range(20):
    X_cov[i, 0] = 1.0 + i * 0.2
Y_cov = ot.Sample(20, 1)
for i in range(20):
    Y_cov[i, 0] = 2.0 * X_cov[i, 0] + 1.0
cov_cov = ot.SquaredExponential(1)
algo_cov = ot.GeneralLinearModelAlgorithm(X_cov, Y_cov, cov_cov)
algo_cov.run()
assert algo_cov.getResult().getCovarianceModel().getInputDimension() == 1
with ott.assert_raises(Exception):
    ot.GeneralLinearModelAlgorithm(
        X_cov, Y_cov, ot.SquaredExponential(2), ot.LinearBasisFactory(1).build()
    )
oldScaleFactor_cov = ot.ResourceMap.GetAsScalar(
    "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor"
)
ot.ResourceMap.SetAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", 0.0)
try:
    with ott.assert_raises(Exception):
        ot.GeneralLinearModelAlgorithm(X_cov, Y_cov, cov_cov, ot.LinearBasisFactory(1).build())
finally:
    ot.ResourceMap.SetAsScalar(
        "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", oldScaleFactor_cov
    )
Xc_cov = ot.Sample(10, ot.Point([5.0]))
Yc_cov = ot.Sample(10, ot.Point([1.0]))
ac_cov = ot.GeneralLinearModelAlgorithm(Xc_cov, Yc_cov, cov_cov, ot.LinearBasisFactory(1).build())
ac_cov.run()
with ott.assert_raises(Exception):
    ot.GeneralLinearModelAlgorithm(X_cov, Y_cov, cov_cov, ot.Basis([ot.SymbolicFunction(["x"], ["x", "x"])]))
algo2_cov = ot.GeneralLinearModelAlgorithm(X_cov, Y_cov, cov_cov, ot.LinearBasisFactory(1).build())
assert algo2_cov.getOptimizationAlgorithm().getClassName() != ""
assert algo2_cov.getOptimizationBounds().getDimension() >= 1
assert algo2_cov.getOptimizeParameters()
assert algo2_cov.getNoise().getSize() == 0
assert algo2_cov.getObjectiveFunction().getInputDimension() >= 1
with ott.assert_raises(Exception):
    algo2_cov.getObjectiveFunction()(ot.Point([0.1, 0.2, 0.3]))
with ott.assert_raises(Exception):
    algo2_cov.setNoise([0.1])
with ott.assert_raises(Exception):
    algo2_cov.setNoise([-1.0] * 20)
algo2_cov.setNoise([1e-6] * 20)
algo2_cov.run()
assert len(algo2_cov.getNoise()) == 20
if ot.PlatformInfo.HasFeature("hmat"):
    oldLinearAlgebra_cov = ot.ResourceMap.GetAsString("GeneralLinearModelAlgorithm-LinearAlgebra")
    ot.ResourceMap.SetAsString("GeneralLinearModelAlgorithm-LinearAlgebra", "HMAT")
    try:
        with ott.assert_raises(Exception):
            ah_cov = ot.GeneralLinearModelAlgorithm(X_cov, Y_cov, cov_cov, ot.LinearBasisFactory(1).build())
            ah_cov.setNoise([1e-6] * 20)
    finally:
        ot.ResourceMap.SetAsString("GeneralLinearModelAlgorithm-LinearAlgebra", oldLinearAlgebra_cov)
str(algo2_cov)
X5_cov = ot.Sample(5, 1)
for i in range(5):
    X5_cov[i, 0] = float(i)
Y5_cov = X5_cov * 2.0
with ott.assert_raises(Exception):
    ot.GeneralLinearModelAlgorithm(X5_cov, Y5_cov, cov_cov, ot.LinearBasisFactory(1).build()).run()
