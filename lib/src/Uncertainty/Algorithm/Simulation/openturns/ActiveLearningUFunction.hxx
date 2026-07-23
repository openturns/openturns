//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningUFunction implements parent class for 
 *  active learning "U" criterion used in reliability algorithms
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

#ifndef OPENTURNS_ACTIVELEARNINGUFUNCTION_HXX
#define OPENTURNS_ACTIVELEARNINGUFUNCTION_HXX

#include "openturns/ActiveLearningReliabilityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningUFunction
*/

class OT_API ActiveLearningUFunction
  : public ActiveLearningReliabilityFunction
{

  CLASSNAME
  
 /** Default constructor */
  ActiveLearningUFunction();
  
  /** Default constructor */  
  ActiveLearningUFunction (const Scalar reliabilityThreshold,
                           const Scalar learningThreshold);
 
  /** Virtual constructor */
  ActiveLearningUFunction * clone() const override;

  /** Computation of active learning function */
  Scalar computeAsScalar(const Point & inputSample) const override;
  
  /** Check convergence of learning */
  Bool checkConvergenceLearning(const Sample  & criterionValues) const override;
  
  /** String converter */
  String __repr__() const override;
  
  /* return infill Sample */
  Sample getInfillSample(const Sample & inputSample,
                         const Sample & criterionValues) const override;
} ; /* class ActiveLearningUFunction */

END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_ACTIVELEARNINGUFUNCTION_HXX */
