//                                               -*- C++ -*-
/**
 *  @brief The test file of the LatentVariableModel class.
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
  RandomGenerator::SetSeed(0);
  try
  {
	LatentVariableModel k = LatentVariableModel(4, 3);
	k.setLatentVariables(Point({0.1, 0.2, 0.3, -0.1, -0.2, -0.3, 0.4}));
	k.setScale(Point({1.5}));
	k.setAmplitude(Point({2.0}));
	assert_almost_equal(k(1, 1)(0, 0), 4.0);
	assert_almost_equal(k(1, 2)(0, 0), 3.903408);
	assert_almost_equal(k(0, 3)(0, 0), 3.750353);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
