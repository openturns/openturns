//                                               -*- C++ -*-
/**
 *  @brief Factory for Beta distribution
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
#ifndef OPENTURNS_BETAFACTORY_HXX
#define OPENTURNS_BETAFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionImplementationFactory.hxx"
#include "Beta.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BetaFactory
 */
class OT_API BetaFactory
  : public DistributionImplementationFactory
{
  CLASSNAME;
public:

  /** Default constructor */
  BetaFactory();

  /** Virtual constructor */
  virtual BetaFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionImplementationFactory::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPointCollection & parameters) const;
  Implementation build() const;
  Beta buildAsBeta(const NumericalSample & sample) const;
  Beta buildAsBeta(const NumericalPointCollection & parameters) const;
  Beta buildAsBeta(const NumericalPointWithDescriptionCollection & parameters) const;
  Beta buildAsBeta() const;

}; /* class BetaFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BETAFACTORY_HXX */
