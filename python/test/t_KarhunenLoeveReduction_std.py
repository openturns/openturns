import openturns as ot
import unittest
import openturns.testing as ott


class CheckKarhunenLoeveReduce(unittest.TestCase):
    def test_ZeroMean(self):
        # Create the KL result
        numberOfVertices = 10
        interval = ot.Interval(-1.0, 1.0)
        mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
        covariance = ot.SquaredExponential()
        process = ot.GaussianProcess(covariance, mesh)
        sampleSize = 10
        processSample = process.getSample(sampleSize)
        threshold = 0.0
        algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
        algo.run()
        klresult = algo.getResult()
        # Create the KL reduction
        processSample.computeMean()
        klreduce = ot.KarhunenLoeveReduction(klresult)
        # Generate a trajectory and reduce it
        field = process.getRealization()
        values = field.getValues()
        reducedValues = klreduce(values)
        ott.assert_almost_equal(values, reducedValues)

    def test_NonZeroMean(self):
        # Create the KL result
        numberOfVertices = 10
        interval = ot.Interval(-1.0, 1.0)
        mesh = ot.IntervalMesher([numberOfVertices - 1]).build(interval)
        covariance = ot.SquaredExponential()
        zeroProcess = ot.GaussianProcess(covariance, mesh)
        # Define a trend function
        f = ot.SymbolicFunction(["t"], ["30 * t"])
        fTrend = ot.TrendTransform(f, mesh)
        # Add it to the process
        process = ot.CompositeProcess(fTrend, zeroProcess)
        # Sample
        sampleSize = 100
        processSample = process.getSample(sampleSize)
        threshold = 0.0
        algo = ot.KarhunenLoeveSVDAlgorithm(processSample, threshold)
        algo.run()
        klresult = algo.getResult()
        # Create the KL reduction
        processSample.computeMean()
        klreduce = ot.KarhunenLoeveReduction(klresult)
        # Generate a trajectory and reduce it
        field = process.getRealization()
        values = field.getValues()
        reducedValues = klreduce(values)
        ott.assert_almost_equal(values, reducedValues)

    def test_trend(self):
        N = 100
        M = 1000
        P = 10
        mean = ot.SymbolicFunction("x", "sign(x)")
        cov = ot.SquaredExponential([1.0], [0.1])
        mesh = ot.IntervalMesher([N]).build(ot.Interval(-2.0, 2.0))
        process = ot.GaussianProcess(ot.TrendTransform(mean, mesh), cov, mesh)
        sample = process.getSample(M)
        algo = ot.KarhunenLoeveSVDAlgorithm(sample, 1e-6)
        algo.run()
        result = algo.getResult()
        trend = ot.TrendTransform(ot.P1LagrangeEvaluation(sample.computeMean()), mesh)
        sample2 = process.getSample(P)
        sample2.setName("reduction of sign(x) w/o trend")
        reduced1 = ot.KarhunenLoeveReduction(result)(sample2)
        reduced2 = ot.KarhunenLoeveReduction(result, trend)(sample2)
        g = sample2.drawMarginal(0)
        g.setColors(["red"])
        g1 = reduced1.drawMarginal(0)
        g1.setColors(["blue"])
        drs = g1.getDrawables()
        for i, d in enumerate(drs):
            d.setLineStyle("dashed")
            drs[i] = d
        g1.setDrawables(drs)
        g.add(g1)
        g2 = reduced2.drawMarginal(0)
        g2.setColors(["green"])
        drs = g2.getDrawables()
        for i, d in enumerate(drs):
            d.setLineStyle("dotted")
            drs[i] = d
        g2.setDrawables(drs)
        g.add(g2)
        if 0:
            from openturns.viewer import View

            View(g).save("reduction.png")


if __name__ == "__main__":
    unittest.main()
