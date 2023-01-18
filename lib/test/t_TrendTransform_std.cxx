//                                               -*- C++ -*-
/**
 *  @brief The test file of class TrendTransform for standard methods
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

  try
  {
    Description inputVars;
    inputVars.add("t");

    Description formula;
    formula.add("sin(t)");
    formula.add("cos(t)");
    SymbolicFunction myFunc(inputVars, formula);
    RegularGrid tg(0.0, 0.1, 11);
    TrendTransform myTrendFunc(myFunc, tg);

    fullprint << "myTrendFunc=" << myTrendFunc << std::endl;
    fullprint << "myTrendFunc input description=" << myTrendFunc.getInputDescription() << std::endl;
    fullprint << "myTrendFunc input dimension=" << myTrendFunc.getInputDimension() << std::endl;

    fullprint << "myTrendFunc output description=" << myTrendFunc.getOutputDescription() << std::endl;
    fullprint << "myTrendFunc output dimension=" << myTrendFunc.getOutputDimension() << std::endl;

    /* Create a TimeSeries */
    Sample data(tg.getN(), 2);
    for (UnsignedInteger i = 0; i < data.getSize(); ++i)
      for (UnsignedInteger j = 0; j < data.getDimension(); ++j)
        data(i, j) = i * data.getDimension() + j;
    TimeSeries ts(tg, data);
    fullprint << "input time series=" << ts << std::endl;
    fullprint << "output time series=" << myTrendFunc(ts.getValues()) << std::endl;
    /* Get the number of calls */
    fullprint << "called " << myTrendFunc.getCallsNumber() << " times" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
