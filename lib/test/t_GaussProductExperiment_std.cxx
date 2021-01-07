//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussProductExperiment for standard methods
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
    Collection<Distribution> marginals(0);
    marginals.add(Exponential());
    marginals.add(Triangular(-1.0, -0.5, 1.0));
    ComposedDistribution distribution(marginals);
    Indices marginalDegrees(0);
    marginalDegrees.add(3);
    marginalDegrees.add(6);
    // Test the constructor based on marginal degrees
    fullprint << "experiment1=" << GaussProductExperiment(marginalDegrees) << std::endl;
    // Test the constructor based on distribution
    fullprint << "experiment2=" << GaussProductExperiment(distribution) << std::endl;
    // Test the constructor based on marginal degrees and distribution
    GaussProductExperiment experiment(distribution, marginalDegrees);
    fullprint << "experiment = " << experiment << std::endl;
    Point weights(0);
    Sample sample(experiment.generateWithWeights(weights));
    fullprint << "sample = " << sample << std::endl;
    fullprint << "weights = " << weights << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
