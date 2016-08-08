//                                               -*- C++ -*-
/**
 *  @brief The UserDefinedPair class.
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
#include "openturns/UserDefinedPair.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UserDefinedPair);


static const Factory<UserDefinedPair> Factory_UserDefinedPair;

/* Default constructor */
UserDefinedPair::UserDefinedPair()
  : x_(1, 0.0)
  , p_(1.0)
{
  Log::Warn(OSS() << "UserDefinedPair class is deprecated.");
}

/* Parameters constructor */
UserDefinedPair::UserDefinedPair(const NumericalPoint & x,
                                 const NumericalScalar p)
  : x_(x)
  , p_(0.0)
{
  Log::Warn(OSS() << "UserDefinedPair class is deprecated.");
  setP(p);
}

/* X accessor */
void UserDefinedPair::setX(const NumericalPoint & x)
{
  x_ = x;
}

NumericalPoint UserDefinedPair::getX() const
{
  return x_;
}

/* P accessor */
void UserDefinedPair::setP(const NumericalScalar p)
{
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  if ((p < -epsilon) || (p > 1.0 + epsilon)) throw InvalidArgumentException(HERE) << "Error: the probability of a UserDefinedPair must be in [0, 1], here p=" << p << " and 1-p=" << 1.0 - p;
  p_ = std::min(1.0, std::max(0.0, p));
}

NumericalScalar UserDefinedPair::getP() const
{
  return p_;
}

/* Comparison operator */
Bool UserDefinedPair::operator == (const UserDefinedPair & other) const
{
  if (this == &other) return true;
  return (x_ == other.x_) && (p_ == other.p_);
}

/* Virtual constructor */
UserDefinedPair * UserDefinedPair::clone() const
{
  return new UserDefinedPair(*this);
}

String UserDefinedPair::__repr__() const
{
  OSS oss;
  oss << "(" << x_ << "," << p_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void UserDefinedPair::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "x_", x_ );
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedPair::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "x_", x_ );
  adv.loadAttribute( "p_", p_ );
}

END_NAMESPACE_OPENTURNS
