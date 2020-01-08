//                                               -*- C++ -*-
/**
 *  @brief DomainComplement represents the complement of a Domain
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DomainComplement.hxx"
#include "openturns/Domain.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainComplement)

static const Factory<DomainComplement> Factory_DomainComplement;


/* Default constructor */
DomainComplement::DomainComplement()
  : DomainImplementation()
{
  // Nothing to do
}

/* Default constructor */
DomainComplement::DomainComplement(const Domain & domain)
  : DomainImplementation(domain.getDimension())
  , domain_(domain)
{
  // Nothing to do
}

/* Clone method */
DomainComplement * DomainComplement::clone() const
{
  return new DomainComplement(*this);
}

/* String converter */
String DomainComplement::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " domain=" << domain_;

}

String DomainComplement::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainComplement::contains(const Point & point) const
{
  return !domain_.contains(point);
}

/* Check if the given points are inside of the domain */
DomainComplement::BoolCollection DomainComplement::contains(const Sample & sample) const
{
  BoolCollection result(domain_.contains(sample));
  for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
    result[i] = (result[i] == 0 ? 1 : 0);
  return result;
}

Bool DomainComplement::operator == (const DomainComplement & other) const
{
  if (this == &other) return true;
  return domain_ == other.domain_;
}

Bool DomainComplement::operator != (const DomainComplement & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainComplement::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("domain_", domain_);
}

/* Method load() reloads the object from the StorageManager */
void DomainComplement::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("domain_", domain_);
}

END_NAMESPACE_OPENTURNS
