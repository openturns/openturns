//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all Sampler
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SAMPLER_HXX
#define OPENTURNS_SAMPLER_HXX

#include "TypedInterfaceObject.hxx"
#include "SamplerImplementation.hxx"
#include "Distribution.hxx"
#include "CalibrationStrategy.hxx"
#include "ConditionalDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Sampler
 *
 * @brief The class describes the concept of a calibration strategy.
 *
 *
 * The implementation defines how the
 * @see SamplerImplementation
 */
class OT_API Sampler
  : public TypedInterfaceObject<SamplerImplementation>
{
  CLASSNAME;
public:

  /** Default constructor */
  Sampler();

  /** Constructor from implementation */
  Sampler(const SamplerImplementation & op);

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Returns a realization */
  NumericalPoint getRealization() const;

  /** Returns a realization */
  NumericalSample getSample(const UnsignedInteger size) const;

}; /* class Sampler */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SAMPLER_HXX */
