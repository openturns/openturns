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
#include "openturns/Full.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Full
 */

CLASSNAMEINIT(Full);

static const Factory<Full> Factory_Full;

/* Constructor with parameters */
Full::Full()
  : HistoryStrategyImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
Full * Full::clone() const
{
  return new Full(*this);
}

/* Store the point according to the strategy */
void Full::store(const NumericalPoint & point)
{
  if (!isInitialized_)
  {
    sample_ = NumericalSample(0, point.getDimension());
    isInitialized_ = true;
  }
  sample_.add(point);
}

/* Store the sample according to the strategy */
void Full::store(const NumericalSample & sample)
{
  if (!isInitialized_)
  {
    sample_ = sample;
    isInitialized_ = true;
  }
  else sample_.add(sample);
}

/* String converter */
String Full::__repr__() const
{
  OSS oss;
  oss << "class=" << Full::GetClassName();
  oss << " sample_= " << sample_;
  return oss;
}

END_NAMESPACE_OPENTURNS
