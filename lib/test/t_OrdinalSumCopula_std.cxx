//                                               -*- C++ -*-
/**
 *  @brief The test file of class OrdinalSumCopula for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instantiate one distribution object
    CorrelationMatrix R(2);
    R(0, 1) = 0.5;
    OrdinalSumCopula::DistributionCollection collection(3);
    collection[0] = FrankCopula(3.0);
    collection[1] = NormalCopula(R);
    collection[2] = ClaytonCopula(2.0);
    Point bounds(2);
    bounds[0] = 0.2;
    bounds[1] = 0.7;
    OrdinalSumCopula copula(collection, bounds);
    fullprint << "Copula " << copula << std::endl;
    std::cout << "Copula " << copula << std::endl;

    // Is this copula elliptical ?
    fullprint << "Elliptical distribution= " << (copula.isElliptical() ? "true" : "false") << std::endl;

    // Is this copula continuous ?
    fullprint << "Continuous = " << (copula.isContinuous() ? "true" : "false") << std::endl;

    // Is this copula elliptical ?
    fullprint << "Elliptical = " << (copula.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Is this copula independent ?
    fullprint << "Independent = " << (copula.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of copula
    Point oneRealization = copula.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample oneSample = copula.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(5);
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);

    // Define a point
    UnsignedInteger dim = copula.getDimension();
    Point point( dim, 0.6 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    //Scalar eps(1e-5);
    Point DDF = copula.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    Point ddfFD(dim);
    Scalar PDF = copula.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    Scalar CDF = copula.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
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

    if (dim <= 2)
    {
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
    }

    fullprint << "entropy=" << copula.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -copula.computeLogPDF(copula.getSample(1000000)).computeMean()[0] << std::endl;
    Point mean = copula.getMean();
    fullprint << "mean=" << mean << std::endl;
    precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(5);
    // Covariance and correlation
    CovarianceMatrix covariance = copula.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CorrelationMatrix correlation = copula.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    CorrelationMatrix spearman = copula.getSpearmanCorrelation();
    fullprint << "spearman=" << spearman << std::endl;
    CorrelationMatrix kendall = copula.getKendallTau();
    fullprint << "kendall=" << kendall << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
    OrdinalSumCopula::PointWithDescriptionCollection parameters = copula.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    Scalar x = 0.6;
    Point y(dim - 1, 0.2);
    fullprint << "conditional PDF=" << copula.computeConditionalPDF(x, y) << std::endl;
    fullprint << "conditional CDF=" << copula.computeConditionalCDF(x, y) << std::endl;
    fullprint << "conditional quantile=" << copula.computeConditionalQuantile(x, y) << std::endl;
    Point pt(dim);
    for (UnsignedInteger i = 0; i < dim; ++i) pt[i] = 0.1 * i + 0.05;
    fullprint << "sequential conditional PDF=" << copula.computeSequentialConditionalPDF(pt) << std::endl;
    Point resCDF(copula.computeSequentialConditionalCDF(pt));
    fullprint << "sequential conditional CDF(" << pt << ")=" << resCDF << std::endl;
    fullprint << "sequential conditional quantile(" << resCDF << ")=" << copula.computeSequentialConditionalQuantile(resCDF) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

