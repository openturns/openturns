//                                               -*- C++ -*-
/**
 *  @brief The test file of class BootstrapExperiment for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger size = 5;
    UnsignedInteger dim = 6;
    Sample refSample(size, dim);
    for (UnsignedInteger i = 0; i < size; ++i)
      for (UnsignedInteger j = 0; j < dim; ++j)
        refSample[i][j] = i + j;
    fullprint << "ref. sample=" << refSample << std::endl;
    BootstrapExperiment experiment(refSample);
    fullprint << "experiment = " << experiment << std::endl;
    Point weights(0);
    Sample sample(experiment.generateWithWeights(weights));
    fullprint << "sample = " << sample << std::endl;
    fullprint << "weights = " << weights << std::endl;
    fullprint << "selection=" << BootstrapExperiment::GenerateSelection(size, size) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
