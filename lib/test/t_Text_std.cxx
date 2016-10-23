//                                               -*- C++ -*-
/**
 *  @brief The test file of class Text for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // Instanciate one distribution object
    UnsignedInteger dim = 2;
    NumericalPoint meanPoint(dim, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    NumericalPoint sigma(dim, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }

    Normal distribution(meanPoint, sigma, R);

    // Test for sampling
    NumericalSample sample(distribution.getSample( 30 ));

    // Create an empty graph
    Graph myGraph("Normal sample", "x1", "x2", true, "topright");

    Cloud myCloud(sample, "blue", "circle", "Cloud");
    // Add it to the graph and draw everything
    myGraph.add(myCloud);

    // Display extrema indices
    NumericalScalar x1min(sample[0][0]);
    NumericalScalar x1max(x1min);
    NumericalScalar x2min(sample[0][1]);
    NumericalScalar x2max(x2min);
    Indices idx(4);
    for (UnsignedInteger i = 1; i < sample.getSize(); ++i)
    {
      const NumericalScalar v1 = sample[i][0];
      const NumericalScalar v2 = sample[i][1];
      if (v1 < x1min)
      {
        x1min = v1;
        idx[0] = i;
      }
      if (v1 > x1max)
      {
        x1max = v1;
        idx[1] = i;
      }
      if (v2 < x2min)
      {
        x2min = v2;
        idx[2] = i;
      }
      if (v2 > x2max)
      {
        x2max = v2;
        idx[3] = i;
      }
    }
    Description labels(sample.getSize());
    for (UnsignedInteger i = 0; i < 4; ++i)
    {
      labels[idx[i]] = (OSS() << idx[i]);
    }
    Description position(sample.getSize(), "top");
    position[idx[0]] = "right";
    position[idx[1]] = "left";
    position[idx[2]] = "top";
    position[idx[3]] = "bottom";

    Text myText(sample, labels);
    myText.setColor("red");
    myText.setTextPositions(position);

    // Add it to the graph and draw everything
    myGraph.add(myText);
    myGraph.draw("Graph_Text_c_OT", 640, 480);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
