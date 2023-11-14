//                                               -*- C++ -*-
/**
 *  @brief The test file of Graph class for EDF case number 9
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

    // Instantiate one distribution object
    UnsignedInteger dim = 2;
    Point meanPoint(dim, 1.0);
    meanPoint[0] = 12.0;
    meanPoint[1] = 5.0;
    Point sigma(dim, 1.0);
    sigma[0] = 3.0;
    sigma[1] = 1.0;
    CorrelationMatrix R = IdentityMatrix(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.75;
    }


    Normal distribution(meanPoint, sigma, R);

    // Test for sampling
    UnsignedInteger size = 200;
    Sample sample(distribution.getSample( size ));
    Sample tmp(2, Point(2));
    tmp[0][0] = sample.getMin()[0];
    tmp[0][1] = meanPoint[1] - sigma[1] / sigma[0] * (meanPoint[0] - tmp[0][0]);
    tmp[1][0] = sample.getMax()[0];
    tmp[1][1] = meanPoint[1] - sigma[1] / sigma[0] * (meanPoint[0] - tmp[1][0]);

    // Create the cloud
    Cloud myCloud(sample, "blue", "fsquare", "Normal Cloud");

    // Create the line
    Curve myLine(tmp, "red", "solid", 2, "Line");

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

    Point centre(2);
    centre[0] = 7.0;
    centre[1] = 10.0;
    // Create the piechart
    // wingdi.h defines a Pie(...) function
    OT::Pie myPie(data, labels, centre, 4, palette);

    // Create an empty graph
    Graph myGraph("Cloud/line/pie", "x1", "x2", true, "topright", 1.0);

    // Then, draw it
    myGraph.add(myCloud);
    myGraph.add(myLine);
    myGraph.add(myPie);

    // Test if a drawable can be extracted from a graph to be added to another graph
    Graph firstGraph("Overall graph", "x", "y", true, "topright");
    firstGraph.add(myCloud);
    Graph secondGraph("temporary graph", "x", "y", true, "topright");
    secondGraph.add(myLine);
    // Extract the drawable from the second graph and add it to the first graph
    firstGraph.add(secondGraph.getDrawables()[0]);

    // Add drawables with default colors
    Curve myCurve(Point({ 0.0, 1.0, 2.0 }), Point({ 1.0, 3.0, 2.0 }));
    firstGraph.add(myCurve);
    secondGraph.add(Graph::DrawableCollection({ myCurve, myCloud }));

    //  Set new colors
    Description colors(0);
    colors.add("red");
    colors.add("green");
    colors.add("#120345");
    myGraph.setColors(colors);

    // Test empty new colors
    myGraph.setColors(Description());

    // Set new legends
    Description legends(0);
    legends.add("item1");
    legends.add("item2");
    legends.add("item3");
    myGraph.setLegends(legends);

    // Test empty new legends
    myGraph.setLegends(Description());
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
