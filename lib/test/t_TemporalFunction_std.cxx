//                                               -*- C++ -*-
/**
 *  @brief The test file of class TemporalFunction for standard methods
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Create an intance */
    Description inputVars;
    inputVars.add("t");
    inputVars.add("x");
    Description formula(1, "x + t^2");
    NumericalMathFunction myFunc(inputVars, formula);
    TemporalFunction myTemporalFunc(myFunc);

    fullprint << "myTemporalFunc=" << myTemporalFunc << std::endl;
    /* Get the input and output description */
    fullprint << "myTemporalFunc input description=" << myTemporalFunc.getInputDescription() << std::endl;
    fullprint << "myTemporalFunc output description=" << myTemporalFunc.getOutputDescription() << std::endl;
    /* Get the input and output dimension, based on description */
    fullprint << "myTemporalFunc input dimension=" << myTemporalFunc.getInputDimension() << std::endl;
    fullprint << "myTemporalFunc output dimension=" << myTemporalFunc.getOutputDimension() << std::endl;
    /* Create a TimeSeries */
    RegularGrid tg(0.0, 0.2, 6);
    NumericalSample data(tg.getN(), myFunc.getInputDimension() - 1);
    for (UnsignedInteger i = 0; i < data.getSize(); ++i)
      for (UnsignedInteger j = 0; j < data.getDimension(); ++j)
        data(i, j) = i * data.getDimension() + j;
    TimeSeries ts(tg, data);
    fullprint << "input time series=" << ts << std::endl;
    fullprint << "output time series=" << myTemporalFunc(ts) << std::endl;
    /* Get the number of calls */
    fullprint << "called " << myTemporalFunc.getCallsNumber() << " times" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
