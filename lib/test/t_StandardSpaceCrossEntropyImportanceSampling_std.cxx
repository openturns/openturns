//                                               -*- C++ -*-
/**
 *  @brief The test file of class StandardSpaceCrossEntropyResult and CrossEntropyResult for standard methods
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
  Point mean(2);
  mean[0] = 0.2;
  mean[1] = 0.0;

  Point sigma(2) ;
  sigma[0] = 0.1;
  sigma[1] = 0.1;

  const Normal distX(mean, sigma);

  const RandomVector inputVector = RandomVector(distX);
  const CompositeRandomVector compositeVector = CompositeRandomVector(beam, inputVector);
  const ThresholdEvent event = ThresholdEvent(compositeVector, Less(), 0.0);

  // Definition of the algorithm
  StandardSpaceCrossEntropyImportanceSampling algoStandardSpaceCE(event, 0.2);

  // Run of the algorithm
  RandomGenerator::SetSeed(1);
  algoStandardSpaceCE.run();

  const CrossEntropyResult resultCE(algoStandardSpaceCE.getResult());
  OT::Test::assert_almost_equal(resultCE.getProbabilityEstimate(), 0.0805575);
  OT::Test::assert_almost_equal(resultCE.getCoefficientOfVariation(), 0.0475189);
  OT::Test::assert_almost_equal(resultCE.getStandardDeviation(), 0.003828);
  OT::Test::assert_almost_equal(resultCE.getVarianceEstimate(), 0.00001465);
  return 0;
}
