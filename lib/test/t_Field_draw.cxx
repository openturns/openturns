//                                               -*- C++ -*-
/**
 *  @brief The test file of class Field for drawing methods
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

typedef Collection<TimeSeries> TimeSeriesCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // A 1D->1D field
    {
      Mesh mesh(IntervalMesher(Indices(1, 10)).build(Interval(-2.0, 2.0)));
      SymbolicFunction function("x", "x");
      Field field(mesh, function(mesh.getVertices()));
      Graph graph(field.draw());
      graph = field.drawMarginal(0, false);
      graph = field.drawMarginal(0, true);
    }
    // A 2D->1D field
    {
      Mesh mesh(IntervalMesher(Indices(2, 10)).build(Interval(Point(2, -2.0), Point(2, 2.0))));
      SymbolicFunction function(Description::BuildDefault(2, "x"), Description(1, "x0+x1"));
      Field field(mesh, function(mesh.getVertices()));
      Graph graph(field.draw());
      graph = field.drawMarginal(0, false);
      graph = field.drawMarginal(0, true);
    }
    // A 2D->2D field
    {
      Mesh mesh(IntervalMesher(Indices(2, 10)).build(Interval(Point(2, -2.0), Point(2, 2.0))));
      SymbolicFunction function(Description::BuildDefault(2, "x"), Description::BuildDefault(2, "x"));
      Field field(mesh, function(mesh.getVertices()));
      Graph graph(field.draw());
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
