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
  , functionCallNumber_(0)
  , probabilityHistory_(0, 1)
  , reliabilityIndexHistory_(0, 1)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_simulationAlgorithm_ = reliabilityAlgorithm.clone();
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
  , functionCallNumber_(0)
  , probabilityHistory_(0, 1)
  , reliabilityIndexHistory_(0, 1)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_simulationAlgorithm_ = reliabilityAlgorithm.clone();
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
  , functionCallNumber_(0)
  , probabilityHistory_(0, 1)
  , reliabilityIndexHistory_(0, 1)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_simulationAlgorithm_ = reliabilityAlgorithm.clone();
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
  , functionCallNumber_(0)
  , probabilityHistory_(0, 1)
  , reliabilityIndexHistory_(0, 1)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_simulationAlgorithm_ = reliabilityAlgorithm.clone();
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
  , functionCallNumber_(0)
  , probabilityHistory_(0, 1)
  , reliabilityIndexHistory_(0, 1)
  {
    p_defaultSimulationAlgorithm_ = reliabilityAlgorithm.clone();
    p_simulationAlgorithm_ = reliabilityAlgorithm.clone();
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

/* Check convergence based on probability uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceProbabilityWithUncertainty() 
{


  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty();
  
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2]; 
  const Bool convergenceProbability = abs((plusProbability - minusProbability) / meanProbability) <= convergenceCriterionThreshold_;
  std::cout<<" ---------------------- Convergence indicator ------------------------"<<std::endl;
  std::cout<< Point({meanProbability,minusProbability,plusProbability})<<std::endl;
  std::cout<<convergenceCriterionThreshold_ <<std::endl;
  std::cout<<abs((plusProbability - minusProbability) / meanProbability) <<std::endl;
  return convergenceProbability;
}

/* Check convergence based on reliability index uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceReliabilityIndexWithUncertainty() 
{

  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty();
  
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar meanReliabilityIndex = - Normal().computeQuantile(meanProbability)[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar minusReliabilityIndex = - Normal().computeQuantile(minusProbability)[0];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2] ;
  const Scalar plusReliabilityIndex = - Normal().computeQuantile(plusProbability)[0]; 
   
  const Bool convergenceReliabilityIndex = abs((plusReliabilityIndex - minusReliabilityIndex) / meanReliabilityIndex) <= convergenceCriterionThreshold_;

  return convergenceReliabilityIndex;
}

/* Compute three probability with GP uncertainty : minus, mean and plus k * std */
Point ActiveLearningReliabilityAlgorithm::computeProbabilityWithUncertainty() 
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
  //Function GPRmetamodel = GPRResult.getMetaModel();
  GaussianProcessConditionalCovariance gpcc(GPRResult);
  
  std::cout<< " --------- k factor ----------"<<std::endl;
  std::cout<< convergenceUncertaintyFactor_<<std::endl;
  
  // Creation of functions for threshold event
  GPWithUncertainty GPMean(GPRResult, 0.);
  GPWithUncertainty GPMinus(GPRResult, - convergenceUncertaintyFactor_);
  GPWithUncertainty GPPlus(GPRResult, + convergenceUncertaintyFactor_);
  
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
  p_simulationAlgorithmPlus_->setEvent(eventPlus);
  p_simulationAlgorithmPlus_->run();
  Scalar plusProbability = p_simulationAlgorithmPlus_-> getResult().getProbabilityEstimate();

  Point probabilitiesWithUncertainty = Point({meanProbability, minusProbability, plusProbability});

  return probabilitiesWithUncertainty;
}


