//                                               -*- C++ -*-
/**
 *  @brief ARMAState class enables to set a State before a simulation
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ARMAState.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ARMAState)
static const Factory<ARMAState> Factory_ARMAState;

/* Default constructor */
ARMAState::ARMAState()
  : PersistentObject()
  , x_()
  , epsilon_()
{
  // Nothing to do
}

/* Standard constructor */
ARMAState::ARMAState(const Sample & x,
                     const Sample & epsilon)
  : PersistentObject()
  , x_(x)
  , epsilon_(epsilon)
{
  // Nothing to do
}

/* Virtual constructor  - clone*/
ARMAState * ARMAState::clone() const
{
  return new ARMAState(*this);
}

/* String converter */
String ARMAState::__repr__() const
{
  OSS oss;
  oss << "class= " << ARMAState::GetClassName();
  oss << " x= " << x_
      << " epsilon= " << epsilon_;
  return oss;
}

String ARMAState::__str__(const String & offset) const
{
  OSS oss;
  // The x part of the state stores the p past values of the process that
  // are needed to compute the current value
  String separator("");
  const UnsignedInteger xSize = x_.getSize();
  for (UnsignedInteger i = 0; i < xSize; ++i, separator = Os::GetEndOfLine() )
  {
    const UnsignedInteger index = xSize - i;
    oss << separator << offset << "X(t-" << index << ")" << std::setw(String(OSS() << xSize).size() - String(OSS() << index).size() + 3) << " = " << x_[i];
  }
  // The epsilon part of the state stores the q past values of the noise
  // that are needed to compute the current value
  const UnsignedInteger epsilonSize = epsilon_.getSize();
  for (UnsignedInteger i = 0; i < epsilonSize; ++i)
  {
    const UnsignedInteger index = epsilonSize - i;
    oss << separator << "epsilon(t-" << index << ")" << std::setw(String(OSS() << epsilonSize).size() - String(OSS() << index).size() + 3) << " = " << epsilon_[i];
  }
  return oss;
}


/* Data X accessor */
Sample ARMAState::getX() const
{
  return x_;
}

void ARMAState::setXEpsilon(const Sample & x,
                            const Sample & epsilon)
{
  x_ = x;
  epsilon_ = epsilon;
}

/* Epsilon \epsilon accessor */
Sample ARMAState::getEpsilon() const
{
  return epsilon_;
}

UnsignedInteger ARMAState::getDimension() const
{
  return x_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void ARMAState::save(Advocate & adv) const
{
  PersistentObject::save( adv );
  adv.saveAttribute("x_", x_);
  adv.saveAttribute("epsilon_", epsilon_);
}

/* Method load() reloads the object from the StorageManager */
void ARMAState::load(Advocate & adv)
{
  PersistentObject::load( adv );
  adv.loadAttribute("x_", x_);
  adv.loadAttribute("epsilon_", epsilon_);
}

END_NAMESPACE_OPENTURNS

