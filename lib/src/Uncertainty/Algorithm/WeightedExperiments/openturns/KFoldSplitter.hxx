//                                               -*- C++ -*-
/**
 *  @brief KFoldSplitter
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KFOLDSPLITTER_HXX
#define OPENTURNS_KFOLDSPLITTER_HXX

#include "openturns/SplitterImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KFoldSplitter
 *
 * KFoldSplitter
 */
class OT_API KFoldSplitter
  : public SplitterImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  KFoldSplitter();

  explicit KFoldSplitter(const UnsignedInteger size,
                         const UnsignedInteger k = 5);

  /** Virtual constructor method */
  KFoldSplitter * clone() const override;

  /** Generate next set of indices */
  Indices generate(Indices & indices2Out) const override;

  /** Number of indices pairs accessor */
  UnsignedInteger getSize() const override;

  /** Randomize folds */
  void setRandomize(const Bool randomize);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  UnsignedInteger k_ = 0;
  Indices shuffle_;

}; /* class KFoldSplitter */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KFOLDSPLITTER_HXX */
