//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HistoryStrategyImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class HistoryStrategyImplementation
 */

CLASSNAMEINIT(HistoryStrategyImplementation)

static const Factory<HistoryStrategyImplementation> Factory_HistoryStrategyImplementation;

/* Default constructor */
HistoryStrategyImplementation::HistoryStrategyImplementation()
  : PersistentObject()
  , sample_()
{
  // Nothing to do
}

/* Virtual constructor */
HistoryStrategyImplementation * HistoryStrategyImplementation::clone() const
{
  return new HistoryStrategyImplementation(*this);
}

/* Store the point according to the strategy */
void HistoryStrategyImplementation::store(const Point & )
{
  throw NotYetImplementedException(HERE) << "In HistoryStrategyImplementation::store(const Point & point)";
}

/* Store the sample according to the strategy */
void HistoryStrategyImplementation::store(const Sample & sample)
{
  for (UnsignedInteger i = 0; i < sample.getSize(); ++i) store(sample[i]);
}

/* Clear the history storage and change dimension of Point stored */
void HistoryStrategyImplementation::setDimension(const UnsignedInteger dimension)
{
  sample_ = Sample(0, dimension);
}

/* Clear the history storage */
void HistoryStrategyImplementation::clear()
{
  setDimension(sample_.getDimension());
}

/* History sample accessor */
Sample HistoryStrategyImplementation::getSample() const
{
  return sample_;
}

/* String converter */
String HistoryStrategyImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << HistoryStrategyImplementation::GetClassName();
  return oss;
}

/** Method save() stores the object through the StorageManager */
void HistoryStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);

  adv.saveAttribute("sample_", sample_);
}

/** Method load() reloads the object from the StorageManager */
void HistoryStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);

  adv.loadAttribute("sample_", sample_);
}


END_NAMESPACE_OPENTURNS
