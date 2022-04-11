//                                               -*- C++ -*-
/**
 *  @brief The test file of class ComposedDistribution for standard methods
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
  ResourceMap::SetAsBool("Distribution-Parallel", false);

  try
  {

    // Test basic functionnalities
    //checkClassWithClassName<TestObject>();

    Point mean(3);
    mean[0] = 1.0;
    mean[1] = 2.0;
    mean[2] = 3.0;
    Point sigma(3);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    sigma[2] = 4.0;
    // Create a collection of distribution
    ComposedDistribution::DistributionCollection aCollection;
    Normal marginal(mean[0], sigma[0]);
    marginal.setName("First");
    Description component(1);
    component[0] = "One";
    marginal.setDescription(component);
    aCollection.add( Distribution(marginal) );
    aCollection[0].setName("First");
    marginal = Normal(mean[1], sigma[1]);
    marginal.setName("Second");
    component[0] = "Two";
    marginal.setDescription(component);
    aCollection.add( Distribution(marginal) );
    aCollection[1].setName("Second");
    marginal = Normal(mean[2], sigma[2]);
    marginal.setName("Third");
    component[0] = "Three";
    marginal.setDescription(component);
    aCollection.add( Distribution(marginal) );
    aCollection[2].setName("Third");

    // Create a copula
    IndependentCopula aCopula(aCollection.getSize());
    aCopula.setName("Independent copula");
    // Instantiate one distribution object
    ComposedDistribution distribution(aCollection, aCopula);
    distribution.setName("myDist");
    UnsignedInteger dim = distribution.getDimension();
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;
    fullprint << "Parameters " << distribution.getParametersCollection() << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
    fullprint << "Mean " << distribution.getMean() << std::endl;
    UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision(2);
    fullprint << "Covariance " << distribution.getCovariance() << std::endl;
    PlatformInfo::SetNumericalPrecision(precision);
    // Is this distribution an elliptical distribution?
    fullprint << "Elliptical distribution= " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Has this distribution an elliptical copula?
    fullprint << "Elliptical copula= " << (distribution.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Has this distribution an independent copula?
    fullprint << "Independent copula= " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    Point oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;
    // Test for sampling
    UnsignedInteger size = 10;
    Sample oneSample = distribution.getSample( size );
    fullprint << "oneSample=" << oneSample << std::endl;

    // Test for sampling
    size = 10000;
    Sample anotherSample = distribution.getSample( size );
    fullprint << "anotherSample mean=" << anotherSample.computeMean() << std::endl;
    fullprint << "anotherSample covariance=" << anotherSample.computeCovariance() << std::endl;

    // Define a point
    Point zero(dim, 0.0);

    // Show PDF and CDF of zero point
    Scalar zeroPDF = distribution.computePDF( zero );
    Scalar zeroCDF = distribution.computeCDF( zero );
    fullprint << "Zero point= " << zero
              << " pdf=" << zeroPDF
              << " cdf=" << zeroCDF
              << std::endl;
    // Get 95% quantile
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "Quantile=" << quantile << std::endl;
    fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;

    // Reference
    Normal ref(mean, sigma, IdentityMatrix(distribution.getDimension()));
    fullprint << "Reference=" << std::endl;
    fullprint << "Zero point= " << zero
              << " pdf= " << ref.computePDF(zero)
              << " cdf= " << ref.computeCDF(zero)
              << " quantile= " << ref.computeQuantile(0.95)
              << std::endl;
    // Extract the marginals
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      Distribution margin(distribution.getMarginal(i));
      fullprint << "margin=" << margin << std::endl;
      fullprint << "margin PDF=" << margin.computePDF(Point(1)) << std::endl;
      fullprint << "margin CDF=" << margin.computeCDF(Point(1)) << std::endl;
      fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
      fullprint << "margin realization=" << margin.getRealization() << std::endl;
    }

    // Extract a 2-D marginal
    Indices indices(2, 0);
    indices[0] = 1;
    indices[1] = 0;
    fullprint << "indices=" << indices << std::endl;
    Distribution margins(distribution.getMarginal(indices));
    fullprint << "margins=" << margins << std::endl;
    fullprint << "margins PDF=" << margins.computePDF(Point(2)) << std::endl;
    fullprint << "margins CDF=" << margins.computeCDF(Point(2)) << std::endl;
    quantile = margins.computeQuantile(0.5);
    fullprint << "margins quantile=" << quantile << std::endl;
    fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
    fullprint << "margins realization=" << margins.getRealization() << std::endl;
    {
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

    // With a Normal copula
    CorrelationMatrix correlation(dim);
    for(UnsignedInteger i = 1; i < dim; i++)
    {
      correlation(i - 1, i) = 0.25;
    }
    NormalCopula anotherCopula(correlation);
    anotherCopula.setName("Normal copula");
    distribution = ComposedDistribution(aCollection, anotherCopula);
    distribution.setName("myDist");
    Normal distributionRef(mean, sigma, correlation);
    fullprint << "Distribution " << distribution << std::endl;
    fullprint << "Parameters " << distribution.getParametersCollection() << std::endl;
    // Show PDF and CDF at point
    Point point(dim, 0.0);
    fullprint << "PDF      =" << distribution.computePDF(point) << std::endl;
    fullprint << "PDF (ref)=" << distributionRef.computePDF(point) << std::endl;
    fullprint << "CDF      =" << distribution.computeCDF(point) << std::endl;
    fullprint << "CDF (ref)=" << distributionRef.computeCDF(point) << std::endl;
    Scalar Survival = distribution.computeSurvivalFunction(point);
    fullprint << "Survival      =" << Survival << std::endl;
    fullprint << "Survival (ref)=" << distribution.computeSurvivalFunction(point) << std::endl;
    Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;

    // 95% quantile
    quantile = distribution.computeQuantile( 0.95 );
    fullprint << "Quantile      =" << quantile << std::endl;
    fullprint << "Quantile (ref)=" << distributionRef.computeQuantile( 0.95 ) << std::endl;
    fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    {
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
    // Confidence regions
    if (distribution.getDimension() <= 2)
    {
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
    // Moments
    fullprint << "Mean      =" << distribution.getMean() << std::endl;
    fullprint << "Mean (ref)=" << distributionRef.getMean() << std::endl;
    Point standardDeviation = distribution.getStandardDeviation();
    fullprint << "Standard deviation      =" << standardDeviation << std::endl;
    fullprint << "Standard deviation (ref)=" << distributionRef.getStandardDeviation() << std::endl;
    Point skewness = distribution.getSkewness();
    fullprint << "Skewness       =" << skewness << std::endl;
    fullprint << "Skewness (ref)==" << distributionRef.getSkewness() << std::endl;
    Point kurtosis = distribution.getKurtosis();
    fullprint << "Kurtosis      =" << kurtosis << std::endl;
    fullprint << "Kurtosis (ref)=" << distributionRef.getKurtosis() << std::endl;
    fullprint << "Covariance      =" << distribution.getCovariance() << std::endl;
    fullprint << "Covariance (ref)=" << distributionRef.getCovariance() << std::endl;
    anotherSample = distribution.getSample(size);
    fullprint << "anotherSample mean=" << anotherSample.computeMean() << std::endl;
    fullprint << "anotherSample covariance=" << anotherSample.computeCovariance() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
