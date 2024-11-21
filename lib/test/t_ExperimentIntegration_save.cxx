//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExperimentIntegration for standard methods
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

  // Create the input distribution
  const UnsignedInteger dimension = 3;
  fullprint << "Create the input distribution" << std::endl;
  const Collection<Distribution> marginals(dimension, Uniform(-M_PI, M_PI));
  const JointDistribution distributionIshigami(marginals);
  const UnsignedInteger sampleSize = 100;
  const MonteCarloExperiment experiment2(distributionIshigami, sampleSize);
  const ExperimentIntegration integration(experiment2);

  // Save
  fullprint << "Save" << std::endl;
  const char fileName[] = "myStudy.xml";
  Study study(fileName);
  study.add("integration", integration);
  fullprint << "study =" << study << std::endl;
  study.save();

  // Load
  fullprint << "Load" << std::endl;
  Study study2(fileName);
  study2.load();
  fullprint << "study2 =" << study2 << std::endl;
  ExperimentIntegration integration2;
  study2.fillObject("integration", integration2);

  // cleanup
  Os::Remove(fileName);

  return ExitCode::Success;
}
