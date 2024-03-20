//                                               -*- C++ -*-
/**
 *  @brief Factory for SmoothedUniform distribution
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
#ifndef OPENTURNS_SMOOTHEDUNIFORMFACTORY_HXX
#define OPENTURNS_SMOOTHEDUNIFORMFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/SmoothedUniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SmoothedUniformFactory
 */
class OT_API SmoothedUniformFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SmoothedUniformFactory();

  /** Virtual constructor */
  SmoothedUniformFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  SmoothedUniform buildAsSmoothedUniform(const Sample & sample) const;
  SmoothedUniform buildAsSmoothedUniform(const Point & parameters) const;
  SmoothedUniform buildAsSmoothedUniform() const;

}; /* class SmoothedUniformFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SMOOTHEDUNIFORMFACTORY_HXX */
