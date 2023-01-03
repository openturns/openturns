//                                               -*- C++ -*-
/**
 *  @brief Field to point function allowing to perform the lifting of a
 *         point into a field using a Karhunen-Loeve basis
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KARHUNENLOEVELIFTING_HXX
#define OPENTURNS_KARHUNENLOEVELIFTING_HXX

#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/KarhunenLoeveResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveLifting
 *
 *  Field to point function allowing to perform the lifting of a
 *  point into a field using a Karhunen-Loeve basis
 */
class OT_API KarhunenLoeveLifting
  : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  KarhunenLoeveLifting();

  /** Parameter constructor */
  explicit KarhunenLoeveLifting(const KarhunenLoeveResult & result);

  /** Virtual constructor */
  KarhunenLoeveLifting * clone() const override;

  /** Comparison operator */
  Bool operator ==(const KarhunenLoeveLifting & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using PointToFieldFunctionImplementation::operator();
  Sample operator() (const Point & inP) const override;

  /** Get the i-th marginal function */
  PointToFieldFunction getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  PointToFieldFunction getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Underlying KarhunenLoeveResult */
  KarhunenLoeveResult result_;

}; /* class KarhunenLoeveLifting */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVELIFTING_HXX */
