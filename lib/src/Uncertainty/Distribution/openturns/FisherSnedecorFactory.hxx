//                                               -*- C++ -*-
/**
 *  @brief Factory for FisherSnedecor distribution
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
#ifndef OPENTURNS_FISHERSNEDECORFACTORY_HXX
#define OPENTURNS_FISHERSNEDECORFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/FisherSnedecor.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FisherSnedecorFactory
 */
class OT_API FisherSnedecorFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FisherSnedecorFactory();

  /** Virtual constructor */
  virtual FisherSnedecorFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const Sample & sample) const;
  Implementation build(const Point & parameters) const;
  Implementation build() const;

  using DistributionFactoryImplementation::buildEstimator;
  DistributionFactoryResult buildEstimator(const Sample & sample) const;

  FisherSnedecor buildAsFisherSnedecor(const Sample & sample) const;
  FisherSnedecor buildAsFisherSnedecor(const Point & parameters) const;
  FisherSnedecor buildAsFisherSnedecor() const;

}; /* class FisherSnedecorFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FISHERSNEDECORFACTORY_HXX */
