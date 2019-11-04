//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomMixture for grid computatioons - 1D cases
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
  PlatformInfo::SetNumericalPrecision( 6 );
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Defining RandomMixture
    Point weights(0);
    Collection< Distribution > coll(0);
    coll.add(Gamma(0.5, 1.0));
    weights.add(1.0);
    coll.add(Gamma(0.5, 1.0));
    weights.add(1.0);
    coll.add(Gamma(0.5, 1.0));
    weights.add(1.0);
    coll.add(Gamma(0.5, 1.0));
    weights.add(1.0);
    coll.add(Gamma(1.0, 1.0));
    weights.add(1.0);
    RandomMixture distribution(coll, weights);
    Gamma referenceDistribution(3.0, 1.0);
    // Compute PDF on regular grid
    const UnsignedInteger N = 256;
    Indices points(1, N);
    Point mean(distribution.getMean());
    Point sigma(distribution.getStandardDeviation());
    Point xMin(mean - 3.9 * sigma);
    Point xMax(mean + 3.9 * sigma);
    Sample grid;
    fullprint << "distribution = " << distribution << std::endl;
    fullprint << "distribution = " << distribution.__str__() << std::endl;
    fullprint << "range = " << distribution.getRange() << std::endl;
    fullprint << "mean = " <<  distribution.getMean() << std::endl;
    fullprint << "cov = " << distribution.getCovariance() << std::endl;
    fullprint << "sigma = " << distribution.getStandardDeviation() << std::endl;
    fullprint << "xMin = " << xMin << std::endl;
    fullprint << "xMax = " << xMax << std::endl;
    Sample result(distribution.computePDF(xMin, xMax, points, grid));
    for (UnsignedInteger i = 0; i < grid.getSize(); ++i)
      fullprint << grid[i][0] << ";" << result[i][0] << ";" << referenceDistribution.computePDF(grid[i]) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
