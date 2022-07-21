//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition validation services
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
#ifndef OPENTURNS_KARHUNENLOEVEVALIDATION_HXX
#define OPENTURNS_KARHUNENLOEVEVALIDATION_HXX

#include "openturns/KarhunenLoeveResult.hxx"
#include "openturns/GridLayout.hxx"
#include "openturns/TrendTransform.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveValidation
 *
 * KL decomposition validation services
 */
class OT_API KarhunenLoeveValidation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  KarhunenLoeveValidation();

  /** Constructor with parameters */
  KarhunenLoeveValidation(const ProcessSample & sample,
                          const KarhunenLoeveResult & result);

  KarhunenLoeveValidation(const ProcessSample & sample,
                          const KarhunenLoeveResult & result,
                          const TrendTransform & trend);

  /** Virtual constructor */
  KarhunenLoeveValidation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Compute residual */
  ProcessSample computeResidual() const;

  /** Compute residual mean */
  Field computeResidualMean() const;

  /** Compute residual standard deviation */
  Field computeResidualStandardDeviation() const;

  /** Draw model vs metamodel validation graph */
  GridLayout drawValidation() const;

  /** Draw contribution weight of each observation */
  Graph drawObservationWeight(const UnsignedInteger k = 0) const;

  /** Draw contribution quality of each observation */
  Graph drawObservationQuality() const;


protected:
  ProcessSample sample_;
  KarhunenLoeveResult result_;
  TrendTransform trend_;

}; /* class KarhunenLoeveValidation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEVALIDATION_HXX */
