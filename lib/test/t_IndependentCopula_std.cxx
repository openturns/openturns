//                                               -*- C++ -*-
/**
 *  @brief The test file of class IndependentCopula for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public IndependentCopula
{
public:
  TestObject() : IndependentCopula(3) {}
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
    //    checkClassWithClassName<TestObject>();

    // Instantiate one distribution object
    UnsignedInteger dim = 3;
    IndependentCopula copula(dim);
    fullprint << "Copula " << copula << std::endl;
    std::cout << "Copula " << copula << std::endl;

    // Is this copula elliptical ?
    fullprint << "Elliptical distribution= " << (copula.isElliptical() ? "true" : "false") << std::endl;

    // Is this copula continuous ?
    fullprint << "Continuous = " << (copula.isContinuous() ? "true" : "false") << std::endl;

    // Is this copula elliptical ?
    fullprint << "Elliptical = " << (copula.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Is this copula independant ?
    fullprint << "Independent = " << (copula.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of copula
    Point oneRealization = copula.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample oneSample = copula.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

    // Define a point
    Point point( copula.getDimension(), 0.6 );
    fullprint << "Point= " << point << std::endl;

    // Show DDF, PDF and CDF of point
    Point  pointDDF = copula.computeDDF( point );
    Scalar pointPDF = copula.computePDF( point );
    Scalar pointCDF = copula.computeCDF( point );
    fullprint << "point= " << point
              << " ddf=" << pointDDF
              << " pdf=" << pointPDF
              << " cdf=" << pointCDF
              << std::endl;
    Scalar Survival = copula.computeSurvivalFunction(point);
    fullprint << "Survival      =" << Survival << std::endl;
    fullprint << "Survival (ref)=" << copula.computeSurvivalFunction(point) << std::endl;
    Point InverseSurvival = copula.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << copula.computeSurvivalFunction(InverseSurvival) << std::endl;
    // Get 50% quantile
    Point quantile = copula.computeQuantile( 0.5 );
    fullprint << "Quantile=" << quantile << std::endl;
    fullprint << "CDF(quantile)=" << copula.computeCDF(quantile) << std::endl;

    // Confidence regions
    Scalar threshold;
    fullprint << "Minimum volume interval=" << copula.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold) << std::endl;
    fullprint << "threshold=" << threshold << std::endl;
    Scalar beta;
    LevelSet levelSet(copula.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
    fullprint << "Minimum volume level set=" << levelSet << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Bilateral confidence interval=" << copula.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Unilateral confidence interval (lower tail)=" << copula.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Unilateral confidence interval (upper tail)=" << copula.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;

    Point PDFgr = copula.computePDFGradient( point );
    fullprint << "pdf gradient     =" << PDFgr << std::endl;
    Point CDFgr = copula.computeCDFGradient( point );
    fullprint << "cdf gradient     =" << CDFgr << std::endl;
    Point mean = copula.getMean();
    fullprint << "mean=" << mean << std::endl;
    IndependentCopula::PointWithDescriptionCollection parameters = copula.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    // Covariance and correlation
    CovarianceMatrix covariance = copula.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CorrelationMatrix correlation = copula.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    CorrelationMatrix spearman = copula.getSpearmanCorrelation();
    fullprint << "spearman=" << spearman << std::endl;
    CorrelationMatrix kendall = copula.getKendallTau();
    fullprint << "kendall=" << kendall << std::endl;
    // Extract the marginals
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      Distribution margin(copula.getMarginal(i));
      fullprint << "margin=" << margin << std::endl;
      fullprint << "margin PDF=" << margin.computePDF(Point(1, 0.25)) << std::endl;
      fullprint << "margin CDF=" << margin.computeCDF(Point(1, 0.25)) << std::endl;
      fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
      fullprint << "margin realization=" << margin.getRealization() << std::endl;
    }

    // Extract a 2-D marginal
    Indices indices(2, 0);
    indices[0] = 1;
    indices[1] = 0;
    fullprint << "indices=" << indices << std::endl;
    Distribution margins(copula.getMarginal(indices));
    fullprint << "margins=" << margins << std::endl;
    fullprint << "margins PDF=" << margins.computePDF(Point(2, 0.25)) << std::endl;
    fullprint << "margins CDF=" << margins.computeCDF(Point(2, 0.25)) << std::endl;
    quantile = margins.computeQuantile(0.95);
    fullprint << "margins quantile=" << quantile << std::endl;
    fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
    fullprint << "margins realization=" << margins.getRealization() << std::endl;


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
