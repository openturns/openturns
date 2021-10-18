import openturns as ot
import unittest
import openturns.testing as ott
import sys

ot.TESTPREAMBLE()


class CheckKarhunenLoeveValidation(unittest.TestCase):
    def test_KarhunenLoeveValidation(self):
        # Create the KL result
        numberOfVertices = 20
        interval = ot.Interval(-1.0, 1.0)
        mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
        covariance = ot.SquaredExponential()
        process = ot.GaussianProcess(covariance, mesh)
        sampleSize = 100
        processSample = process.getSample(sampleSize)
        threshold = 1.0e-7
        algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
        algo.run()
        klresult = algo.getResult()
        # Create validation
        validation = ot.KarhunenLoeveValidation(processSample, klresult)
        # Check residuals
        residualProcessSample = validation.computeResidual()
        assert(type(residualProcessSample) is ot.ProcessSample)
        # Check standard deviation
        residualSigmaField = validation.computeResidualStandardDeviation()
        exact = ot.Sample(numberOfVertices, 1)
        #ott.assert_almost_equal(residualSigmaField, exact)
        # Check mean
        residualMean = validation.computeResidualMean()
        exact = ot.Sample(numberOfVertices, 1)
        #ott.assert_almost_equal(residualMean, exact)
        # Check graph
        graph0 = validation.drawValidation()
        graph1 = residualProcessSample.drawMarginal(0)
        graph2 = residualMean.drawMarginal(0)
        graph3 = residualSigmaField.drawMarginal(0)
        graph4 = validation.drawObservationWeight(0)
        graph5 = validation.drawObservationQuality()
        if 0:
            from openturns.viewer import View
            View(graph0).save('validation1.png')
            View(graph1).save('validation1-residual.png')
            View(graph2).save('validation1-residual-mean.png')
            View(graph3).save('validation1-residual-stddev.png')
            View(graph4).save('validation1-indiv-weight.png')
            View(graph5).save('validation1-indiv-quality.png')

    def test_KarhunenLoeveValidationMultidimensional(self):
        # Create the KL result
        numberOfVertices = 20
        interval = ot.Interval(-1.0, 1.0)
        mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
        outputDimension = 2
        univariateCovariance = ot.SquaredExponential()
        covarianceCollection = [univariateCovariance] * outputDimension
        multivariateCovariance = ot.TensorizedCovarianceModel(
            covarianceCollection)
        process = ot.GaussianProcess(multivariateCovariance, mesh)
        sampleSize = 100
        sampleSize = 10
        processSample = process.getSample(sampleSize)
        threshold = 1.0e-7
        algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
        algo.run()
        klresult = algo.getResult()
        # Create the validation
        validation = ot.KarhunenLoeveValidation(processSample, klresult)
        # Check residuals
        residualProcessSample = validation.computeResidual()
        assert(type(residualProcessSample) is ot.ProcessSample)
        # Check standard deviation
        residualSigmaField = validation.computeResidualStandardDeviation()
        zeroSample = ot.Sample(numberOfVertices, outputDimension)
        ott.assert_almost_equal(residualSigmaField, zeroSample)
        # Check graph
        graph = validation.drawValidation()
        if False:
            from openturns.viewer import View
            View(graph).save('validation2.png')


if __name__ == "__main__":
    unittest.main()
