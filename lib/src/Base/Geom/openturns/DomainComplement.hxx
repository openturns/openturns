//                                               -*- C++ -*-
/**
 *  @brief DomainComplement represents the complement of a Domain
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
#ifndef OPENTURNS_DOMAINCOMPLEMENT_HXX
#define OPENTURNS_DOMAINCOMPLEMENT_HXX

#include "openturns/DomainImplementation.hxx"
#include "openturns/Domain.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Interval
 *
 * A class that holds a collection of interval
 */
class OT_API DomainComplement
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef DomainImplementation::BoolCollection  BoolCollection;

  /** Default constructor */
  DomainComplement();

  /** Default constructor */
  explicit DomainComplement(const Domain & domain);

  /** Virtual constructor method */
  DomainComplement * clone() const override;

  /** Check if the given point is inside this domain */
  Bool contains(const Point & point) const override;

  /** Check if the given points are inside this domain */
  BoolCollection contains(const Sample & sample) const override;

  /** Comparison operator */
  Bool operator == (const DomainComplement & other) const;
  Bool operator != (const DomainComplement & other) const;
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

  // The domain which is complemented
  Domain domain_;

}; /* class DomainComplement */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINCOMPLEMENT_HXX */
