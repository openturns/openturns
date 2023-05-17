//                                               -*- C++ -*-
/**
 *  @brief The test file of class BoxCoxFactory for standard methods
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
  setRandomGenerator();

  try
  {
    // TimeGrid parameters
    const UnsignedInteger n = 101;
    const Scalar timeStart = 0.0;
    const Scalar timeStep = 0.1;
    RegularGrid timeGrid(timeStart, timeStep, n);

    // White noise
    const WhiteNoise whiteNoise(Uniform(), timeGrid);

    // Composite process
    const CompositeProcess process(ValueFunction(SymbolicFunction("x", "x+2"), timeGrid), whiteNoise);
    // A realization of the process
    const TimeSeries timeSeries(process.getRealization());
    const Sample sample(timeSeries.getValues());

    // Now we build the factory
    BoxCoxFactory factory;

    // Creation of the BoxCoxTransform
    BoxCoxTransform myBoxCox(factory.build(timeSeries));

    fullprint << "myBoxCox (time-series)=" << myBoxCox << std::endl;
    fullprint << "myBoxCox (sample)     =" << factory.build(sample) << std::endl;

    // Creation of the BoxCoxTransform using shift
    Point shift(1, 1.0);
    BoxCoxTransform myBoxCoxShift(factory.build(timeSeries, shift));

    fullprint << "myBoxCox with shift (time-series)=" << myBoxCoxShift << std::endl;
    fullprint << "myBoxCox with shift (sample)     =" << factory.build(sample, shift) << std::endl;

    // Creation of the BoxCoxTransform using shift with graph
    Graph graph;
    BoxCoxTransform myBoxCoxShiftGraph(factory.buildWithGraph(timeSeries, shift, graph));

    fullprint << "BoxCox graph (time-series)=" << graph << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
