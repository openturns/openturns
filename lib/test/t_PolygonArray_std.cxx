//                                               -*- C++ -*-
/**
 *  @brief The test file of class PolygonArray for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Generate the data for the polygonArrays to be drawn
    Normal generator(2);
    UnsignedInteger size = 50;
    Collection<OT::Polygon > array(size);
    Description palette(Drawable::BuildDefaultPalette(size));
    palette[8] = "blue";
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      Sample vertices(generator.getSample(3));
      array[i] = OT::Polygon(vertices, palette[i], palette[size - i - 1]);
    }
    Graph graph("An array of polygons", "x", "y", true, "");
    PolygonArray polygonArray(array);
    std::cout << "palette" << polygonArray.getPalette() << std::endl;
    std::cout << "palette (RGBA)" << polygonArray.getPaletteAsNormalizedRGBA() << std::endl;
    graph.add(polygonArray);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
