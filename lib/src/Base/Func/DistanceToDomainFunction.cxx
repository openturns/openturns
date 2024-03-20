//                                               -*- C++ -*-
/**
 *  @brief The class that implements DistanceToDomain functions.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DistanceToDomainFunction.hxx"
#include "openturns/DistanceToDomainEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistanceToDomainFunction)

/* Default constructor */
DistanceToDomainFunction::DistanceToDomainFunction ()
  : Function()
{
  // Nothing to do
}


/* Parameter constructor */
DistanceToDomainFunction::DistanceToDomainFunction (const Domain & domain)
  : Function()
{
  const Pointer<DistanceToDomainEvaluation> p_evaluation = new DistanceToDomainEvaluation(domain);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(NoGradient().clone());
  setHessian(NoHessian().clone());
}


/* Comparison operator */
Bool DistanceToDomainFunction::operator ==(const DistanceToDomainFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

/* String converter */
String DistanceToDomainFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String DistanceToDomainFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
