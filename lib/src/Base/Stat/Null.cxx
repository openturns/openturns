//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Null.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Null
 */

CLASSNAMEINIT(Null)

static const Factory<Null> Factory_Null;

/* Default constructor */
Null::Null()
  : HistoryStrategyImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
Null * Null::clone() const
{
  return new Null(*this);
}

/* Store the point according to the strategy */
void Null::store(const Point & )
{
  // Nothing to do
}

/* Store the sample according to the strategy */
void Null::store(const Sample & )
{
  // Nothing to do
}

/* String converter */
String Null::__repr__() const
{
  OSS oss;
  oss << "class=" << Null::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
