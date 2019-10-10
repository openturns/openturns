//                                               -*- C++ -*-
/**
 * @brief Intersection of several events
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

#include "openturns/IntersectionEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UnionEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IntersectionEvent)

// static const Factory<IntersectionEvent> Factory_IntersectionEvent;

/* Default constructor */
IntersectionEvent::IntersectionEvent()
  : RandomVectorImplementation()
{
  // Nothing to do
}

/* Constructor from RandomVector */
IntersectionEvent::IntersectionEvent(const RandomVectorCollection & collection)
  : RandomVectorImplementation()
{
  setEventCollection(collection);
}

IntersectionEvent * IntersectionEvent::clone() const
{
  return new IntersectionEvent(*this);
}

/* String converter */
String IntersectionEvent::__repr__() const
{
  OSS oss;
  oss << "class=" << IntersectionEvent::GetClassName()
      << " eventCollection=" << eventCollection_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger IntersectionEvent::getDimension() const
{
  return 1;
}

/* Domain accessor */
IntersectionEvent::RandomVectorCollection IntersectionEvent::getEventCollection() const
{
  return eventCollection_;
}

void IntersectionEvent::setEventCollection(const RandomVectorCollection & collection)
{
  const UnsignedInteger size = collection.getSize();
  if (!size) throw InvalidArgumentException(HERE) << "Empty collection";

  // Explore the deepest leftmost node of the tree which is not Intersection/Union to get the root cause
  // Also we initialize composedEvent_ if Intersection/Union use getComposedEvent from top node else take the EventRandomVector
  if (!collection[0].isEvent())
    throw InvalidArgumentException(HERE) << "Element 0 is not an event";
  UnsignedInteger depth = 0;
  RandomVector current = collection[0];
  String implementationName = current.getImplementation()->getClassName();
  while ((implementationName == "IntersectionEvent") || (implementationName == "UnionEvent"))
  {
    Collection<RandomVector> children;
    if (implementationName == "IntersectionEvent")
    {
      IntersectionEvent *intersectionEvent = static_cast<IntersectionEvent*>(current.getImplementation().get());
      if (depth == 0)
        composedEvent_ = intersectionEvent->getComposedEvent();
      children = intersectionEvent->getEventCollection();
    }
    else if (implementationName == "UnionEvent")
    {
      UnionEvent *unionEvent = static_cast<UnionEvent*>(current.getImplementation().get());
      if (depth == 0)
        composedEvent_ = unionEvent->getComposedEvent();
      children = unionEvent->getEventCollection();
    }
    current = children[0];
    ++ depth;
    implementationName = current.getImplementation()->getClassName();
  }
  rootCauseId_ = current.getAntecedent().getImplementation()->getId();
  if (depth == 0) // no IntersectionEvent/Union was found, take the first node
    composedEvent_ = collection[0];

  // Explore the tree, check root cause, compose top-nodes
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    if (!collection[i].isEvent())
      throw InvalidArgumentException(HERE) << "Element " << i << " is not an event";
    if (collection[i].getImplementation()->getClassName() == "IntersectionEvent")
    {
      // IntersectionEvent
      IntersectionEvent* intersectionEvent = static_cast<IntersectionEvent*>(collection[i].getImplementation().get());
      if (intersectionEvent->getRootCauseId() != rootCauseId_)
        throw InvalidArgumentException(HERE) << "Different root cause";
      composedEvent_ = composedEvent_.intersect(intersectionEvent->getComposedEvent());
    }
    else if (collection[i].getImplementation()->getClassName() == "UnionEvent")
    {
      // UnionEvent
      UnionEvent* unionEvent = static_cast<UnionEvent*>(collection[i].getImplementation().get());
      if (unionEvent->getRootCauseId() != rootCauseId_)
        throw InvalidArgumentException(HERE) << "Different root cause";
      composedEvent_ = composedEvent_.intersect(unionEvent->getComposedEvent());
    }
    else
    {
      // EventRandomVector
      if (collection[i].getAntecedent().getImplementation()->getId() != rootCauseId_)
        throw NotYetImplementedException(HERE) << "Root cause not found";
      composedEvent_ = composedEvent_.intersect(collection[i]);
    }
  }
  eventCollection_ = collection;
  setDescription(composedEvent_.getDescription());
}

/* Realization accessor */
Point IntersectionEvent::getRealization() const
{
  return composedEvent_.getRealization();
}

Bool IntersectionEvent::isEvent() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void IntersectionEvent::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "eventCollection_", eventCollection_ );
}

/* Method load() reloads the object from the StorageManager */
void IntersectionEvent::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "eventCollection_", eventCollection_ );
}

UnsignedInteger IntersectionEvent::getRootCauseId() const
{
  return rootCauseId_;
}

RandomVector IntersectionEvent::getComposedEvent() const
{
  return composedEvent_;
}

END_NAMESPACE_OPENTURNS
