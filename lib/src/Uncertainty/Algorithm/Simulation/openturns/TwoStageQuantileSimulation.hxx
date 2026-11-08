//                                               -*- C++ -*-
/**
 *  @brief Two-stage quantile simulation
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
#ifndef OPENTURNS_TWOSTAGEQUANTILESIMULATION_HXX
#define OPENTURNS_TWOSTAGEQUANTILESIMULATION_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TwoStageQuantileSimulation
 *
 * Two-stage quantile estimator with an exact fixed-sample confirmation
 * interval.  The first stage draws a pilot sample and measures the width
 * of the exact bilateral pilot interval at the quantile.  The
 * confirmation sample is sized so that the expected half-width of the
 * exact order-statistic interval is below the target, using the
 * asymptotic scaling of that width with the inverse square root of the
 * sample size.  The second stage draws an independent confirmation
 * sample and computes the empirical inverse and the exact binomial
 * order-statistic interval on that sample only.  Because the confirmation
 * size is a function of the pilot sample, which is independent of the
 * confirmation sample, the coverage is exact unconditionally, pilot
 * randomness included.
 *
 * References:
 * S. S. Wilks. Determination of sample sizes for setting tolerance
 * limits. The Annals of Mathematical Statistics, 12(1):91-96, 1941.
 * The article gives the binomial order-statistic coverage used by the
 * confirmation interval.
 * C. Stein. A two-sample test for a linear hypothesis whose power is
 * independent of the variance. The Annals of Mathematical Statistics,
 * 16(3):243-258, 1945. The article introduces the two-stage design in
 * which an independent pilot sample fixes the size of the main sample.
 */
class OT_API TwoStageQuantileSimulation
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  TwoStageQuantileSimulation();

  /** Constructor with parameters */
  TwoStageQuantileSimulation(const RandomVector & output,
                             const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                             const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  TwoStageQuantileSimulation * clone() const override;

  /** Pilot sample size accessor */
  void setPilotSize(const UnsignedInteger pilotSize);
  UnsignedInteger getPilotSize() const;

  /** Target half-width accessor */
  void setTargetHalfWidth(const Scalar halfWidth);
  Scalar getTargetHalfWidth() const;
  Bool hasTargetHalfWidth() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the two-stage experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  UnsignedInteger pilotSize_ = ResourceMap::GetAsUnsignedInteger("TwoStageQuantileSimulation-DefaultPilotSize");
  Scalar targetHalfWidth_ = ResourceMap::GetAsScalar("TwoStageQuantileSimulation-DefaultTargetHalfWidth");

}; /* class TwoStageQuantileSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TWOSTAGEQUANTILESIMULATION_HXX */
