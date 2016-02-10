//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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

#include "Event.hxx"
#include "CompositeRandomVector.hxx"
#include "Less.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Event);

/* Default constructor for save/load */
Event::Event()
  : RandomVector(CompositeRandomVector(NumericalMathFunction("x", "x"), RandomVector(NumericalPoint(1, 0.0))), Less(), 0.0)
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const ComparisonOperator & op,
             const NumericalScalar threshold)
  : RandomVector(antecedent, op, threshold)
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Domain & domain)
  : RandomVector(antecedent, domain)
{
  // Nothing to do
}


/* Constructor from RandomVector */
Event::Event(const RandomVector & antecedent,
             const Interval & interval)
  : RandomVector(antecedent, interval)
{
  // Nothing to do
}

/* Constructor from RandomVector */
Event::Event(const Process & process,
             const Domain & domain)
  : RandomVector(process, domain)
{
  // Nothing to do
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
String Event::__str__(const String & offset) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
