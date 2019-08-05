//                                               -*- C++ -*-
/**
 * @brief Union of several events
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/UnionEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UnionEvent)

// static const Factory<UnionEvent> Factory_UnionEvent;

/* Default constructor */
UnionEvent::UnionEvent()
  : RandomVectorImplementation()
{
  // Nothing to do
}

/* Constructor from RandomVector */
UnionEvent::UnionEvent(const RandomVectorCollection & collection)
  : RandomVectorImplementation()
{
  setEventCollection(collection);
}

UnionEvent * UnionEvent::clone() const
{
  return new UnionEvent(*this);
}

/* String converter */
String UnionEvent::__repr__() const
{
  OSS oss;
  oss << "class=" << UnionEvent::GetClassName()
      << " eventCollection=" << eventCollection_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger UnionEvent::getDimension() const
{
  return 1;
}

/* Domain accessor */
UnionEvent::RandomVectorCollection UnionEvent::getEventCollection() const
{
  return eventCollection_;
}

void UnionEvent::setEventCollection(const RandomVectorCollection & collection)
{
  const UnsignedInteger size = collection.getSize();
  if (!size) throw InvalidArgumentException(HERE) << "Empty collection";
  rootCauseId_ = collection[0].getAntecedent().getImplementation()->getId();
  aggregatedEvent_ = collection[0];
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
//     if (!collection[i].isEvent())
//       throw InvalidArgumentException(HERE) << "Element " << i << " is not an event";
    if (collection[i].getAntecedent().getImplementation()->getId() != rootCauseId_)
      throw NotYetImplementedException(HERE) << "Root cause not found";
    aggregatedEvent_ = aggregatedEvent_.join(collection[i]);
  }
  eventCollection_ = collection;
  setDescription(aggregatedEvent_.getDescription());
}

/* Realization accessor */
Point UnionEvent::getRealization() const
{
  return aggregatedEvent_.getRealization();
}

Bool UnionEvent::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void UnionEvent::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "eventCollection_", eventCollection_ );
}

/* Method load() reloads the object from the StorageManager */
void UnionEvent::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "eventCollection_", eventCollection_ );
}

END_NAMESPACE_OPENTURNS
