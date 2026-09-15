//                                               -*- C++ -*-
/**
 * @brief Failure-penalty evaluation, returning a large value on failure
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

#include "openturns/FailurePenaltyEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BatchFailedException.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FailurePenaltyEvaluation)

static const Factory<FailurePenaltyEvaluation> Factory_FailurePenaltyEvaluation;

/* Default constructor */
FailurePenaltyEvaluation::FailurePenaltyEvaluation()
  : EvaluationProxy()
  , penalizedValue_(1, std::sqrt(SpecFunc::MaxScalar))
{
  // Nothing to do
}

/* Parameter constructor */
FailurePenaltyEvaluation::FailurePenaltyEvaluation(const Evaluation & evaluation,
    const Scalar penalizedValue)
  : EvaluationProxy(evaluation)
  , penalizedValue_(evaluation.getOutputDimension(), penalizedValue)
{
  // Nothing to do
}

/* Parameter constructor with per-output penalized values */
FailurePenaltyEvaluation::FailurePenaltyEvaluation(const Evaluation & evaluation,
    const Point & penalizedValue)
  : EvaluationProxy(evaluation)
  , penalizedValue_(penalizedValue)
{
  if (penalizedValue.getDimension() != evaluation.getOutputDimension())
    throw InvalidArgumentException(HERE) << "The penalized value must match the output dimension";
}

/* Virtual constructor */
FailurePenaltyEvaluation * FailurePenaltyEvaluation::clone() const
{
  return new FailurePenaltyEvaluation(*this);
}

/* Function implementation accessors */
void FailurePenaltyEvaluation::setEvaluation(const Evaluation & evaluation)
{
  if (penalizedValue_.getDimension() != evaluation.getOutputDimension())
    throw InvalidArgumentException(HERE) << "The penalized value must match the output dimension";
  evaluation_ = evaluation;
}

/* Penalized value accessors */
Point FailurePenaltyEvaluation::getPenalizedValue() const
{
  return penalizedValue_;
}

void FailurePenaltyEvaluation::setPenalizedValue(const Point & penalizedValue)
{
  if (penalizedValue.getDimension() != evaluation_.getOutputDimension())
    throw InvalidArgumentException(HERE) << "The penalized value must match the output dimension";
  penalizedValue_ = penalizedValue;
}

/* Comparison operator */
Bool FailurePenaltyEvaluation::operator ==(const FailurePenaltyEvaluation & other) const
{
  return hasEqualBase(other) && (evaluation_ == other.evaluation_) && (penalizedValue_ == other.penalizedValue_);
}

Bool FailurePenaltyEvaluation::equals(const EvaluationImplementation & other) const
{
  return *this == dynamic_cast<const FailurePenaltyEvaluation &>(other);
}

/* String converter */
String FailurePenaltyEvaluation::__repr__() const
{
  return OSS(true) << "FailurePenaltyEvaluation(" << evaluation_.getImplementation()->__repr__() << ", penalizedValue=" << penalizedValue_.__repr__() << ")";
}

String FailurePenaltyEvaluation::__str__(const String & offset) const
{
  return OSS(false) << offset << "FailurePenaltyEvaluation(" << evaluation_.getImplementation()->__str__() << ", penalizedValue=" << penalizedValue_.__str__() << ")";
}

/* Operator () */
Point FailurePenaltyEvaluation::operator() (const Point & inPoint) const
{
  try
  {
    const Point result(evaluation_(inPoint));
    callsNumber_.increment();
    return result;
  }
  catch (const InterruptionException &)
  {
    throw;
  }
  catch (const std::exception & exc)
  {
    LOGDEBUG(OSS() << "FailurePenaltyEvaluation: caught exception at point " << inPoint.__str__() << ": " << exc.what() << ", returning penalized value");
    callsNumber_.increment();
    return penalizedValue_;
  }
}

