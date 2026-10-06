import openturns as ot
import openturns.testing as ottest

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
ot.ResourceMap.SetAsUnsignedInteger("HODLRMatrix-MinLeafSize", 4)
ot.ResourceMap.SetAsScalar("HODLRMatrix-AssemblyEpsilon", 1.0e-6)
ot.ResourceMap.SetAsScalar("HODLRMatrix-RecompressionEpsilon", 1.0e-6)


def test_simple():
    """1D GP with HODLR: compare against LAPACK reference."""
    sampleSize = 10
    dimension = 1

    f = ot.SymbolicFunction(["x0"], ["x0 * sin(x0)"])

    X = ot.Sample(sampleSize, dimension)
    for i in range(sampleSize):
        X[i, 0] = -2.0 + 4.0 * i / (sampleSize - 1)
    Y = f(X)

    basis = ot.ConstantBasisFactory(dimension).build()
    covarianceModel = ot.SquaredExponential([1.0], [1.0])

    # LAPACK reference
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    fit_lapack = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    fit_lapack.setOptimizeParameters(False)
    fit_lapack.run()
    gpr_lapack = ot.GaussianProcessRegression(fit_lapack.getResult())
    gpr_lapack.run()
    Yhat_ref = gpr_lapack.getResult().getMetaModel()(X)

    # HODLR
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    fit_hodlr = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    fit_hodlr.setOptimizeParameters(False)
    fit_hodlr.run()
    gpr_hodlr = ot.GaussianProcessRegression(fit_hodlr.getResult())
    gpr_hodlr.run()
    Yhat_hodlr = gpr_hodlr.getResult().getMetaModel()(X)

    # HODLR should match LAPACK closely for interpolation
    ottest.assert_almost_equal(Yhat_hodlr, Yhat_ref, 1.0e-4, 1.0e-4)
    print("test_simple PASSED")


def test_two_dimensional():
    """2D GP with HODLR: interpolation and prediction."""
    inputDimension = 2
    levels = [4, 3]
    box = ot.Box(levels)
    inputSample = box.generate()
    inputSample *= 10.0

    model = ot.SymbolicFunction(["x", "y"], ["cos(0.5*x) + sin(y)"])
    outputSample = model(inputSample)

    sampleSize = 5
    inputValidSample = ot.JointDistribution(2 * [ot.Uniform(0, 10.0)]).getSample(sampleSize)

    covarianceModel = ot.SquaredExponential([5.0, 3.0], [1.0])
    basis = ot.ConstantBasisFactory(inputDimension).build()

    # LAPACK reference
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    fit_lapack = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
    fit_lapack.setOptimizeParameters(False)
    fit_lapack.run()
    gpr_lapack = ot.GaussianProcessRegression(fit_lapack.getResult())
    gpr_lapack.run()
    Yhat_ref = gpr_lapack.getResult().getMetaModel()
    pred_ref = Yhat_ref(inputValidSample)

    # HODLR
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    fit_hodlr = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
    fit_hodlr.setOptimizeParameters(False)
    fit_hodlr.run()
    gpr_hodlr = ot.GaussianProcessRegression(fit_hodlr.getResult())
    gpr_hodlr.run()
    Yhat_hodlr = gpr_hodlr.getResult().getMetaModel()
    pred_hodlr = Yhat_hodlr(inputValidSample)

    # HODLR predictions should match LAPACK
    ottest.assert_almost_equal(pred_hodlr, pred_ref, 0.1, 0.1)
    print("test_two_dimensional PASSED")


