//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
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
#ifndef OPENTURNS_DOMAINIMPLEMENTATION_HXX
#define OPENTURNS_DOMAINIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainImplementation
 *
 * A class that holds a collection of domain
 */
class OT_API DomainImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<UnsignedInteger>   BoolCollection;
  typedef Pointer<DomainImplementation> Implementation;

  /** Default constructor */
  explicit DomainImplementation(const UnsignedInteger dimension = 1);

  /** Virtual constructor method */
  DomainImplementation * clone() const override;

  /** Check if the closed domain contains a given point */
  virtual Bool contains(const Point & point) const;

  /** Check if the closed domain contains given points */
  virtual BoolCollection contains(const Sample & sample) const;

  /** Compute the Euclidean distance from a given point to the domain */
  virtual Scalar computeDistance(const Point & point) const;

  /** Compute the Euclidean distance from given points to the domain */
  virtual Sample computeDistance(const Sample & sample) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Dimension accessors */
  UnsignedInteger getDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** The dimension of the DomainImplementation */
  UnsignedInteger dimension_;

}; /* class DomainImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINIMPLEMENTATION_HXX */
