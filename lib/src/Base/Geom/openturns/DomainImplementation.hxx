//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DOMAINIMPLEMENTATION_HXX
#define OPENTURNS_DOMAINIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DomainImplementation
 *
 * A class that holds a collection of domain
 */
class OT_API DomainImplementation
  : public PersistentObject
{
  CLASSNAME;

public:
  typedef Pointer<DomainImplementation> Implementation;

  /** Default constructor */
  explicit DomainImplementation(const UnsignedInteger dimension = 1);

  /** Virtual constructor method */
  virtual DomainImplementation * clone() const;

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

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Dimension accessors */
  UnsignedInteger getDimension() const;

  /** Lower bound of the bounding box */
  virtual NumericalPoint getLowerBound() const;

  /** Upper bound of the bounding box */
  virtual NumericalPoint getUpperBound() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  // Compute the total volume of the domain
  virtual void computeVolume() const;

  /** The dimension of the DomainImplementation */
  UnsignedInteger dimension_;

  // The global volume
  mutable NumericalScalar volume_;

  // Flag to tell if the global volume has already been computed
  mutable Bool isAlreadyComputedVolume_;

}; /* class DomainImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DOMAINIMPLEMENTATION_HXX */
