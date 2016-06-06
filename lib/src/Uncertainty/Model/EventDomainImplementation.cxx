//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector (composite) built
 *        upon a boolean function which is described by a domain appartenance.
 *        The antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/EventDomainImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EventDomainImplementation);

static const Factory<EventDomainImplementation> Factory_EventDomainImplementation;
/* Default constructor */
EventDomainImplementation::EventDomainImplementation()
  : CompositeRandomVector()
{
  // Nothing to do
}

/* Constructor from RandomVector */
EventDomainImplementation::EventDomainImplementation(const RandomVectorImplementation & antecedent,
    const Domain & domain)
  : CompositeRandomVector()
  , domain_(domain)
{
  // Event can only be constructed from composite random vectors
  if (!antecedent.isComposite())
    throw InvalidArgumentException(HERE) << "Event can only be constructed from composite random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of EventDomainImplementation "
                                         << " has incorrect type";
  function_ = antecedent.getFunction();
  p_antecedent_ = antecedent.getAntecedent();
  setName(antecedent.getName());
  setDescription(Description(1, OSS() << antecedent.getName() << " in " << domain.getName()));
}

EventDomainImplementation * EventDomainImplementation::clone() const
{
  return new EventDomainImplementation(*this);
}

/* String converter */
String EventDomainImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << EventDomainImplementation::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " domain=" << domain_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger EventDomainImplementation::getDimension() const
{
  return 1;
}

/* Domain accessor */
Domain EventDomainImplementation::getDomain() const
{
  return domain_;
}


/* Realization accessor */
NumericalPoint EventDomainImplementation::getRealization() const
{
  return NumericalPoint(1, domain_.contains(CompositeRandomVector::getRealization()));
}

/* Numerical sample accessor */
NumericalSample EventDomainImplementation::getSample(const UnsignedInteger size) const
{
  // First, compute a sample of the event antecedent
  const NumericalSample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample to check each point in sequence
  NumericalSample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i][0] = domain_.contains(returnSample[i]);
  result.setName("EventDomainImplementation sample");
  result.setDescription(getDescription());
  return result;
}

/* Method save() stores the object through the StorageManager */
void EventDomainImplementation::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "domain_", domain_ );
}

/* Method load() reloads the object from the StorageManager */
void EventDomainImplementation::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "domain_", domain_ );
}

END_NAMESPACE_OPENTURNS
