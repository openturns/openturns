//                                               -*- C++ -*-
/**
 *  @brief The test file of class PhysicalSpaceCrossEntropyResult and CrossEntropyResult for standard methods
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

int main()
{
  
  // Definition of function
  const String formula = "R-F";

  // Definition about input parameter of function
  const Description input = {"R", "F"};

  const Function beam = SymbolicFunction(input, Description(1, formula));
  // Definition of input variable PDF
  Point mean(2);
  mean[0] = 0.2;
  mean[1] = 0.0;
  
  Point sigma(2) ;
  sigma[0] = 0.1;
  sigma[1] = 0.1;
    

  const Normal distX(mean, sigma);

  const RandomVector inputVector = RandomVector(distX);
  const CompositeRandomVector compositeVector = CompositeRandomVector(beam, inputVector);
  const ThresholdEvent event = ThresholdEvent(compositeVector, Less(), 0.0);

  Normal aux_distrib(mean, sigma);
  
  Point active_parameters(4);
  active_parameters[0]=0;
  active_parameters[1]=1;
  active_parameters[2]=2;
  active_parameters[3]=3;
  
  Point lowerBnd(4);
  
  lowerBnd[0]=0.1;
  lowerBnd[1]=0.05;
  lowerBnd[2]=0.;
  lowerBnd[3]=0.05;
  
  Point upperBnd(4);
  
  upperBnd[0]=0.3;
  upperBnd[1]=0.2;
  upperBnd[2]=0.3;
  upperBnd[3]=0.2;  

  const Interval bounds(lowerBnd,upperBnd);
                     
  Point initial_theta(4);
  initial_theta[0]=0.2;
  initial_theta[1]=0.1;
  initial_theta[2]=0.;
  initial_theta[3]=0.1;

  // Definition of the algorithm
  PhysicalSpaceCrossEntropyImportanceSampling algoPhysicalSpaceCE(event,
                                                                  aux_distrib,
                                                                  active_parameters,
                                                                  initial_theta,
                                                                  bounds,;
  
  // Run of the algorithm           
  RandomGenerator::SetSeed(1); 

  algoPhysicalSpaceCE.run();

  const CrossEntropyResult resultCE(algoPhysicalSpaceCE.getResult());

  OT::Test::assert_almost_equal(resultCE.getProbabilityEstimate(), 0.078949);
  return 0;
}
