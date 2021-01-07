//                                               -*- C++ -*-
/**
 *  @brief The test file of class FiniteDifferenceStep for standard methods
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
    UnsignedInteger dimension = 2;
    Point epsilon( dimension, 1e-5 );
    Point eta( dimension, 1.0 );
    Collection<FiniteDifferenceStep> coll;
    coll.add(ConstantStep(epsilon));
    coll.add(BlendedStep(epsilon, eta));


    Point x(dimension, -3.0);

    for (UnsignedInteger i = 0; i < coll.getSize(); ++ i)
    {
      FiniteDifferenceStep step_i( coll[i] );
      std::cout << "step type=" << step_i.getImplementation()->getClassName() << " x=" << x << " epsilon=" << step_i.getEpsilon() << " step value=" << step_i.operator()(x) << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
