//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/IndicatorEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Less.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicatorEvaluation)

static const Factory<IndicatorEvaluation> Factory_IndicatorEvaluation;

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation()
  : EvaluationImplementation()
  , evaluation_()
  , comparisonOperator_()
  , threshold_(0.0)
{
  // Nothing to do
} // IndicatorEvaluation

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation(const Evaluation & evaluation,
    const ComparisonOperator & comparisonOperator,
    const Scalar threshold)
  : EvaluationImplementation()
  , evaluation_()
  , comparisonOperator_(comparisonOperator)
  , threshold_(threshold)
{
  setEvaluation(evaluation);
  setDescription(evaluation.getDescription());
} // IndicatorEvaluation

/* Virtual constructor */
IndicatorEvaluation * IndicatorEvaluation::clone() const
{
  return new IndicatorEvaluation(*this);
}


/* Comparison operator */
Bool IndicatorEvaluation::operator ==(const IndicatorEvaluation & ) const
{
  return true;
}

/* String converter */
String IndicatorEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << IndicatorEvaluation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_.getImplementation()->__repr__()
      << " comparisonOperator=" << comparisonOperator_
      << " threshold=" << threshold_;
  return oss;
}

/* Operator () */
Point IndicatorEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  const Point result(1, (comparisonOperator_.compare(evaluation_.operator()(inP)[0], threshold_) ? 1.0 : 0.0));
  callsNumber_.increment();
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger IndicatorEvaluation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger IndicatorEvaluation::getOutputDimension() const
{
  return 1;
}

/* Accessor for the underlying evaluation */
Evaluation IndicatorEvaluation::getEvaluation() const
{
  return evaluation_;
}

void IndicatorEvaluation::setEvaluation(const Evaluation & evaluation)
{
  if (evaluation.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot use an evaluation implementation with output dimension not equal to 1";
  evaluation_ = evaluation;
}

/* Accessor for the comparison operator */
ComparisonOperator IndicatorEvaluation::getComparisonOperator() const
{
  return comparisonOperator_;
}

void IndicatorEvaluation::setComparisonOperator(const ComparisonOperator & comparisonOperator)
{
  comparisonOperator_ = comparisonOperator;
}

/* Accessor for the threshold */
Scalar IndicatorEvaluation::getThreshold() const
{
  return threshold_;
}

void IndicatorEvaluation::setThreshold(const Scalar threshold)
{
  threshold_ = threshold;
}

/* Method save() stores the object through the StorageManager */
void IndicatorEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
  adv.saveAttribute( "comparisonOperator_", comparisonOperator_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void IndicatorEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  adv.loadAttribute( "comparisonOperator_", comparisonOperator_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS
