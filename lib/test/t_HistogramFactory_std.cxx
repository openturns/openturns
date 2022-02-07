//                                               -*- C++ -*-
/**
 *  @brief The test file of class Histogram for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger collectionSize = 4;
    Point l(collectionSize);
    l[0] = 1.0;
    l[1] = 0.7;
    l[2] = 1.2;
    l[3] = 0.9;
    Point h(collectionSize);
    h[0] = 0.5;
    h[1] = 1.5;
    h[2] = 3.5;
    h[3] = 2.5;
    Histogram distribution(-1.5, l, h);
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    HistogramFactory factory;
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    fullprint << "Bandwidth=" << factory.computeBandwidth(sample) << std::endl;
    {
      Histogram estimatedHistogram(factory.buildAsHistogram(sample));
      fullprint << "Histogram          =" << distribution << std::endl;
      fullprint << "Estimated histogram=" << estimatedHistogram << std::endl;
    }
    {
      Histogram estimatedHistogram(factory.buildAsHistogram(sample, 0.1));
      fullprint << "Histogram          =" << distribution << std::endl;
      fullprint << "Estimated histogram=" << estimatedHistogram << std::endl;
    }
    {
      Histogram estimatedHistogram(factory.buildAsHistogram(sample, UnsignedInteger(15)));
      fullprint << "Histogram          =" << distribution << std::endl;
      fullprint << "Estimated histogram=" << estimatedHistogram << std::endl;
    }
    Histogram estimatedHistogram(factory.buildAsHistogram());
    fullprint << "Default histogram=" << estimatedHistogram << std::endl;

    // Build from first and width
    // 1. Create a Sample
    Sample newsample(Normal().getSample(100));
    // 2. Set the parameters first and width
    Scalar first = newsample.getMin()[0];
    UnsignedInteger binNumber = 10;
    Scalar binwidth = 1.;
    Point width = Point(binNumber, binwidth);
    // 3. Create a new histogram from first and width
    Distribution newDistribution(factory.build(newsample, first, width));
    fullprint << "New histogram=" << newDistribution << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
