//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningGMMFunction implements parent class for 
 *  active learning "Generalized Maximin" criterion used in reliability algorithms
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

#ifndef OPENTURNS_ACTIVELEARNINGGMMFUNCTION_HXX
#define OPENTURNS_ACTIVELEARNINGGMMFUNCTION_HXX

#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningGMMFunction
*/

class OT_API ActiveLearningGMMFunction
  : public ActiveLearningReliabilityFunction
{

  CLASSNAME
  
  /** Default constructor */
  ActiveLearningGMMFunction();
  
  /** Default constructor */  
  ActiveLearningGMMFunction(const Scalar reliabilityThreshold,
                            const Scalar learningThreshold,
                            const Distribution & inputDistribution,
                            const Scalar outputQuantileLevel);
                          
  /** Virtual constructor */
  ActiveLearningGMMFunction * clone() const override;

  /** Computation of active learning function */
  Scalar computeAsScalar(const Point & inputSample) const override;

  /** Computation of active learning function */
  Sample operator()(const Sample & inputSample, const Sample & inputDoE) const override;
      
  /** Check convergence of learning */
  Bool checkConvergenceLearning(const Sample  & criterionValues) const override;
  
  /** String converter */
  String __repr__() const override;
  
  /* return infill Sample */
  Sample getInfillSample(const Sample & inputSample,
                         const Sample & criterionValues) const override;
                  
  Distribution inputDistribution_;
  Scalar outputQuantileLevel_;
} ; /* class ActiveLearningUFunction */

END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_ACTIVELEARNINGGMMFUNCTION_HXX */
