//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector (composite) built
 *        upon a boolean function which is described by a domain appartenance.
 *        The antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/EventDomain.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EventDomain)

static const Factory<EventDomain> Factory_EventDomain;
/* Default constructor */
EventDomain::EventDomain()
  : CompositeRandomVector()
{
  // Nothing to do
}

/* Constructor from RandomVector */
EventDomain::EventDomain(const RandomVectorImplementation & antecedent,
                         const Domain & domain)
  : CompositeRandomVector()
  , domain_(domain)
{
  if (antecedent.isComposite())
  {
    function_ = antecedent.getFunction();
    antecedent_ = antecedent.getAntecedent();
  }
  else
  {
    function_ = IdentityFunction(antecedent.getDimension());
    antecedent_ = antecedent;
  }

  if (domain.getDimension() != antecedent.getDimension())
    throw InvalidArgumentException(HERE) << "The domain dimension must match the vector dimension";

  setName(antecedent.getName());
  setDescription(Description(1, OSS() << antecedent.getName() << " in " << domain.getName()));
}

EventDomain * EventDomain::clone() const
{
  return new EventDomain(*this);
}

/* String converter */
String EventDomain::__repr__() const
{
  OSS oss;
  oss << "class=" << EventDomain::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " domain=" << domain_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger EventDomain::getDimension() const
{
  return 1;
}

/* Domain accessor */
Domain EventDomain::getDomain() const
{
  return domain_;
}


/* Realization accessor */
Point EventDomain::getRealization() const
{
  return Point(1, domain_.contains(CompositeRandomVector::getRealization()));
}

/* Numerical sample accessor */
Sample EventDomain::getSample(const UnsignedInteger size) const
{
  // First, compute a sample of the event antecedent
  const Sample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample to check each point in sequence
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    result(i, 0) = domain_.contains(returnSample[i]);
  result.setName("EventDomain sample");
  result.setDescription(getDescription());
  return result;
}

Bool EventDomain::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void EventDomain::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "domain_", domain_ );
}

/* Method load() reloads the object from the StorageManager */
void EventDomain::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "domain_", domain_ );
}

END_NAMESPACE_OPENTURNS
