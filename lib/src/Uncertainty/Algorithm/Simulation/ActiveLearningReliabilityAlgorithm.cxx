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
    p_activeLearningFunction = activelearningFunction.clone();
    p_defaultSimulationAlgorithm_->setKeepSample(true);
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
    p_activeLearningFunction = activelearningFunction.clone();
    p_defaultSimulationAlgorithm_->setKeepSample(true);
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
    p_activeLearningFunction = activelearningFunction.clone();
    p_defaultSimulationAlgorithm_->setKeepSample(true);
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
    p_activeLearningFunction = activelearningFunction.clone();
    p_defaultSimulationAlgorithm_->setKeepSample(true);
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
    p_activeLearningFunction = activelearningFunction.clone();
    p_defaultSimulationAlgorithm_->setKeepSample(true);
  }
                            
/* Virtual constructor */
ActiveLearningReliabilityAlgorithm * ActiveLearningReliabilityAlgorithm::clone() const
{
  return new ActiveLearningReliabilityAlgorithm(*this);
}



/* Class to retrieve GP plus uncertainty as Function*/
class GPWithUncertainty : public EvaluationImplementation {
public:
  GPWithUncertainty(const GaussianProcessRegressionResult & gprResult, const Scalar kFactor)
    : kFactor_(kFactor)
    , gprCov_(gprResult)
    , gprMetamodel_(gprResult.getMetaModel())
    {}
    
  GPWithUncertainty * clone() const override
  {
    return new GPWithUncertainty(*this);
  }
  // Interface obligatoire pour Function
  Sample operator()(const Sample & x) const 
  {
    // Calculation of mean
    const Sample mean = gprMetamodel_(x);
    // Calculation of variance
    const Sample variance = gprCov_.getConditionalMarginalVariance(x);
    // Calculation of k * std
    Sample result(mean.getSize(), mean.getDimension());
    for (UnsignedInteger i = 0; i < mean.getSize(); ++i) 
    {
      Scalar stdDev = std::sqrt(variance(i, 0));
      result(i, 0) = mean(i, 0) + kFactor_ * stdDev;
    }
    return result;
  }

  UnsignedInteger getInputDimension() const override
  {
    return gprMetamodel_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }
  
protected:
  Function gprMetamodel_;
  Scalar kFactor_;
  GaussianProcessConditionalCovariance gprCov_;
};




