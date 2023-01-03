//                                               -*- C++ -*-
/**
 *  @brief The test file of class InverseTrendTransform for standard methods
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

    /* Create an intance */
    Description inputVars;
    inputVars.add("t");

    Description formula;
    formula.add("sin(t)");
    formula.add("cos(t)");
    SymbolicFunction myFunc(inputVars, formula);
    RegularGrid tg(0.0, 0.1, 11);
    InverseTrendTransform myInverseTrendFunc(myFunc, tg);

    fullprint << "myInverseTrendFunc=" << myInverseTrendFunc << std::endl;
    fullprint << "myInverseTrendFunc input description=" << myInverseTrendFunc.getInputDescription() << std::endl;
    fullprint << "myInverseTrendFunc input dimension=" << myInverseTrendFunc.getInputDimension() << std::endl;

    fullprint << "myInverseTrendFunc output description=" << myInverseTrendFunc.getOutputDescription() << std::endl;
    fullprint << "myInverseTrendFunc output dimension=" << myInverseTrendFunc.getOutputDimension() << std::endl;

    /* Create a TimeSeries */
    Sample data(tg.getN(), 2);
    for (UnsignedInteger i = 0; i < data.getSize(); ++i)
    {
      const Scalar instant = tg.getStart() + i * tg.getStep();
      data(i, 0) = i + 1 + sin(instant);
      data(i, 1) = i + cos(instant);
    }
    TimeSeries ts(tg, data);
    fullprint << "input time series=" << ts << std::endl;
    fullprint << "output time series=" << myInverseTrendFunc(ts.getValues()) << std::endl;
    /* Get the number of calls */
    fullprint << "called " << myInverseTrendFunc.getCallsNumber() << " times" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
