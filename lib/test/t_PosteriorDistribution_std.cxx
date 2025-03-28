//                                               -*- C++ -*-
/**
 *  @brief The test file of class PosteriorDistribution for standard methods
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
//#include <iomanip>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    //checkClassWithClassName<TestObject>();

    ResourceMap::SetAsUnsignedInteger("DeconditionedDistribution-MarginalIntegrationNodesNumber", 24);

    UnsignedInteger observationsSize = 5;
    // Create a collection of distribution
    Normal conditionedDistribution;
    Collection< Distribution > conditioningDistributionCollection;
    // First conditioning distribution: continuous/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Uniform(0.0, 1.0));
      atoms.add(Uniform(1.0, 2.0));
      conditioningDistributionCollection.add(JointDistribution(atoms));
    }
    // Second conditioning distribution: discrete/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Binomial(3, 0.5));
      atoms.add(Uniform(1.0, 2.0));
//       conditioningDistributionCollection.add(JointDistribution(atoms));
    }
    // Third conditioning distribution: dirac/continuous
    {
      Collection< Distribution > atoms;
      atoms.add(Dirac(0.0));
      atoms.add(Uniform(1.0, 2.0));
//      conditioningDistributionCollection.add(JointDistribution(atoms));
    }
    for (UnsignedInteger i = 0; i < conditioningDistributionCollection.getSize(); ++i)
    {
      fullprint << "\n==========================================================================================\n" << std::endl;
      fullprint << "conditioning distribution=" << conditioningDistributionCollection[i].__str__() << std::endl;
      Distribution observationsDistribution(conditionedDistribution);
      observationsDistribution.setParameter(conditioningDistributionCollection[i].getMean());
      Sample observations(observationsDistribution.getSample(observationsSize));
      PosteriorDistribution distribution(DeconditionedDistribution(conditionedDistribution, conditioningDistributionCollection[i]), observations);
      UnsignedInteger dim = distribution.getDimension();
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;
      fullprint << "range=" << distribution.getRange().__str__() << std::endl;
      fullprint << "entropy=" << std::setprecision(4) << distribution.computeEntropy() << std::endl;
      fullprint << "entropy (MC)=" << std::setprecision(4) << -distribution.computeLogPDF(distribution.getSample(10000)).computeMean()[0] << std::endl;
      Point mean(distribution.getMean());
      fullprint << "Mean " << mean.__str__() << std::endl;
      fullprint << "Covariance " << distribution.getCovariance().__str__() << std::endl;
      // Is this distribution an elliptical distribution?
      fullprint << "Elliptical distribution= " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Has this distribution an elliptical copula?
      fullprint << "Elliptical copula= " << (distribution.hasEllipticalCopula() ? "true" : "false") << std::endl;

      // Has this distribution an independent copula?
      fullprint << "Independent copula= " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;

      // Show PDF and CDF of mean point
      Scalar meanPDF = distribution.computePDF(mean);
      Scalar meanCDF = distribution.computeCDF(mean);
      fullprint << "Mean point= " << mean
                << " pdf=" << meanPDF
                << " cdf=" << meanCDF
                << std::endl;
      // Get 95% quantile
      Point quantile = distribution.computeQuantile(0.95);
      fullprint << "Quantile=" << quantile.__str__() << std::endl;
      fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      // Extract the marginals
      for (UnsignedInteger j = 0; j < dim; ++ j)
      {
        Distribution margin(distribution.getMarginal(j));
        fullprint << "margin=" << margin.__str__() << std::endl;
        fullprint << "margin PDF=" << margin.computePDF(mean[j]) << std::endl;
        fullprint << "margin CDF=" << margin.computeCDF(mean[j]) << std::endl;
        fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
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
