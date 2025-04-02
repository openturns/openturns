import math as m
import openturns as ot


def _exec(X):
    wab, wcd, t, P, L, rho, g, a, b = X
    wab = wab / 1000
    wcd = wcd / 1000
    t = t / 1000
    L = L / 1000
    P = P * 1000
    try:
        MB = 1 / 3 * (P * L) + (rho * g * wcd * t * (L**a)) / 18
        Masse = rho * t * L * (2 / (3 * m.sin(b)) * wab + wcd)
        return [MB, Masse]
    except OverflowError:
        return [1e30] * 2


prior = ot.JointDistribution([ot.Normal(0.1, 0.01)] * 3)
model = ot.PythonFunction(9, 2, _exec)
model_p = ot.ParametricFunction(model, [6, 7, 8], prior.getMean())

obs_sample = ot.Sample.ImportFromCSVFile("t_Calibration_console.csv", ",")
obs_in = obs_sample.getMarginal([0, 1, 2, 3, 4, 5])
obs_out = obs_sample.getMarginal([10, 11])
err_cov = ot.CovarianceMatrix(2)
for i in range(2):
    err_cov[i, i] = 0.1**2
p0cov = prior.getCovariance()
p0 = prior.getMean()
algos = [ot.GaussianNonLinearCalibration(model_p, obs_in, obs_out, p0, p0cov, err_cov),
         ot.NonLinearLeastSquaresCalibration(model_p, obs_in, obs_out, p0)]
for algo in algos:
    names = []
    if ot.PlatformInfo.HasFeature("cminpack"):
        names.append("CMinpack")
    if ot.PlatformInfo.HasFeature("ceres"):
        names.append("LEVENBERG_MARQUARDT")

    for name in names:
        solver = ot.OptimizationAlgorithm.GetByName(name)
        algo.setOptimizationAlgorithm(solver)
        algo.setBootstrapSize(0)
        algo.run()
        result = algo.getResult()
        theta = result.getParameterMAP()
        print("theta=", theta)

        posterior = result.getParameterPosterior()
        sample = posterior.getSample(100000)
        level = 0.95
        lb = sample.computeQuantilePerComponent((1 - level) / 2)
        ub = sample.computeQuantilePerComponent(1 - (1 - level) / 2)
        ci = ot.Interval(lb, ub)
        print("ci=", ci)
        assert theta in ci
