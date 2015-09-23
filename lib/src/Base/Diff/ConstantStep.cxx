//                                             -*- C++ -*-
/**
 *  @brief Implementation of ConstantStep
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "ConstantStep.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantStep);

static const Factory<ConstantStep> RegisteredFactory;

/* Default constructor */
ConstantStep::ConstantStep()
  : FiniteDifferenceStepImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
ConstantStep::ConstantStep(const NumericalPoint & epsilon)
  : FiniteDifferenceStepImplementation(epsilon)
{
  // Nothing to do
}

/* String converter */
String ConstantStep::__repr__() const
{
  OSS oss;
  oss << "class=" << ConstantStep::GetClassName()
      << " derived from " << FiniteDifferenceStepImplementation::__repr__();
  return oss;
}

/* Virtual constructor */
ConstantStep * ConstantStep::clone() const
{
  return new ConstantStep(*this);
}

/* Compute step */
NumericalPoint ConstantStep::operator()(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension( epsilon_.getDimension() );
  if (dimension != inP.getDimension()) throw InvalidArgumentException(HERE) << "Invalid dimension eps:" << dimension << " x:" << inP.getDimension();
  return epsilon_;
}

/* Method save() stores the object through the StorageManager */
void ConstantStep::save(Advocate & adv) const
{
  FiniteDifferenceStepImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ConstantStep::load(Advocate & adv)
{
  FiniteDifferenceStepImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS


