//                                               -*- C++ -*-
/**
 *  @brief Factory for MeixnerDistribution distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MEIXNERDISTRIBUTIONFACTORY_HXX
#define OPENTURNS_MEIXNERDISTRIBUTIONFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionFactoryImplementation.hxx"
#include "MeixnerDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MeixnerDistributionFactory
 */
class OT_API MeixnerDistributionFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  MeixnerDistributionFactory();

  /** Virtual constructor */
  virtual MeixnerDistributionFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  MeixnerDistribution buildAsMeixnerDistribution(const NumericalSample & sample) const;
  MeixnerDistribution buildAsMeixnerDistribution(const NumericalPoint & parameters) const;
  MeixnerDistribution buildAsMeixnerDistribution() const;

}; /* class MeixnerDistributionFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEIXNERDISTRIBUTIONFACTORY_HXX */