/*Point ActiveLearningReliabilityAlgorithm::checkConvergenceWithUncertainty(Scalar epsilon,
                                                                          Scalar kFactor)
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  // Build of GPR with current DoE
  GaussianProcessFitter GPfitter = GaussianProcessFitter(inputDoE_,
                                                         outputDoE_,
                                                         defaultGPFitter_.getResult().getCovarianceModel(),
                                                         defaultGPFitter_.getResult().getBasis());
  GPfitter.run();
  GaussianProcessRegression GPR;
  GPR = GaussianProcessRegression(GPfitter.getResult());
  GPR.run();
      
  GaussianProcessRegressionResult GPRResult = GPR.getResult();
  Function GPRmetamodel = GPRResult.getMetaModel();
  GaussianProcessConditionalCovariance gpcc(GPRResult);
  
  // Creation of functions for threshold event
  GPWithUncertainty GPMean(GPRResult, 0.);
  GPWithUncertainty GPMinus(GPRResult, kFactor);
  GPWithUncertainty GPPlus(GPRResult, - kFactor);
  
  // Computation of probability with GP mean
  CompositeRandomVector meanEventRV = CompositeRandomVector(GPMean,
                                                            RandomVector(inputDistribution));
  ThresholdEvent meanEvent = ThresholdEvent(meanEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());
                                               
  Pointer<EventSimulation> p_simulationAlgorithmMean_ = p_defaultSimulationAlgorithm_->clone();
                                                
  p_simulationAlgorithmMean_->setEvent(meanEvent);
  p_simulationAlgorithmMean_->run();
  Scalar meanProbability = p_simulationAlgorithmMean_-> getResult().getProbabilityEstimate();
  Scalar meanReliabilityIndex = - Normal().computeQuantile(meanProbability)[0];
  
  // Computation of probability with GP mean - k sigma
  CompositeRandomVector minusEventRV(GPMinus,
                                     RandomVector(inputDistribution));

  ThresholdEvent eventMinus = ThresholdEvent(minusEventRV,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());
                                               
  Pointer<EventSimulation> p_simulationAlgorithmMinus_ = p_defaultSimulationAlgorithm_->clone();                                               
  p_simulationAlgorithmMinus_->setEvent(eventMinus);
  p_simulationAlgorithmMinus_->run();
  Scalar minusProbability = p_simulationAlgorithmMinus_-> getResult().getProbabilityEstimate();
  Scalar minusReliabilityIndex = - Normal().computeQuantile(minusProbability)[0];
  
  // Computation of probability with GP mean + k sigma
  CompositeRandomVector plusEventRV(GPPlus,
                                    RandomVector(inputDistribution));

  ThresholdEvent eventPlus = ThresholdEvent(plusEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());
  
  Pointer<EventSimulation> p_simulationAlgorithmPlus_ = p_defaultSimulationAlgorithm_->clone();                                               
  p_simulationAlgorithmPlus_->setEvent(eventMinus);
  p_simulationAlgorithmPlus_->run();
  Scalar plusProbability = p_simulationAlgorithmPlus_-> getResult().getProbabilityEstimate();
  Scalar plusReliabilityIndex = - Normal().computeQuantile(plusProbability)[0];  
  const Bool convergenceProbability = abs((plusProbability - minusProbability) / meanProbability) <= epsilon;
  const Bool convergenceReliabilityIndex = abs((plusProbability - minusProbability) / meanProbability) <= epsilon;
  
  Collection<UnsignedInteger> convergenceIndicators(2);
  convergenceIndicators[0] = convergenceProbability;
  convergenceIndicators[1] = convergenceReliabilityIndex;
  
  std::cout<<convergenceIndicators<<std::endl;
  return convergenceIndicators;
}*/

/* Check convergence based on probability uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceProbabilityWithUncertainty(const Scalar epsilon,
                                                                                    const Scalar kFactor) 
{

  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty(kFactor);
  
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2]; 
  const Bool convergenceProbability = abs((plusProbability - minusProbability) / meanProbability) <= epsilon;
  
  return convergenceProbability;
}

/* Check convergence based on reliability index uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceReliabilityIndexWithUncertainty(const Scalar epsilon,
                                                                                         const Scalar kFactor) 
{

  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty(kFactor);
  
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar meanReliabilityIndex = - Normal().computeQuantile(meanProbability)[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar minusReliabilityIndex = - Normal().computeQuantile(minusProbability)[0];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2] ;
  const Scalar plusReliabilityIndex = - Normal().computeQuantile(plusProbability)[0]; 
   
  const Bool convergenceReliabilityIndex = abs((plusReliabilityIndex - minusReliabilityIndex) / meanReliabilityIndex) <= epsilon;
  
  return convergenceReliabilityIndex;
}

/* Compute three probability with GP uncertainty : minus, mean and plus k * std */
Point ActiveLearningReliabilityAlgorithm::computeProbabilityWithUncertainty(const Scalar kFactor) 
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  // Build of GPR with current DoE
  GaussianProcessFitter GPfitter = GaussianProcessFitter(inputDoE_,
                                                         outputDoE_,
                                                         defaultGPFitter_.getResult().getCovarianceModel(),
                                                         defaultGPFitter_.getResult().getBasis());
  GPfitter.run();
  GaussianProcessRegression GPR;
  GPR = GaussianProcessRegression(GPfitter.getResult());
  GPR.run();
      
  GaussianProcessRegressionResult GPRResult = GPR.getResult();
  Function GPRmetamodel = GPRResult.getMetaModel();
  GaussianProcessConditionalCovariance gpcc(GPRResult);
  
  // Creation of functions for threshold event
  GPWithUncertainty GPMean(GPRResult, 0.);
  GPWithUncertainty GPMinus(GPRResult, kFactor);
  GPWithUncertainty GPPlus(GPRResult, - kFactor);
  
  // Computation of probability with GP mean
  CompositeRandomVector meanEventRV = CompositeRandomVector(GPMean,
                                                            RandomVector(inputDistribution));
  ThresholdEvent meanEvent = ThresholdEvent(meanEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());
                                               
  Pointer<EventSimulation> p_simulationAlgorithmMean_ = p_defaultSimulationAlgorithm_->clone();
                                                
  p_simulationAlgorithmMean_->setEvent(meanEvent);
  p_simulationAlgorithmMean_->run();
  Scalar meanProbability = p_simulationAlgorithmMean_-> getResult().getProbabilityEstimate();
  
  // Computation of probability with GP mean - k sigma
  CompositeRandomVector minusEventRV(GPMinus,
                                     RandomVector(inputDistribution));

  ThresholdEvent eventMinus = ThresholdEvent(minusEventRV,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());
                                               
  Pointer<EventSimulation> p_simulationAlgorithmMinus_ = p_defaultSimulationAlgorithm_->clone();                                               
  p_simulationAlgorithmMinus_->setEvent(eventMinus);
  p_simulationAlgorithmMinus_->run();
  Scalar minusProbability = p_simulationAlgorithmMinus_-> getResult().getProbabilityEstimate();
  
  // Computation of probability with GP mean + k sigma
  CompositeRandomVector plusEventRV(GPPlus,
                                    RandomVector(inputDistribution));

  ThresholdEvent eventPlus = ThresholdEvent(plusEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());
  
  Pointer<EventSimulation> p_simulationAlgorithmPlus_ = p_defaultSimulationAlgorithm_->clone();                                               
  p_simulationAlgorithmPlus_->setEvent(eventMinus);
  p_simulationAlgorithmPlus_->run();
  Scalar plusProbability = p_simulationAlgorithmPlus_-> getResult().getProbabilityEstimate();

  Point probabilitiesWithUncertainty = Point({meanProbability, minusProbability, plusProbability});
  return probabilitiesWithUncertainty;
}

