//                                               -*- C++ -*-
/**
 *  @brief The test file of class PosteriorDistribution for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Test basic functionnalities
    //checkClassWithClassName<TestObject>();

    UnsignedInteger observationsSize = 5;
    // Create a collection of distribution
    Normal conditionedDistribution;
    Collection< Distribution > conditioningDistributionCollection;
    // First conditioning distribution: continuous/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Uniform(0.0, 1.0));
      atoms.add(Uniform(1.0, 2.0));
      conditioningDistributionCollection.add(ComposedDistribution(atoms));
    }
    // Second conditioning distribution: discrete/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Binomial(3, 0.5));
      atoms.add(Uniform(1.0, 2.0));
//       conditioningDistributionCollection.add(ComposedDistribution(atoms));
    }
    // Third conditioning distribution: dirac/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Dirac(0.0));
      atoms.add(Uniform(1.0, 2.0));
      conditioningDistributionCollection.add(ComposedDistribution(atoms));
    }
    for (UnsignedInteger i = 0; i < conditioningDistributionCollection.getSize(); ++i)
    {
      fullprint << "conditioning distribution=" << conditioningDistributionCollection[i].__str__() << std::endl;
      Distribution observationsDistribution(conditionedDistribution);
      observationsDistribution.setParameter(conditioningDistributionCollection[i].getMean());
      Sample observations(observationsDistribution.getSample(observationsSize));
      PosteriorDistribution distribution(ConditionalDistribution(conditionedDistribution, conditioningDistributionCollection[i]), observations);
      UnsignedInteger dim = distribution.getDimension();
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;
      fullprint << "range=" << distribution.getRange() << std::endl;
      // fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
      // fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
      Point mean(distribution.getMean());
      fullprint << "Mean " << mean << std::endl;
      fullprint << "Covariance " << distribution.getCovariance() << std::endl;
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
      Sample oneSample = distribution.getSample(size);
      fullprint << "oneSample=" << oneSample << std::endl;

      // Test for sampling
//       size = 10000;
//       Sample anotherSample = distribution.getSample(size);
//       fullprint << "anotherSample mean=" << anotherSample.computeMean() << std::endl;
//       fullprint << "anotherSample covariance=" << anotherSample.computeCovariance() << std::endl;

      // Define a point
      Point zero(dim, 0.0);

      // Show PDF and CDF of zero point
      Scalar zeroPDF = distribution.computePDF(zero);
      Scalar zeroCDF = distribution.computeCDF(zero);
      fullprint << "Zero point= " << zero
                << " pdf=" << zeroPDF
                << " cdf=" << zeroCDF
                << std::endl;
      // Get 95% quantile
      Point quantile = distribution.computeQuantile(0.95);
      fullprint << "Quantile=" << quantile << std::endl;
      fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      // Extract the marginals
      for (UnsignedInteger j = 0; j < dim; ++ j)
      {
        Distribution margin(distribution.getMarginal(j));
        fullprint << "margin=" << margin << std::endl;
        fullprint << "margin PDF=" << margin.computePDF(Point(1)) << std::endl;
        fullprint << "margin CDF=" << margin.computeCDF(Point(1)) << std::endl;
        fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
        fullprint << "margin realization=" << margin.getRealization() << std::endl;
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
