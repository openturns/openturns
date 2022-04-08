//                                               -*- C++ -*-
/**
 *  @brief The test file of class BlockIndependentDistribution for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // First test: comparison with a Normal distribution with block-diagonal
    // correlation
    {
      BlockIndependentDistribution::DistributionCollection collection(3);
      CorrelationMatrix R0(2);
      R0(0, 1) = 0.5;
      collection[0] = Normal(Point(2), Point(2, 1.0), R0);
      CorrelationMatrix R1(3);
      R1(0, 1) = 0.2;
      R1(0, 2) = 0.1;
      R1(1, 2) = 0.15;
      collection[1] = Normal(Point(3), Point(3, 1.0), R1);
      CorrelationMatrix R2(2);
      R2(0, 1) = 0.3;
      collection[2] = Normal(Point(2), Point(2, 1.0), R2);
      BlockIndependentDistribution distribution(collection);
      Collection<Distribution> copulaCollection(0);
      copulaCollection.add(NormalCopula(R0));
      copulaCollection.add(NormalCopula(R1));
      copulaCollection.add(NormalCopula(R2));
      ComposedCopula copula(copulaCollection);
      ComposedDistribution ref(Collection<Distribution>(7, Normal(0.0, 1.0)), copula);
      // Define a point
      Point point( distribution.getDimension(), 0.3 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      //Scalar eps(1e-5);
      Point DDF = distribution.computeDDF( point );
      fullprint << "ddf      =" << DDF << std::endl;
      Point ddfFD(distribution.getDimension());
      fullprint << "ddf (ref)=" << ref.computeDDF(point) << std::endl;
      Scalar PDF = distribution.computePDF( point );
      fullprint << "pdf      =" << PDF << std::endl;
      fullprint << "pdf (ref)=" << ref.computePDF(point) << std::endl;
      Scalar CDF = distribution.computeCDF( point );
      fullprint << "cdf      =" << CDF << std::endl;
      fullprint << "cdf (ref)=" << ref.computeCDF(point) << std::endl;
      Scalar Survival = distribution.computeSurvivalFunction(point);
      fullprint << "Survival      =" << Survival << std::endl;
      fullprint << "Survival (ref)=" << ref.computeSurvivalFunction(point) << std::endl;
      Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
      fullprint << "Inverse survival      =" << InverseSurvival << std::endl;
      fullprint << "Inverse survival (ref)=" << ref.computeInverseSurvivalFunction(0.95) << std::endl;
      fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
      // Get 50% quantile
      Point quantile = distribution.computeQuantile( 0.5 );
      fullprint << "Quantile      =" << quantile << std::endl;
      fullprint << "Quantile (ref)=" << ref.computeQuantile( 0.5 ) << std::endl;
      fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    }
    // Instantiate one distribution object
    CorrelationMatrix R(3);
    R(0, 1) = 0.5;
    R(0, 2) = 0.25;
    BlockIndependentDistribution::DistributionCollection collection(3);
    collection[0] = ComposedDistribution(Collection<Distribution>(2, Normal()), AliMikhailHaqCopula(0.5));
    collection[1] = Normal(Point(3, 1.0), Point(3, 2.0), R);
    collection[2] = ComposedDistribution(Collection<Distribution>(2, Exponential()), FrankCopula(0.5));
    BlockIndependentDistribution distribution(collection);
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;

    // Is this distribution elliptical ?
    fullprint << "Elliptical distribution= " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Has this distribution an elliptical copula ?
    fullprint << "Elliptical = " << (distribution.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Has this distribution an independent copula ?
    fullprint << "Independent = " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    Point oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample oneSample = distribution.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(4);
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);

    // Define a point
    Point point( distribution.getDimension(), 0.3 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    //Scalar eps(1e-5);
    Point DDF = distribution.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    Point ddfFD(distribution.getDimension());
    fullprint << "ddf (FD)=" << distribution.DistributionImplementation::computeDDF(point) << std::endl;
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    Scalar Survival = distribution.computeSurvivalFunction(point);
    fullprint << "Survival      =" << Survival << std::endl;
    fullprint << "Survival (ref)=" << distribution.computeSurvivalFunction(point) << std::endl;
    Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << distribution.DistributionImplementation::computeSurvivalFunction(InverseSurvival) << std::endl;
    // Get 50% quantile
    Point quantile = distribution.computeQuantile( 0.5 );
    fullprint << "Quantile=" << quantile << std::endl;
    fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;

    if (distribution.getDimension() <= 2)
    {
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
    }

    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
    const Point mean = distribution.getMean();
    const Point refMean = {0, 0, 1, 1, 1, 1, 1};
    assert_almost_equal(mean, refMean);

    //BlockIndependentDistribution::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    //fullprint << "parameters=" << parameters << std::endl;
    // Covariance and correlation
    PlatformInfo::SetNumericalPrecision(4);
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CorrelationMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    CorrelationMatrix spearman = distribution.getSpearmanCorrelation();
    fullprint << "spearman=" << spearman << std::endl;
    CorrelationMatrix kendall = distribution.getKendallTau();
    fullprint << "kendall=" << kendall << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
    {
      UnsignedInteger dim = distribution.getDimension();
      Scalar x = 0.6;
      Point y(dim - 1, 0.2);
      fullprint << "conditional PDF=" << distribution.computeConditionalPDF(x, y) << std::endl;
      fullprint << "conditional CDF=" << distribution.computeConditionalCDF(x, y) << std::endl;
      fullprint << "conditional quantile=" << distribution.computeConditionalQuantile(x, y) << std::endl;
      Point pt(dim);
      for (UnsignedInteger i = 0; i < dim; ++i) pt[i] = 0.1 * i + 0.05;
      fullprint << "sequential conditional PDF=" << distribution.computeSequentialConditionalPDF(pt) << std::endl;
      Point resCDF(distribution.computeSequentialConditionalCDF(pt));
      fullprint << "sequential conditional CDF(" << pt << ")=" << resCDF << std::endl;
      fullprint << "sequential conditional quantile(" << resCDF << ")=" << distribution.computeSequentialConditionalQuantile(resCDF) << std::endl;
    }

    // Extract a 5-D marginal
    UnsignedInteger dim = 5;
    point = Point(dim, 0.25);
    Indices indices(dim, 0);
    indices[0] = 1;
    indices[1] = 2;
    indices[2] = 3;
    indices[3] = 5;
    indices[4] = 6;
    fullprint << "indices=" << indices << std::endl;
    Distribution margins(distribution.getMarginal(indices));
    fullprint << "margins=" << margins << std::endl;
    fullprint << "margins PDF=" << margins.computePDF(point) << std::endl;
    fullprint << "margins CDF=" << margins.computeCDF(point) << std::endl;
    quantile = margins.computeQuantile(0.95);
    fullprint << "margins quantile=" << quantile << std::endl;
    fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
    fullprint << "margins realization=" << margins.getRealization() << std::endl;
    // Tests o the isoprobabilistic transformation
    // General case with normal standard distribution
    fullprint << "isoprobabilistic transformation (general normal)=" << distribution.getIsoProbabilisticTransformation().__str__() << std::endl;
    // General case with non-normal standard distribution
    collection[0] = SklarCopula(Student(3.0, Point(2, 1.0), Point(2, 3.0), CorrelationMatrix(2)));
    distribution = BlockIndependentDistribution(collection);
    fullprint << "isoprobabilistic transformation (general non-normal)=" << distribution.getIsoProbabilisticTransformation().__str__() << std::endl;
    {
      dim = distribution.getDimension();
      Scalar x = 0.6;
      Point y(dim - 1, 0.2);
      fullprint << "conditional PDF=" << distribution.computeConditionalPDF(x, y) << std::endl;
      fullprint << "conditional CDF=" << distribution.computeConditionalCDF(x, y) << std::endl;
      fullprint << "conditional quantile=" << distribution.computeConditionalQuantile(x, y) << std::endl;
      Point pt(dim);
      for (UnsignedInteger i = 0; i < dim; ++i) pt[i] = 0.1 * i + 0.05;
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
