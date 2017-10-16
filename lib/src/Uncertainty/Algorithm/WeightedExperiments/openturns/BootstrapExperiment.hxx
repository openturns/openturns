//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an bootstrapExperiment plane
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_BOOTSTRAPEXPERIMENT_HXX
#define OPENTURNS_BOOTSTRAPEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BootstrapExperiment
 *
 * The class describes the probabilistic concept of bootstrapExperiment plan
 */
class OT_API BootstrapExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  BootstrapExperiment();

  /** Parameters constructor */
  explicit BootstrapExperiment(const Sample & sample);

  /** Virtual constructor */
  virtual BootstrapExperiment * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const;

protected:

private:

}; /* class BootstrapExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOOTSTRAPEXPERIMENT_HXX */
