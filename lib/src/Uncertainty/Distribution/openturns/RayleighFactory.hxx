//                                               -*- C++ -*-
/**
 *  @brief Factory for Rayleigh distribution
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
#ifndef OPENTURNS_RAYLEIGHFACTORY_HXX
#define OPENTURNS_RAYLEIGHFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Rayleigh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RayleighFactory
 */
class OT_API RayleighFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  RayleighFactory();

  /** Virtual constructor */
  RayleighFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  Rayleigh buildAsRayleigh(const Sample & sample) const;
  Rayleigh buildAsRayleigh(const Point & parameters) const;
  Rayleigh buildAsRayleigh() const;

}; /* class RayleighFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RAYLEIGHFACTORY_HXX */
