//                                               -*- C++ -*-
/**
 *  @brief An implementation class for distribution-based random vectors
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
#include "PersistentObjectFactory.hxx"
#include "ConditionalRandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(ConditionalRandomVector);

static const Factory<ConditionalRandomVector> RegisteredFactory;

/* Default constructor */
ConditionalRandomVector::ConditionalRandomVector(const Distribution & distribution,
    const RandomVector & randomParameters)
  : RandomVectorImplementation(),
    distribution_(distribution),
    randomParameters_(randomParameters)
{
  // Check if the random parameters random vector has a dimension compatible with
  // the number of parameters of the distribution
  if (randomParameters.getDimension() != distribution.getParametersNumber()) throw InvalidArgumentException(HERE) << "Error: the random parameters dimension must be equal with the number of parameters of the distribution.";
  // Get the description from the underlying distribution
  setDescription(distribution.getDescription());
}

/* Virtual constructor */
ConditionalRandomVector * ConditionalRandomVector::clone() const
{
  return new ConditionalRandomVector(*this);
}

/* String converter */
String ConditionalRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << ConditionalRandomVector::GetClassName()
      << " distribution=" << distribution_
      << " random parameters=" << randomParameters_;
  return oss;
}



/* Here is the interface that all derived class must implement */


/* Dimension accessor */
UnsignedInteger ConditionalRandomVector::getDimension() const
{
  return distribution_.getDimension();
}

/* Realization accessor */
NumericalPoint ConditionalRandomVector::getRealization() const
{
  NumericalPoint parameters;
  return getRealization(parameters);
}

/* Realization accessor */
NumericalPoint ConditionalRandomVector::getRealization(NumericalPoint & parameters) const
{
  parameters = randomParameters_.getRealization();
  distribution_.setParametersCollection(parameters);
  return distribution_.getRealization();
}

/* Distribution accessor */
Distribution ConditionalRandomVector::getDistribution() const
{
  return distribution_;
}

/* Random parameters accessor */
RandomVector ConditionalRandomVector::getRandomParameters() const
{
  return randomParameters_;
}

/* Method save() stores the object through the StorageManager */
void ConditionalRandomVector::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "randomParameters_", randomParameters_ );
}

/* Method load() reloads the object from the StorageManager */
void ConditionalRandomVector::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "randomParameters_", randomParameters_ );
}

END_NAMESPACE_OPENTURNS
