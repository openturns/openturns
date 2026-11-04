//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunctionImplementation implements parent class for 
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

#include "openturns/ActiveLearningReliabilityFunctionImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunctionImplementation
 */
 
CLASSNAMEINIT(ActiveLearningReliabilityFunctionImplementation)

static const Factory<ActiveLearningReliabilityFunctionImplementation> Factory_ActiveLearningReliabilityFunctionImplementation;

// Default constructor
ActiveLearningReliabilityFunctionImplementation::ActiveLearningReliabilityFunctionImplementation()
  : EvaluationImplementation()
  , reliabilityThreshold_(0.0)
  , learningThreshold_(0.0)
{
  // Nothing to do
}

/* Constructor with parameters */
ActiveLearningReliabilityFunctionImplementation::ActiveLearningReliabilityFunctionImplementation(const Scalar reliabilityThreshold,
                                                                     const Scalar learningThreshold)
    : EvaluationImplementation()
    , reliabilityThreshold_(reliabilityThreshold)
    , learningThreshold_(learningThreshold)
{
  if (!std::isfinite(reliabilityThreshold)) throw InvalidArgumentException(HERE) << "Reliability threshold must be finite, here threshold=" << reliabilityThreshold;
  if (!(learningThreshold >= 0.0) || !std::isfinite(learningThreshold)) throw InvalidArgumentException(HERE) << "Learning threshold must be finite and nonnegative, here threshold=" << learningThreshold;
}

ActiveLearningReliabilityFunctionImplementation * ActiveLearningReliabilityFunctionImplementation::clone() const 
{
  return new ActiveLearningReliabilityFunctionImplementation(*this);
}

Point ActiveLearningReliabilityFunctionImplementation::operator()(const Point & x) const 
{
  return Point(1, computeAsScalar(x));
}

Sample ActiveLearningReliabilityFunctionImplementation::operator()(const Sample & x) const
{
  // The single-argument call carries no design sample: criteria that do not
  // need it (U, EFF) ignore it, criteria that do (GMM) reject the empty one
  return operator()(x, Sample(0, x.getDimension()));
}

Field ActiveLearningReliabilityFunctionImplementation::operator()(const OT::Field&) const 
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunctionImplementation::operator()(const OT::Field&)";
}
  
Sample ActiveLearningReliabilityFunctionImplementation::operator()(const Sample & inputSample, const Sample &) const 
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

Scalar ActiveLearningReliabilityFunctionImplementation::computeAsScalar(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunctionImplementation::computeAsScalar(const Point & )";
}

Bool ActiveLearningReliabilityFunctionImplementation::checkConvergenceLearning(const Sample &) const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunctionImplementation::checkConvergenceLearning(const Sample &)";
}

Bool ActiveLearningReliabilityFunctionImplementation::isMaximization() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunctionImplementation::isMaximization()";
}

/* Return sample corresponding to  criterion*/
Sample ActiveLearningReliabilityFunctionImplementation::getInfillSample(const Sample &,
                                                          const Sample &) const 
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityFunctionImplementation::getInfillSample(const Sample & , const Sample & criterionValues) "; 
}

/* String converter */
String ActiveLearningReliabilityFunctionImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << EvaluationImplementation::__repr__();
  return oss;
}


/* update of GPR model */
void ActiveLearningReliabilityFunctionImplementation::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  gprResult_ = gprResult;
  gprCov_ = GaussianProcessConditionalCovariance(gprResult);
}

/* Dimensions accessors */
UnsignedInteger ActiveLearningReliabilityFunctionImplementation::getInputDimension() const
{
  return gprResult_.getMetaModel().getInputDimension();
}

UnsignedInteger ActiveLearningReliabilityFunctionImplementation::getOutputDimension() const
{
  return 1;
}
/* accessor reliability threshold */
void ActiveLearningReliabilityFunctionImplementation::setReliabilityThreshold(const Scalar reliabilityThreshold)
{
  if (!std::isfinite(reliabilityThreshold)) throw InvalidArgumentException(HERE) << "Reliability threshold must be finite, here threshold=" << reliabilityThreshold;
  reliabilityThreshold_ = reliabilityThreshold;
}

Scalar ActiveLearningReliabilityFunctionImplementation::getReliabilityThreshold() const
{
  return reliabilityThreshold_;
}

/* accessor learning threshold */
void ActiveLearningReliabilityFunctionImplementation::setLearningThreshold(const Scalar learningThreshold)
{
  if (!(learningThreshold >= 0.0) || !std::isfinite(learningThreshold)) throw InvalidArgumentException(HERE) << "Learning threshold must be finite and nonnegative, here threshold=" << learningThreshold;
  learningThreshold_ = learningThreshold;
}

Scalar ActiveLearningReliabilityFunctionImplementation::getLearningThreshold() const
{
  return learningThreshold_;
}

GaussianProcessRegressionResult ActiveLearningReliabilityFunctionImplementation::getGaussianProcessRegression() const
{
  return gprResult_;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningReliabilityFunctionImplementation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("learningThreshold_", learningThreshold_);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningReliabilityFunctionImplementation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("reliabilityThreshold_", reliabilityThreshold_);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("learningThreshold_", learningThreshold_);
  // gprCov_ is redundant with gprResult_: rebuild it instead of persisting it
  gprCov_ = GaussianProcessConditionalCovariance(gprResult_);
}

END_NAMESPACE_OPENTURNS
