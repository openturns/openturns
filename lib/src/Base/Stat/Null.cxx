//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/Null.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Null
 */

CLASSNAMEINIT(Null)

static const Factory<Null> Factory_Null;
/* Constructor with parameters */
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
void Null::store(const Point & point)
{
  if (!isInitialized_)
  {
    sample_ = Sample(0, point.getDimension());
    isInitialized_ = true;
  }
}

/* Store the sample according to the strategy */
void Null::store(const Sample & sample)
{
  if (!isInitialized_)
  {
    sample_ = Sample(0, sample.getDimension());
    isInitialized_ = true;
  }
}

/* String converter */
String Null::__repr__() const
{
  OSS oss;
  oss << "class=" << Null::GetClassName();
  oss << " sample_= " << sample_;
  return oss;
}

END_NAMESPACE_OPENTURNS
