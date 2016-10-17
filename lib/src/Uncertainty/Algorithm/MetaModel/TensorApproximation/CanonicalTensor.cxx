//                                               -*- C++ -*-
/**
 *  @brief Canonical tensor format
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
#include "openturns/CanonicalTensor.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CanonicalTensor);

static const Factory<CanonicalTensor> Factory_CanonicalTensor;



/* Default constructor */
CanonicalTensor::CanonicalTensor()
  : PersistentObject()
{
  // Nothing to do
}

/* Standard constructor */
CanonicalTensor::CanonicalTensor (const FunctionFamilyCollection & functionFamilies,
                                  const Indices & nk,
                                  const UnsignedInteger rank)
  : PersistentObject()
  , functionFamilies_(functionFamilies)
  , degrees_(nk)
  , rank1tensors_(rank)
{
  const UnsignedInteger dimension = functionFamilies.getSize();
  if (dimension != nk.getSize()) throw InvalidArgumentException(HERE) << "The number of function factories (" << dimension << ") is different from number of basis sizes (" << nk.getSize() << ")";

  for (UnsignedInteger r = 0; r < rank; ++ r)
  {
    rank1tensors_[r] = RankOneTensor(*this);
  }
}

/* Virtual constructor */
CanonicalTensor * CanonicalTensor::clone() const
{
  return new CanonicalTensor(*this);
}


void CanonicalTensor::setRank(const UnsignedInteger rank)
{
  const UnsignedInteger oldRank = getRank();
  rank1tensors_.resize(rank);
  for (UnsignedInteger r = oldRank; r < rank; ++ r)
  {
    rank1tensors_[r] = RankOneTensor(*this);
  }
}


UnsignedInteger CanonicalTensor::getRank() const
{
  return rank1tensors_.getSize();
}


CanonicalTensor::FunctionFamilyCollection CanonicalTensor::getFunctionFamilies() const
{
  return functionFamilies_;
}


Indices CanonicalTensor::getDegrees() const
{
  return degrees_;
}


void CanonicalTensor::setRankOneTensor(const UnsignedInteger k,
                                       const RankOneTensor & rankOneTensor)
{
  rank1tensors_[k] = rankOneTensor;
}


RankOneTensor CanonicalTensor::getRankOneTensor(const UnsignedInteger k) const
{
  return rank1tensors_[k];
}

/* String converter */
String CanonicalTensor::__repr__() const
{
  OSS oss;
  oss << "";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CanonicalTensor::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("degrees_", degrees_);
}

/* Method load() reloads the object from the StorageManager */
void CanonicalTensor::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("degrees_", degrees_);
}

END_NAMESPACE_OPENTURNS
