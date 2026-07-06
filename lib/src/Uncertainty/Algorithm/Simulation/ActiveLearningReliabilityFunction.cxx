//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunction implements parent class for 
 *  active learning criterion used in reliability analysis
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
#include <chrono>

#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx""

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ActiveLearningReliabilityFunction)

static const Factory<ActiveLearningReliabilityFunction> Factory_ActiveLearningReliabilityFunction;

/* Constructor with parameters */

  ActiveLearningReliabilityFunction (const Scalar reliabilityThreshold,
                                     const Distribution inputDistribution,
                                     const GaussianProcessRegressionResult & gprResult);
    : EvaluationImplementation()
    , reliabilityThreshold_(reliabilityThreshold)
    , inputDistribution_(inputDistribution)
    , gprResult_(gprResult)
  {
  }
  
/* Method save() stores the object through the StorageManager */
void ActiveLearningReliabilityFunction::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.saveAttribute("inputDistribution_", inputDistribution_);
  adv.saveAttribute("gprResult_", gprResult_);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningReliabilityFunction::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.loadAttribute("inputDistribution_", inputDistribution_);
  adv.loadAttribute("gprResult_", gprResult_);
}

END_NAMESPACE_OPENTURNS
