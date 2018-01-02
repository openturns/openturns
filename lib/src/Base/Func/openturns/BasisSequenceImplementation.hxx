//                                               -*- C++ -*-
/**
 *  @brief A sequence of numerical math function collection
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
#ifndef OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX
#define OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX

#include "openturns/PersistentCollection.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BasisSequenceImplementation
 *
 * A sequence of numerical math function collection.
 */
class OT_API BasisSequenceImplementation
  : public PersistentCollection<Indices>
{
  CLASSNAME

public:

  /** Default constructor */
  BasisSequenceImplementation();

  /** Default constructor */
  BasisSequenceImplementation(const Basis & masterBasis);

  /** Virtual constructor */
  virtual BasisSequenceImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Returns the master basis */
  Basis getMasterBasis() const;

  /** Basis accessor */
  Basis getBasis(const UnsignedInteger index) const;

  /** Indices accessor */
  Indices getIndices(const UnsignedInteger index) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The internal basis of the sequence */
  Basis masterBasis_;

}; /* class BasisSequenceImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX */
