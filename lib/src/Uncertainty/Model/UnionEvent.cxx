//                                               -*- C++ -*-
/**
 * @brief Union of several events
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

#include "openturns/UnionEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IntersectionEvent.hxx"
#include "openturns/DomainEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UnionEvent)

static const Factory<UnionEvent> Factory_UnionEvent;
static const Factory<PersistentCollection<RandomVector> > Factory_PersistentCollection_RandomVector;

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

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    if (!collection[i].isEvent())
      throw InvalidArgumentException(HERE) << "Element " << i << " is not an event";
  }

  antecedent_ = collection[0].getAntecedent();
  const UnsignedInteger rootCauseId = antecedent_.getImplementation()->getId();

  // Explore the tree, check root cause
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    if (collection[i].getAntecedent().getImplementation()->getId() != rootCauseId)
      throw NotYetImplementedException(HERE) << "Root cause not found";
  }
  eventCollection_ = collection;
  setDescription(collection[0].getDescription());
}

Bool UnionEvent::buildComposedEvent()
{
  // Short way out if we know composedEvent_ has already been set.
  if (hasComposedEvent_) return true;

  const UnsignedInteger size = eventCollection_.getSize();
  if (!size) throw InvalidArgumentException(HERE) << "Empty eventCollection_, please call setEventCollection first.";

  RandomVector composedEvent;

  // Initialize composedEvent with the first event in eventCollection_
  if (eventCollection_[0].getImplementation()->getClassName() == "IntersectionEvent")
  {
    // IntersectionEvent
    IntersectionEvent *intersectionEvent = static_cast<IntersectionEvent*>(eventCollection_[0].getImplementation().get());
    const Bool hasComposedEvent = intersectionEvent->buildComposedEvent();
    if (!hasComposedEvent) return false;
    composedEvent = intersectionEvent->getComposedEvent();
  }
  else if (eventCollection_[0].getImplementation()->getClassName() == "UnionEvent")
  {
    // UnionEvent
    UnionEvent *unionEvent = static_cast<UnionEvent*>(eventCollection_[0].getImplementation().get());
    const Bool hasComposedEvent = unionEvent->buildComposedEvent();
    if (!hasComposedEvent) return false;
    composedEvent = unionEvent->getComposedEvent();
  }
  else
  {
    // ThresholdEvent
    composedEvent = eventCollection_[0];
  }

  // Further build composedEvent by composing with the other events in the eventCollection_
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    if (eventCollection_[i].getImplementation()->getClassName() == "IntersectionEvent")
    {
      // IntersectionEvent
      IntersectionEvent* intersectionEvent = static_cast<IntersectionEvent*>(eventCollection_[i].getImplementation().get());
      const Bool hasComposedEvent = intersectionEvent->buildComposedEvent();
      if (!hasComposedEvent) return false;
      composedEvent = composedEvent.join(intersectionEvent->getComposedEvent());
    }
    else if (eventCollection_[i].getImplementation()->getClassName() == "UnionEvent")
    {
      // UnionEvent
      UnionEvent* unionEvent = static_cast<UnionEvent*>(eventCollection_[i].getImplementation().get());
      const Bool hasComposedEvent = unionEvent->buildComposedEvent();
      if (!hasComposedEvent) return false;
      composedEvent = composedEvent.join(unionEvent->getComposedEvent());
    }
    else
    {
      // ThresholdEvent
      composedEvent = composedEvent.join(eventCollection_[i]);
    }
  }
  composedEvent_ = composedEvent;
  hasComposedEvent_ = true;
  return true;
}

/* Realization accessor */
Point UnionEvent::getRealization() const
{
  return getFrozenRealization(antecedent_.getRealization());
}

/* Fixed value accessor */
Point UnionEvent::getFrozenRealization(const Point & fixedValue) const
{
  LOGINFO(OSS() << "antecedent value = " << fixedValue);
  Point realization(1);
  for (UnsignedInteger i = 0; i < eventCollection_.getSize(); ++ i)
  {
    if (eventCollection_[i].getFrozenRealization(fixedValue)[0] == 1.0)
    {
      realization[0] = 1.0;
      return realization;
    }
  }
  return realization;
}

Bool UnionEvent::isEvent() const
{
  return true;
}

Bool UnionEvent::isComposite() const
{
  return true;
}

RandomVector UnionEvent::getAntecedent() const
{
  return antecedent_;
}

Function UnionEvent::getFunction() const
{
  return composedEvent_.getFunction();
}

Domain UnionEvent::getDomain() const
{
  return composedEvent_.getDomain();
}

ComparisonOperator UnionEvent::getOperator() const
{
  return composedEvent_.getOperator();
}

Scalar UnionEvent::getThreshold() const
{
  return composedEvent_.getThreshold();
}

/* Method save() stores the object through the StorageManager */
void UnionEvent::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute("eventCollection_", eventCollection_);
}

/* Method load() reloads the object from the StorageManager */
void UnionEvent::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  RandomVectorPersistentCollection eventCollection;
  adv.loadAttribute("eventCollection_", eventCollection);
  setEventCollection(eventCollection);
}

RandomVector UnionEvent::getComposedEvent() const
{
  if (hasComposedEvent_)
    return composedEvent_;
  throw NotYetImplementedException(HERE) << "Please call buildComposedEvent before getComposedEvent.";
}

END_NAMESPACE_OPENTURNS
