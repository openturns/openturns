//                                               -*- C++ -*-
/**
 *  @brief The test file of class ComposedDistribution for standard methods
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
  ResourceMap::SetAsBool("Distribution-Parallel", false);

  try
  {

    Point meanPoint(1);
    meanPoint[0] = 1.0;
    Point sigma(1);
    sigma[0] = 3.0;
    CorrelationMatrix R(1);
    R(0, 0) = 1.0;
    // Create a collection of distribution
    UnsignedInteger dimension = 2000;
    fullprint << "Creating a composed distribution of dimension " << dimension << std::endl;
    ComposedDistribution::DistributionCollection aCollection(dimension, Normal(meanPoint, sigma, R));

    for (UnsignedInteger i = 0; i < dimension; i++)
    {
      aCollection[i] = Normal(meanPoint, sigma, R);
    }

    // Create a copula
    IndependentCopula aCopula(dimension);
    // Instantiate one distribution object
    ComposedDistribution distribution(aCollection, aCopula);
    fullprint << "Distribution created." << std::endl;
    // Is this distribution an elliptical distribution?
    fullprint << "Elliptical distribution= " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Has this distribution an elliptical copula?
    fullprint << "Elliptical copula= " << (distribution.hasEllipticalCopula() ? "true" : "false") << std::endl;

    // Has this distribution an independent copula?
    fullprint << "Independent copula= " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for sampling
    UnsignedInteger size = 10;
    Sample anotherSample = distribution.getSample( size );

    // Define a point
    Point zero(dimension, 0.0);

    // Show PDF and CDF of zero point
    Scalar zeroPDF = distribution.computePDF( zero );
    Scalar zeroCDF = distribution.computeCDF( zero );
    fullprint << " pdf=" << zeroPDF
              << " cdf=" << zeroCDF
              << std::endl;
    // Get 95% quantile
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "Quantile=" << quantile << std::endl;
    fullprint << "CDF(quantile)=" << distribution.computeCDF(quantile) << std::endl;

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
    Sample sample(margins.getSample(1000));
    fullprint << "margins sample mean=" << sample.computeMean() << std::endl;
    fullprint << "margins sample covariance=" << sample.computeCovariance() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
