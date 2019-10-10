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
#include "openturns/RandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/EventDomain.hxx"
#include "openturns/ProcessEvent.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/Domain.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
#include "openturns/SymbolicFunction.hxx"
#endif
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(RandomVector)

/* Default constructor */
RandomVector:: RandomVector()
  : TypedInterfaceObject<RandomVectorImplementation>(new RandomVectorImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
RandomVector::RandomVector(const RandomVectorImplementation & implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
RandomVector::RandomVector(const Implementation & p_implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
RandomVector::RandomVector(RandomVectorImplementation * p_implementation)
  : TypedInterfaceObject<RandomVectorImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor for distribution-based vector */
RandomVector::RandomVector(const Distribution & distribution)
  : TypedInterfaceObject<RandomVectorImplementation>(new UsualRandomVector(distribution))
{
  // Nothing to do
}

/* String converter */
String RandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomVector::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String RandomVector::__str__(const String & ) const
{
  return __repr__();
}



/* Description Accessor */
void RandomVector::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}


/* Description Accessor */
Description RandomVector::getDescription() const
{
  return getImplementation()->getDescription();
}


/* Is the underlying random vector composite ? */
Bool RandomVector::isComposite() const
{
  return getImplementation()->isComposite();
}


/* Here is the interface that all derived class may implement */

/* Dimension accessor */
UnsignedInteger RandomVector::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Realization accessor */
Point RandomVector::getRealization() const
{
  return getImplementation()->getRealization();
}

/* Numerical sample accessor */
Sample RandomVector::getSample(UnsignedInteger size) const
{
  return getImplementation()->getSample(size);
}

/* Mean accessor */
Point RandomVector::getMean() const
{
  return getImplementation()->getMean();
}

/* Covariance accessor */
CovarianceMatrix RandomVector::getCovariance() const
{
  return getImplementation()->getCovariance();
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector RandomVector::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the random vector corresponding to the i-th marginal component */
RandomVector RandomVector::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* This method allows to access the antecedent RandomVector in case of a composite RandomVector */
RandomVector RandomVector::getAntecedent() const
{
  return getImplementation()->getAntecedent();
}

/* This method allows to access the Function in case of a composite RandomVector */
Function RandomVector::getFunction() const
{
  return getImplementation()->getFunction();
}

/* This method allows to access the Distribution in case of a usual RandomVector */
Distribution RandomVector::getDistribution() const
{
  return getImplementation()->getDistribution();
}

/* Operator accessor */
ComparisonOperator RandomVector::getOperator() const
{
  return getImplementation()->getOperator();
}

/* Threshold accessor */
Scalar RandomVector::getThreshold() const
{
  return getImplementation()->getThreshold();
}

/* Domain accessor */
Domain RandomVector::getDomain() const
{
  return getImplementation()->getDomain();
}

Point RandomVector::getParameter() const
{
  return getImplementation()->getParameter();
}

void RandomVector::setParameter(const Point & parameter)
{
  getImplementation()->setParameter(parameter);
}

Description RandomVector::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}

Bool RandomVector::isEvent() const
{
  return getImplementation()->isEvent();
}


RandomVector RandomVector::intersect(const RandomVector & other)
{
  if (&other == this)
    return *this;

  if (!isComposite() || !other.isComposite())
    throw InvalidArgumentException(HERE) << "Events must be composite";

  if (getAntecedent().getImplementation()->getId() != other.getAntecedent().getImplementation()->getId())
    throw NotYetImplementedException(HERE) << "Root cause not found";

  LevelSet d1;
  try {
    // ThresholdEvent
    d1 = LevelSet(getFunction(), getOperator(), getThreshold());
  }
  catch (NotYetImplementedException &) {
    // EventDomain with LevelSet
    const EventDomain* eventDomain = dynamic_cast<EventDomain*>(getImplementation().get());
    if (!eventDomain)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    const LevelSet* levelSet = dynamic_cast<LevelSet*>(eventDomain->getDomain().getImplementation().get());
    if (!levelSet)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    d1 = *levelSet;
  }

  LevelSet d2;
  try {
    // ThresholdEvent
    d2 = LevelSet(other.getFunction(), other.getOperator(), other.getThreshold());
  }
  catch (NotYetImplementedException &) {
    // EventDomain with LevelSet
    const EventDomain* eventDomain = dynamic_cast<EventDomain*>(other.getImplementation().get());
    if (!eventDomain)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    const LevelSet* levelSet = dynamic_cast<LevelSet*>(eventDomain->getDomain().getImplementation().get());
    if (!levelSet)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    d2 = *levelSet;
  }

  return EventDomain(getAntecedent(), d1.intersect(d2));
}


RandomVector RandomVector::join(const RandomVector & other)
{
  if (&other == this)
    return *this;

  if (!isComposite() || !other.isComposite())
    throw InvalidArgumentException(HERE) << "Events must be composite";

  if (getAntecedent().getImplementation()->getId() != other.getAntecedent().getImplementation()->getId())
    throw NotYetImplementedException(HERE) << "Root cause not found";

LevelSet d1;
  try {
    // ThresholdEvent
    d1 = LevelSet(getFunction(), getOperator(), getThreshold());
  }
  catch (NotYetImplementedException &) {
    // EventDomain with LevelSet
    const EventDomain* eventDomain = dynamic_cast<EventDomain*>(getImplementation().get());
    if (!eventDomain)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    const LevelSet* levelSet = dynamic_cast<LevelSet*>(eventDomain->getDomain().getImplementation().get());
    if (!levelSet)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    d1 = *levelSet;
  }

  LevelSet d2;
  try {
    // ThresholdEvent
    d2 = LevelSet(other.getFunction(), other.getOperator(), other.getThreshold());
  }
  catch (NotYetImplementedException &) {
    // EventDomain with LevelSet
    const EventDomain* eventDomain = dynamic_cast<EventDomain*>(other.getImplementation().get());
    if (!eventDomain)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    const LevelSet* levelSet = dynamic_cast<LevelSet*>(eventDomain->getDomain().getImplementation().get());
    if (!levelSet)
      throw NotYetImplementedException(HERE) << "in RandomVector::intersect";
    d2 = *levelSet;
  }

  return EventDomain(getAntecedent().getImplementation(), d1.join(d2));
}

END_NAMESPACE_OPENTURNS
