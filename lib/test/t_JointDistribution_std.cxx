//                                               -*- C++ -*-
/**
 *  @brief The test file of class JointDistribution for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

    UnsignedInteger dim = 3;
    Point mean(dim);
    mean[0] = 3.0;
    mean[1] = 2.0;
    mean[2] = 1.0;
    Point sigma(dim);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    sigma[2] = 4.0;
    Collection<CovarianceMatrix> refCovariances;
    refCovariances.add(CovarianceMatrix(3, {4.0, 0.0, 0.0, 0.0, 9.0, 0.0, 0.0, 0.0, 16.0}));
    refCovariances.add(CovarianceMatrix(3, {4.0, 1.5, 0.0, 1.5, 9.0, 3.0, 0.0, 3.0, 16.0}));
    refCovariances.add(CovarianceMatrix(3, {4.0, 1.125, 0.0, 1.125, 9.0, 2.25, 0.0, 2.25, 16.0}));
    refCovariances.add(CovarianceMatrix(3, {4.0, 2.0696999, -4.403889, 2.0696999, 9.0, 4.1393998, -4.403889, 4.1393998, 16.0}));
    refCovariances.add(CovarianceMatrix(3, {0.39606657, 0.0, 0.0, 0.0, 0.891149785, 0.0, 0.0, 0.0, 1.584266284}));
    Collection<Point> refStandardDeviation;
    refStandardDeviation.add({2, 3, 4});
    refStandardDeviation.add({2, 3, 4});
    refStandardDeviation.add({2, 3, 4});
    refStandardDeviation.add({1.49595080640498, 2.00948748222124, 2.99190161280996});
    refStandardDeviation.add({0.62933820074628, 0.94400730111948, 1.25867640149264});
    Collection<Point> refSkewness;
    refSkewness.add({0, 0, 0});
    refSkewness.add({0, 0, 0});
    refSkewness.add({0, 0, 0});
    refSkewness.add({-0.213157049688829, 0.0, 0.213157049689032});
    refSkewness.add({ 0.22711106425,0.22711106425,0.22711106425});
    Collection<Point> refKurtosis;
    refKurtosis.add({3, 3, 3});
    refKurtosis.add({3, 3, 3});
    refKurtosis.add({3, 3, 3});
    refKurtosis.add({3.11664895604121, 3.03472746922749, 3.11664895604127});
    refKurtosis.add({2.439305739629, 2.439305739629, 2.439305739629});

    // Create a collection of distribution
    JointDistribution::DistributionCollection aCollection;
    Normal marginal(mean[0], sigma[0]);
    marginal.setName("First");
    Description component(1);
    component[0] = "One";
    marginal.setDescription(component);
    aCollection.add(marginal);
    aCollection[0].setName("First");
    marginal = Normal(mean[1], sigma[1]);
    marginal.setName("Second");
    component[0] = "Two";
    marginal.setDescription(component);
    aCollection.add(marginal);
    aCollection[1].setName("Second");
    marginal = Normal(mean[2], sigma[2]);
    marginal.setName("Third");
    component[0] = "Three";
    marginal.setDescription(component);
    aCollection.add(marginal);
    aCollection[2].setName("Third");

    // Create a copula
    IndependentCopula aCopula(aCollection.getSize());
    aCopula.setName("Independent copula");
    Collection<Distribution> cores;
    cores.add(aCopula);
    // With a Normal copula
    CorrelationMatrix correlation(dim);
    for(UnsignedInteger i = 1; i < dim; i++)
    {
      correlation(i - 1, i) = 0.25;
    }
    NormalCopula anotherCopula(correlation);
    anotherCopula.setName("Normal copula");
    cores.add(anotherCopula);
    // With a copula which is not a copula by type
    Collection<Distribution> atoms;
    atoms.add(aCopula);
    atoms.add(anotherCopula);
    cores.add(Mixture(atoms, {0.25, 0.75}));
    // With a non-copula core
    cores.add(UniformOrderStatistics(dim));
    // With a core which support is strictly included in the unit cube
    cores.add(KernelMixture(Beta(2.0, 3.0, 0.2, 0.8), Point(dim, 1.0), Sample(1, dim)));
    
    ResourceMap::SetAsBool("JointDistribution-UseGenericCovarianceAlgorithm", true);
    for (UnsignedInteger nCore = 0; nCore < cores.getSize(); ++nCore)
    {
      std::cout << "\n\n" << std::endl;
      // Instantiate one distribution object
      JointDistribution distribution(aCollection, cores[nCore]);
      distribution.setName("myDist");
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << std::endl;
      std::cout << distribution << std::endl;
      std::cout << "Distribution (Markdown)" << std::endl;
      std::cout << distribution.__repr_markdown__() << std::endl;
      fullprint << "Parameters " << distribution.getParametersCollection() << std::endl;
      // Too slow if nCore == 2
      if (nCore != 2)
        {
          fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
          fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
        }
      fullprint << "Mean " << distribution.getMean() << std::endl;
      if (nCore != 3)
        {
          CovarianceMatrix covariance(distribution.getCovariance());
          covariance.checkSymmetry();
          assert_almost_equal(covariance, refCovariances[nCore]);
        }
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

      // Moments other than mean and covariance
      Point standardDeviation = distribution.getStandardDeviation();
      assert_almost_equal(standardDeviation, refStandardDeviation[nCore]);
      Point skewness = distribution.getSkewness();
      assert_almost_equal(skewness, refSkewness[nCore]);
      Point kurtosis = distribution.getKurtosis();
      assert_almost_equal(kurtosis, refKurtosis[nCore]);
      anotherSample = distribution.getSample(size);
      fullprint << "anotherSample mean=" << anotherSample.computeMean() << std::endl;
      fullprint << "anotherSample covariance=" << anotherSample.computeCovariance() << std::endl;
    } // for (UnsignedInteger nCore = 0; nCore < cores.getSize(); ++nCore)
    // Create and print a complex distribution
    JointDistribution::DistributionCollection aCollection2;
    Normal marginalN(0.0, 1.0);
    marginalN.setName("First");
    Description componentN(1);
    componentN[0] = "One";
    marginalN.setDescription(componentN);
    aCollection2.add(marginalN);
    aCollection2[0].setName("First");
    Uniform marginalU(12345.6, 123456.7);
    marginalU.setName("Second");
    Description componentU(1);
    componentU[0] = "Two";
    marginalU.setDescription(componentU);
    aCollection2.add(marginalU);
    aCollection2[1].setName("Second");
    TruncatedDistribution marginalTN(Normal(2.0, 1.5), 1.0, 4.0);
    marginalTN.setName("Third");
    Description componentTN(1);
    componentTN[0] = "Three";
    marginalTN.setDescription(componentTN);
    aCollection2.add(marginalTN);
    aCollection2[2].setName("Third");
    JointDistribution distribution2(aCollection2);
    distribution2.setName("myDist2");
    fullprint << "Distribution " << distribution2 << std::endl;
    std::cout << "Distribution " << std::endl;
    std::cout << distribution2 << std::endl;
    std::cout << "Distribution (Markdown)" << std::endl;
    std::cout << distribution2.__repr_markdown__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
