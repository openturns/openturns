//                                               -*- C++ -*-
/**
 *  @brief Line-sampling quantile estimator
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
#ifndef OPENTURNS_LINESAMPLINGQUANTILE_HXX
#define OPENTURNS_LINESAMPLINGQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/RootStrategy.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LineSamplingQuantile
 *
 * Quantile estimator obtained by a deterministic bisection on the
 * line-sampling probability estimate.  The important direction is
 * fixed before the root-finding loop: it is either provided by the
 * user, computed by FORM at a pilot threshold, or taken from the most
 * extreme pilot standard point.  The orthogonal line offsets are drawn
 * once and reused at every candidate threshold, so the probability
 * estimate is a smooth function of the threshold.
 *
 * References:
 * H. J. Pradlwarter, G. I. Schueller, P. S. Koutsourelakis and
 * D. C. Charmpis. Application of line sampling simulation method to
 * reliability benchmark problems. Structural Safety, 29(3):208-221,
 * 2007.  This article describes the line decomposition, the important
 * direction and the conditional normal probability used here.
 */
class OT_API LineSamplingQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  LineSamplingQuantile();

  /** Constructor with parameters */
  LineSamplingQuantile(const RandomVector & output,
                       const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                       const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  LineSamplingQuantile * clone() const override;

  /** Important direction accessor */
  void setInitialAlpha(const Point & initialAlpha);
  Point getInitialAlpha() const;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Opposite-direction search accessor */
  void setSearchOppositeDirection(const Bool searchOppositeDirection);
  Bool getSearchOppositeDirection() const;

  /** Root-finding tolerance accessor */
  void setRootTolerance(const Scalar tolerance);
  Scalar getRootTolerance() const;

  /** Maximum root-finding iteration accessor */
  void setMaximumRootIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumRootIteration() const;

  /** Pilot sample size accessor */
  void setPilotSampleSize(const UnsignedInteger pilotSampleSize);
  UnsignedInteger getPilotSampleSize() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the line-sampling root-finding experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** Probability estimate and conditional contributions at a threshold */
  Scalar computeProbability(const Scalar threshold, Point & contributions);

  Point initialAlpha_;
  Bool hasInitialAlpha_ = false;
  RootStrategy rootStrategy_;
  Bool searchOppositeDirection_ = ResourceMap::GetAsBool("LineSampling-DefaultSearchOppositeDirection");
  Scalar rootTolerance_ = ResourceMap::GetAsScalar("QuantileRootSolver-DefaultTolerance");
  UnsignedInteger maximumRootIteration_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultMaximumIteration");
  UnsignedInteger pilotSampleSize_ = ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-PilotSampleSize");

  /** State cached for the duration of a run */
  Function standardFunction_;
  Distribution standardDistribution_;
  Point importantDirection_;
  Sample orthogonalPoints_;
  Point originPoints_;
  Point originValues_;

}; /* class LineSamplingQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINESAMPLINGQUANTILE_HXX */
