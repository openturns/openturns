//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
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
#ifndef OPENTURNS_FINITEBASIS_HXX
#define OPENTURNS_FINITEBASIS_HXX

#include "BasisImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FiniteBasis
 *
 * Allows to use a NMF collection as FunctionalBasis required to use chaos algo, used in TrendFactory
 */

class OT_API FiniteBasis
  : public BasisImplementation
{
  CLASSNAME;
public:
  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction> NumericalMathFunctionPersistentCollection;

  /** Default constructor */
  explicit FiniteBasis(const UnsignedInteger size = 0);

  /** Collection constructor */
  explicit FiniteBasis(const NumericalMathFunctionCollection & collection);

  /** Build the NumericalMathFunction of the given index */
  virtual NumericalMathFunction build(const UnsignedInteger index) const;
  virtual NumericalMathFunction & operator[](const UnsignedInteger index);

  /** Virtual constructor */
  virtual FiniteBasis * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;
  virtual UnsignedInteger getSize() const;

  virtual void add(const NumericalMathFunction & elt);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  NumericalMathFunctionPersistentCollection collection_;
} ; /* class FiniteBasis */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEBASIS_HXX */
