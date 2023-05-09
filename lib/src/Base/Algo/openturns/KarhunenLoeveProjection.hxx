//                                               -*- C++ -*-
/**
 *  @brief Field to point function allowing to perform the projection of a
 *         field over a Karhunen-Loeve basis
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
#ifndef OPENTURNS_KARHUNENLOEVEPROJECTION_HXX
#define OPENTURNS_KARHUNENLOEVEPROJECTION_HXX

#include "openturns/FieldToPointFunctionImplementation.hxx"
#include "openturns/KarhunenLoeveResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveProjection
 *
 * Field to point function allowing to perform the projection of a
 * field over a Karhunen-Loeve basis
 */
class OT_API KarhunenLoeveProjection
  : public FieldToPointFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  KarhunenLoeveProjection();

  /** Parameter constructor */
  explicit KarhunenLoeveProjection(const KarhunenLoeveResult & result);

  /** Virtual constructor */
  KarhunenLoeveProjection * clone() const override;

  /** Comparison operator */
  using FieldToPointFunctionImplementation::operator ==;
  Bool operator ==(const KarhunenLoeveProjection & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Point operator() (const Sample & inFld) const override;
  Sample operator() (const ProcessSample & inPS) const override;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Underlying KarhunenLoeveResult */
  KarhunenLoeveResult result_;

}; /* class KarhunenLoeveProjection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEPROJECTION_HXX */
