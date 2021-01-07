//                                             -*- C++ -*-
/**
 *  @brief Interface for finite difference steps
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FiniteDifferenceStep.hxx"
#include "openturns/ConstantStep.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteDifferenceStep)

/* Default constructor */
FiniteDifferenceStep::FiniteDifferenceStep()
  : TypedInterfaceObject<FiniteDifferenceStepImplementation>(new ConstantStep())
{
  // Nothing to do
}

/* Parameters constructor */
FiniteDifferenceStep::FiniteDifferenceStep(const Point & eps)
  : TypedInterfaceObject<FiniteDifferenceStepImplementation>(new ConstantStep(eps))
{
  // Nothing to do
}

/* Constructor from implementation */
FiniteDifferenceStep::FiniteDifferenceStep(const FiniteDifferenceStepImplementation & implementation)
  : TypedInterfaceObject<FiniteDifferenceStepImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
FiniteDifferenceStep::FiniteDifferenceStep(const Implementation & p_implementation)
  : TypedInterfaceObject<FiniteDifferenceStepImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String FiniteDifferenceStep::__repr__() const
{
  OSS oss;
  oss << "class=" << FiniteDifferenceStep::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Epsilon accessor */
void FiniteDifferenceStep::setEpsilon(const Point & epsilon)
{
  copyOnWrite();
  getImplementation()->setEpsilon(epsilon);
}

Point FiniteDifferenceStep::getEpsilon() const
{
  return getImplementation()->getEpsilon();
}

/* Compute step */
Point FiniteDifferenceStep::operator()(const Point & in) const
{
  return getImplementation()->operator()(in);
}


END_NAMESPACE_OPENTURNS


