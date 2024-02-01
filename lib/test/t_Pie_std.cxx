//                                               -*- C++ -*-
/**
 *  @brief The test file of class Pie for standard methods
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

    Point data(0);
    Description palette;
    Description labels;

    data.add(5665855);
    labels.add("chirac");
    palette.add("blue");

    data.add(4804713);
    labels.add("lepen");
    palette.add("white");

    data.add(4610113);
    labels.add("jospin");
    palette.add("pink");

    data.add(1949170);
    labels.add("bayrou");
    palette.add("yellow");

    data.add(1630045);
    labels.add("laguiller");
    palette.add("red");

    data.add(1518528);
    labels.add("chevenement");
    palette.add("ivory");

    data.add(1495724);
    labels.add("mamere");
    palette.add("green");

    data.add(1210562);
    labels.add("besancenot");
    palette.add("orange");

    data.add(1204689);
    labels.add("saint-josse");
    palette.add("maroon");

    data.add(1113484);
    labels.add("madelin");
    palette.add("violet");

    data.add(3295588);
    labels.add("cumul candidats < 3.5% ");
    palette.add("purple");


    // Create an empty graph
    Graph myGraph("Presidentielle 2002 - 1er tour", "", "", false, "");

    // Create the first cloud
    //  wingdi.h defines a Pie(...) function
    OT::Pie myPie1(data, labels, Point(2), 1, palette);

    // Then, draw it
    myGraph.add(myPie1);

    // Check that the correct files have been generated by computing their checksum
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
