//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_BASISIMPLEMENTATION_HXX
#define OPENTURNS_BASISIMPLEMENTATION_HXX

#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BasisImplementation
 *
 */

class OT_API BasisImplementation
  : public PersistentObject
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Size constructor */
  BasisImplementation();

  /** Size constructor */
  explicit BasisImplementation(const UnsignedInteger size);

  /** Collection constructor */
  explicit BasisImplementation(const FunctionCollection & collection);


  /** Build the function of the given index */
  virtual Function build(const UnsignedInteger index) const;

  /** Get the function of the given index */
  virtual Function operator[](const UnsignedInteger index) const;
  virtual Function & operator[](const UnsignedInteger index);

  /** Accessor to the sub-basis */
  virtual FunctionCollection getSubBasis(const Indices & indices) const;

  /** Virtual constructor */
  virtual BasisImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Size accessor */
  virtual UnsignedInteger getSize() const;
  virtual UnsignedInteger getCurrentSize() const;

  /** Tells whether the basis is orthogonal */
  virtual Bool isOrthogonal() const;

  /** Tells whether the basis is finite */
  virtual Bool isFinite() const;

  virtual void add(const Function & elt) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  mutable FunctionPersistentCollection collection_;

} ; /* class BasisImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASISIMPLEMENTATION_HXX */
