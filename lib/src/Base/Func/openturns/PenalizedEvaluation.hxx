//                                               -*- C++ -*-
/**
 * @brief Penalized evaluation, returning a large value on failure
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

#ifndef OPENTURNS_PENALIZEDEVALUATION_HXX
#define OPENTURNS_PENALIZEDEVALUATION_HXX

#include <cmath>

#include "openturns/EvaluationProxy.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedEvaluation
 *
 * This class wraps an Evaluation and returns a penalized value
 * when the underlying evaluation throws, e.g. for optimization.
 */
class OT_API PenalizedEvaluation
  : public EvaluationProxy
{
  CLASSNAME
public:

  /** Default constructor */
  PenalizedEvaluation();

  /** Parameter constructor */
  explicit PenalizedEvaluation(const Evaluation & evaluation,
                               const Scalar penalizedValue = std::sqrt(SpecFunc::MaxScalar));

  /** Parameter constructor with per-output penalized values */
  PenalizedEvaluation(const Evaluation & evaluation,
                      const Point & penalizedValue);

  /** Virtual constructor */
  PenalizedEvaluation * clone() const override;

  /** Function implementation accessors */
  void setEvaluation(const Evaluation & evaluation);
  using EvaluationProxy::getEvaluation;

  /** Penalized value accessors */
  Point getPenalizedValue() const;
  void setPenalizedValue(const Point & penalizedValue);

  /** Comparison operator */
  using EvaluationProxy::operator ==;
  Bool operator ==(const PenalizedEvaluation & other) const;
protected:
  Bool equals(const EvaluationImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inPoint) const override;

  /** Operator () */
  Sample operator() (const Sample & inSample) const override;

  /** Operator () */
  Field operator() (const Field & inField) const override;

  /** Get the evaluation corresponding to indices components */
  using EvaluationProxy::getMarginal;
  Evaluation getMarginal(const UnsignedInteger i) const override;
  Evaluation getMarginal(const Indices & indices) const override;

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const override;

  /** Is it safe to call in parallel? */
  Bool isParallel() const override;

  /** Draw methods must use the penalized evaluation, not the wrapped one (EvaluationProxy::draw delegates to the wrapped evaluation) */
  Graph draw(const UnsignedInteger inputMarginal,
             const UnsignedInteger outputMarginal,
             const Point & centralPoint,
             const Scalar xMin,
             const Scalar xMax,
             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;
  Graph draw(const UnsignedInteger firstInputMarginal,
             const UnsignedInteger secondInputMarginal,
             const UnsignedInteger outputMarginal,
             const Point & centralPoint,
             const Point & xMin,
             const Point & xMax,
             const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE,
             const Bool isFilled = ResourceMap::GetAsBool("Contour-DefaultIsFilled")) const override;
  Graph draw(const Scalar xMin,
             const Scalar xMax,
             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;
  Graph draw(const Point & xMin,
             const Point & xMax,
             const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Penalized value used on failure (scalar stored as size-1 point, or per-output values) */
  Point penalizedValue_;

}; /* class PenalizedEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDEVALUATION_HXX */
