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
#include <cmath>

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
                                                                     const Scalar learningThreshold)
    : EvaluationImplementation()
    , reliabilityThreshold_(reliabilityThreshold)
    , learningThreshold_(learningThreshold)
{
  if (!std::isfinite(reliabilityThreshold)) throw InvalidArgumentException(HERE) << "Reliability threshold must be finite, here threshold=" << reliabilityThreshold;
  if (!(learningThreshold >= 0.0) || !std::isfinite(learningThreshold)) throw InvalidArgumentException(HERE) << "Learning threshold must be finite and nonnegative, here threshold=" << learningThreshold;
}

ActiveLearningReliabilityFunction * ActiveLearningReliabilityFunction::clone() const 
{
  return new ActiveLearningReliabilityFunction(*this);
}

Point ActiveLearningReliabilityFunction::operator()(const Point & x) const 
{
  return Point(1, computeAsScalar(x));
}

Sample ActiveLearningReliabilityFunction::operator()(const Sample & x) const
{
  // The single-argument call carries no design sample: criteria that do not
  // need it (U, EFF) ignore it, criteria that do (GMM) reject the empty one
  return operator()(x, Sample(0, x.getDimension()));
}

Field ActiveLearningReliabilityFunction::operator()(const OT::Field&) const 
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::operator()(const OT::Field&)";
}
  
Sample ActiveLearningReliabilityFunction::operator()(const Sample & inputSample, const Sample &) const 
{


  const UnsignedInteger size = inputSample.getSize();
  // avoid creating size points
  Point inputSample_i(inputSample.getDimension());
  Sample outS(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < inputSample_i.getSize(); ++ j)
    {
      inputSample_i[j] = inputSample(i, j);
    } 
    outS(i, 0) = computeAsScalar(inputSample_i);
  }
  return outS;
}

Scalar ActiveLearningReliabilityFunction::computeAsScalar(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::computeAsScalar(const Point & )";
}

Bool ActiveLearningReliabilityFunction::checkConvergenceLearning(const Sample &) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::checkConvergenceLearning(const Sample &)";
}

Bool ActiveLearningReliabilityFunction::isMaximization() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::isMaximization()";
}

/* Return sample corresponding to  criterion*/
Sample ActiveLearningReliabilityFunction::getInfillSample(const Sample &,
                                                          const Sample &) const 
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunction::getInfillSample(const Sample & , const Sample & criterionValues) "; 
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

/* Dimensions accessors */
UnsignedInteger ActiveLearningReliabilityFunction::getInputDimension() const
{
  return gprResult_.getMetaModel().getInputDimension();
}

UnsignedInteger ActiveLearningReliabilityFunction::getOutputDimension() const
{
  return 1;
}
/* accessor reliability threshold */
void ActiveLearningReliabilityFunction::setReliabilityThreshold(const Scalar reliabilityThreshold)
{
  if (!std::isfinite(reliabilityThreshold)) throw InvalidArgumentException(HERE) << "Reliability threshold must be finite, here threshold=" << reliabilityThreshold;
  reliabilityThreshold_ = reliabilityThreshold;
}

Scalar ActiveLearningReliabilityFunction::getReliabilityThreshold() const
{
  return reliabilityThreshold_;
}

/* accessor learning threshold */
void ActiveLearningReliabilityFunction::setLearningThreshold(const Scalar learningThreshold)
{
  if (!(learningThreshold >= 0.0) || !std::isfinite(learningThreshold)) throw InvalidArgumentException(HERE) << "Learning threshold must be finite and nonnegative, here threshold=" << learningThreshold;
  learningThreshold_ = learningThreshold;
}

Scalar ActiveLearningReliabilityFunction::getLearningThreshold() const
{
  return learningThreshold_;
}

GaussianProcessRegressionResult ActiveLearningReliabilityFunction::getGaussianProcessRegression() const
{
  return gprResult_;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningReliabilityFunction::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("learningThreshold_", learningThreshold_);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningReliabilityFunction::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("learningThreshold_", learningThreshold_);
  // gprCov_ is redundant with gprResult_: rebuild it instead of persisting it
  gprCov_ = GaussianProcessConditionalCovariance(gprResult_);
}

END_NAMESPACE_OPENTURNS
