//                                               -*- C++ -*-
/**
 *  @brief A sequence of numerical math function collection
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
#ifndef OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX
#define OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollectionImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BasisSequenceImplementation
 *
 * A sequence of numerical math function collection.
 */
class OT_API BasisSequenceImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  BasisSequenceImplementation();

  /** Default constructor */
  BasisSequenceImplementation(const Basis & masterBasis);

  /** Virtual constructor */
  BasisSequenceImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Size accessor */
  UnsignedInteger getSize() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Returns the master basis */
  Basis getMasterBasis() const;

  /** Basis accessor */
  FunctionCollection getBasis(const UnsignedInteger index) const;

  /** IndicesCollection accessor */
  IndicesCollection getIndicesCollection() const;

  /** Indices accessor */
  Indices getIndices(const UnsignedInteger index) const;

  /** Extend basis */
  void add(const Indices & indices);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The internal basis of the sequence */
  Basis masterBasis_;

  /** The indices */
  PersistentCollection<Indices> indices_;

}; /* class BasisSequenceImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASISSEQUENCEIMPLEMENTATION_HXX */
