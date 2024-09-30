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
        DoF = 3.0 + U.getRealization()[0]  # Degrees of Freedom
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
                    mean_inverse[indice], coefficient * Scale[k, j], 0.15, 1.0e-3
                )
                ott.assert_almost_equal(
                    mean[indice], DoF * Scale_wishart[k, j], 0.15, 1.0e-3
                )
                indice += 1


# Instantiate one distribution object
distribution = ot.InverseWishart(ot.CovarianceMatrix(1), 5.0)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Get mean and covariance
print("Mean= ", repr(distribution.getMean()))
print("Covariance= ", repr(distribution.getCovariance()))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

size = 100
for i in range(2):
    msg = ""
    if ot.FittingTest.Kolmogorov(
        distribution.getSample(size), distribution
    ).getBinaryQualityMeasure():
        msg = "accepted"
    else:
        msg = "rejected"
    print("Kolmogorov test for the generator, sample size=", size, " is", msg)
    size *= 10

# Define a point
point = ot.Point(distribution.getDimension(), 9.1)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5
# max = distribution.getB() + distribution.getA()
# min = distribution.getB() - distribution.getA()
# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))

# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", repr(PDFgr))

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", repr(CDFgr))

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
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

mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())

loadTestsFromTestCase(TestInverseWishartMethods)

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.setSkewnessTolerance(1.0)  # converges slowly
checker.setKurtosisTolerance(1.0)  # converges slowly
checker.skipEntropy()  # slow
checker.skipMinimumVolumeLevelSet()  # slow
checker.run()
