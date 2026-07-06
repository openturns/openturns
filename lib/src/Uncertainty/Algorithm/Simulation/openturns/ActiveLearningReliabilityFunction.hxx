//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunction implements parent class for 
 *  active learning criterion used in reliability algorithms
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
#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningReliabilityFunction
*/

CLASSNAMEINIT(ActiveLearningReliabilityFunction)

static const Factory<ActiveLearningReliabilityFunction> Factory_ActiveLearningReliabilityFunction;


class OT_API ActiveLearningReliabilityFunction
  : public EvaluationImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityFunction();
  
  /** Default constructor */  
  ActiveLearningReliabilityFunction (const Scalar reliabilityThreshold,
                                     const Distribution inputDistribution,
                                     const GaussianProcessRegressionResult & gprResult);
                                                                         
  /** Virtual constructor */
  ActiveLearningReliabilityFunction * clone() const override;                                     

  /** Computation of active learning function */
  virtual Point operator()(const Sample & inputSample) const
protected:
 
  Scalar reliabilityThreshold_;
  Distribution inputDistribution_;
  GaussianProcessRegressionResult gprResult_;

};

} ; /* class ActiveLearningReliabilityFunction */

END_NAMESPACE_OPENTURNS




#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX
