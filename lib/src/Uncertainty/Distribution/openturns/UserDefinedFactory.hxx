//                                               -*- C++ -*-
/**
 *  @brief Factory for UserDefined distribution
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
#ifndef OPENTURNS_USERDEFINEDFACTORY_HXX
#define OPENTURNS_USERDEFINEDFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/UserDefined.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedFactory
 */
class OT_API UserDefinedFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  UserDefinedFactory();

  /** Virtual constructor */
  virtual UserDefinedFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const;
  Distribution build(const Sample & sample,
                     const Scalar epsilon) const;
  Distribution build() const;
  UserDefined buildAsUserDefined(const Sample & sample,
                                 const Scalar epsilon = 0.0) const;
  UserDefined buildAsUserDefined() const;

}; /* class UserDefinedFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDFACTORY_HXX */
