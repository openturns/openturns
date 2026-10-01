//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningEFFFunction implements the "EFF" active learning
 *  criterion used in reliability analysis
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

#include "openturns/ActiveLearningEFFFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningEFFFunction
 */
 
CLASSNAMEINIT(ActiveLearningEFFFunction)

static const Factory<ActiveLearningEFFFunction> Factory_ActiveLearningEFFFunction;

 /** Default constructor */
ActiveLearningEFFFunction::ActiveLearningEFFFunction()
  :ActiveLearningReliabilityFunction()
  {
    // Nothing to do
  } 

/* Constructor with parameters */
ActiveLearningEFFFunction::ActiveLearningEFFFunction(const Scalar reliabilityThreshold,
                                                     const Scalar learningThreshold)
    : ActiveLearningReliabilityFunction(reliabilityThreshold,
                                        learningThreshold)
  {
  }
    
/* Virtual constructor */
ActiveLearningEFFFunction * ActiveLearningEFFFunction::clone() const
{
  return new ActiveLearningEFFFunction(*this);
}

/* Return sample corresponding to EFF criterion*/
Sample ActiveLearningEFFFunction::getInfillSample(const Sample & inputSample,
                                                  const Sample & criterionValues) const 
{
  Indices indextoReturn(1);
  
  indextoReturn[0] = criterionValues.argsort(false)[0];
  
  Sample sampleToReturn(inputSample.select(indextoReturn));
  
  return sampleToReturn;
}


/* Compute EFF criterion for one sample */
Scalar ActiveLearningEFFFunction::computeAsScalar(const Point & x) const
{
  const Scalar mx = gprCov_.getConditionalMean(x)[0];
  const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
  // Null variance, typically on a design point: the expected feasibility
  // improvement is zero and the point must never be selected
  if (!(sk2 > 0.0)) return 0.0;
  const Scalar sk = std::sqrt(sk2);
  const Scalar epsilon = 2.0 * sk;

  const Scalar reduced_x = ( reliabilityThreshold_ - mx ) / sk;
  const Scalar reduced_x_minus_epsilon = ( reliabilityThreshold_ - epsilon - mx ) / sk ;
  const Scalar reduced_x_plus_epsilon = ( reliabilityThreshold_ + epsilon - mx ) / sk;

  const Scalar PDF_x = DistFunc::dNormal(reduced_x);
  const Scalar PDF_x_plus_epsilon = DistFunc::dNormal(reduced_x_plus_epsilon);
  const Scalar PDF_x_minus_epsilon = DistFunc::dNormal(reduced_x_minus_epsilon);

  const Scalar CDF_x = DistFunc::pNormal(reduced_x);
  const Scalar CDF_x_plus_epsilon = DistFunc::pNormal(reduced_x_plus_epsilon);
  const Scalar CDF_x_minus_epsilon = DistFunc::pNormal(reduced_x_minus_epsilon);

  const Scalar EFF = ( mx - reliabilityThreshold_ ) * ( 2.0 * CDF_x - CDF_x_plus_epsilon - CDF_x_minus_epsilon ) -\
                sk * ( 2.0 * PDF_x - PDF_x_plus_epsilon - PDF_x_minus_epsilon ) +\
                epsilon * ( CDF_x_plus_epsilon  - CDF_x_minus_epsilon );
  return EFF;
}


/* Compute EFF criterion for one sample */
Bool ActiveLearningEFFFunction::checkConvergenceLearning(const Sample & criterionValues) const 
{
  if (criterionValues.getSize() == 0) throw InvalidArgumentException(HERE) << "Cannot check learning convergence on an empty sample of criterion values";
  const Scalar max_EFF = criterionValues.getMax()[0];
  
  if (max_EFF <=learningThreshold_)
  {
    return true;
  }
  else
  {
    return false;
  } 
}

/* String converter */
String ActiveLearningEFFFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << ActiveLearningReliabilityFunction::__repr__();
  return oss;
}

/* Selection direction: the infill point maximizes EFF */
Bool ActiveLearningEFFFunction::isMaximization() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningEFFFunction::save(Advocate & adv) const
{
  ActiveLearningReliabilityFunction::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningEFFFunction::load(Advocate & adv)
{
  ActiveLearningReliabilityFunction::load(adv);
}


END_NAMESPACE_OPENTURNS
