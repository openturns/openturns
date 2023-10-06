//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector (composite) built
 *        upon a boolean function which is described by a domain appartenance.
 *        The antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/DomainEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainEvent)

static const Factory<DomainEvent> Factory_DomainEvent;
/* Default constructor */
DomainEvent::DomainEvent()
  : CompositeRandomVector()
{
  // Nothing to do
}

/* Constructor from RandomVector */
DomainEvent::DomainEvent(const RandomVector & antecedent,
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

DomainEvent * DomainEvent::clone() const
{
  return new DomainEvent(*this);
}

/* String converter */
String DomainEvent::__repr__() const
{
  OSS oss;
  oss << "class=" << DomainEvent::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " domain=" << domain_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger DomainEvent::getDimension() const
{
  return 1;
}

/* Domain accessor */
Domain DomainEvent::getDomain() const
{
  return domain_;
}


/* Realization accessor */
Point DomainEvent::getRealization() const
{
  return Point(1, domain_.contains(CompositeRandomVector::getRealization()));
}

/* Fixed value accessor */
Point DomainEvent::getFrozenRealization(const Point & fixedValue) const
{
  return Point(1, domain_.contains(CompositeRandomVector::getFrozenRealization(fixedValue)));
}

/* Numerical sample accessor */
Sample DomainEvent::getSample(const UnsignedInteger size) const
{
  // First, compute a sample of the event antecedent
  const Sample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample to check each point in sequence
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    result(i, 0) = domain_.contains(returnSample[i]);
  result.setName("DomainEvent sample");
  result.setDescription(getDescription());
  return result;
}

Bool DomainEvent::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void DomainEvent::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "domain_", domain_ );
}

/* Method load() reloads the object from the StorageManager */
void DomainEvent::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "domain_", domain_ );
}

END_NAMESPACE_OPENTURNS