/* Operator () */
Sample FailurePenaltyEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, penalizedValue_);
  result.setDescription(getOutputDescription());
  // Fast path: try the batch evaluation
  try
  {
    const Sample resultOk(evaluation_(inSample));
    callsNumber_.fetchAndAdd(size);
    return resultOk;
  }
  catch (const BatchFailedException & exc)
  {
    LOGDEBUG(OSS() << "FailurePenaltyEvaluation: caught BatchFailedException (" << exc.getFailedIndices().getSize() << " failed points), returning penalized values");
    callsNumber_.fetchAndAdd(size);
    const Indices succeededIndices(exc.getSucceededIndices());
    const Sample succeededSample(exc.getOutputSample());
    for (UnsignedInteger i = 0; i < succeededIndices.getSize(); ++ i)
      result[succeededIndices[i]] = succeededSample[i];
    return result;
  }
  catch (const InterruptionException &)
  {
    throw;
  }
  catch (const std::exception & exc)
  {
    LOGDEBUG(OSS() << "FailurePenaltyEvaluation: caught exception on sample: " << exc.what() << ", retrying point by point");
  }
  // Fallback: evaluate point by point to salvage successful points
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    try
    {
      result[i] = evaluation_(inSample[i]);
    }
    catch (const InterruptionException &)
    {
      throw;
    }
    catch (const std::exception & exc)
    {
      LOGDEBUG(OSS() << "FailurePenaltyEvaluation: caught exception at point " << inSample[i] << ": " << exc.what() << ", returning penalized value");
    }
    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InterruptionException(HERE) << "User stopped evaluation";
  }
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Operator () */
Field FailurePenaltyEvaluation::operator() (const Field & inField) const
{
  return Field(inField.getMesh(), operator()(inField.getValues()));
}

/* Get the i-th marginal evaluation */
Evaluation FailurePenaltyEvaluation::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the evaluation corresponding to indices components */
Evaluation FailurePenaltyEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal evaluation must be in the range [0, outputDimension-1] and must be different";
  Point marginalPenalizedValue(indices.getSize());
  for (UnsignedInteger i = 0; i < indices.getSize(); ++ i)
    marginalPenalizedValue[i] = penalizedValue_[indices[i]];
  return new FailurePenaltyEvaluation(evaluation_.getMarginal(indices), marginalPenalizedValue);
}

/* Get the number of calls to operator() */
UnsignedInteger FailurePenaltyEvaluation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Is it safe to call in parallel? */
Bool FailurePenaltyEvaluation::isParallel() const
{
  return evaluation_.getImplementation()->isParallel();
}

/* Draw methods: evaluate through the penalized operator, not the wrapped evaluation */
Graph FailurePenaltyEvaluation::draw(const UnsignedInteger inputMarginal,
                                const UnsignedInteger outputMarginal,
                                const Point & centralPoint,
                                const Scalar xMin,
                                const Scalar xMax,
                                const UnsignedInteger pointNumber,
                                const GraphImplementation::LogScale scale) const
{
  return EvaluationImplementation::draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

Graph FailurePenaltyEvaluation::draw(const UnsignedInteger firstInputMarginal,
                                const UnsignedInteger secondInputMarginal,
                                const UnsignedInteger outputMarginal,
                                const Point & centralPoint,
                                const Point & xMin,
                                const Point & xMax,
                                const Indices & pointNumber,
                                const GraphImplementation::LogScale scale,
                                const Bool isFilled) const
{
  return EvaluationImplementation::draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale, isFilled);
}

Graph FailurePenaltyEvaluation::draw(const Scalar xMin,
                                const Scalar xMax,
                                const UnsignedInteger pointNumber,
                                const GraphImplementation::LogScale scale) const
{
  return EvaluationImplementation::draw(xMin, xMax, pointNumber, scale);
}

Graph FailurePenaltyEvaluation::draw(const Point & xMin,
                                const Point & xMax,
                                const Indices & pointNumber,
                                const GraphImplementation::LogScale scale) const
{
  return EvaluationImplementation::draw(xMin, xMax, pointNumber, scale);
}

/* Method save() stores the object through the StorageManager */
void FailurePenaltyEvaluation::save(Advocate & adv) const
{
  EvaluationProxy::save(adv);
  adv.saveAttribute("penalizedValue_", penalizedValue_);
}

/* Method load() reloads the object from the StorageManager */
void FailurePenaltyEvaluation::load(Advocate & adv)
{
  EvaluationProxy::load(adv);
  adv.loadAttribute("penalizedValue_", penalizedValue_);
}

END_NAMESPACE_OPENTURNS
