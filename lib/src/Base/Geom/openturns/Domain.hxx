//                                               -*- C++ -*-
/**
 *  @brief Domain is the interface of DomainImplementation
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
#ifndef OPENTURNS_DOMAIN_HXX
#define OPENTURNS_DOMAIN_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/DomainImplementation.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Domain
 *
 * A class that holds a domain
 */
class OT_API Domain
  : public TypedInterfaceObject<DomainImplementation>
{
  CLASSNAME

public:
  typedef DomainImplementation::BoolCollection BoolCollection;

  /** Default constructor */
  Domain();

  /** Copy-Standard constructors */
  Domain(const DomainImplementation & implementation);

  /** Check if the closed domain contains a given point */
  virtual Bool contains(const Point & point) const;

  /** Check if the closed domain contains given points */
  virtual BoolCollection contains(const Sample & sample) const;

  /** Dimension accessors */
  virtual UnsignedInteger getDimension() const;

  /** String converter */
  virtual String __repr__() const;

}; /* class Domain */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAIN_HXX */
