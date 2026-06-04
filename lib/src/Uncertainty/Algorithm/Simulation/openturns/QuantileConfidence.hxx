//                                               -*- C++ -*-
/**
 *  @brief Quantile confidence interval computation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_QUANTILECONFIDENCE_HXX
#define OPENTURNS_QUANTILECONFIDENCE_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class QuantileConfidence
*/
class OT_API QuantileConfidence
  : public PersistentObject
{

  CLASSNAME
public:

  /** Default constructor */
  QuantileConfidence();

  /** Constructor with parameters */
  explicit QuantileConfidence(const Scalar alpha, const Scalar beta = 0.95);

  /** Virtual constructor */
  QuantileConfidence * clone() const override;

  /** Compute ranks */
  UnsignedInteger computeUnilateralRank(const UnsignedInteger size, const Bool tail = false) const;
  Indices computeBilateralRank(const UnsignedInteger size) const;

  /** Compute confidence intervals */
  Interval computeUnilateralConfidenceInterval(const Sample & sample, const Bool tail = false) const;
  Interval computeUnilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut, const Bool tail = false) const;
  Interval computeBilateralConfidenceInterval(const Sample & sample) const;
  Interval computeBilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut) const;

  /** Compute minimum sample size */
  UnsignedInteger computeUnilateralMinimumSampleSize(const UnsignedInteger rank = 0, const Bool tail = false) const;
  UnsignedInteger computeBilateralMinimumSampleSize() const;

  /** Asymptotic confidence */
  Indices computeAsymptoticBilateralRank(const UnsignedInteger size) const;
  Interval computeAsymptoticBilateralConfidenceInterval(const Sample & sample) const;

  /** Quantile level accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Confidence level accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Scalar alpha_ = 0.0;
  Scalar beta_ = 0.0;

}; /* class QuantileConfidence */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILECONFIDENCE_HXX */
