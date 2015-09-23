//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
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

#include "EventRandomVectorImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EventRandomVectorImplementation);

static const Factory<EventRandomVectorImplementation> RegisteredFactory;

/* Default constructor */
EventRandomVectorImplementation::EventRandomVectorImplementation()
  : CompositeRandomVector()
  , operator_()
  , threshold_()
{
  // Nothing to do
}

/* Constructor from RandomVector */
EventRandomVectorImplementation::EventRandomVectorImplementation(const RandomVectorImplementation & antecedent,
    const ComparisonOperator & op,
    const NumericalScalar threshold)
  : CompositeRandomVector()
  , operator_(op)
  , threshold_(threshold)
{
  // Event can only be constructed from composite random vectors
  if (!antecedent.isComposite())
    throw InvalidArgumentException(HERE) << "Event can only be constructed from composite random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of EventRandomVectorImplementation "
                                         << " has incorrect type";
  // EventRandomVectorImplementation can only be constructed from 1D random vectors
  if (antecedent.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "EventRandomVectorImplementation can only be constructed from 1D random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of EventRandomVectorImplementation "
                                         << " has incorrect dimension";
  function_ = antecedent.getFunction();
  p_antecedent_ = antecedent.getAntecedent();
  setName(antecedent.getName());
  setDescription(antecedent.getDescription());
}

EventRandomVectorImplementation * EventRandomVectorImplementation::clone() const
{
  return new EventRandomVectorImplementation(*this);
}

/* String converter */
String EventRandomVectorImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << EventRandomVectorImplementation::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " operator=" << operator_
      << " threshold=" << threshold_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger EventRandomVectorImplementation::getDimension() const
{
  return 1;
}

/* Operator accessor */
ComparisonOperator EventRandomVectorImplementation::getOperator() const
{
  return operator_;
}

/* Threshold accessor */
NumericalScalar EventRandomVectorImplementation::getThreshold() const
{
  return threshold_;
}

/* Realization accessor */
NumericalPoint EventRandomVectorImplementation::getRealization() const
{
  return NumericalPoint(1, operator_(CompositeRandomVector::getRealization()[0], threshold_));
}

/* Numerical sample accessor */
NumericalSample EventRandomVectorImplementation::getSample(const UnsignedInteger size) const
{
  // We don't build the return sample element by element because it doesn't
  // use the potential distribution of the computation. As the returned
  // sample can be huge, we use it twice in place
  // First, it stores a sample of its antecedent
  NumericalSample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample and substitute realizations of the eventRandomVectorImplementation
  // in place of the realizations of the antecedent
  for (UnsignedInteger i = 0; i < size; ++i) returnSample[i][0] = operator_(returnSample[i][0], threshold_);
  returnSample.setName("EventRandomVectorImplementation sample");
  returnSample.setDescription(getDescription());
  return returnSample;
}

/* Method save() stores the object through the StorageManager */
void EventRandomVectorImplementation::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "operator_", operator_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void EventRandomVectorImplementation::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "operator_", operator_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS
