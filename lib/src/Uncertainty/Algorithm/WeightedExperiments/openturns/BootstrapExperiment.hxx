//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an bootstrapExperiment plane
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  BootstrapExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution) override;
  Distribution getDistribution() const override;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Selection generation */
  static Indices GenerateSelection(const UnsignedInteger size,
                                   const UnsignedInteger length);

protected:

private:

  /** The sample from which we resample */
  Sample sample_;
}; /* class BootstrapExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOOTSTRAPEXPERIMENT_HXX */
