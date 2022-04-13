//                                               -*- C++ -*-
/**
 *  @brief RandomGeneratorState implements methods to manage the random generator state
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
#include "openturns/RandomGeneratorState.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Indices.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(RandomGeneratorState)

static const Factory<RandomGeneratorState> Factory_RandomGeneratorState;


/* Default constructor */
RandomGeneratorState::RandomGeneratorState()
  : buffer_(0), index_(0)
{
  // Nothing to do
}

/* Standard constructor */
RandomGeneratorState::RandomGeneratorState(const Indices buffer, const UnsignedInteger index)
  : buffer_(buffer), index_(index)
{
  // Nothing to do
}

/* Virtual constructor */
RandomGeneratorState * RandomGeneratorState::clone() const
{
  return new RandomGeneratorState(*this);
}

/* String converter */
String RandomGeneratorState::__repr__() const
{
  return OSS(true) << "RandomGeneratorState("
         << "buffer=" << buffer_ << ", "
         << "index=" << index_ << ")";
}

String RandomGeneratorState::__str__(const String & ) const
{
  return OSS(false) << "RandomGeneratorState("
         << "buffer=" << buffer_ << ", "
         << "index=" << index_ << ")";
}

/* Buffer Accessor */
Indices RandomGeneratorState::getBuffer() const
{
  return buffer_;
}

/* Index Accessor */
UnsignedInteger RandomGeneratorState::getIndex() const
{
  return index_;
}

/* Comparison operator */
Bool RandomGeneratorState::operator ==(const RandomGeneratorState & other) const
{
  Bool result = true;
  if (this != &other) result = (buffer_ == other.buffer_) && (index_ == other.index_);

  return result;
}

/* Method save() stores the object through the StorageManager */
void RandomGeneratorState::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "buffer_", buffer_);
  adv.saveAttribute( "index_", index_);
}

/* Method load() reloads the object from the StorageManager */
void RandomGeneratorState::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "buffer_", buffer_);
  adv.loadAttribute( "index_", index_);
}

END_NAMESPACE_OPENTURNS

