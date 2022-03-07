#include <iostream>
#include <algorithm>
#include <string>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "openturns/NAISResult.hxx"
#include "openturns/NAIS.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/Less.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/SymbolicFunction.hxx"

using namespace OT;

int main()
{
  NAISResult naisResult;
  naisResult.setProbabilityEstimate(0.67);
  OT::Test::assert_almost_equal(naisResult.getProbabilityEstimate(), 0.67);
  
  // Definition of function
  const String formula = "min(3+(0.1*pow(a-b,2))-((a+b)/(sqrt(2))),3+(0.1*pow(a-b,2))+((a+b)/(sqrt(2))),(a-b)+(c/ (sqrt(2))),(b-a)+(c/(sqrt(2))))";
  
  // Definition about input parameter of function
  Description input(3);
  input[0] = "a";
  input[1] = "b";
  input[2] = "c";

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
  const int numberSamples= 10 ;
  const int blockSize = 1 ;
  
  // Quantile determining the percentage of failure samples in the current population
  const float rhoQuantile = 0.25 ;

  // Definition of the algoritm
  NAIS NAIS_algo = NAIS(event,numberSamples,blockSize,rhoQuantile);

  // Run of the algorithm
  NAIS_algo.run();

  const NAISResult NAIS_result = NAIS_algo.getResult();
  OT::Test::assert_almost_equal(NAIS_result.getProbabilityEstimate(), 5.25234e-05);
  /*const Point pointTest(0.6075705477671911);
  const Sample sampleTest(pointTest);
  OT::Test::assert_almost_equal(NAIS_result.getAuxiliarySample()[0][0], sampleTest);*/
  
  return 0;
}

