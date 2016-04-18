//                                               -*- C++ -*-
/**
 *  @brief The class that implements an event based on a composite random vector whose
 *  antecedent has a spherical distribution
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
#include "openturns/StandardEvent.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS


typedef Distribution::InverseIsoProbabilisticTransformation InverseIsoProbabilisticTransformation;

CLASSNAMEINIT(StandardEvent);

/* Default constructor */
StandardEvent::StandardEvent()
  : Event()
{
  // Nothing to do
}

/* Constructor from a RandomVector */
StandardEvent::StandardEvent(const RandomVector & antecedent,
                             const ComparisonOperator & op,
                             const NumericalScalar threshold)
  : Event(antecedent, op, threshold)
{
  // StandardEvent can only be constructed from composite random vector whose antecedent has a spherical distribution. As we cannot check it, we just check that the distribution is elliptical
  if (!antecedent.getImplementation()->getAntecedent()->getDistribution().isElliptical()) throw InvalidArgumentException(HERE) << "Error: StandardEvent can only be constructed from composite random vectors whose antecedent is standard spherical, here the distribution is " << antecedent.getImplementation()->getAntecedent()->getDistribution().getImplementation()->__str__();
  CovarianceMatrix covariance(antecedent.getImplementation()->getAntecedent()->getDistribution().getCovariance());
  const UnsignedInteger dimension = antecedent.getImplementation()->getAntecedent()->getDistribution().getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (covariance(i, i) != 1.0) throw InvalidArgumentException(HERE) << "Error: StandardEvent can only be constructed from composite random vectors whose antecedent is standard spherical, here the distribution is " << antecedent.getImplementation()->getAntecedent()->getDistribution().getImplementation()->__str__();
    for (UnsignedInteger j = 0; j < i; ++j) if (covariance(i, j) != 0.0) throw InvalidArgumentException(HERE) << "Error: StandardEvent can only be constructed from composite random vectors whose antecedent is standard spherical, here the distribution is " << antecedent.getImplementation()->getAntecedent()->getDistribution().getImplementation()->__str__();
  }
}

/* Constructor from an Event */
StandardEvent::StandardEvent(const Event & event)
  : Event(event)
{
  // Get the isoprobabilistic inverse transformation from the antecedent distribution
  const Distribution distribution(event.getImplementation()->getAntecedent()->getDistribution());
  const InverseIsoProbabilisticTransformation inverse(distribution.getInverseIsoProbabilisticTransformation());
  // Get the function
  const NumericalMathFunction function(event.getImplementation()->getFunction());
  // Get the standard distribution from the antecedent distribution
  const Distribution standard(distribution.getStandardDistribution());
  // Build the antecedent
  const RandomVector antecedent(standard);
  const NumericalMathFunction composed(function, inverse);
  const RandomVector vect(composed, antecedent);
  // Set the random vector implementation
  (*this) = StandardEvent(vect, event.getOperator(), event.getThreshold());
  setName(event.getName());
}

END_NAMESPACE_OPENTURNS
