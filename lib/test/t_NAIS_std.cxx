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
  NAISResult a;
  a.setProbabilityEstimate(0.67);
  std::cout << a.getProbabilityEstimate() << std::endl;


	String formula;
	/*formula += "float g1 = 3+(0.1*std::pow(a-b,2))-((a+b)/(std::sqrt(2)));";
	formula += "float g2 = 3+(0.1*std::pow(a-b,2))+((a+b)/(std::sqrt(2)));";
	formula += "float g3 = (a-b)+(c/(std::sqrt(2)));";
	formula += "float g4 = (b-a)+(c/(std::sqrt(2)));";
	formula += "return std::min({g1,g2,g3,g4});";*/
	formula = "min(3+(0.1*pow(a-b,2))-((a+b)/(sqrt(2))),3+(0.1*pow(a-b,2))+((a+b)/(sqrt(2))),(a-b)+(c/(sqrt(2))),(b-a)+(c/(sqrt(2))))";
	// Definition de la pythonfunction generale
	std::string string_variable = "a";
	string_variable.append("b");
	string_variable.append("c");
	String input_variables = string_variable;

	Description input(3);
    	input[0] = "a";
    	input[1] = "b";
    	input[2] = "c";

	Function my_four_branch = SymbolicFunction(input, Description(1, formula));//formula);


	// Transformation de la pythonfunction vers parametricfunction en figeant le parametre k
	Indices index_frozen = Indices(1,2);
	Point identif_point6(1,6);
	Point identif_point7(1,7);
	ParametricFunction my_four_branch_7(my_four_branch, index_frozen, identif_point7);

	// Definition of input variable PDF
	int dim_inputs = 2;
	Point mean(2,(0.0, 0.0)) ;
	Point sigma(2,(1.0, 1.0)) ;
	Normal dist_x(mean, sigma, CorrelationMatrix(dim_inputs));
	RandomVector inputVector = RandomVector(dist_x);

  	// Determination of reference probability
  	// MonteCarlo experiment
  	int n_MC = 1000000;

  	// Creation of event
  	RandomGenerator::SetSeed(1);

  	RandomVector vect = RandomVector(dist_x);
  	CompositeRandomVector G = CompositeRandomVector(my_four_branch_7, vect);
  	ThresholdEvent event = ThresholdEvent(G, Less(), 0.0);
	
	// create a Monte Carlo algorithm
	MonteCarloExperiment experiment = MonteCarloExperiment();
	ProbabilitySimulationAlgorithm algo = ProbabilitySimulationAlgorithm(event, experiment);
	algo.setMaximumOuterSampling(n_MC);
	algo.run();

	// retrieve results
	ProbabilitySimulationResult result = algo.getResult();
	Scalar probability = result.getProbabilityEstimate();
	std::cout << "Pf= " << probability << std::endl;
  		
  	// Hyperparameters of the algorithm
  	// Number of samples at each iteration
  	int n_IS= 10 ;
  	// Quantile determining the percentage of failure samples in the current population
  	float rho_quantile = 0.25 ;
	
  	// Definition of the algoritm
  	NAIS NAIS_algo = NAIS(event,n_IS,rho_quantile);
	
  	// Run of the algorithm
  	NAIS_algo.run();
  	
  	NAISResult NAIS_result = NAIS_algo.getResult();
	std::cout << "Probability of failure: " << NAIS_result.getProbabilityEstimate()<< std::endl;
  	std::cout << "Samples: " << NAIS_result.getSamples()<< std::endl;

  return 0;
}

