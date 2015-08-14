//                                               -*- C++ -*-
/**
 *  @brief The test file of class Histogram for standard methods
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    UnsignedInteger collectionSize(4);
    Histogram::HistogramPairCollection collection(collectionSize);
    collection[0] = HistogramPair(1.0, 0.5);
    collection[1] = HistogramPair(0.7, 1.5);
    collection[2] = HistogramPair(1.2, 3.5);
    collection[3] = HistogramPair(0.9, 2.5);
    Histogram distribution(-1.5, collection);
    UnsignedInteger size(10000);
    NumericalSample sample(distribution.getSample(size));
    HistogramFactory factory;
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    Histogram estimatedHistogram(factory.buildAsHistogram(sample));
    fullprint << "Histogram          =" << distribution << std::endl;
    fullprint << "Estimated histogram=" << estimatedHistogram << std::endl;
    estimatedHistogram = factory.buildAsHistogram();
    fullprint << "Default histogram=" << estimatedHistogram << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
