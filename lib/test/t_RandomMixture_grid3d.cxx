//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomMixture for standard methods
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
  PlatformInfo::SetNumericalPrecision( 6 );
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // 1) Analytical test ==> No FFT
    Collection<Distribution> coll(3);
    coll[0] = Normal(0.0, 1.0);
    coll[1] = Uniform(2.0, 5.0);
    coll[2] = Uniform(2.0, 5.0);
    Matrix weights(3, 3);
    weights(0, 0) = 1.0;
    weights(0, 1) = 2.0;
    weights(0, 2) = 4.0;
    weights(1, 0) = 3.0;
    weights(1, 1) = 4.0;
    weights(1, 2) = 5.0;
    weights(2, 0) = 6.0;
    weights(2, 1) = 0.0;
    weights(2, 2) = 1.0;
    RandomMixture distribution(coll, weights);
    fullprint << "distribution=" << distribution << std::endl;
    fullprint << "distribution=" << distribution.__str__() << std::endl;
    fullprint << "range = " << distribution.getRange() << std::endl;
    fullprint << "mean = " <<  distribution.getMean() << std::endl;
    fullprint << "cov = " << distribution.getCovariance() << std::endl;
    fullprint << "sigma = " << distribution.getStandardDeviation() << std::endl;
    const UnsignedInteger N = 4;
    Indices points(3, N);
    Point mean(distribution.getMean());
    Point sigma(distribution.getStandardDeviation());
    Point xMin(mean - 2.9 * sigma);
    Point xMax(mean + 2.9 * sigma);
    Sample grid;
    Sample result(distribution.computePDF(xMin, xMax, points, grid));
    for (UnsignedInteger i = 0; i < grid.getSize(); ++i)
      fullprint << grid[i][0] << ";" << grid[i][1] << ";" << grid[i][2] << ";" << result[i][0] << std::endl;
    // 2) 3D test using FFT
    Collection<Distribution> collection(0);
    collection.add(Normal(2.0, 1.0));
    collection.add(Normal(-2.0, 1.0));
    const Mixture mixture(collection);
    Collection<Distribution> collection3D(0);
    collection3D.add(Normal(0.0, 1.0));
    collection3D.add(mixture);
    collection3D.add(Uniform(0, 1));
    collection3D.add(Uniform(0, 1));
    // Set weights
    weights = Matrix(3, 4);
    weights(0, 0) = 1.0;
    weights(0, 1) = -0.05;
    weights(0, 2) = 1.0;
    weights(0, 3) = -0.5;
    weights(1, 0) = 0.5;
    weights(1, 1) = 1.0;
    weights(1, 2) = -0.05;
    weights(1, 3) = 0.3;
    weights(2, 0) = -0.5;
    weights(2, 1) = -0.1;
    weights(2, 2) = 1.2;
    weights(2, 3) = -0.8;
    // Defining RandomMixture
    RandomMixture dist_3D(collection3D, weights);
    mean = dist_3D.getMean();
    sigma = dist_3D.getStandardDeviation();
    fullprint << "distribution = " << dist_3D << std::endl;
    fullprint << "distribution = " << dist_3D.__str__() << std::endl;
    fullprint << "range = " << dist_3D.getRange() << std::endl;
    fullprint << "mean = " <<  dist_3D.getMean() << std::endl;
    fullprint << "cov = " << dist_3D.getCovariance() << std::endl;
    fullprint << "sigma = " << dist_3D.getStandardDeviation() << std::endl;
    xMin = mean - 2.9 * sigma;
    xMax = mean + 2.9 * sigma;
    fullprint << "xMin = " << xMin << std::endl;
    fullprint << "xMax = " << xMax << std::endl;
    result = dist_3D.computePDF(xMin, xMax, points, grid);
    for (UnsignedInteger i = 0; i < grid.getSize(); ++i)
      fullprint << grid[i][0] << ";" << grid[i][1] << ";" << grid[i][2] << ";" << result[i][0] << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
