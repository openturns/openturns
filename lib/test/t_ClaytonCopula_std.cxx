//                                               -*- C++ -*-
/**
 *  @brief The test file of class ClaytonCopula for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instanciate one distribution object
    UnsignedInteger dim = 2;
    ClaytonCopula copula(2.5);
    copula.setName("a clayton copula");
    fullprint << "Copula " << copula << std::endl;
    std::cout << "Copula " << copula << std::endl;
    fullprint << "entropy=" << copula.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -copula.computeLogPDF(copula.getSample(1000000)).computeMean()[0] << std::endl;
    fullprint << "Mean " << copula.getMean() << std::endl;
    // Is this copula an elliptical distribution?
    fullprint << "Elliptical distribution= " << (copula.isElliptical() ? "true" : "false") << std::endl;

    // Is this copula elliptical ?
    fullprint << "Elliptical copula= " << (copula.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Is this copula independent ?
    fullprint << "Independent copula= " << (copula.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of copula
    Point oneRealization = copula.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10;
    Sample oneSample = copula.getSample( size );
    fullprint << "oneSample=" << oneSample << std::endl;

    // Test for sampling
    size = 10000;
    Sample anotherSample = copula.getSample( size );
    fullprint << "anotherSample mean=" << anotherSample.computeMean() << std::endl;
    fullprint << "anotherSample covariance=" << anotherSample.computeCovariance() << std::endl;

    // Define a point
    Point point(dim, 0.2);

    // Show DDF, PDF and CDF of point
    Point  pointDDF = copula.computeDDF( point );
    Scalar pointPDF = copula.computePDF( point );
    Scalar pointCDF = copula.computeCDF( point );
    fullprint << "point= " << point
              << " ddf=" << pointDDF.__str__()
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

    // Covariance and correlation
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(4);
    CovarianceMatrix covariance = copula.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CorrelationMatrix correlation = copula.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    CorrelationMatrix spearman = copula.getSpearmanCorrelation();
    fullprint << "spearman=" << spearman << std::endl;
    CorrelationMatrix kendall = copula.getKendallTau();
    fullprint << "kendall=" << kendall << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
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

    // Additional tests for PDF/CDF in extreme cases
    Sample points(0, 2);
    points.add(Point(2, 1.0e-12));
    points.add(Point(2, 1.0e-7));
    points.add(Point(2, 0.1));
    points.add(Point(2, 0.5));
    points.add(Point(2, 0.1));
    points.add(Point(2, 0.1));
    points.add(Point(2, 0.1));

    Point thetas;
    thetas.add(1.0e-12);
    thetas.add(0.9e-8);
    thetas.add(1.1e-8);
    thetas.add(-0.99);
    thetas.add(9.9e1);
    thetas.add(1.1e2);
    thetas.add(1.0e5);
    for (UnsignedInteger i = 0; i < thetas.getSize(); ++i)
    {
      Point x(points[i]);
      copula = ClaytonCopula(thetas[i]);
      fullprint << copula.__str__() << std::endl;
      fullprint << "PDF(" << x.__str__() << ")=" << std::setprecision(12) << copula.computePDF(x) << std::endl;
      fullprint << "CDF(" << x.__str__() << ")=" << std::setprecision(12) << copula.computeCDF(x) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
