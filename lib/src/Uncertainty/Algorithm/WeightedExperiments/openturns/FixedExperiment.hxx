//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an fixedExperiment plane
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
#ifndef OPENTURNS_FIXEDEXPERIMENT_HXX
#define OPENTURNS_FIXEDEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FixedExperiment
 *
 * The class describes the probabilistic concept of fixedExperiment plan
 */
class OT_API FixedExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  FixedExperiment();

  /** Parameters constructor */
  explicit FixedExperiment(const NumericalSample & sample);

  /** Parameters constructor */
  FixedExperiment(const NumericalSample & sample,
                  const NumericalPoint & weights);

  /** Virtual constructor */
  virtual FixedExperiment * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** Sample generation */
  using WeightedExperimentImplementation::generate;
  NumericalSample generate();

protected:

private:
  // The fixed sample that will be returned at each call
  NumericalSample sample_;

}; /* class FixedExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIXEDEXPERIMENT_HXX */
