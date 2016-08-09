//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an weightedExperiment plane
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
#ifndef OPENTURNS_WEIGHTEDEXPERIMENTIMPLEMENTATION_HXX
#define OPENTURNS_WEIGHTEDEXPERIMENTIMPLEMENTATION_HXX

#include "openturns/ExperimentImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Indices.hxx"
//#include "OrthogonalBasis.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class WeightedExperimentImplementation
 *
 * The class describes the probabilistic concept of weightedExperiment plan
 */
class OT_API WeightedExperimentImplementation
  : public ExperimentImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  WeightedExperimentImplementation();

  /** Parameters constructor */
  explicit WeightedExperimentImplementation(const UnsignedInteger size);

  /** Parameters constructor */
  WeightedExperimentImplementation(const Distribution & distribution,
                                   const UnsignedInteger size);

  /** Virtual constructor */
  virtual WeightedExperimentImplementation * clone() const;

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
  virtual NumericalSample generate();

  /** Sample generation with weights*/
  virtual NumericalSample generateWithWeights(NumericalPoint & weights);

  /** Weight accessor */
  virtual NumericalPoint getWeight() const;

protected:

  /** Distribution that defines the weights of the experiment */
  Distribution distribution_;
  /** The size of the sample to be generated */
  UnsignedInteger size_;
  /** The weights associated with the sample for numerical integration */
  NumericalPoint weights_;

}; /* class WeightedExperimentImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WEIGHTEDEXPERIMENTIMPLEMENTATION_HXX */
