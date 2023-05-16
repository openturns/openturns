//                                               -*- C++ -*-
/**
 *  @brief DomainUnion represents the union of two Domain
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
#ifndef OPENTURNS_DOMAINUNION_HXX
#define OPENTURNS_DOMAINUNION_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainUnion
 *
 * A class that holds the union of two Domain.
 * A point x is contained in this domain if it is contained
 * either in left or right domains.
 */
class OT_API DomainUnion
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef Collection<Domain> DomainCollection;
  typedef PersistentCollection<Domain> DomainPersistentCollection;

  /** Default constructor */
  DomainUnion();

  /** Default constructor */
  explicit DomainUnion(const DomainCollection & collection);

  /** Virtual constructor method */
  DomainUnion * clone() const override;

  /** Check if the given point is inside this domain */
  Bool contains(const Point & point) const override;
  using DomainImplementation::contains;

  /** Compute the Euclidean distance from a given point to the domain */
  Scalar computeDistance(const Point & point) const override;
  using DomainImplementation::computeDistance;

  /** Comparison operator */
  Bool operator == (const DomainUnion & other) const;
  Bool operator != (const DomainUnion & other) const;
  using DomainImplementation::operator ==;
  using DomainImplementation::operator !=;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  DomainPersistentCollection collection_;

}; /* class DomainUnion */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINUNION_HXX */
