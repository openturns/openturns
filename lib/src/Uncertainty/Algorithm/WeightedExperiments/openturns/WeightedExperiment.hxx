//                                               -*- C++ -*-
/**
 *  @brief WeightedExperiment interface
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_WEIGHTEDEXPERIMENT_HXX
#define OPENTURNS_WEIGHTEDEXPERIMENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/WeightedExperimentImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WeightedExperiment
 * WeightedExperiment interface
 */

class OT_API WeightedExperiment
  : public TypedInterfaceObject<WeightedExperimentImplementation>
{

  CLASSNAME
public:

  typedef Pointer<WeightedExperimentImplementation> Implementation;

  /** Default constructor */
  WeightedExperiment();

  /** Constructor from an implementation */
  WeightedExperiment(const WeightedExperimentImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  WeightedExperiment(const Implementation & p_implementation);

  /** String converter */
  virtual String __repr__() const;

  /** Distribution accessor */
  virtual void setDistribution(const Distribution & distribution);
  virtual Distribution getDistribution() const;

  /** Size accessor */
  virtual void setSize(const UnsignedInteger size);
  virtual UnsignedInteger getSize() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  virtual Sample generate() const;

  /** Sample generation with weights*/
  virtual Sample generateWithWeights(Point & weightsOut) const;

  /** Uniform weights ? */
  virtual Bool hasUniformWeights() const;

} ; /* class WeightedExperiment */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WEIGHTEDEXPERIMENT_HXX */
