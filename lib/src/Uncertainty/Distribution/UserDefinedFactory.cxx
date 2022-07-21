//                                               -*- C++ -*-
/**
 *  @brief Factory for UserDefined distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UserDefinedFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UserDefinedFactory)

static const Factory<UserDefinedFactory> Factory_UserDefinedFactory;

/* Default constructor */
UserDefinedFactory::UserDefinedFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
UserDefinedFactory * UserDefinedFactory::clone() const
{
  return new UserDefinedFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution UserDefinedFactory::build(const Sample & sample) const
{
  return buildAsUserDefined(sample, 0.0).clone();
}

Distribution UserDefinedFactory::build(const Sample & sample,
                                       const Scalar epsilon) const
{
  return buildAsUserDefined(sample, epsilon).clone();
}

Distribution UserDefinedFactory::build() const
{
  return buildAsUserDefined().clone();
}

UserDefined UserDefinedFactory::buildAsUserDefined(const Sample & sample,
    const Scalar epsilon) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a UserDefined distribution from an empty sample";
  const Scalar mean = sample.computeMean()[0];
  if (!SpecFunc::IsNormal(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an UserDefined distribution if data contains NaN or Inf";
  const Scalar p = 1.0 / size;
  UserDefined result(sample, Point(size, p));
  result.compactSupport(epsilon);
  result.setDescription(sample.getDescription());
  return result;
}

UserDefined UserDefinedFactory::buildAsUserDefined() const
{
  return UserDefined();
}


END_NAMESPACE_OPENTURNS
