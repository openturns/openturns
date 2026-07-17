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

#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/NAIS.hxx"
#include "openturns/SubsetSampling.hxx"
#include "openturns/StandardSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/PhysicalSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/ActiveLearningUFunction.hxx"
#include "openturns/ActiveLearningEFFFunction.hxx"
BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningReliabilityAlgorithm
*/

class OT_API ActiveLearningReliabilityAlgorithm
  : public EventSimulation
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityAlgorithm();
    
  /** Default constructor */  
  /*ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                               const EventSimulation & reliabilityAlgorithm);*/
                                               
  ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                      const ProbabilitySimulationAlgorithm & reliabilityAlgorithm,
                                      const ActiveLearningUFunction & activelearningFunction);

  ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                      const NAIS & reliabilityAlgorithm,
                                      const ActiveLearningUFunction & activelearningFunction);
                                      
  ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                      const SubsetSampling & reliabilityAlgorithm,
                                      const ActiveLearningUFunction & activelearningFunction);
                                      
  ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                      const StandardSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm,
                                      const ActiveLearningUFunction & activelearningFunction);
                                      
  ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                      const PhysicalSpaceCrossEntropyImportanceSampling & reliabilityAlgorithm,
                                      const ActiveLearningUFunction & activelearningFunction);
                                      
  /** Virtual constructor */
  ActiveLearningReliabilityAlgorithm * clone() const override;

  void run();

  enum convergenceCriterion {PROBABILITY_UNCERTAINTY = 0,
                             RELIABILITY_INDEX_UNCERTAINTY = 1,
                             ACTIVE_LEARNING = 2,
                             PROBABILITY_STABILITY = 3,
                             RELIABILITY_INDEX_STABILITY = 4};
  
  // Accessor to convergence attributes
  void setConvergenceCriterion(const UnsignedInteger convergenceCriterion = PROBABILITY_UNCERTAINTY);
  
  void setConvergenceUncertaintyFactor(const Scalar convergenceUncertaintyFactor);
  
  void setConvergenceCriterionThreshold(const Scalar convergenceCriterionThreshold);
  
  void setSimulationBudget(const UnsignedInteger simulationBudget);
  
  Pointer<EventSimulation> getSimulationAlgorithm() const;
  
  //EventSimulation & getSimulationAlgorithm() const;
  UnsignedInteger getFunctionCallNumber() const;
  
  Sample getInputDoE() const;
  
  Sample getOutputDoE() const;
  
protected:

  Bool checkConvergenceProbabilityWithUncertainty();

  Bool checkConvergenceReliabilityIndexWithUncertainty();

  Bool checkConvergenceStability(const Point currentValue,
                                 const Point previousValue);
  
  Point computeProbabilityWithUncertainty();                                                                                         
  const RandomVector defaultEvent_;
  Pointer<EventSimulation> p_defaultSimulationAlgorithm_;
  Pointer<EventSimulation> p_simulationAlgorithm_;
  Pointer<ActiveLearningReliabilityFunction> p_activeLearningFunction;
  
  GaussianProcessFitter defaultGPFitter_;
  Sample currentInputSample_;
  Sample inputDoE_;
  Sample outputDoE_;
  UnsignedInteger convergenceCriterion_;
  
  Scalar convergenceCriterionThreshold_;
  Scalar convergenceUncertaintyFactor_;
  
  UnsignedInteger simulationBudget_;
  UnsignedInteger functionCallNumber_;
  Sample probabilityHistory_;
  Sample reliabilityIndexHistory_;
      
} ; /* class ActiveLearningReliabilityAlgorithm */

END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX */
