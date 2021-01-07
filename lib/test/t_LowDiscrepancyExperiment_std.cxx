//                                               -*- C++ -*-
/**
 *  @brief The test file of class LowDiscrepancyExperiment for standard methods
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

  try
  {
    Normal distribution(4);
    distribution.setMean(Point(4, 5.0));
    UnsignedInteger size = 16;
    LowDiscrepancyExperiment experiment(HaltonSequence(), distribution, size);
    fullprint << "experiment = " << experiment << std::endl;
    // Test sampling with weights
    Point weights(0);
    Sample sample(experiment.generateWithWeights(weights));
    fullprint << "sample  = " << sample << std::endl;
    fullprint << "weights = " << weights << std::endl;

    // Test sampling with reinitialization each time the distribution is
    // set (default behaviour)
    // sample 2 != sample
    fullprint << "sample 2=" << experiment.generate() << std::endl;
    experiment.setDistribution(distribution);
    // sample 3 == sample
    fullprint << "sample 3=" << experiment.generate() << std::endl;

    // Test sampling without reinitialization excepted when distribution
    // dimension changes
    experiment = LowDiscrepancyExperiment(HaltonSequence(), distribution, size, false);
    fullprint << "sample  =" << experiment.generate() << std::endl;
    // sample 2 != sample
    fullprint << "sample 2=" << experiment.generate() << std::endl;
    experiment.setDistribution(distribution);
    // sample 3 != sample && sample 3 != sample 2
    fullprint << "sample 3=" << experiment.generate() << std::endl;
    // Test dimension change
    experiment.setDistribution(Normal());
    fullprint << "sample =" << experiment.generate() << std::endl;

    // Test constructor with no distribution and dimension>1
    experiment = LowDiscrepancyExperiment(HaltonSequence(2), size);
    fullprint << "sample = " << experiment.generate() << std::endl;

    // Test with dependent marginals
    CorrelationMatrix R(4);
    for (UnsignedInteger i = 1; i < 4; ++i)
      R(i - 1, i) = 0.5;
    distribution.setCorrelation(R);
    experiment = LowDiscrepancyExperiment(HaltonSequence(), distribution, size, false);
    fullprint << "sample = " << experiment.generate() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
