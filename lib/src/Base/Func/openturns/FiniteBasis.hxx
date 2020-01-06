//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
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
#ifndef OPENTURNS_FINITEBASIS_HXX
#define OPENTURNS_FINITEBASIS_HXX

#include "openturns/BasisImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FiniteBasis
 *
 * Allows to use a NMF collection as a Basis, required to use chaos algo, used in TrendFactory
 */

class OT_API FiniteBasis
  : public BasisImplementation
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor */
  explicit FiniteBasis(const UnsignedInteger size = 0);

  /** Collection constructor */
  explicit FiniteBasis(const FunctionCollection & collection);

  /** Build the Function of the given index */
  virtual Function build(const UnsignedInteger index) const;
  virtual Function & operator[](const UnsignedInteger index);

  /** Virtual constructor */
  virtual FiniteBasis * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;
  virtual UnsignedInteger getSize() const;

  /** Tells whether the basis is finite */
  virtual Bool isFinite() const;

  virtual void add(const Function & elt);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  FunctionPersistentCollection collection_;
} ; /* class FiniteBasis */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEBASIS_HXX */
