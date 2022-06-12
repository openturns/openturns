//                                               -*- C++ -*-
/**
 *  @brief Splitter implementation
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
#ifndef OPENTURNS_SPLITTERIMPLEMENTATION_HXX
#define OPENTURNS_SPLITTERIMPLEMENTATION_HXX

#include "openturns/IndicesCollection.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SplitterImplementation
 *
 * Splitter implementation
 */
class OT_API SplitterImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  SplitterImplementation();

  /** Constructor with parameters */
  explicit SplitterImplementation(const UnsignedInteger N);

  /** Virtual constructor method */
  SplitterImplementation * clone() const override;

  /** Generate next set of indices */
  virtual Indices generate(Indices & indices2Out) const;

  /** Reset sequence state */
  virtual void reset() const;

  /** Set of indices size accessor */
  virtual UnsignedInteger getN() const;

  /** Number of indices pairs accessor */
  virtual UnsignedInteger getSize() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Total size */
  UnsignedInteger N_ = 0;

  /** Sequence state */
  mutable UnsignedInteger currentIndex_ = 0;
}; /* class SplitterImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPLITTERIMPLEMENTATION_HXX */
