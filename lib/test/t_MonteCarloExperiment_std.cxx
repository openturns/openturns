//                                               -*- C++ -*-
/**
 *  @brief The test file of class MonteCarloExperiment for standard methods
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

  fullprint << "Test generateWithWeights()" << std::endl;
  const Normal distribution(4);
  const UnsignedInteger size = 100000;
  const MonteCarloExperiment experiment(distribution, size);
  fullprint << "experiment = " << experiment << std::endl;
  Point weights(0);
  const Sample sample(experiment.generateWithWeights(weights));
  assert(sample.getSize() == size);
  assert(sample.getDimension() == 4);
  assert(weights.getDimension() == size);
  const Scalar atol = 10.0 / std::sqrt(size);
  const Scalar rtol = 0.0;
  const Point meanExact(distribution.getMean());
  assert_almost_equal(sample.computeMean(), meanExact, rtol, atol);
  const CovarianceMatrix covarianceExact(distribution.getCovariance());
  assert_almost_equal(sample.computeCovariance(), covarianceExact, rtol, atol);
  const Point weightsExact(size, 1.0 / size);
  assert_almost_equal(weights, weightsExact, 1.e-15);


  return ExitCode::Success;
}
