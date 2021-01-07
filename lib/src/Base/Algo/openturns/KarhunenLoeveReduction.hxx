//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve reduction
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KARHUNENLOEVEREDUCTION_HXX
#define OPENTURNS_KARHUNENLOEVEREDUCTION_HXX

#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/KarhunenLoeveResult.hxx"
#include "openturns/FieldToFieldConnection.hxx"
#include "openturns/TrendTransform.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveReduction
 *
 * Performs field reduction: projection + lifting
 */
class OT_API KarhunenLoeveReduction
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  KarhunenLoeveReduction();

  /** Parameter constructor */
  explicit KarhunenLoeveReduction(const KarhunenLoeveResult & result);

  /** Parameter constructor */
  KarhunenLoeveReduction(const KarhunenLoeveResult & result,
                         const TrendTransform & trend);

  /** Virtual constructor */
  KarhunenLoeveReduction * clone() const override;

  /** Comparison operator */
  Bool operator ==(const KarhunenLoeveReduction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Trend accessor */
  void setTrend(const TrendTransform & trend);

  /** Operator () */
  using FieldFunctionImplementation::operator();
  Sample operator() (const Sample & inFld) const override;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  FieldToFieldConnection connection_;

}; /* class KarhunenLoeveReduction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEREDUCTION_HXX */
