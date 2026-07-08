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

#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/EventSimulation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningReliabilityAlgorithm
*/

class OT_API ActiveLearningReliabilityAlgorithm
  : public SimulationAlgorithm
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityAlgorithm();
    
  /** Default constructor */  
  explicit ActiveLearningReliabilityAlgorithm (const GaussianProcessFitter & gpFitter,
                                               const EventSimulation & reliabilityAlgorithm);

  /** Virtual constructor */
  ActiveLearningReliabilityAlgorithm * clone() const override;
  
protected:

   
  const RandomVector defaultEvent_;
  
  const SimulationAlgorithm defaultSimulationAlgorithm_;
  
  Sample currentInputSample_;
  Sample inputDoE_;
  Sample outputDoE_;
  
} ; /* class ActiveLearningReliabilityAlgorithm */

END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX */
