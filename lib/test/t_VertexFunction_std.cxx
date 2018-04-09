//                                               -*- C++ -*-
/**
 *  @brief The test file of class VertexFunction for standard methods
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Create an intance */
    Description inputVars;
    inputVars.add("t");
    Description formula(1, "t + t^2");
    SymbolicFunction myFunc(inputVars, formula);
    VertexFunction myVertexFunc(myFunc);

    fullprint << "myVertexFunc=" << myVertexFunc << std::endl;
    /* Get the input and output description */
    fullprint << "myVertexFunc input description=" << myVertexFunc.getInputDescription() << std::endl;
    fullprint << "myVertexFunc output description=" << myVertexFunc.getOutputDescription() << std::endl;
    /* Get the input and output dimension, based on description */
    fullprint << "myVertexFunc input dimension=" << myVertexFunc.getInputDimension() << std::endl;
    fullprint << "myVertexFunc output dimension=" << myVertexFunc.getOutputDimension() << std::endl;
    /* Create a TimeSeries */
    RegularGrid tg(0.0, 0.2, 6);
    Sample data(tg.getN(), myFunc.getInputDimension() - 1);
    for (UnsignedInteger i = 0; i < data.getSize(); ++i)
      for (UnsignedInteger j = 0; j < data.getDimension(); ++j)
        data(i, j) = i * data.getDimension() + j;
    TimeSeries ts(tg, data);
    fullprint << "input time series=" << ts << std::endl;
    fullprint << "output time series=" << myVertexFunc(ts) << std::endl;
    /* Get the number of calls */
    fullprint << "called " << myVertexFunc.getCallsNumber() << " times" << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