// Set type of convergence
void ActiveLearningReliabilityAlgorithm::setConvergenceCriterion(const UnsignedInteger typeConvergence)
{
  if (typeConvergence > 2)
    throw InvalidArgumentException(HERE) << "ActiveLearningReliability algorithm convergence criterion (" << typeConvergence << ") must be in [0-2]";
    
  convergenceCriterion_ = typeConvergence;
}

// Set thresholds for active learning and convergence
void ActiveLearningReliabilityAlgorithm::setConvergenceUncertaintyFactor(const Scalar convergenceUncertaintyFactor)
{
  if (convergenceUncertaintyFactor < 0)
  {
    throw InvalidArgumentException(HERE) << "convergenceUncertaintyFactor (" << convergenceUncertaintyFactor << ") must be in greater than 0";
  }
  convergenceUncertaintyFactor_ = convergenceUncertaintyFactor;
}

void ActiveLearningReliabilityAlgorithm::setConvergenceCriterionThreshold(const Scalar convergenceCriterionThreshold)
{
  if (convergenceCriterionThreshold < 0)
  {
    throw InvalidArgumentException(HERE) << "convergenceCriterionThreshold (" << convergenceCriterionThreshold << ") must be in greater than 0";
  }
  
  convergenceCriterionThreshold_ = convergenceCriterionThreshold;
}

void ActiveLearningReliabilityAlgorithm::setSimulationBudget(const UnsignedInteger simulationBudget)
{
  if (simulationBudget < 1)
  {
    throw InvalidArgumentException(HERE) << "simulationBudget (" << simulationBudget << ") must be in greater than 1";
  }
  
  simulationBudget_ = simulationBudget;
}

