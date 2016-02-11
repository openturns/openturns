//                                               -*- C++ -*-
/**
 *  @brief DirectionalSampling is an implementation of the directional sampling Montex Carlo simulation method
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_DIRECTIONALSAMPLING_HXX
#define OPENTURNS_DIRECTIONALSAMPLING_HXX

#include "Simulation.hxx"
#include "NumericalPoint.hxx"
#include "Distribution.hxx"
#include "StandardEvent.hxx"
#include "RootStrategy.hxx"
#include "SamplingStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DirectionalSampling
 */

class OT_API DirectionalSampling :
  public Simulation
{
  CLASSNAME;
public:

  typedef Collection<NumericalScalar> NumericalScalarCollection;
  typedef Distribution::Implementation            Implementation;

  /** for save/load mecanism */
  DirectionalSampling();

  /** Constructor with parameters */
  explicit DirectionalSampling(const Event & event);

  /** Constructor with parameters */
  DirectionalSampling(const Event & event,
                      const RootStrategy & rootStrategy,
                      const SamplingStrategy & samplingStrategy);


  /** Virtual constructor */
  virtual DirectionalSampling * clone() const;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Sampling strategy */
  void setSamplingStrategy(const SamplingStrategy & samplingStrategy);
  SamplingStrategy getSamplingStrategy() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Compute the block sample and the points that realized the event */
  NumericalSample computeBlockSample();

  /** Compute the contribution of a direction to the probability given the roots of the performance function along the direction */
  NumericalScalar computeContribution(const NumericalScalarCollection & roots);

  /** Compute the mean point of a direction given the roots of the performance function along the direction */
  NumericalScalar computeMeanContribution(const NumericalScalarCollection & roots);

  /** Compute the contribution of a set of direction to the probability given the contributions of each direction in the set */
  NumericalScalar computeTotalContribution(const NumericalSample & directionSample);

  StandardEvent standardEvent_;
  NumericalMathFunction standardFunction_;
  Implementation inputDistribution_;
  RootStrategy rootStrategy_;
  SamplingStrategy samplingStrategy_;

} ; /* class DirectionalSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DIRECTIONALSAMPLING_HXX */
