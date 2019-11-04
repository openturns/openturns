//                                               -*- C++ -*-
/**
 *  @brief DomainIntersection represents the intersection of two Domain
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
#ifndef OPENTURNS_DOMAININTERSECTION_HXX
#define OPENTURNS_DOMAININTERSECTION_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainIntersection
 *
 * A class that holds the intersection of two Domain.
 * A point x is contained in this domain if it is contained
 * in both left and right domains.
 */
class OT_API DomainIntersection
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef DomainImplementation::BoolCollection  BoolCollection;

  /** Default constructor */
  DomainIntersection();

  /** Default constructor */
  DomainIntersection(const Domain & left, const Domain & right);

  /** Virtual constructor method */
  virtual DomainIntersection * clone() const;

  /** Check if the given point is inside this domain */
  Bool contains(const Point & point) const;

  /** Check if the given points are inside this domain */
  BoolCollection contains(const Sample & sample) const;

  /** Comparison operator */
  Bool operator == (const DomainIntersection & other) const;
  Bool operator != (const DomainIntersection & other) const;
  using DomainImplementation::operator ==;
  using DomainImplementation::operator !=;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  // The first domain
  Domain left_;

  // The second domain
  Domain right_;

}; /* class DomainIntersection */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAININTERSECTION_HXX */
