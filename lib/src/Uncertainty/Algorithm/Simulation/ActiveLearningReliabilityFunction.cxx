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

#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunction
 */
 
CLASSNAMEINIT(ActiveLearningReliabilityFunction)

static const Factory<ActiveLearningReliabilityFunction> Factory_ActiveLearningReliabilityFunction;

// Default constructor
ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction(const Scalar reliabilityThreshold,
                                                                     const GaussianProcessRegressionResult & gprResult,
                                                                     const Scalar learningThreshold)
    : EvaluationImplementation()
    , reliabilityThreshold_(reliabilityThreshold)
    , learningThreshold_(learningThreshold)
    , gprResult_(gprResult)
    , gprCov_(gprResult)
{
}
                                                                     
/*ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction(const Scalar reliabilityThreshold,
                                    const GaussianProcessRegressionResult & gprResult,
                                    const Scalar learningThreshold,
                                    const Distribution & inputDistribution,
                                    const Scalar outputQuantileLevel)    
    : EvaluationImplementation()
    , reliabilityThreshold_(reliabilityThreshold)
    , learningThreshold_(learningThreshold)
    , gprResult_(gprResult)
    , gprCov_(gprResult)
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction(const Scalar reliabilityThreshold,const GaussianProcessRegressionResult & gprResult,const Distribution & inputDistribution,const Scalar learningThreshold)";
}*/


ActiveLearningReliabilityFunction * ActiveLearningReliabilityFunction::clone() const 
{
  return new ActiveLearningReliabilityFunction(*this);
}

Point ActiveLearningReliabilityFunction::operator()(const Point & x) const 
{
  return Point(1, computeAsScalar(x));
}

  
Sample ActiveLearningReliabilityFunction::operator()(const Sample & inputSample, const Sample & inputDoE) const 
{
  const UnsignedInteger size = inputSample.getSize();
  // avoid creating size points
  Point inputSample_i(inputSample.getDimension());
  Sample outS(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < inputSample_i.getSize(); ++ j)
      inputSample_i[j] = inputSample(i, j);
      outS(i, 0) = computeAsScalar(inputSample_i);
  }
  return outS;
}

Scalar ActiveLearningReliabilityFunction::computeAsScalar(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::computeAsScalar(const Point & x)";
}

Bool ActiveLearningReliabilityFunction::checkConvergenceLearning(const Sample & criterionValues) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::checkConvergenceLearning(const Sample & criterionValues)";
}

/* Return sample corresponding to  criterion*/
Sample ActiveLearningReliabilityFunction::getInfillSample(const Sample & inputSample,
                                                          const Sample & criterionValues) const 
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::findSampletoEvaluate(const Sample & inputSample, const Sample & criterionValues) "; 
}

/* String converter */
String ActiveLearningReliabilityFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << EvaluationImplementation::__repr__();
  return oss;
}


/* update of GPR model */
void ActiveLearningReliabilityFunction::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  gprResult_ = gprResult;
  gprCov_ = GaussianProcessConditionalCovariance(gprResult);
}
/* accessor reliability threshold */
void ActiveLearningReliabilityFunction::setReliabilityThreshold(const Scalar reliabilityThreshold)
{
  reliabilityThreshold_ = reliabilityThreshold;
}

/* accessor learning threshold */
void ActiveLearningReliabilityFunction::setLearningThreshold(const Scalar learningThreshold)
{
  learningThreshold_ = learningThreshold;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningReliabilityFunction::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("gprCov_", gprCov_);
  adv.saveAttribute("learningThreshold_", learningThreshold_); 
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningReliabilityFunction::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("gprCov_", gprCov_);
  adv.loadAttribute("learningThreshold_", learningThreshold_); 
}

END_NAMESPACE_OPENTURNS
