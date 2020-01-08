//                                               -*- C++ -*-
/**
 *  @brief Compact history storage
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

#include "openturns/Compact.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Compact
 */

CLASSNAMEINIT(Compact)

static const Factory<Compact> Factory_Compact;

/* Default constructor */
Compact::Compact()
  : HistoryStrategyImplementation()
  , halfMaximumSize_(ResourceMap::GetAsUnsignedInteger( "Compact-DefaultHalfMaximumSize" ))
  , index_(0)
  , step_(1)
  , throwingCounter_(0)
{
  // Nothing to do
}

/* Constructor with parameters */
Compact::Compact(const UnsignedInteger halfMaximumSize)
  : HistoryStrategyImplementation()
  , halfMaximumSize_(halfMaximumSize)
  , index_(0)
  , step_(1)
  , throwingCounter_(0)
{
  // Nothing to do
}

/* Clear the history storage and change dimension of Point stored */
void Compact::setDimension(const UnsignedInteger dimension)
{
  sample_ = Sample(2 * halfMaximumSize_, dimension);
  index_ = 0;
  step_ = 1;
  throwingCounter_ = 0;
}

/* Virtual constructor */
Compact * Compact::clone() const
{
  return new Compact(*this);
}

/* Store the point according to the strategy */
void Compact::store(const Point & point)
{
  // If we don't throw this point
  if (throwingCounter_ == 0)
  {
    sample_.at(index_) = point;
    ++index_;
    // Reinitialize the counter
    throwingCounter_ = step_;
  }
  // Check if one needs compression
  if (index_ == 2 * halfMaximumSize_)
  {
    for (UnsignedInteger i = 0; i < halfMaximumSize_; ++i) sample_[i] = sample_[2 * i + 1];
    step_ *= 2;
    throwingCounter_ = step_;
    index_ = halfMaximumSize_;
  }
  --throwingCounter_;
}

/* Sample accessor */
Sample Compact::getSample() const
{
  // If nothing has been stored
  Sample outSample(index_, sample_.getDimension());
  for (UnsignedInteger i = 0; i < index_; ++i) outSample[i] = sample_[i];
  return outSample;
}

/* HalfMaximumSize accessor */
UnsignedInteger Compact::getHalfMaximumSize() const
{
  return halfMaximumSize_;
}

/* Index accessor */
UnsignedInteger Compact::getIndex() const
{
  return index_;
}

/* String converter */
String Compact::__repr__() const
{
  OSS oss;
  oss << "class=" << Compact::GetClassName();
  oss << " sample_=" << sample_;
  oss << " halfMaximumSize_=" << halfMaximumSize_;
  oss << " index_=" << index_;
  oss << " step_=" << step_;
  oss << " throwingCounter_=" << throwingCounter_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Compact::save(Advocate & adv) const
{
  HistoryStrategyImplementation::save(adv);
  adv.saveAttribute("halfMaximumSize_", halfMaximumSize_);
  adv.saveAttribute("index_", index_);
  adv.saveAttribute("step_", step_);
  adv.saveAttribute("throwingCounter_", throwingCounter_);
}

/* Method load() reloads the object from the StorageManager */
void Compact::load(Advocate & adv)
{
  HistoryStrategyImplementation::load(adv);
  adv.loadAttribute("halfMaximumSize_", halfMaximumSize_);
  adv.loadAttribute("index_", index_);
  adv.loadAttribute("step_", step_);
  adv.loadAttribute("throwingCounter_", throwingCounter_);

}


END_NAMESPACE_OPENTURNS
