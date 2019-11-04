//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomMixture for grid computatioons - 2D cases
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
    // 2D test using FFT
    // Collection of distributions
    Collection< Distribution > coll(0);
    coll.add(Uniform(0, 1));
    coll.add(Uniform(0, 1));
    coll.add(Uniform(0, 1));
    // Set weights
    Matrix weights(2, 3);
    weights(0, 0) = 1.0;
    weights(0, 1) = -2.0;
    weights(0, 2) = 1.0;
    weights(1, 0) = 1.0;
    weights(1, 1) = 1.0;
    weights(1, 2) = -3.0;
    // Build the RandomMixture
    RandomMixture dist_2D(coll, weights);
    // Defining RandomMixture
    const UnsignedInteger N = 16;
    Indices points(2, N);
    Point mean(dist_2D.getMean());
    Point sigma(dist_2D.getStandardDeviation());
    Point xMin(mean - 2.9 * sigma);
    Point xMax(mean + 2.9 * sigma);
    Sample grid;
    fullprint << "distribution = " << dist_2D << std::endl;
    fullprint << "distribution = " << dist_2D.__str__() << std::endl;
    fullprint << "range = " << dist_2D.getRange() << std::endl;
    fullprint << "mean = " <<  dist_2D.getMean() << std::endl;
    fullprint << "cov = " << dist_2D.getCovariance() << std::endl;
    fullprint << "sigma = " << dist_2D.getStandardDeviation() << std::endl;
    fullprint << "xMin = " << xMin << std::endl;
    fullprint << "xMax = " << xMax << std::endl;
    Sample result(dist_2D.computePDF(xMin, xMax, points, grid));
    for (UnsignedInteger i = 0; i < grid.getSize(); ++i)
      fullprint << grid[i][0] << ";" << grid[i][1] << ";" << result[i][0] << std::endl;
    // Defining new case, involving Normal distributions
    Collection< Distribution > collection(0);
    collection.add(Normal(2.0, 3.0));
    collection.add(Normal(1.0, 4.0));
    weights = Matrix(2, 2);
    weights(0, 0) = 4.0;
    weights(0, 1) = 1.0;
    weights(1, 0) = 2.0;
    weights(1, 1) = 1.4;
    // Build the RandomMixture
    RandomMixture distribution2D(collection, weights);
    fullprint << "distribution = " << distribution2D << std::endl;
    fullprint << "range = " << distribution2D.getRange() << std::endl;
    fullprint << "mean = " <<  distribution2D.getMean() << std::endl;
    fullprint << "cov = " << distribution2D.getCovariance() << std::endl;
    fullprint << "sigma = " << distribution2D.getStandardDeviation() << std::endl;
    xMin = distribution2D.getMean() - 2.9 * distribution2D.getStandardDeviation();
    xMax = distribution2D.getMean() + 2.9 * distribution2D.getStandardDeviation();
    result = distribution2D.computePDF(xMin, xMax, points, grid);
    for (UnsignedInteger i = 0; i < grid.getSize(); ++i)
      fullprint << grid[i][0] << ";" << grid[i][1] << ";" << result[i][0] << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
