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
  , rank1tensors_(rank, RankOneTensor(functionFamilies, nk))// this constructor assumes the same nks for all ranks
{
  const UnsignedInteger dimension = functionFamilies.getSize();
  if (dimension != nk.getSize()) throw InvalidArgumentException(HERE) << "The number of function factories (" << dimension << ") is different from number of basis sizes (" << nk.getSize() << ")";
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
    rank1tensors_[r] = rank1tensors_[0];
  }
}


UnsignedInteger CanonicalTensor::getRank() const
{
  return rank1tensors_.getSize();
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
}

/* Method load() reloads the object from the StorageManager */
void CanonicalTensor::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