/* Run of the algorithm */
void ActiveLearningReliabilityAlgorithm::run()
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  const Function model(defaultEvent_.getFunction());
  UnsignedInteger size = inputDoE_.getSize();
  
  /* Training of initial GPR */  
  GaussianProcessFitter currentFitter = GaussianProcessFitter(inputDoE_,
                                                              outputDoE_,
                                                              defaultGPFitter_.getResult().getCovarianceModel(),
                                                              defaultGPFitter_.getResult().getBasis());
                                                              
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
                                               
  Pointer<EventSimulation> p_initialSimulationAlgorithm_ = p_defaultSimulationAlgorithm_->clone();                                               
  p_initialSimulationAlgorithm_->setEvent(currentEvent);
  p_initialSimulationAlgorithm_->run();

  Sample currentInputSample = p_initialSimulationAlgorithm_->getInputSample();
   
  p_activeLearningFunction->setGaussianProcessRegression(currentGPRResult);
  
  Sample activeLearningValues = (*p_activeLearningFunction)(currentInputSample);
  Sample infillInputSample = p_activeLearningFunction->getInfillSample(currentInputSample, activeLearningValues);
  

  Bool activeLearningIndicator;
  activeLearningIndicator = p_activeLearningFunction->checkConvergenceLearning(activeLearningValues);
  
  std::cout<< "----------------------- First probability -----------------------"<<std::endl;  
  std::cout<<p_initialSimulationAlgorithm_->getResult()<<std::endl;   
  std::cout<< "----------------------- --------- -----------------------"<<std::endl;
  
  std::cout<< "----------------------- is converged before loop? -----------------------"<<std::endl;  
  std::cout<<activeLearningIndicator<<std::endl;   
  std::cout<< "----------------------- --------- -----------------------"<<std::endl;
  
  int functionCallNumber = 0;
  
  Bool convergenceIndicator = false;
  
  while ((!convergenceIndicator) && (functionCallNumber<=simulationBudget_))
    {
      while ((functionCallNumber<=simulationBudget_) && (!activeLearningIndicator))
      {
        std::cout<<"******************** Call number ************************"<<std::endl;
        std::cout<<functionCallNumber<<std::endl;
        // Evaluation of infill sample
        Sample infillOutputSample = model(infillInputSample);
          
        // Update of GPR
        inputDoE_.add(infillInputSample);
        outputDoE_.add(infillOutputSample);
        GaussianProcessFitter newGPfitter = GaussianProcessFitter(inputDoE_,
                                                                  outputDoE_,
                                                                  defaultGPFitter_.getResult().getCovarianceModel(),
                                                                  defaultGPFitter_.getResult().getBasis());
        newGPfitter.run();
        
        GaussianProcessRegression newGPR;
        newGPR = GaussianProcessRegression(newGPfitter.getResult());
        newGPR.run();
          
        GaussianProcessRegressionResult newGPRResult = newGPR.getResult();
       
        // Compute active learning values
        p_activeLearningFunction->setGaussianProcessRegression(newGPRResult);
        activeLearningValues = (*p_activeLearningFunction)(currentInputSample);
        //Get input sample to evaluate
        infillInputSample = p_activeLearningFunction->getInfillSample(currentInputSample, activeLearningValues);
        // check convergence of active learning
        activeLearningIndicator = p_activeLearningFunction->checkConvergenceLearning(activeLearningValues);

        
        functionCallNumber += 1;
        std::cout<<"------- active learning values --------"<<std::endl;
        std::cout<<activeLearningValues<<std::endl;
        
        
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
                                                       
    Pointer<EventSimulation> p_currentSimulationAlgorithm_ = p_defaultSimulationAlgorithm_->clone();
    p_currentSimulationAlgorithm_->setEvent(newEvent);
    p_currentSimulationAlgorithm_->run();
    
    
    std::cout<<"------- Probability value --------"<<std::endl;
    std::cout<<p_currentSimulationAlgorithm_->getResult().getProbabilityEstimate()<<std::endl;
        
      
    if (convergenceCriterion_ == 0)
    {
      convergenceIndicator = checkConvergenceProbabilityWithUncertainty(convergenceCriterionThreshold_, convergenceUncertaintyFactor_ );
    }
    else if (convergenceCriterion_ == 1)
    {
      convergenceIndicator = checkConvergenceReliabilityIndexWithUncertainty(convergenceCriterionThreshold_, convergenceUncertaintyFactor_ );
    }
           
    }
}

END_NAMESPACE_OPENTURNS
