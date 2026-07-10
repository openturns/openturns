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
#include "openturns/ActiveLearningReliabilityFunction.hxx"


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
                                                                        const NAIS & reliabilityAlgorithm,
                                                                        const ActiveLearningUFunction & activelearningFunction)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_currentSimulationAlgorithm_ = reliabilityAlgorithm.clone();  
    p_activeLearningFunction = activelearningFunction.clone();

  }

/** Constructor with StandardSpaceCrossEntropyImportanceSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const StandardSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm,
                                                                        const ActiveLearningUFunction & activelearningFunction)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_currentSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_activeLearningFunction = activelearningFunction.clone();
  }

/** Constructor with PhysicalSpaceCrossEntropyImportanceSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const PhysicalSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm,
                                                                        const ActiveLearningUFunction & activelearningFunction)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_currentSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_activeLearningFunction = activelearningFunction.clone();
  }
  
     
/** Constructor with SubsetSampling */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const SubsetSampling & reliabilityAlgorithm,
                                                                        const ActiveLearningUFunction & activelearningFunction)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_currentSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_activeLearningFunction = activelearningFunction.clone();
  }

/** Constructor with ProbabilitySimulationAlgorithm*/   
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const ProbabilitySimulationAlgorithm  &reliabilityAlgorithm,
                                                                        const ActiveLearningUFunction & activelearningFunction)
  : EventSimulation(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultGPFitter_(gpFitter)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_currentSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_activeLearningFunction = activelearningFunction.clone();
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
  p_currentSimulationAlgorithm_->setEvent(currentEvent);
  p_currentSimulationAlgorithm_->run();

  Sample currentInputSample = p_currentSimulationAlgorithm_->getInputSample();
   
  p_activeLearningFunction->setGaussianProcessRegression(currentGPRResult);
  
  Sample activeLearningValues = (*p_activeLearningFunction)(currentInputSample);
  Sample infillInputSample = p_activeLearningFunction->getInfillSample(currentInputSample, activeLearningValues);
  

  Bool activeLearningIndicator;
  activeLearningIndicator = p_activeLearningFunction->checkConvergenceLearning(activeLearningValues);
  
  std::cout<< "----------------------- First probability -----------------------"<<std::endl;  
  std::cout<<p_currentSimulationAlgorithm_->getResult()<<std::endl;   
  std::cout<< "----------------------- --------- -----------------------"<<std::endl;
  
  std::cout<< "----------------------- is converged before loop? -----------------------"<<std::endl;  
  std::cout<<activeLearningIndicator<<std::endl;   
  std::cout<< "----------------------- --------- -----------------------"<<std::endl;
  
  int iterationNumber = 0;
  
  while (iterationNumber<5)
  {
    // Evaluation of infill sample
    Sample infillOutputSample = model( infillInputSample);
      
    // Update of GPR
    inputDoE_.add(infillInputSample);
    outputDoE_.add(infillOutputSample);
    GaussianProcessFitter newGPfitter = GaussianProcessFitter(inputDoE_,
                                                              outputDoE_,
                                                              currentFitter.getResult().getCovarianceModel(),
                                                              currentFitter.getResult().getBasis());
    newGPfitter.run();
    
    GaussianProcessRegression newGPR;
    newGPR = GaussianProcessRegression(newGPfitter.getResult());
    newGPR.run();
      
    GaussianProcessRegressionResult newGPRResult = newGPR.getResult();

      
    /* Update of current event with metamodel as function */  

    /*
    Function newGPRmetamodel =  newGPRResult.getMetaModel();
    CompositeRandomVector newRandomVector = CompositeRandomVector(newGPRmetamodel,
                                                                  RandomVector(inputDistribution));
                                                                        
    ThresholdEvent newEvent = ThresholdEvent(newRandomVector,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());
                                                   
    Pointer<EventSimulation> p_newSimulationAlgorithm_ = p_defaultSimulationAlgorithm_->clone();
      
    p_newSimulationAlgorithm_->setEvent(newEvent);
      
    p_newSimulationAlgorithm_->run();
      
    std::cout<< "----------------------- New probability -----------------------"<<std::endl;  
    std::cout<<p_newSimulationAlgorithm_->getResult()<<std::endl;   
    std::cout<< "----------------------- --------- -----------------------"<<std::endl;
      
    currentInputSample = p_newSimulationAlgorithm_->getInputSample();*/
   
    p_activeLearningFunction->setGaussianProcessRegression(newGPRResult);
    activeLearningValues = (*p_activeLearningFunction)(currentInputSample);
    std::cout<< "----------------------------U sample---------------------------------------"<<std::endl;
    std::cout<<      activeLearningValues<<std::endl;
    infillInputSample = p_activeLearningFunction->getInfillSample(currentInputSample, activeLearningValues);
    
    activeLearningIndicator = p_activeLearningFunction->checkConvergenceLearning(activeLearningValues);

    std::cout<< "----------------------------infill input---------------------------------------"<<std::endl;
     std::cout<<      infillInputSample<<std::endl;
    std::cout<< "----------------------------infill output---------------------------------------"<<std::endl;
    std::cout<<      model(infillInputSample)<<std::endl;
    std::cout<< "----------------------------size DOE---------------------------------------"<<std::endl;      
    std::cout<<     inputDoE_.getSize()<<std::endl;
    std::cout<< "----------------------------Min U---------------------------------------"<<std::endl;
    std::cout<<(*p_activeLearningFunction)(Sample(infillInputSample));
    std::cout<<"---------------------------Is AK converged------------------------------"<<std::endl;
    std::cout<<activeLearningIndicator<<std::endl;
    std::cout<< "----------------------- --------- -----------------------"<<std::endl;
    
    iterationNumber += 1;
  }
  // test probability estimation
  
  GaussianProcessFitter newGPfitter = GaussianProcessFitter(inputDoE_,
                                                              outputDoE_,
                                                              currentFitter.getResult().getCovarianceModel(),
                                                              currentFitter.getResult().getBasis());
  newGPfitter.run();
    
  GaussianProcessRegression newGPR;
  newGPR = GaussianProcessRegression(newGPfitter.getResult());
  newGPR.run();
      
  GaussianProcessRegressionResult newGPRResult = newGPR.getResult();
  Function newGPRmetamodel =  newGPRResult.getMetaModel();
  CompositeRandomVector newRandomVector = CompositeRandomVector(newGPRmetamodel,
                                                                  RandomVector(inputDistribution));
                                                                        
  ThresholdEvent newEvent = ThresholdEvent(newRandomVector,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());
                                                   
  Pointer<EventSimulation> p_newSimulationAlgorithm_ = p_defaultSimulationAlgorithm_->clone();
      
  p_newSimulationAlgorithm_->setEvent(newEvent);
      
  p_newSimulationAlgorithm_->run();
      
  std::cout<< "----------------------- New probability -----------------------"<<std::endl;  
  std::cout<<p_newSimulationAlgorithm_->getResult()<<std::endl; 
                                                            
    
}


END_NAMESPACE_OPENTURNS
