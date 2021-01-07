//                                               -*- C++ -*-
/**
 *  @brief The test file of class RsenblattEvaluation for standard methods
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
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Scalar nu = 4.5;
    UnsignedInteger dim = 5;
    Student distribution(nu, dim);
    RosenblattEvaluation transformation(distribution);
    fullprint << "transformation=" << transformation << std::endl;
    Point point(dim, 0.75);
    fullprint << "transformation(" << point.__str__() << ")=" << transformation(point).__str__() << std::endl;
    fullprint << "transformation parameters gradient=" << transformation.parameterGradient(point) << std::endl;
    fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
    fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
