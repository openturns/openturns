//                                               -*- C++ -*-
/**
 *  @brief Compact history storage
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
#ifndef OPENTURNS_COMPACT_HXX
#define OPENTURNS_COMPACT_HXX

#include "openturns/HistoryStrategyImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Compact
 */

class OT_API Compact
  : public HistoryStrategyImplementation
{

  CLASSNAME

public:


  /** Constructor with parameters */
  Compact();

  /** Constructor with parameters */
  explicit Compact(const UnsignedInteger halfMaximumSize);

  /** Virtual constructor */
  Compact * clone() const override;

  /** Store the point according to the strategy */
  using HistoryStrategyImplementation::store;
  void store(const Point & point) override;

  /** Clear the history storage and change dimension of Point stored */
  void setDimension(const UnsignedInteger dimension) override;

  /** Sample accessor */
  Sample getSample() const override;

  /** HalfMaximumSize accessor */
  UnsignedInteger getHalfMaximumSize() const;

  /** Index accessor */
  UnsignedInteger getIndex() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
protected:

private:
  // Maximum number of points to be hitorized
  UnsignedInteger halfMaximumSize_;

  // Index where the next point will be stored
  UnsignedInteger index_;

  // Storage step
  UnsignedInteger step_;

  // Throwing counter
  UnsignedInteger throwingCounter_;

} ; /* class Compact */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPACT_HXX */
