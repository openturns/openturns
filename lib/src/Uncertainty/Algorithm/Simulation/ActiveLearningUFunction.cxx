//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningUFunction implements parent class for 
 *  active learning "U" criterion used in reliability analysis
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

#include "openturns/ActiveLearningUFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningUFunction
 */
 
CLASSNAMEINIT(ActiveLearningUFunction)

static const Factory<ActiveLearningUFunction> Factory_ActiveLearningUFunction;

 /** Default constructor */
ActiveLearningUFunction::ActiveLearningUFunction()
  :ActiveLearningReliabilityFunction()
  {
    // Nothing to do
  } 

/* Constructor with parameters */
ActiveLearningUFunction::ActiveLearningUFunction(const Scalar reliabilityThreshold,
                                                 const GaussianProcessRegressionResult & gprResult,
                                                 const Scalar learningThreshold)
    : ActiveLearningReliabilityFunction(reliabilityThreshold,
                                        gprResult,
                                        learningThreshold)
  {
  }
  
/* Virtual constructor */
ActiveLearningUFunction * ActiveLearningUFunction::clone() const
{
  return new ActiveLearningUFunction(*this);
}


/* String converter */
String ActiveLearningUFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << ActiveLearningReliabilityFunction::__repr__();
  return oss;
}


/* Compute U criterion for one sample */
Scalar ActiveLearningUFunction::computeAsScalar(const Point & x) const
{
  const Scalar mx = gprCov_.getConditionalMean(x)[0];
  const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
  const Scalar sk = std::sqrt(sk2);
    
  const Scalar u = std::abs(mx - reliabilityThreshold_) / (sk+1e-10);

  return u;
}

/* Compute U criterion for one sample */
Bool ActiveLearningUFunction::checkConvergenceLearning(const Sample & criterionValues) const 
{
  Scalar min_U = criterionValues.getMin()[0];
  
  if (min_U >=learningThreshold_)
  {
    return true;
  }
  else
  {
    return false;
  } 
}

/* Return sample corresponding to U criterion*/
Sample ActiveLearningUFunction::getInfillSample(const Sample & inputSample,
                                                const Sample & criterionValues) const 
{
  Indices indextoReturn(1);
  indextoReturn[0] = criterionValues.argsort(true)[0];
 
  Sample sampleToReturn = Sample(inputSample.select(indextoReturn));

  return sampleToReturn;
}


  
  
END_NAMESPACE_OPENTURNS
