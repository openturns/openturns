//                                               -*- C++ -*-
/**
 *  @brief Domain is the interface of DomainImplementation
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
#ifndef OPENTURNS_DOMAIN_HXX
#define OPENTURNS_DOMAIN_HXX

#include "TypedInterfaceObject.hxx"
#include "DomainImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Domain
 *
 * A class that holds a domain
 */
class OT_API Domain
  : public TypedInterfaceObject<DomainImplementation>
{
  CLASSNAME;

public:

  /** Default constructor */
  Domain();

  /** Copy-Standard constructors */
  Domain(const DomainImplementation & implementation);

  /** Standard constructor based on Interval(a,b) */
  Domain(const NumericalPoint & a,
         const NumericalPoint & b);

  /** Check if the domain is empty, ie if its volume is zero */
  Bool isEmpty() const;

  /** Check if the mesh is numerically empty, i.e. if the volume of its discretization is zero */
  Bool isNumericallyEmpty() const;

  /** Check if the closed domain contains a given point */
  virtual Bool contains(const NumericalPoint & point) const;

  /** Check if the given point is numerically inside of the domain, i.e. in its discretization */
  Bool numericallyContains(const NumericalPoint & point) const;

  /** Get the numerical volume of the discretization of the domain */
  NumericalScalar getNumericalVolume() const;

  /** Get the volume of the domain */
  NumericalScalar getVolume() const;

  /** Dimension accessors */
  virtual UnsignedInteger getDimension() const;

}; /* class Domain */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAIN_HXX */
