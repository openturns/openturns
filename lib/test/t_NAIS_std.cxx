#include <iostream>
#include <algorithm>
#include <string>

#include "openturns/NAISResult.hxx"
#include "openturns/NAIS.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/Less.hxx"
#include "openturns/SymbolicFunction.hxx"

using namespace OT;

int main()
{
  NAISResult naisResult;
  naisResult.setProbabilityEstimate(0.67);
  std::cout << naisResult.getProbabilityEstimate() << std::endl;

  String formula = "min(3+(0.1*pow(a-b,2))-((a+b)/(sqrt(2))),3+(0.1*pow(a-b,2))+((a+b)/(sqrt(2))),(a-b)+(c/ (sqrt(2))),(b-a)+(c/(sqrt(2))))";
  
  // Definition de la pythonfunction generale
  Description input(3);
  input[0] = "a";
  input[1] = "b";
  input[2] = "c";

  Function myfourBranch = SymbolicFunction(input, Description(1, formula));

  // Transformation de la pythonfunction vers parametricfunction en figeant le parametre k
  Indices indexFrozen = Indices(1,2);
  Point identifPoint(1,7);
  ParametricFunction myfourBranchUpdate(myfourBranch, indexFrozen, identifPoint);

  // Definition of input variable PDF
  Point mean(2,(0.0, 0.0)) ;
  Point sigma(2,(1.0, 1.0)) ;
  Normal distX(mean, sigma, CorrelationMatrix(2));
  RandomVector inputVector = RandomVector(distX);

  // Determination of reference probability
  
  // MonteCarlo experiment
  int numberOfMonteCarlo = 1000000;

  // Creation of event
  RandomGenerator::SetSeed(1);

  RandomVector vect = RandomVector(distX);
  CompositeRandomVector compositeVector = CompositeRandomVector(myfourBranchUpdate, vect);
  ThresholdEvent event = ThresholdEvent(compositeVector, Less(), 0.0);

  // Create a Monte Carlo algorithm
  MonteCarloExperiment experiment = MonteCarloExperiment();
  ProbabilitySimulationAlgorithm algo = ProbabilitySimulationAlgorithm(event, experiment);
  algo.setMaximumOuterSampling(numberOfMonteCarlo);
  algo.run();

  // Retrieve results
  ProbabilitySimulationResult result = algo.getResult();
  Scalar probability = result.getProbabilityEstimate();
  std::cout << "Pf= " << probability << std::endl;
	
  // Hyperparameters of the algorithm
  
  // Number of samples at each iteration
  int numberSamples= 10 ;
  
  // Quantile determining the percentage of failure samples in the current population
  float rhoQuantile = 0.25 ;

  // Definition of the algoritm
  NAIS NAIS_algo = NAIS(event,numberSamples,rhoQuantile);

  // Run of the algorithm
  NAIS_algo.run();

  NAISResult NAIS_result = NAIS_algo.getResult();
  std::cout << "Probability of failure: " << NAIS_result.getProbabilityEstimate()<< std::endl;
  std::cout << "Sample: " << NAIS_result.getSample()<< std::endl;

  return 0;
}

