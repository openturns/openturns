//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/ComparisonOperator.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class IndicatorEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API IndicatorEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  IndicatorEvaluation();

  /** Default constructor */
  IndicatorEvaluation(const Evaluation & evaluation,
                      const ComparisonOperator & comparisonOperator,
                      const Scalar threshold);

  /** Virtual constructor */
  IndicatorEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const IndicatorEvaluation & other) const;

  /** String converter */
  String __repr__() const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;

  /** Accessor for the underlying evaluation */
  Evaluation getEvaluation() const;
  void setEvaluation(const Evaluation & evaluation);

  /** Accessor for the comparison operator */
  ComparisonOperator getComparisonOperator() const;
  void setComparisonOperator(const ComparisonOperator & comparisonOperator);

  /** Accessor for the threshold */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  Evaluation evaluation_;
  ComparisonOperator comparisonOperator_;
  Scalar threshold_;

}; /* class IndicatorEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICATORNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
