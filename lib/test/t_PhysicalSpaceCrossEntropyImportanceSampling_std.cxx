//                                               -*- C++ -*-
/**
 *  @brief The test file of class PhysicalSpaceCrossEntropyResult and CrossEntropyResult for standard methods
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

int main()
{
  // Definition of function
  const String formula = "R-F";

  // Definition about input parameter of function
  const Description input = {"R", "F"};

  const Function beam = SymbolicFunction(input, Description(1, formula));
  // Definition of input variable PDF
  const Point mean = {0.2, 0.0};

  const Point sigma = {0.1, 0.1};

  const Normal distX(mean, sigma);

  const RandomVector inputVector = RandomVector(distX);
  const CompositeRandomVector compositeVector = CompositeRandomVector(beam, inputVector);
  const ThresholdEvent event = ThresholdEvent(compositeVector, Less(), 0.0);

  Normal aux_distrib(mean, sigma);

  const Indices active_parameters = {0, 1, 2, 3};

  const Point lowerBnd = {0.1, 0.05, 0.0, 0.05};

  const Point  upperBnd = {0.3, 0.2, 0.3, 0.2};

  const Interval bounds(lowerBnd, upperBnd);

  const Point initial_theta = {0.2, 0.1, 0.0, 0.1};

  // Definition of the algorithm
  PhysicalSpaceCrossEntropyImportanceSampling algoPhysicalSpaceCE(event,
      aux_distrib,
      active_parameters,
      initial_theta,
      bounds);

  // Run of the algorithm
  RandomGenerator::SetSeed(1);

  algoPhysicalSpaceCE.run();

  const CrossEntropyResult resultCE(algoPhysicalSpaceCE.getResult());

  OT::Test::assert_almost_equal(resultCE.getProbabilityEstimate(), 0.078949);
  OT::Test::assert_almost_equal(resultCE.getCoefficientOfVariation(), 0.093684);
  OT::Test::assert_almost_equal(resultCE.getStandardDeviation(), 0.00739625);
  OT::Test::assert_almost_equal(resultCE.getVarianceEstimate(), 0.0000547);
  return 0;
}
