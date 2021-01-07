//                                               -*- C++ -*-
/**
 *  @brief The test file of class CompositeProcess
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

  try
  {

    /*2 D case */
    RandomGenerator::SetSeed(0);


    /* Time grid creation */
    Scalar Tmin = 0.0;
    Scalar deltaT = 0.1;
    UnsignedInteger steps = 11;

    /* Initialization of the time grid timeGrid1*/
    RegularGrid timeGrid(Tmin, deltaT, steps);

    /* Default ARMA creation */
    ARMA myARMAProcess;

    /* We fix the time grid to the ARMA process */
    myARMAProcess.setTimeGrid(timeGrid);

    fullprint << "myAntecedent = " << myARMAProcess << std::endl;


    /* We build a 1D function */
    Description inputDescription(1, "x");
    Description formula(1, "2 * x + 5");
    SymbolicFunction myOneDimensionalFunction(inputDescription, formula);

    /* We build a spatial function */
    ValueFunction myFunction(myOneDimensionalFunction, timeGrid);

    /* Definition of the CompositeProcess */
    CompositeProcess myCompositeProcess(myFunction, myARMAProcess);
    fullprint << "myCompositeProcess =" << myCompositeProcess << std::endl;

    // We get a TimeSeries as realization of the CompositeProcess
    const TimeSeries realization(myCompositeProcess.getRealization());
    fullprint << "One ARMA realization=" << realization << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
