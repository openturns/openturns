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

#include "openturns/DistanceToDomainEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(DistanceToDomainEvaluation)

static const Factory<DistanceToDomainEvaluation> Factory_DistanceToDomainEvaluation;

/* Default constructor */
DistanceToDomainEvaluation::DistanceToDomainEvaluation()
  : EvaluationImplementation()
  , domain_(Interval())
{
  // Nothing to do
} // DistanceToDomainEvaluation

/* Default constructor */
DistanceToDomainEvaluation::DistanceToDomainEvaluation(const Domain & domain)
  : EvaluationImplementation()
  , domain_(domain)
{
  EvaluationImplementation::setInputDescription(Description::BuildDefault(domain_.getDimension(), "x"));
} // DistanceToDomainEvaluation

/* Virtual constructor */
DistanceToDomainEvaluation * DistanceToDomainEvaluation::clone() const
{
  return new DistanceToDomainEvaluation(*this);
}


/* Comparison operator */
Bool DistanceToDomainEvaluation::operator ==(const DistanceToDomainEvaluation & other) const
{
  return domain_ == other.domain_;
}

/* String converter */
String DistanceToDomainEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << DistanceToDomainEvaluation::GetClassName()
      << " name=" << getName()
      << " domain=" << domain_.getImplementation()->__repr__();
  return oss;
}

/* Operator () */
Point DistanceToDomainEvaluation::operator() (const Point & inP) const
{
  Point result(1);
  result[0] = domain_.computeDistance(inP);
  callsNumber_.increment();
  return result;
}

Sample DistanceToDomainEvaluation::operator() (const Sample & inSample) const
{
  callsNumber_.fetchAndAdd(inSample.getSize());
  return domain_.computeDistance(inSample);
}

/* Accessor for input point dimension */
UnsignedInteger DistanceToDomainEvaluation::getInputDimension() const
{
  return domain_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger DistanceToDomainEvaluation::getOutputDimension() const
{
  return 1;
}

/* Method save() stores the object through the StorageManager */
void DistanceToDomainEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "domain_", domain_ );
}

/* Method load() reloads the object from the StorageManager */
void DistanceToDomainEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "domain_", domain_ );
}

END_NAMESPACE_OPENTURNS
