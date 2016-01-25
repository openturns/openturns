//                                               -*- C++ -*-
/**
 *  @brief Factory for InverseNormal distribution
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
#ifndef OPENTURNS_INVERSENORMALFACTORY_HXX
#define OPENTURNS_INVERSENORMALFACTORY_HXX

#include "OTprivate.hxx"
#include "DistributionFactoryImplementation.hxx"
#include "InverseNormal.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNormalFactory
 */
class OT_API InverseNormalFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  InverseNormalFactory();

  /** Virtual constructor */
  virtual InverseNormalFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  InverseNormal buildAsInverseNormal(const NumericalSample & sample) const;
  InverseNormal buildAsInverseNormal(const NumericalPoint & parameters) const;
  InverseNormal buildAsInverseNormal() const;

}; /* class InverseNormalFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENORMALFACTORY_HXX */
