//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SamplePartition.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SamplePartition
 */

CLASSNAMEINIT(SamplePartition)

static const Factory<SamplePartition> Factory_SamplePartition;

/* Constructor without parameters */
SamplePartition::SamplePartition()
: PersistentObject()
{
  // Nothing to do
}

SamplePartition::SamplePartition(const Sample & sample,
                const Collection<Indices> & indicesCollection)
: PersistentObject()
, sample_(sample)
, indicesCollection_(indicesCollection)
{
}

/* Virtual constructor */
SamplePartition * SamplePartition::clone() const
{
  return new SamplePartition(*this);
}

Sample SamplePartition::getSample() const
{
  return sample_;
}

/* String converter */
String SamplePartition::__repr__() const
{
  OSS oss(true);
  oss << "class= " << SamplePartition::GetClassName();
  return oss;
}

/* String converter */
String SamplePartition::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class= " << SamplePartition::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SamplePartition::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("sample_", sample_);
  adv.saveAttribute("indicesCollection_", indicesCollection_);
}

/* Method load() reloads the object from the StorageManager */
void SamplePartition::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("sample_", sample_);
  adv.loadAttribute("indicesCollection_", indicesCollection_);
}

END_NAMESPACE_OPENTURNS
