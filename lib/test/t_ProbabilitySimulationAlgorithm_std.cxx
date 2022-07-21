//                                               -*- C++ -*-
/**
 *  @brief The test file of ProbabilitySimulationAlgorithm class
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

static Bool stop(void * )
{
  std::cerr << "-- stop?" << std::endl;
  return false;
}

static void progress(Scalar percent, void * )
{
  std::cerr << "-- progress=" << percent << "%" << std::endl;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);


  try
  {
    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    SymbolicFunction myFunction(input, Description(1, "-F*L^3/(3*E*I)"));

    UnsignedInteger dim = myFunction.getInputDimension();
    /* We create a normal distribution point of dimension 1 */
    Point mean(dim, 0.0);
    mean[0] = 50.0; // E
    mean[1] =  1.0; // F
    mean[2] = 10.0; // L
    mean[3] =  5.0; // I
    Point sigma(dim, 1.0);
    IdentityMatrix R(dim);
    Normal myDistribution(mean, sigma, R);

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    CompositeRandomVector output(myFunction, vect);

    /* We create an Event from this RandomVector */
    ThresholdEvent myEvent(output, Less(), -3.0);

    Collection<WeightedExperiment> experiments(1, MonteCarloExperiment());
    // qmc
    experiments.add(LowDiscrepancyExperiment());
    // importance sampling
    mean[0] = 4.99689645939288809018e+01;
    mean[1] = 1.84194175946153282375e+00;
    mean[2] = 1.04454036676956398821e+01;
    mean[3] = 4.66776215562709406726e+00;
    Normal myImportance(mean, sigma, R);
    experiments.add(ImportanceSamplingExperiment(myImportance));
    // randomized qmc
    LowDiscrepancyExperiment lde;
    lde.setRandomize(true);
    experiments.add(lde);
    // randomized lhs
    LHSExperiment lhse;
    lhse.setAlwaysShuffle(true);
    experiments.add(lhse);

    for (UnsignedInteger i = 0; i < experiments.getSize(); ++ i)
    {
      RandomGenerator::SetSeed(0);

      /* We create a Monte Carlo algorithm */
      ProbabilitySimulationAlgorithm myAlgo(myEvent, experiments[i]);
      myAlgo.setMaximumOuterSampling(250);
      myAlgo.setBlockSize(4);
      myAlgo.setMaximumCoefficientOfVariation(0.1);
      myAlgo.setProgressCallback(&progress);
      myAlgo.setStopCallback(&stop);

      fullprint << "algo=" << myAlgo << std::endl;

      /* Perform the simulation */
      myAlgo.run();

      /* Stream out the result */
      fullprint << "algo result=" << myAlgo.getResult() << std::endl;
      fullprint << "Confidence length at level 99%=" << myAlgo.getResult().getConfidenceLength(0.99) << std::endl;
      fullprint << "Confidence length at level 80%=" << myAlgo.getResult().getConfidenceLength(0.8) << std::endl;
      fullprint << "Probability distribution=" << myAlgo.getResult().getProbabilityDistribution() << std::endl;

      /* Use the standard deviation as a stoping rule */
      myAlgo = ProbabilitySimulationAlgorithm(myEvent, experiments[i]);
      myAlgo.setMaximumOuterSampling(250);
      myAlgo.setBlockSize(4);
      myAlgo.setMaximumCoefficientOfVariation(0.0);
      myAlgo.setMaximumStandardDeviation(0.1);

      fullprint << "algo=" << myAlgo << std::endl;

      /* Perform the simulation */
      myAlgo.run();

      /* Stream out the result */
      fullprint << "algo result=" << myAlgo.getResult() << std::endl;
      fullprint << "Probability distribution=" << myAlgo.getResult().getProbabilityDistribution() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
