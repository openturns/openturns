//                                               -*- C++ -*-
/**
 *  @brief The test file of class NAIS and NAISResult for standard methods
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

int main()
{
  NAISResult naisResult;
  naisResult.setProbabilityEstimate(0.67);
  OT::Test::assert_almost_equal(naisResult.getProbabilityEstimate(), 0.67);
  
  // Definition of function
  const String formula = "min(3+(0.1*pow(a-b,2))-((a+b)/(sqrt(2))),3+(0.1*pow(a-b,2))+((a+b)/(sqrt(2))),(a-b)+(c/ (sqrt(2))),(b-a)+(c/(sqrt(2))))";
  
  // Definition about input parameter of function
  const Description input = {"a", "b", "c"};

  const Function myfourBranch = SymbolicFunction(input, Description(1, formula));

  // Transformation of SymbolicFunction to ParametricFunction fixing parameter k
  const Indices indexFrozen = Indices(1,2);
  const Point identifPoint(1,7);
  ParametricFunction myfourBranchUpdate(myfourBranch, indexFrozen, identifPoint);

  // Definition of input variable PDF
  const Point mean(2,0.0) ;
  const Point sigma(2,1.0) ;
  const Normal distX(mean, sigma, CorrelationMatrix(2));
  const RandomVector inputVector = RandomVector(distX);

  // Determination of reference probability
  
  // MonteCarlo experiment
  int numberOfMonteCarlo = 1000000;

  // Creation of event
  RandomGenerator::SetSeed(1);

  const RandomVector vect = RandomVector(distX);
  const CompositeRandomVector compositeVector = CompositeRandomVector(myfourBranchUpdate, vect);
  const ThresholdEvent event = ThresholdEvent(compositeVector, Less(), 0.0);

  // Create a Monte Carlo algorithm
  const MonteCarloExperiment experiment = MonteCarloExperiment();
  ProbabilitySimulationAlgorithm algo = ProbabilitySimulationAlgorithm(event, experiment);
  algo.setMaximumOuterSampling(numberOfMonteCarlo);
  algo.run();

  // Retrieve results
  const ProbabilitySimulationResult result = algo.getResult();
  const Scalar probability = result.getProbabilityEstimate();
  OT::Test::assert_almost_equal(probability, 0.00238288);
  	
  // Hyperparameters of the algorithm
  
  // Number of samples at each iteration
  const UnsignedInteger numberSamples= 10 ;
  const UnsignedInteger blockSize = 1 ;
  
  // Quantile determining the percentage of failure samples in the current population
  const float rhoQuantile = 0.25 ;

  // Definition of the algoritm
  NAIS algoNais(event,rhoQuantile);
  algoNais.setMaximumOuterSampling(numberSamples);
  algoNais.setBlockSize(blockSize);
  
  // Run of the algorithm
  algoNais.run();

  const NAISResult resultNais(algoNais.getResult());
  OT::Test::assert_almost_equal(resultNais.getProbabilityEstimate(), 0.00145074);
  std::cout<< resultNais.getAuxiliarySample() << std::endl;
  return 0;
}

