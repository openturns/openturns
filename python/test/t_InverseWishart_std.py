#! /usr/bin/env python

from __future__ import print_function
import unittest as ut
from math import pi, log, log10, floor

import openturns as ot
from openturns.testing import assert_almost_equal # (value, reference, rtol, atol)

ot.TESTPREAMBLE()

# Prevent from using unittest.TestLoader.loadTestsFromTestCase
# so as not to get the test execution duration
def loadTestsFromTestCase(cls):
    """Launch the tests of the test case class"""
    import sys
    from inspect import getmembers
    cls.setUpClass()
    for member in getmembers(TestInverseWishartMethods):
        member_name = member[0]
        if member_name[0:5] == 'test_':
            test = cls(member_name)
            test.setUp()
            print("Run " + member_name + "... ", end="")
            sys.stdout.flush()
            try:
                test.debug()
                print("SUCCESS")
            except  Exception, exception:
                print("FAILURE")
                print(exception)
            test.tearDown()
    cls.tearDownClass()

class TestInverseWishartMethods(ut.TestCase):
    """Test case for the class InverseWishart"""

    @classmethod
    def setUpClass(cls):
        # attributes to compare a one-dimensional InverseWishart to the
        # equivalent InverseGamma distribution
        U = ot.Uniform(0., 1.)
        scale = 10.*U.getRealization()[0]
        DoF = 3.+ U.getRealization()[0] # Degrees of Freedom
        cls.k, cls.beta = 0.5*DoF, 0.5*scale
        cls.one_dimensional_inverse_wishart \
            = ot.InverseWishart(ot.CovarianceMatrix([[scale]]), DoF)
        cls.inverse_gamma = ot.InverseGamma(cls.k, 1./cls.beta)
        # attributes to test a multi-dimensional InverseWishart
        cls.dimension = 5
        cls.DoF = cls.dimension + 3 + U.getRealization()[0]
        cls.L = ot.TriangularMatrix(cls.dimension)
        diagL = ot.Uniform(0.5, 1.).getSample(cls.dimension)
        cls.determinant = 1.
        for i in range(cls.dimension):
            cls.determinant *= diagL[i, 0]
            cls.L[i, i] = diagL[i, 0]
            for j in range(i):
                cls.L[i, j] = U.getRealization()[0]
        cls.determinant *= cls.determinant
        cls.Scale = ot.CovarianceMatrix(cls.L*cls.L.transpose())

    def test_computeLogPDF_1D_case(self):
        """Test InverseWishart.computeLogPDF in the one-dimensional case"""
        k, beta = self.k, self.beta
        def logPDF(x):
            if x <= 0.:
                raise ValueError("math domain error")
            return k*log(beta) - ot.SpecFunc_LogGamma(k) - (k+1)*log(x) - beta/x
        data = ((self.inverse_gamma.drawPDF()).getDrawable(0)).getData()
        i = 0
        while data[i, 0] <= 0.:
            i += 1
        for d in data[i:, 0]:
            x = d[0]
            logPDFx = logPDF(x)
            logPDFx_IW = self.one_dimensional_inverse_wishart.computeLogPDF(x)
            logPDFx_IG = self.inverse_gamma.computeLogPDF(x)
            assert_almost_equal(logPDFx_IW, logPDFx)
            assert_almost_equal(logPDFx_IG, logPDFx)
            assert_almost_equal(logPDFx_IW, logPDFx_IG)

    # Not a test
    # The log multi-gamma function appears in the log PDF
    def logmultigamma(self, p, x):
        """Computes the logarithm of the multi-gamma function at x"""
        logmgpx = 0.25*p*(p-1)*log(pi)
        for i in range(1, p+1):
            logmgpx = logmgpx + ot.SpecFunc_LnGamma(x + 0.5*(1-i))
        return logmgpx
            
    # Test InverseWishart.computeLogPDF in the special case of diagonal matrices
    # (scale covariance matrix and point/matrice at which to evaluate the
    # PDF) by comparing the logarithm of the ratio of the InverseWishart PDF
    # by the product of the InverseGamma PDFs
    def test_computeLogPDF_diagonal_case(self):
        """Test InverseWishart.computeLogPDF in the case of diagonal matrices"""
        dimension, DoF = self.dimension, self.DoF
        k = 0.5*(DoF + dimension - 1)
        diagX = ot.Uniform(0.5, 1.).getSample(dimension)
        Scale = ot.CovarianceMatrix(dimension)
        X = ot.CovarianceMatrix(dimension)
        for d in range(dimension):
            Scale[d, d], X[d, d] = self.Scale[d, d], diagX[d, 0]
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        logdensity = inverse_wishart.computeLogPDF(X)
        logratio = - self.logmultigamma(dimension, 0.5*DoF) \
                 + dimension*ot.SpecFunc_LnGamma(0.5*(DoF+dimension-1))
        for d in range(dimension):
            inverse_gamma = ot.InverseGamma(k, 2./Scale[d, d])
            logdensity = logdensity - inverse_gamma.computeLogPDF(diagX[d, 0])
            logratio = logratio + 0.5*(1-dimension)*log(0.5*Scale[d, d])
        assert_almost_equal(logdensity, logratio)

    # Test InverseWishart.computeLogPDF by evaluating the log PDF
    # at the scale covariance matrix
    def test_computeLogPDF(self):
        """Test InverseWishart.computeLogPDF"""
        dimension, DoF = self.dimension, self.DoF
        Scale, determinant = self.Scale, self.determinant
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        logPDFatX = - self.logmultigamma(dimension, 0.5*DoF) \
                  - 0.5*(DoF*dimension*log(2.) + dimension \
                        + (dimension + 1)*log(determinant))
        assert_almost_equal(inverse_wishart.computeLogPDF(Scale), logPDFatX)

    # Compare the empirical expectations of a large matrix sample
    # and of the corresponding inverse matrix sample
    # to the corresponding theoretical expectations
    def test_getSample_getMean(self):
        """Test InverseWishart.getSample and InverseWishart.getMean"""
	d, Scale, DoF, N = self.dimension, self.Scale, self.DoF, int(1E+5)
        Identity = ot.CovarianceMatrix(d)
        Scale_wishart = ot.CovarianceMatrix(Scale.solveLinearSystem(Identity))
        inverse_wishart = ot.InverseWishart(Scale, DoF)
        sample_inverse = ot.NumericalSample(N, (d*(d+1))/2)
        sample = ot.NumericalSample(N, (d*(d+1))/2)
        for i in range(N):
            M_inverse = inverse_wishart.getRealizationAsMatrix()
            M = M_inverse.solveLinearSystem(Identity)
            indice = 0
            for j in range(d):
                for k in range(j+1):
                    sample_inverse[i, indice] = M_inverse[k, j]
                    sample[i, indice] = M[k, j]
                    indice += 1
        mean_inverse = sample_inverse.computeMean()
        mean = sample.computeMean()
        theoretical_mean_inverse = inverse_wishart.getMean()
        theoretical_mean = (ot.Wishart(Scale_wishart, DoF)).getMean()
        indice, coefficient = 0, 1./(DoF - d - 1)
        for j in range(d):
            for k in range(j+1):
                assert_almost_equal(theoretical_mean_inverse[indice],
                                    coefficient*Scale[k, j])
                assert_almost_equal(theoretical_mean[indice],
                                    DoF*Scale_wishart[k, j])
                assert_almost_equal(mean_inverse[indice],
                                    coefficient*Scale[k, j], 0.1, 1.E-3)
                assert_almost_equal(mean[indice],
                                    DoF*Scale_wishart[k, j], 0.1, 1.E-3)
                indice += 1

if __name__ == '__main__':
    ot.RandomGenerator.SetSeed(0)
    try:
        loadTestsFromTestCase(TestInverseWishartMethods)
    except:
        import sys
        print("InverseWishart.py", sys.exc_info()[0], sys.exc_info()[1])

