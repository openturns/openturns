//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
  : RandomVector(new EventDomain(*antecedent.getImplementation(), interval))
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  UnsignedInteger dimension = interval.getDimension();
  UnsignedInteger inputDimension = antecedent.getFunction().getInputDimension();
  Interval::BoolCollection finiteLowerBound(interval.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(interval.getFiniteUpperBound());
  Point lowerBound(interval.getLowerBound());
  Point upperBound(interval.getUpperBound());
  SymbolicFunction testFunction(Description::BuildDefault(inputDimension, "x"), Description(1, "0.0"));

  // easy case: 1d interval
  if (interval.getDimension() == 1)
  {
    if (finiteLowerBound[0] && !finiteUpperBound[0])
    {
      *this = Event(antecedent, Greater(), lowerBound[0]);
    }
    if (!finiteLowerBound[0] && finiteUpperBound[0])
    {
      *this = Event(antecedent, Less(), upperBound[0]);
    }

    if (finiteLowerBound[0] && finiteUpperBound[0])
    {
      testFunction = SymbolicFunction("x", OSS() << "min(x-(" << lowerBound[0] << "), (" << upperBound[0] << ") - x)");
      CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = Event(newVector, Greater(), 0.0);
    }
    if (!finiteLowerBound[0] && !finiteUpperBound[0])
    {
      CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
      *this = Event(newVector, Less(), 1.0);
    }
  }
  // general case
  else
  {
    Description inVars(Description::BuildDefault(dimension, "y"));
    Description slacks(0);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      if (finiteLowerBound[i])
        slacks.add(OSS() << inVars[i] << "-(" << lowerBound[i] << ")");
      if (finiteUpperBound[i])
        slacks.add(OSS() << "(" << upperBound[i] << ")-" << inVars[i]);
    }
    // No constraint
    if (slacks.getSize() == 0)
    {
      CompositeRandomVector newVector(Function(testFunction), antecedent.getAntecedent());
      *this = Event(newVector, Less(), 1.0);
    }
    else
    {
      String formula;
      if (slacks.getSize() == 1)
      {
        formula = slacks[0];
      }
      else
      {
        formula = "min(" + slacks[0];
        for (UnsignedInteger i = 1; i < slacks.getSize(); ++ i)
          formula += "," + slacks[i];
        formula += ")";
      }
      testFunction = SymbolicFunction(inVars, Description(1, formula));
      CompositeRandomVector newVector(ComposedFunction(testFunction, antecedent.getFunction()), antecedent.getAntecedent());
      *this = Event(newVector, Greater(), 0.0);
    }
  }
#endif
}

/* Constructor from RandomVector */
Event::Event(const Process & process,
             const Domain & domain)
  : RandomVector(new EventProcess(process, domain))
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
String Event::__str__(const String & ) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