def test_conditional_covariance():
    """Conditional covariance/mean must work with HODLR results.

    Regression test: solveTriangularSystem used to branch on the HMAT factor
    for any non-LAPACK result, crashing with 'Empty HMatrix' for HODLR.
    """
    sampleSize = 6
    dimension = 1

    X = ot.Sample(sampleSize, dimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + i
    X[0, 0] = 1.0
    f = ot.SymbolicFunction(["x0"], ["x0 * sin(x0)"])
    Y = f(X)

    basis = ot.ConstantBasisFactory(dimension).build()
    covarianceModel = ot.SquaredExponential([1.0], [1.0])

    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    fit = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    fit.setOptimizeParameters(False)
    fit.run()
    gpr = ot.GaussianProcessRegression(fit.getResult())
    gpr.run()
    gccc = ot.GaussianProcessConditionalCovariance(gpr.getResult())

    mean = gccc.getConditionalMean(X)
    ottest.assert_almost_equal(mean, Y, 1.0e-4, 1.0e-4)

    covariance = gccc.getConditionalCovariance(X)
    nullMatrix = ot.Matrix(sampleSize, sampleSize)
    ottest.assert_almost_equal(covariance, nullMatrix, 1.0e-5, 1.0e-5)

    var = gccc.getConditionalMarginalVariance(X)
    ottest.assert_almost_equal(var, ot.Sample(sampleSize, 1), 1.0e-5, 1.0e-5)

    # Compare against the LAPACK reference on validation points
    X2 = ot.Sample([[1.5], [3.5], [5.5]])
    covHodlr = gccc.getConditionalCovariance(X2)

    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "LAPACK")
    fitLapack = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    fitLapack.setOptimizeParameters(False)
    fitLapack.run()
    gprLapack = ot.GaussianProcessRegression(fitLapack.getResult())
    gprLapack.run()
    gcccLapack = ot.GaussianProcessConditionalCovariance(gprLapack.getResult())
    covLapack = gcccLapack.getConditionalCovariance(X2)

    ottest.assert_almost_equal(covHodlr, covLapack, 1.0e-5, 1.0e-5)
    print("test_conditional_covariance PASSED")


def test_save_load():
    """A saved HODLR fitter result must be usable after reload.

    Regression test: the HMAT/HODLR Cholesky factors are not serialized
    (hmat-oss native structures), so load() used to leave them empty and any
    downstream solve crashed with 'HODLRMatrix not assembled'. load() now
    rebuilds the factor from the stored covariance model and input sample.
    """
    sampleSize = 6
    dimension = 1

    X = ot.Sample(sampleSize, dimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + i
    X[0, 0] = 1.0
    f = ot.SymbolicFunction(["x0"], ["x0 * sin(x0)"])
    Y = f(X)
    X2 = ot.Sample([[1.5], [3.5], [5.5]])

    basis = ot.ConstantBasisFactory(dimension).build()
    covarianceModel = ot.SquaredExponential([1.0], [1.0])

    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HODLR")
    fit = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
    fit.setOptimizeParameters(False)
    fit.setKeepCholeskyFactor(True)
    fit.run()
    result = fit.getResult()

    gpr = ot.GaussianProcessRegression(result)
    gpr.run()
    predOrig = gpr.getResult().getMetaModel()(X2)
    gcccOrig = ot.GaussianProcessConditionalCovariance(gpr.getResult())
    covOrig = gcccOrig.getConditionalCovariance(X2)

    import os
    import tempfile

    studyPath = os.path.join(tempfile.mkdtemp(), "result.xml")
    study = ot.Study(studyPath)
    study.add("result", result)
    study.save()

    loaded = ot.GaussianProcessFitterResult()
    study = ot.Study(studyPath)
    study.load()
    study.fillObject("result", loaded)

    ottest.assert_almost_equal(
        loaded.getHODLRCholeskyFactor().getNbRows(), sampleSize, 0.0, 0.0
    )

    gprReload = ot.GaussianProcessRegression(loaded)
    gprReload.run()
    predReload = gprReload.getResult().getMetaModel()(X2)
    ottest.assert_almost_equal(predReload, predOrig, 1.0e-8, 1.0e-8)

    gcccReload = ot.GaussianProcessConditionalCovariance(gprReload.getResult())
    covReload = gcccReload.getConditionalCovariance(X2)
    ottest.assert_almost_equal(covReload, covOrig, 1.0e-8, 1.0e-8)
    print("test_save_load PASSED")


if __name__ == "__main__":
    test_simple()
    test_two_dimensional()
    test_conditional_covariance()
    test_save_load()
