//                                               -*- C++ -*-
/**
 *  @brief ADS simulation algorithm
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
#ifndef OPENTURNS_ADAPTATIVEDIRECTIONALSAMPLING_HXX
#define OPENTURNS_ADAPTATIVEDIRECTIONALSAMPLING_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Point.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/RootStrategy.hxx"
#include "openturns/SamplingStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS
/**
 * @class AdaptiveDirectionalStratification
 *
 * AdaptiveDirectionalStratification
 */
class OT_API AdaptiveDirectionalStratification
  : public EventSimulation
{
  CLASSNAME

public:

  /** Default constructor */
  AdaptiveDirectionalStratification();

  /** Constructor with parameters */
  AdaptiveDirectionalStratification(const RandomVector & event,
                                    const RootStrategy & rootStrategy = RootStrategy(),
                                    const SamplingStrategy & samplingStrategy = SamplingStrategy());

  /** Virtual constructor */
  AdaptiveDirectionalStratification * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Sampling strategy */
  void setSamplingStrategy(const SamplingStrategy & samplingStrategy);
  SamplingStrategy getSamplingStrategy() const;

  /** Gamma accessor */
  void setGamma(const Point & gamma);
  Point getGamma() const;

  /** Quadrant orientation_ accessor */
  void setQuadrantOrientation(const Point & quadrantOrientation);
  Point getQuadrantOrientation() const;

  /** Use partial stratification */
  void setPartialStratification(Bool partialStratification);
  Bool getPartialStratification() const;

  /** Set maximum stratification dimension */
  void setMaximumStratificationDimension(UnsignedInteger maximumStratificationDimension);
  UnsignedInteger getMaximumStratificationDimension() const;

  /** T statistic accessor */
  Point getTStatistic() const;

  /** Performs the actual computation. */
  void run() override;

private:
  /** Compute the block sample and the points that realized the event */
  Sample computeBlockSample() override;

  StandardEvent standardEvent_;

  RootStrategy rootStrategy_;
  SamplingStrategy samplingStrategy_;

  // Repartition of points in learning/simulation steps
  Point gamma_;

  // Quadrant orientation
  Point quadrantOrientation_;

  // Use partial stratification
  Bool partialStratification_;

  // Maximum number of stratified vars
  UnsignedInteger maximumStratificationDimension_;

  // T statistic
  Point T_;

}; /* class AdaptiveDirectionalStratification */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ADAPTATIVEDIRECTIONALSAMPLING_HXX */
