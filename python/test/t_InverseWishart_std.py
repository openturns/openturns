#! /usr/bin/env python

import unittest as ut
from math import log, pi

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.TBB.Disable()

# Prevent from using unittest.TestLoader.loadTestsFromTestCase
# so as not to get the test execution duration


def loadTestsFromTestCase(cls):
    """Launch the tests of the test case class"""
    import sys
    from inspect import getmembers

    members = getmembers(cls)
    test_names = []
    is_tearDownClass_there = False
    for member in members:
        member_name = member[0]
        if member_name[0:5] == "test_":
            test_names.append(member_name)
        elif member_name == "setUpClass":
            cls.setUpClass()
        elif member_name == "tearDownClass":
            is_tearDownClass_there = True
    for test_name in test_names:
        test = cls(test_name)
        test.setUp()
        print("Run " + test_name + "... ", end="")
        sys.stdout.flush()
        try:
            test.debug()
            print("SUCCESS")
        except Exception as exception:
            print("FAILURE")
            print(exception)
        test.tearDown()
    if is_tearDownClass_there:
        cls.tearDownClass()


class TestInverseWishartMethods(ut.TestCase):
    """Test case for the class InverseWishart"""

    @classmethod
    def setUpClass(cls):
        # attributes to compare a one-dimensional InverseWishart to the
        # equivalent InverseGamma distribution
        U = ot.Uniform(0.0, 1.0)
        scale = 10.0 * U.getRealization()[0]
        DoF = 10.0 + U.getRealization()[0]  # Degrees of Freedom
        cls.k, cls.beta = 0.5 * DoF, 0.5 * scale
        cls.one_dimensional_inverse_wishart = ot.InverseWishart(
            ot.CovarianceMatrix([[scale]]), DoF
        )
        cls.inverse_gamma = ot.InverseGamma(cls.k, 1.0 / cls.beta)
        # attributes to test a multi-dimensional InverseWishart
        cls.dimension = 5
        cls.DoF = cls.dimension + 3 + U.getRealization()[0]
        cls.L = ot.TriangularMatrix(cls.dimension)
        diagL = ot.Uniform(0.5, 1.0).getSample(cls.dimension)
        cls.determinant = 1.0
        for i in range(cls.dimension):
            cls.determinant *= diagL[i, 0]
            cls.L[i, i] = diagL[i, 0]
            for j in range(i):
                cls.L[i, j] = U.getRealization()[0]
        cls.determinant *= cls.determinant
        cls.Scale = ot.CovarianceMatrix(cls.L * cls.L.transpose())

    def test_computeLogPDF_1D_case(self):
        """Test InverseWishart.computeLogPDF in the one-dimensional case"""
        k, beta = self.k, self.beta

        def logPDF(x):
            if x <= 0.0:
                raise ValueError("math domain error")
            return k * log(beta) - ot.SpecFunc.LogGamma(k) - (k + 1) * log(x) - beta / x

        data = ((self.inverse_gamma.drawPDF()).getDrawable(0)).getData()
        i = 0
        while data[i, 0] <= 0.0:
            i += 1
        for d in data[i:, 0]:
            x = d[0]
            logPDFx = logPDF(x)
            logPDFx_IW = self.one_dimensional_inverse_wishart.computeLogPDF(x)
            logPDFx_IG = self.inverse_gamma.computeLogPDF(x)
            ott.assert_almost_equal(logPDFx_IW, logPDFx)
            ott.assert_almost_equal(logPDFx_IG, logPDFx)
            ott.assert_almost_equal(logPDFx_IW, logPDFx_IG)

    # Not a test
    # The log multi-gamma function appears in the log PDF
    def logmultigamma(self, p, x):
        """Computes the logarithm of the multi-gamma function at x"""
        logmgpx = 0.25 * p * (p - 1) * log(pi)
        for i in range(1, p + 1):
            logmgpx = logmgpx + ot.SpecFunc.LogGamma(x + 0.5 * (1 - i))
        return logmgpx

    # Test InverseWishart.computeLogPDF in the special case of diagonal matrices
    # (scale covariance matrix and point/matrice at which to evaluate the
    # PDF) by comparing the logarithm of the ratio of the InverseWishart PDF
    # by the product of the InverseGamma PDFs
    def test_computeLogPDF_diagonal_case(self):
        """Test InverseWishart.computeLogPDF in the case of diagonal matrices"""
        dimension, DoF = self.dimension, self.DoF
        k = 0.5 * (DoF + dimension - 1)
        diagX = ot.Uniform(0.5, 1.0).getSample(dimension)
        Scale = ot.CovarianceMatrix(dimension)
        X = ot.CovarianceMatrix(dimension)
        for d in range(dimension):
            Scale[d, d], X[d, d] = self.Scale[d, d], diagX[d, 0]
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        logdensity = inverse_wishart.computeLogPDF(X)
        logratio = -self.logmultigamma(
            dimension, 0.5 * DoF
        ) + dimension * ot.SpecFunc.LogGamma(0.5 * (DoF + dimension - 1))
        for d in range(dimension):
            inverse_gamma = ot.InverseGamma(k, 2.0 / Scale[d, d])
            logdensity = logdensity - inverse_gamma.computeLogPDF(diagX[d, 0])
            logratio = logratio + 0.5 * (1 - dimension) * log(0.5 * Scale[d, d])
        ott.assert_almost_equal(logdensity, logratio)

    # Test InverseWishart.computeLogPDF by evaluating the log PDF
    # at the scale covariance matrix
    def test_computeLogPDF(self):
        """Test InverseWishart.computeLogPDF"""
        dimension, DoF = self.dimension, self.DoF
        Scale, determinant = self.Scale, self.determinant
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        logPDFatX = -self.logmultigamma(dimension, 0.5 * DoF) - 0.5 * (
            DoF * dimension * log(2.0) + dimension + (dimension + 1) * log(determinant)
        )
        ott.assert_almost_equal(inverse_wishart.computeLogPDF(Scale), logPDFatX)

    # Compare the empirical expectations of a large matrix sample
    # and of the corresponding inverse matrix sample
    # to the corresponding theoretical expectations
    def test_getSample_getMean(self):
        """Test InverseWishart.getSample and InverseWishart.getMean"""
        d, Scale, DoF, N = self.dimension, self.Scale, self.DoF, int(1e4)
        Identity = ot.CovarianceMatrix(d)
        Scale_wishart = ot.CovarianceMatrix(Scale.solveLinearSystem(Identity))
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        sample_inverse = ot.Sample(N, (d * (d + 1)) // 2)
        sample = ot.Sample(N, (d * (d + 1)) // 2)
        for i in range(N):
            M_inverse = inverse_wishart.getRealizationAsMatrix()
            M = M_inverse.solveLinearSystem(Identity)
            indice = 0
            for j in range(d):
                for k in range(j + 1):
                    sample_inverse[i, indice] = M_inverse[k, j]
                    sample[i, indice] = M[k, j]
                    indice += 1
        mean_inverse = sample_inverse.computeMean()
        mean = sample.computeMean()
        theoretical_mean_inverse = inverse_wishart.getMean()
        theoretical_mean = (ot.Wishart(Scale_wishart, DoF)).getMean()
        indice, coefficient = 0, 1.0 / (DoF - d - 1)
        for j in range(d):
            for k in range(j + 1):
                ott.assert_almost_equal(
                    theoretical_mean_inverse[indice], coefficient * Scale[k, j]
                )
                ott.assert_almost_equal(
                    theoretical_mean[indice], DoF * Scale_wishart[k, j]
                )
                ott.assert_almost_equal(
                    mean_inverse[indice], coefficient * Scale[k, j], 0.15, 1.0e-2
                )
                ott.assert_almost_equal(
                    mean[indice], DoF * Scale_wishart[k, j], 0.15, 1.0e-2
                )
                indice += 1


# Instantiate one distribution object
distribution = ot.InverseWishart(ot.CovarianceMatrix(1), 15.0)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

loadTestsFromTestCase(TestInverseWishartMethods)

ot.Log.Show(ot.Log.TRACE)
print("distribution", distribution)
ot.RandomGenerator.SetSeed(2)
validation = ott.DistributionValidation(distribution)
validation.skipEntropy()  # slow
validation.skipMinimumVolumeLevelSet()  # slow
validation.run()
# 3D test case
ot.RandomGenerator.SetSeed(2)
distribution = ot.InverseWishart(ot.CovarianceMatrix([[1, 0.5], [0.5, 1]]), 15.0)
validation.skipEntropy()  # slow
validation.skipMinimumVolumeLevelSet()  # slow
validation.run()
