//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SequentialSamplingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SequentialSamplingAlgorithmImplementation)

static const Factory<SequentialSamplingAlgorithmImplementation> Factory_SequentialSamplingAlgorithmImplementation;

/* Default constructor */
SequentialSamplingAlgorithmImplementation::SequentialSamplingAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Default constructor */
SequentialSamplingAlgorithmImplementation::SequentialSamplingAlgorithmImplementation(const Sample & x,
    const Sample & y)
  : PersistentObject()
  , x_(x)
  , y_(y)
  , generationIndices_({x.getSize()})
{
  const UnsignedInteger dataSize = x.getSize();
  if (dataSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot perform approximation based on an empty sample.";
  if (dataSize != y.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot perform approximation based on samples of different size.";
  if (y.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot perform approximation based on output sample of dimension different from 1.";
}

/* Virtual constructor */
SequentialSamplingAlgorithmImplementation * SequentialSamplingAlgorithmImplementation::clone() const
{
  return new SequentialSamplingAlgorithmImplementation(*this);
}

/* Accessors */
Sample SequentialSamplingAlgorithmImplementation::getX() const
{
  return x_;
}

Sample SequentialSamplingAlgorithmImplementation::getY() const
{
  return y_;
}

Indices SequentialSamplingAlgorithmImplementation::getGenerationIndices() const
{
  return generationIndices_;
}

/* Generate new samples */
Sample SequentialSamplingAlgorithmImplementation::generate(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << " in SequentialSamplingAlgorithmImplementation::generate";
}

void SequentialSamplingAlgorithmImplementation::update(const Sample & x, const Sample & y)
{
  if (x.getSize() != y.getSize())
    throw InvalidArgumentException(HERE) << "Input sample size (" << x.getSize() << ") must match output sample size (" << y.getSize() << ")";
  x_.add(x);
  y_.add(y);
  generationIndices_.add(x_.getSize());
}

/* String converter */
String SequentialSamplingAlgorithmImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}


String SequentialSamplingAlgorithmImplementation::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void SequentialSamplingAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("x_", x_);
  adv.saveAttribute("y_", y_);
  adv.saveAttribute("generationIndices_", generationIndices_);
}


/* Method load() reloads the object from the StorageManager */
void SequentialSamplingAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("x_", x_);
  adv.loadAttribute("y_", y_);
  adv.loadAttribute("generationIndices_", generationIndices_);
}


END_NAMESPACE_OPENTURNS
