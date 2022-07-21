//                                               -*- C++ -*-
/**
 *  @brief The test file of Graph class for EDF case number 1
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger maxPoint = 101;
    Scalar xMin = -2.0 * M_PI;
    Scalar xMax = 2.0 * M_PI;
    Sample data(maxPoint, 2);
    for (UnsignedInteger i = 0; i < maxPoint; ++i)
    {
      Scalar x = xMin + (xMax - xMin) * i / (maxPoint - 1.0);
      data[i][0] = x;
      data[i][1] = sin(x);
    }
    Graph sinGraph("Sin function", "x", "y", true, "topright");
    sinGraph.add(Curve(data, "red", "solid", 2, "sin"));
    // Test export to all formats
    sinGraph.draw("sinGraph_ALL", 640, 480, GraphImplementation::ALL);
    // Test export to PNG format
    sinGraph.draw("sinGraph_PNG", 640, 480, GraphImplementation::PNG);
    // Test export to EPS format
    sinGraph.draw("sinGraph_EPS_PDF", 640, 480, GraphImplementation::EPS | GraphImplementation::PDF);
    // Test export to FIG format
    sinGraph.draw("sinGraph_FIG", 640, 480, GraphImplementation::FIG);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
