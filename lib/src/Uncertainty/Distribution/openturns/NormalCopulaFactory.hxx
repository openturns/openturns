//                                               -*- C++ -*-
/**
 *  @brief Factory for NormalCopula distribution
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
#ifndef OPENTURNS_NORMALCOPULAFACTORY_HXX
#define OPENTURNS_NORMALCOPULAFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/NormalCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalCopulaFactory
 */
class OT_API NormalCopulaFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  NormalCopulaFactory();

  /** Virtual constructor */
  virtual NormalCopulaFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build(const Point & parameters) const;
  Distribution build() const;
  NormalCopula buildAsNormalCopula(const Sample & sample) const;
  NormalCopula buildAsNormalCopula(const Point & parameters) const;
  NormalCopula buildAsNormalCopula() const;

}; /* class NormalCopulaFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALCOPULAFACTORY_HXX */
