//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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

#include "openturns/Event.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/ConstantRandomVector.hxx"
#include "openturns/Less.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/EventDomain.hxx"
#include "openturns/EventRandomVector.hxx"
#include "openturns/Greater.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/EventProcess.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Event)

/* Default constructor for save/load */
Event::Event()
  : RandomVector(new EventRandomVector(CompositeRandomVector(SymbolicFunction("x", "x"), ConstantRandomVector(Point(1, 0.0))), Less(), 0.0))
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const ComparisonOperator & op,
             const Scalar threshold)
  : RandomVector(new EventRandomVector(*antecedent.getImplementation(), op, threshold))
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Domain & domain)
  : RandomVector(new EventDomain(*antecedent.getImplementation(), domain))
{
  // Nothing to do
}


/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Interval & interval)
  : RandomVector(new EventRandomVector(antecedent, interval))
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const Process & process,
             const Domain & domain)
  : RandomVector(new EventProcess(process, domain))
{
  // Nothing to do
}

Event Event::intersect(const RandomVector & other)
{
  if (&other == this)
    return *this;

  if (!isComposite() || !other.isComposite())
    throw InvalidArgumentException(HERE) << "Events must be composite";

  if (getAntecedent().getImplementation()->getId() != other.getAntecedent().getImplementation()->getId())
    throw NotYetImplementedException(HERE) << "Root cause not found";

  LevelSet d1(getFunction(), getOperator(), getThreshold());
  LevelSet d2(other.getFunction(), other.getOperator(), other.getThreshold());
  return Event(getAntecedent().getImplementation(), d1.intersect(d2));
}


Event Event::join(const RandomVector & other)
{
  if (&other == this)
    return *this;

  if (!isComposite() || !other.isComposite())
    throw InvalidArgumentException(HERE) << "Events must be composite";

  if (getAntecedent().getImplementation()->getId() != other.getAntecedent().getImplementation()->getId())
    throw NotYetImplementedException(HERE) << "Root cause not found";

  LevelSet d1(getFunction(), getOperator(), getThreshold());
  LevelSet d2(other.getFunction(), other.getOperator(), other.getThreshold());
  return Event(getAntecedent().getImplementation(), d1.join(d2));
}


/* String converter */
String Event::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Event::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String Event::__str__(const String & ) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
