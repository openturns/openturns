//                                               -*- C++ -*-
/**
 *  @brief The test file of class Student for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

class TestObject : public Student
{
public:
  TestObject() : Student(5.0, -0.5, 2.0) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    Collection< Student > allDistributions;
    allDistributions.add(Student(6.5, -0.5, 5.0));
    {
      const UnsignedInteger dim = 2;
      CorrelationMatrix R(dim);
      Point mu(dim);
      Point sigma(dim);
      for (UnsignedInteger i = 0; i < dim; ++i)
      {
        mu[i] = i;
        sigma[i] = (1.0 + i) / dim;
        for (UnsignedInteger j = 0; j < i; ++j)
          R(i, j) = 1.0 / (1.0 + dim + i + j);
      }
      allDistributions.add(Student(4.5, mu, sigma, R));
    }
    for (UnsignedInteger iDist = 0; iDist < allDistributions.getSize(); ++iDist)
    {
      Student distribution(allDistributions[iDist]);
      const UnsignedInteger dim = distribution.getDimension();
      // Instantiate one distribution object
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      Point oneRealization = distribution.getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      Sample oneSample = distribution.getSample( size );
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
      if (distribution.getDimension() == 1)
      {
        size = 100;
        for (UnsignedInteger i = 0; i < 2; ++i)
        {
          fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
          size *= 10;
        }
      }
      // Define a point
      Point point( dim, 1.0 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      Scalar eps = 1e-5;
      Point DDF = distribution.computeDDF( point );
      fullprint << "ddf     =" << DDF << std::endl;
      if (dim == 1)
        fullprint << "ddf (FD)=" << Point(1, (distribution.computePDF( point + Point(1, eps) ) - distribution.computePDF( point  + Point(1, -eps) )) / (2.0 * eps)) << std::endl;
      Scalar LPDF = distribution.computeLogPDF( point );
      fullprint << "log pdf=" << LPDF << std::endl;
      Scalar PDF = distribution.computePDF( point );
      fullprint << "pdf     =" << PDF << std::endl;
      if (dim == 1)
        fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(1, eps) ) - distribution.computeCDF( point  + Point(1, -eps) )) / (2.0 * eps) << std::endl;
      Scalar CDF = distribution.computeCDF( point );
      fullprint << "cdf=" << CDF << std::endl;
      Scalar CCDF = distribution.computeComplementaryCDF( point );
      fullprint << "ccdf=" << CCDF << std::endl;
      Scalar Survival = distribution.computeSurvivalFunction( point );
      fullprint << "survival=" << Survival << std::endl;
      Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
      fullprint << "Inverse survival=" << InverseSurvival << std::endl;
      fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
      if (dim == 1)
      {
        Point PDFgr = distribution.computePDFGradient( point );
        fullprint << "pdf gradient     =" << PDFgr << std::endl;
        Point PDFgrFD(3);
        PDFgrFD[0] = (Student(distribution.getNu() + eps, distribution.getMu(), distribution.getSigma(), distribution.getR()).computePDF(point) -
                      Student(distribution.getNu() - eps, distribution.getMu(), distribution.getSigma(), distribution.getR()).computePDF(point)) / (2.0 * eps);
        PDFgrFD[1] = (Student(distribution.getNu(), distribution.getMu() + Point(1, eps), distribution.getSigma(), distribution.getR()).computePDF(point) -
                      Student(distribution.getNu(), distribution.getMu() - Point(1, eps), distribution.getSigma(), distribution.getR()).computePDF(point)) / (2.0 * eps);
        PDFgrFD[2] = (Student(distribution.getNu(), distribution.getMu(), distribution.getSigma() + Point(1, eps), distribution.getR()).computePDF(point) -
                      Student(distribution.getNu(), distribution.getMu(), distribution.getSigma() - Point(1, eps), distribution.getR()).computePDF(point)) / (2.0 * eps);
        fullprint << "pdf gradient (FD)=" << PDFgrFD << std::endl;
        Point CDFgr = distribution.computeCDFGradient( point );
        fullprint << "cdf gradient     =" << CDFgr << std::endl;
        Point CDFgrFD(3);
        CDFgrFD[0] = (Student(distribution.getNu() + eps, distribution.getMu(), distribution.getSigma(), distribution.getR()).computeCDF(point) -
                      Student(distribution.getNu() - eps, distribution.getMu(), distribution.getSigma(), distribution.getR()).computeCDF(point)) / (2.0 * eps);
        CDFgrFD[1] = (Student(distribution.getNu(), distribution.getMu() + Point(1, eps), distribution.getSigma(), distribution.getR()).computeCDF(point) -
                      Student(distribution.getNu(), distribution.getMu() - Point(1, eps), distribution.getSigma(), distribution.getR()).computeCDF(point)) / (2.0 * eps);
        CDFgrFD[2] = (Student(distribution.getNu(), distribution.getMu(), distribution.getSigma() + Point(1, eps), distribution.getR()).computeCDF(point) -
                      Student(distribution.getNu(), distribution.getMu(), distribution.getSigma() - Point(1, eps), distribution.getR()).computeCDF(point)) / (2.0 * eps);
        fullprint << "cdf gradient (FD)=" << CDFgrFD << std::endl;
      }
      Point quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile=" << quantile << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      // Confidence regions
      Scalar threshold;
      fullprint << "Minimum volume interval=" << distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold) << std::endl;
      fullprint << "threshold=" << threshold << std::endl;
      Scalar beta;
      LevelSet levelSet(distribution.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
      fullprint << "Minimum volume level set=" << levelSet << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Bilateral confidence interval=" << distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (lower tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (upper tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
      fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
      Point mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      Point standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      Point skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      Point kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;
      CovarianceMatrix correlation = distribution.getR();
      fullprint << "correlation=" << correlation << std::endl;
      // Useless if dim==1 and too costly for dim>1
      // CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
      // fullprint << "spearman=" << spearman << std::endl;
      CovarianceMatrix kendall = distribution.getKendallTau();
      fullprint << "kendall=" << kendall << std::endl;
      Student::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      distribution.setParametersCollection(parameters);
      fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

      // Specific to this distribution
      beta = point.normSquare();
      Scalar densityGenerator = distribution.computeDensityGenerator(beta);
      fullprint << "density generator=" << densityGenerator << std::endl;
      fullprint << "pdf via density generator=" << distribution.EllipticalDistribution::computePDF(point) << std::endl;
      Scalar densityGeneratorDerivative = distribution.computeDensityGeneratorDerivative(beta);
      fullprint << "density generator derivative     =" << densityGeneratorDerivative << std::endl;
      fullprint << "density generator derivative (FD)=" << (distribution.computeDensityGenerator(beta + eps) - distribution.computeDensityGenerator(beta - eps)) / (2.0 * eps) << std::endl;
      Scalar densityGeneratorSecondDerivative = distribution.computeDensityGeneratorSecondDerivative(beta);
      fullprint << "density generator second derivative     =" << densityGeneratorSecondDerivative << std::endl;
      fullprint << "density generator second derivative (FD)=" << (distribution.computeDensityGeneratorDerivative(beta + eps) - distribution.computeDensityGeneratorDerivative(beta - eps)) / (2.0 * eps) << std::endl;
      Scalar x = 0.6;
      Point y(dim - 1, 0.2);
      fullprint << "conditional PDF=" << distribution.computeConditionalPDF(x, y) << std::endl;
      fullprint << "conditional CDF=" << distribution.computeConditionalCDF(x, y) << std::endl;
      fullprint << "conditional quantile=" << distribution.computeConditionalQuantile(x, y) << std::endl;
      Point pt(dim);
      for (UnsignedInteger i = 0; i < dim; ++i) pt[i] = 1.0 * i + 1.5;
      fullprint << "sequential conditional PDF=" << distribution.computeSequentialConditionalPDF(pt) << std::endl;
      Point resCDF(distribution.computeSequentialConditionalCDF(pt));
      fullprint << "sequential conditional CDF(" << pt << ")=" << resCDF << std::endl;
      fullprint << "sequential conditional quantile(" << resCDF << ")=" << distribution.computeSequentialConditionalQuantile(resCDF) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
