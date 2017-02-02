//                                               -*- C++ -*-
/**
 *  @brief Factory for Weibull distribution
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
#ifndef OPENTURNS_WEIBULLFACTORY_HXX
#define OPENTURNS_WEIBULLFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Weibull.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WeibullFactory
 */
class OT_API WeibullFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  WeibullFactory();

  /** Virtual constructor */
  virtual WeibullFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  Weibull buildAsWeibull(const NumericalSample & sample) const;
  Weibull buildAsWeibull(const NumericalPoint & parameters) const;
  Weibull buildAsWeibull() const;

}; /* class WeibullFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WEIBULLFACTORY_HXX */
