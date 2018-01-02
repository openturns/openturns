//                                               -*- C++ -*-
/**
 *  @brief Factory for DerivativeArcsine distribution
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ARCSINEFACTORY_HXX
#define OPENTURNS_ARCSINEFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Arcsine.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ArcsineFactory
 */
class OT_API ArcsineFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ArcsineFactory();

  /** Virtual constructor */
  ArcsineFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const Sample & sample) const;
  Implementation build(const Point & parameters) const;
  Implementation build() const;

  using DistributionFactoryImplementation::buildEstimator;
  DistributionFactoryResult buildEstimator(const Sample & sample) const;

  Arcsine buildAsArcsine(const Sample & sample) const;
  Arcsine buildAsArcsine(const Point & parameters) const;
  Arcsine buildAsArcsine() const;

}; /* class ArcsineFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARCSINEFACTORY_HXX */
