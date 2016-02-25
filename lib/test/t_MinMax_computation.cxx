//                                               -*- C++ -*-
/**
 * @brief The test file of class Box for deterministic MinMax computation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  try
  {
    // Load the physical model
    Description inputVariables(4);
    inputVariables[0] = "E";
    inputVariables[1] = "F";
    inputVariables[2] = "L";
    inputVariables[3] = "I";
    Description outputVariables(1);
    outputVariables[0] = "d";
    Description formula(1);
    formula[0] = "-F*L^3/(3*E*I)";
    NumericalMathFunction model(inputVariables, outputVariables, formula);
    UnsignedInteger inputDimension(model.getInputDimension());
    NumericalPoint levels(inputDimension);
    levels[0] = 5.;
    levels[1] = 5.;
    levels[2] = 5.;
    levels[3] = 5.;
    Box myPlane(levels);
    NumericalSample sample(myPlane.generate());
    // Then, scale and move the cube to sample around the point of interrest
    NumericalPoint point(inputDimension);
    point[0] = 2.1e11;
    point[1] = 1e3;
    point[2] = 1.5;
    point[3] = 2.e-6;
    sample *= (0.2 * point);
    sample += (0.9 * point);
    // Compute the model over the sample
    NumericalSample response(model(sample));
    // Compute the min and max values taken by the model
    fullprint << "Min=" << response.getMin()[0] << std::endl;
    fullprint << "Max=" << response.getMax()[0] << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