/* Check convergence based on stability of history */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceStability(const Point currentValue,
                                                                   const Point previousValue) 
{

 return abs(currentValue[0] - previousValue[0])/ currentValue[0] <= convergenceCriterionThreshold_;
}
                                                                                         
                                                                                         
// Set type of convergence
void ActiveLearningReliabilityAlgorithm::setConvergenceCriterion(const UnsignedInteger typeConvergence)
{
  if (typeConvergence > 4)
    throw InvalidArgumentException(HERE) << "ActiveLearningReliability algorithm convergence criterion (" << typeConvergence << ") must be in [0-4]";
    
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

Pointer<EventSimulation> ActiveLearningReliabilityAlgorithm::getSimulationAlgorithm() const
{
  return p_simulationAlgorithm_;
}

// Getter of functionCallNumber
UnsignedInteger ActiveLearningReliabilityAlgorithm::getFunctionCallNumber() const
{
  return functionCallNumber_;
}
  
// Getter of DoE inputs and outputs
Sample ActiveLearningReliabilityAlgorithm::getInputDoE() const
{
  return inputDoE_;
}


Sample ActiveLearningReliabilityAlgorithm::getOutputDoE() const
{
  return outputDoE_;
}



/*EventSimulation& ActiveLearningReliabilityAlgorithm::getSimulationAlgorithm() const
{
  return dynamic_cast<EventSimulation>(&p_simulationAlgorithm_);
}*/

/*Sample ActiveLearningReliabilityAlgorithm::getInputSample() const
{
  return inputDoE_;
}*/



/* Run of the algorithm */
void ActiveLearningReliabilityAlgorithm::run()
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  const Function model(defaultEvent_.getFunction());
  UnsignedInteger size = inputDoE_.getSize();

  
  Bool convergenceStatus = false;
  
  while ((!convergenceStatus) && (functionCallNumber_<=simulationBudget_))
    {
      
      std::cout<<"-----------------Remaining budget ---------------"<<std::endl;
      std::cout<< simulationBudget_ - functionCallNumber_<<std::endl;
      // Estimate probability with GP
      
      GaussianProcessFitter newGPfitter = GaussianProcessFitter(inputDoE_,
                                                                outputDoE_,
                                                                defaultGPFitter_.getResult().getCovarianceModel(),
                                                                defaultGPFitter_.getResult().getBasis());
      newGPfitter.run();
      GaussianProcessRegression newGPR;
      newGPR = GaussianProcessRegression(newGPfitter.getResult());
      newGPR.run();
          
      GaussianProcessRegressionResult newGPRResult = newGPR.getResult();
      
      //Estimate probability with current GP
      Function newGPRmetamodel =  newGPRResult.getMetaModel();
      CompositeRandomVector newRandomVector = CompositeRandomVector(newGPRmetamodel,
                                                                  RandomVector(inputDistribution));
                                                                            
      ThresholdEvent newEvent = ThresholdEvent(newRandomVector,
                                               defaultEvent_.getOperator(),
                                               defaultEvent_.getThreshold());
                                                       
      Pointer<EventSimulation> p_currentSimulationAlgorithm = p_defaultSimulationAlgorithm_->clone();
      p_currentSimulationAlgorithm->setEvent(newEvent);
      p_currentSimulationAlgorithm->run();

      std::cout<<"------- Probability value --------"<<std::endl;
      std::cout<<p_currentSimulationAlgorithm->getResult().getProbabilityEstimate()<<std::endl;
    
      *p_simulationAlgorithm_ = *p_currentSimulationAlgorithm;
      
      Sample currentInputSample = p_currentSimulationAlgorithm->getInputSample();
      
      // Compute active learning values
      p_activeLearningFunction->setGaussianProcessRegression(newGPRResult);
      Sample activeLearningValues = (*p_activeLearningFunction)(currentInputSample);
      
      // Store history
      Scalar currentProbabilityEstimate = p_currentSimulationAlgorithm->getResult().getProbabilityEstimate();
      probabilityHistory_.add(Point(1, currentProbabilityEstimate));
      reliabilityIndexHistory_.add(Point(1, - Normal().computeQuantile(currentProbabilityEstimate)[0]));
      
      // Check convergence
      if (convergenceCriterion_ == 0)
      {
        // check convergence of uncertainty of probability estimate
        convergenceStatus = checkConvergenceProbabilityWithUncertainty();
      }
      else if (convergenceCriterion_ == 1)
      {
        // check convergence of uncertainty of reliability index
        convergenceStatus = checkConvergenceReliabilityIndexWithUncertainty();
      }
      else if (convergenceCriterion_ == 2)
      {
        // check convergence of active learning
        convergenceStatus = p_activeLearningFunction->checkConvergenceLearning(activeLearningValues);
      }
      else if (convergenceCriterion_ == 3)
      {
        UnsignedInteger historySize = probabilityHistory_.getSize();
        if (historySize > 1)
        { 
          Indices index(2);
          index[0] = historySize - 1;
          index[1] = historySize - 2;
          Sample probabilityEstimate = probabilityHistory_.select(index);

          convergenceStatus = checkConvergenceStability(probabilityEstimate[0], probabilityEstimate[1]);
        }

      }
      else if (convergenceCriterion_ = 4)
      {
        UnsignedInteger historySize = probabilityHistory_.getSize();
        if (historySize > 1)
        { 
          Indices index(2);
          index[0] = historySize - 1;
          index[1] = historySize - 2;

          Sample reliabilityIndexEstimate = reliabilityIndexHistory_.select(index);

          convergenceStatus = checkConvergenceStability(reliabilityIndexEstimate[0], reliabilityIndexEstimate[1]);
        }
        
      }

      // Add infill sample if convergence is not reached
      if (!convergenceStatus)
      {
        // Get input sample to evaluate
        Sample infillInputSample = p_activeLearningFunction->getInfillSample(currentInputSample, activeLearningValues);
      
        inputDoE_.add(infillInputSample);
        Sample infillOutputSample = model(infillInputSample);
        outputDoE_.add(infillOutputSample);
        functionCallNumber_ += 1;
      }

      
       
    }

}

END_NAMESPACE_OPENTURNS
