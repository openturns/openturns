//                                               -*- C++ -*-
/**
 *  @brief DirectionalSampling is an implementation of the directional sampling Montex Carlo simulation method
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DIRECTIONALSAMPLING_HXX
#define OPENTURNS_DIRECTIONALSAMPLING_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/RootStrategy.hxx"
#include "openturns/SamplingStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DirectionalSampling
 */

class OT_API DirectionalSampling :
  public EventSimulation
{
  CLASSNAME
public:

  typedef Collection<Scalar> ScalarCollection;
  typedef Distribution::Implementation            Implementation;

  /** for save/load mecanism */
  DirectionalSampling();

  /** Constructor with parameters */
  explicit DirectionalSampling(const RandomVector & event);

  /** Constructor with parameters */
  DirectionalSampling(const RandomVector & event,
                      const RootStrategy & rootStrategy,
                      const SamplingStrategy & samplingStrategy);


  /** Virtual constructor */
  DirectionalSampling * clone() const override;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Sampling strategy */
  void setSamplingStrategy(const SamplingStrategy & samplingStrategy);
  SamplingStrategy getSamplingStrategy() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Compute the block sample and the points that realized the event */
  Sample computeBlockSample() override;

  /** Compute the contribution of a direction to the probability given the roots of the performance function along the direction */
  Scalar computeContribution(const ScalarCollection & roots);

  /** Compute the mean point of a direction given the roots of the performance function along the direction */
  Scalar computeMeanContribution(const ScalarCollection & roots);

  /** Compute the contribution of a set of direction to the probability given the contributions of each direction in the set */
  Scalar computeTotalContribution(const Sample & directionSample);

  StandardEvent standardEvent_;
  Function standardFunction_;
  Distribution inputDistribution_;
  RootStrategy rootStrategy_;
  SamplingStrategy samplingStrategy_;

} ; /* class DirectionalSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DIRECTIONALSAMPLING_HXX */
