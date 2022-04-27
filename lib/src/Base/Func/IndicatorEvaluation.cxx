//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(IndicatorEvaluation)

static const Factory<IndicatorEvaluation> Factory_IndicatorEvaluation;

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation()
  : EvaluationImplementation()
  , domain_(Interval())
{
  // Nothing to do
} // IndicatorEvaluation

/* Default constructor */
IndicatorEvaluation::IndicatorEvaluation(const Domain & domain)
  : EvaluationImplementation()
  , domain_(domain)
{
  EvaluationImplementation::setInputDescription(Description::BuildDefault(domain_.getDimension(), "x"));
} // IndicatorEvaluation

/* Virtual constructor */
IndicatorEvaluation * IndicatorEvaluation::clone() const
{
  return new IndicatorEvaluation(*this);
}


/* Comparison operator */
Bool IndicatorEvaluation::operator ==(const IndicatorEvaluation & other) const
{
  return domain_ == other.domain_;
}

/* String converter */
String IndicatorEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << IndicatorEvaluation::GetClassName()
      << " name=" << getName()
      << " domain=" << domain_.getImplementation()->__repr__();
  return oss;
}

/* Operator () */
Point IndicatorEvaluation::operator() (const Point & inP) const
{
  Point result(1);
  if (domain_.contains(inP)) result[0] = 1.0;
  callsNumber_.increment();
  return result;
}

Sample IndicatorEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  const UnsignedIntegerCollection belong(domain_.contains(inSample));
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) result(i, 0) = belong[i];
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger IndicatorEvaluation::getInputDimension() const
{
  return domain_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger IndicatorEvaluation::getOutputDimension() const
{
  return 1;
}

/* Method save() stores the object through the StorageManager */
void IndicatorEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "domain_", domain_ );
}

/* Method load() reloads the object from the StorageManager */
void IndicatorEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "domain_", domain_ );
}

END_NAMESPACE_OPENTURNS
