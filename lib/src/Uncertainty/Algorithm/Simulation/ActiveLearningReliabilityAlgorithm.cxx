//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityAlgorithm implements general purpose
 *  active learning schem for reliability algorithms
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ActiveLearningReliabilityAlgorithm.hxx"
#include "openturns/GaussianProcessRegression.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
//#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EventSimulation.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/SimulationResult.hxx"
#include "openturns/NAIS.hxx"
#include "openturns/SubsetSampling.hxx"
#include "openturns/StandardSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/PhysicalSpaceCrossEntropyImportanceSampling.hxx"
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityAlgorithm
 */
 
CLASSNAMEINIT(ActiveLearningReliabilityAlgorithm)

//static const Factory<ActiveLearningReliabilityAlgorithm> Factory_ActiveLearningReliabilityAlgorithm;

 /** Default constructor */
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm()
  :EventSimulation()
  {
    // Nothing to do
  }

/** Default constructor */  
/*ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const EventSimulation & reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
  
    
    defaultSimulationAlgorithm_ = new EventSimulation(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new EventSimulation(reliabilityAlgorithm);
    //SimulationResult simResults(reliabilityAlgorithm.getResult());
    currentProbabilitySimulationResult_ = new SimulationResult;//(simResults);
    std::cout<<" Type de l'algo au constructeur, event simulation"<<std::endl;
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
  }*/
  
  
/** Constructor with NAIS */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const NAIS & reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    defaultSimulationAlgorithm_ = new NAIS(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new NAIS(reliabilityAlgorithm);
    //NAIS* currentSimulationAlgorithm_ = dynamic_cast<NAIS*>(currentSimulationAlgorithm_);
    std::cout<<" Type de l'algo au constructeur, NAIS"<<std::endl;    
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
    
    //std::cout<<currentSimulationAlgorithm_->getInputSample()<<std::endl;
  }

/** Constructor with StandardSpaceCrossEntropyImportanceSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const StandardSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    defaultSimulationAlgorithm_ = new StandardSpaceCrossEntropyImportanceSampling(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new StandardSpaceCrossEntropyImportanceSampling(reliabilityAlgorithm);
    std::cout<<" Type de l'algo au constructeur, StandardSpaceCrossEntropyImportanceSampling"<<std::endl;
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
  }



/** Constructor with PhysicalSpaceCrossEntropyImportanceSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const PhysicalSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    defaultSimulationAlgorithm_ = new PhysicalSpaceCrossEntropyImportanceSampling(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new PhysicalSpaceCrossEntropyImportanceSampling(reliabilityAlgorithm);
    std::cout<<" Type de l'algo au constructeur, PhysicalSpaceCrossEntropyImportanceSampling"<<std::endl;
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
  }
  
     
/** Constructor with SubsetSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const SubsetSampling & reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    defaultSimulationAlgorithm_ = new SubsetSampling(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new SubsetSampling(reliabilityAlgorithm);
    std::cout<<" Type de l'algo au constructeur, SubsetSampling"<<std::endl;
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
  }

/** Constructor with ProbabilitySimulationAlgorithm*/   
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const ProbabilitySimulationAlgorithm  &reliabilityAlgorithm)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    defaultSimulationAlgorithm_ = new ProbabilitySimulationAlgorithm(reliabilityAlgorithm);
    currentSimulationAlgorithm_ = new ProbabilitySimulationAlgorithm(reliabilityAlgorithm);

    std::cout<<" Type de l'algo au constructeur, Probability Simulation Algorithm"<<std::endl;
    std::cout<<*currentSimulationAlgorithm_<<std::endl;
  }
                            
/* Virtual constructor */
ActiveLearningReliabilityAlgorithm * ActiveLearningReliabilityAlgorithm::clone() const
{
  return new ActiveLearningReliabilityAlgorithm(*this);
}


/* Run of the algorithm */
void ActiveLearningReliabilityAlgorithm::run()
{

  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  
  const Function model(defaultEvent_.getFunction());
  UnsignedInteger size = inputDoE_.getSize();
  

  std::cout<<"-------------------------Simulation algo-------------------"<<std::endl;
  std::cout<<currentSimulationAlgorithm_<<std::endl;
  
  /* training of initial GPR */  
  GaussianProcessFitter currentFitter = defaultGPFitter_;
  currentFitter.run();

  GaussianProcessRegression currentGPR;
  currentGPR = GaussianProcessRegression(currentFitter.getResult());
  currentGPR.run();
  
  GaussianProcessRegressionResult currentGPRResult = currentGPR.getResult();
  
  Function currentGPRmetamodel =  currentGPRResult.getMetaModel();

  /* Update of current event with metamodel as function */  
  CompositeRandomVector currentRandomVector = CompositeRandomVector(currentGPRmetamodel,
                                                                    RandomVector(inputDistribution));

  ThresholdEvent currentEvent = ThresholdEvent(currentRandomVector,
                                               defaultEvent_.getOperator(),
                                               defaultEvent_.getThreshold());

  
  std::cout<< " ------------------ Simulation algorithm with initial event------------------" <<std::endl;
  std::cout<< *currentSimulationAlgorithm_ <<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;
                                                             
  currentSimulationAlgorithm_->setEvent(currentEvent);
  std::cout<< " ------------------ Simulation algorithm with new event------------------" <<std::endl;
  std::cout<< *currentSimulationAlgorithm_ <<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;
    
  currentSimulationAlgorithm_->run();
  
  std::cout<< " ------------------ Probability ------------------" <<std::endl;
  std::cout<< currentSimulationAlgorithm_->getResult().getProbabilityEstimate() <<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;

  std::cout<< " ------------------ Result ------------------" <<std::endl;
  std::cout<< currentSimulationAlgorithm_->getResult()<<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;
  
  std::cout<< " ------------------ Algorithm ------------------" <<std::endl;
  std::cout<< *currentSimulationAlgorithm_<<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;
  
  ProbabilitySimulationResult probaResults = currentSimulationAlgorithm_->getResult();

  std::cout<< " ------------------ ProbabilitySimulationResult ------------------" <<std::endl;

  std::cout<< probaResults<<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;    

  std::cout<< " ------------------ IputSample ------------------" <<std::endl;
  std::cout<< currentSimulationAlgorithm_->getInputSample()<<std::endl;
  std::cout<< " -------------------------------------------------" <<std::endl;
}


END_NAMESPACE_OPENTURNS
