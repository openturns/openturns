//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DomainEvent.hxx"
#include "openturns/ThresholdEventImplementation.hxx"
#include "openturns/Greater.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/ProcessEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Event)

/* Default constructor for save/load */
Event::Event()
  : RandomVector(new ThresholdEventImplementation(CompositeRandomVector(SymbolicFunction("x", "x"), ConstantRandomVector(Point(1, 0.0))), Less(), 0.0))
{
  LOGWARN("Event() is deprecated");
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const ComparisonOperator & op,
             const Scalar threshold)
  : RandomVector(new ThresholdEventImplementation(antecedent, op, threshold))
{
  LOGWARN("Event(RandomVector, ComparisonOperator, float) is deprecated, use ThresholdEvent");
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Domain & domain)
  : RandomVector(new DomainEvent(antecedent, domain))
{
  LOGWARN("Event(RandomVector, Domain) is deprecated, use EventDomain");
}


/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Interval & interval)
  : RandomVector(new ThresholdEventImplementation(antecedent, interval))
{
  LOGWARN("Event(RandomVector, Interval) is deprecated, use ProcessEvent");
}

/* Constructor from RandomVector */
Event::Event(const Process & process,
             const Domain & domain)
  : RandomVector(new ProcessEvent(process, domain))
{
  LOGWARN("Event(Process, Domain) is deprecated, use ProcessEvent");
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
