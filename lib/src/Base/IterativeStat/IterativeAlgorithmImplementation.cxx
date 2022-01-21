//                                               -*- C++ -*-
/**
 *  @brief IterativeAlgorithmImplementation is the implementation of IterativeAlgorithm
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
#include "openturns/IterativeAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeAlgorithmImplementation)

/** Default constructor */
IterativeAlgorithmImplementation::IterativeAlgorithmImplementation()
  : PersistentObject()
  , iteration_(0)
  , dimension_(1)
{
  // Nothing to do
}

/** Default constructor */
IterativeAlgorithmImplementation::IterativeAlgorithmImplementation(const UnsignedInteger dimension)
  : PersistentObject()
  , iteration_(0)
  , dimension_(dimension)
{
  // Nothing to do
}

IterativeAlgorithmImplementation * IterativeAlgorithmImplementation::clone() const
{
  return new IterativeAlgorithmImplementation(*this);
}

/** String converter */
String IterativeAlgorithmImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}

/** String converter */
String IterativeAlgorithmImplementation::__str__(const String &) const
{
  return __repr__();
}

/**
 * Iteration accessor
 *
 * This method returns the current iteration number of the algorithm.
 */
UnsignedInteger IterativeAlgorithmImplementation::getIterationNumber() const
{
  return iteration_;
}

/**
 * Dimension accessor
 *
 * This method returns the dimension of the object.
 */
UnsignedInteger IterativeAlgorithmImplementation::getDimension() const
{
  return dimension_;
}

/** Increment method from a Point */
void IterativeAlgorithmImplementation::increment(const Point & /*newData*/)
{
  throw NotYetImplementedException(HERE) << "You must use a derived class of IterativeAlgorithm!";
}

/** Increment method from a Sample */
void IterativeAlgorithmImplementation::increment(const Sample & /*newData*/)
{
  throw NotYetImplementedException(HERE) << "You must use a derived class of IterativeAlgorithm!";
}

/** Method save() stores the object through the StorageManager
 *
 * @warning This method MUST be overloaded in derived classes.
 * @internal
 */
void IterativeAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "iteration_", iteration_);
}

/** Method load() reloads the object from the StorageManager
 *
 * @warning This method MUST be overloaded in derived classes.
 * @internal
 */
void IterativeAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "iteration_", iteration_);
}

END_NAMESPACE_OPENTURNS
