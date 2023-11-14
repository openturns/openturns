//                                               -*- C++ -*-
/**
 *  @brief A class which implements the WhiteNoise process
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_WHITENOISE_HXX
#define OPENTURNS_WHITENOISE_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Process.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WhiteNoise
 *
 * An interface class for composite White Noise
 */
class OT_API WhiteNoise
  : public ProcessImplementation
{
  CLASSNAME

public:

  /** Some typedefs to ease reading */

  /** Default constructor */
  WhiteNoise();

  /** Standard constructor */
  explicit WhiteNoise(const Distribution & distribution);

  /** Standard constructor */
  WhiteNoise(const Distribution & distribution,
             const Mesh & mesh);

  /** Virtual constructor */
  WhiteNoise * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Is the underlying stationary ? */
  Bool isStationary() const override;

  /** Is the underlying a Gaussian process ? */
  Bool isNormal() const override;

  /** Realization accessor */
  Field getRealization() const override;

  /** Continuous realization accessor */
  Function getContinuousRealization() const override;

  /** Continuation of the last realization on a given number of steps */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const override;

  /** Get the marginal process corresponding to indices components */
  Process getMarginal(const Indices & indices) const override;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The distribution of the WhiteNoise */
  Distribution distribution_;

}; /* class WhiteNoise */
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WHITENOISE_HXX */
