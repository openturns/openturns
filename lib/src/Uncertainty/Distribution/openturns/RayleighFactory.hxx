//                                               -*- C++ -*-
/**
 *  @brief Factory for Rayleigh distribution
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
  CLASSNAME;
public:

  /** Default constructor */
  RayleighFactory();

  /** Virtual constructor */
  virtual RayleighFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  Rayleigh buildAsRayleigh(const NumericalSample & sample) const;
  Rayleigh buildAsRayleigh(const NumericalPoint & parameters) const;
  Rayleigh buildAsRayleigh() const;

}; /* class RayleighFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RAYLEIGHFACTORY_HXX */
