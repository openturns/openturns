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
//#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningReliabilityAlgorithm
 */
 
CLASSNAMEINIT(ActiveLearningReliabilityAlgorithm)

//static const Factory<ActiveLearningReliabilityAlgorithm> Factory_ActiveLearningReliabilityAlgorithm;

 /** Default constructor */
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm()
  :SimulationAlgorithm()
  {
    // Nothing to do
  } 

/** Default constructor */  
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                                                        const EventSimulation & reliabilityAlgorithm)
  :SimulationAlgorithm()
  , defaultEvent_(reliabilityAlgorithm.getEvent().getImplementation()->asComposedEvent())
  , inputDoE_(gpFitter.getInputSample())
  , outputDoE_(gpFitter.getOutputSample())
  , defaultSimulationAlgorithm_(reliabilityAlgorithm)
  {
  }

/* Virtual constructor */
ActiveLearningReliabilityAlgorithm * ActiveLearningReliabilityAlgorithm::clone() const
{
  return new ActiveLearningReliabilityAlgorithm(*this);
}


END_NAMESPACE_OPENTURNS
