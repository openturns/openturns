//                                               -*- C++ -*-
/**
 *  @brief Classification algorithm base type
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ClassifierImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ClassifierImplementation);

static const Factory<ClassifierImplementation> RegisteredFactory;

/* Default constructor */
ClassifierImplementation::ClassifierImplementation()
  : PersistentObject()
  , verbose_(false)
{
  // Nothing to do
}

/* Default constructor */
ClassifierImplementation::ClassifierImplementation(const NumericalSample & inputSample,
    const Indices & classes)
  : PersistentObject()
  , inputSample_(inputSample)
  , classes_(classes)
  , verbose_(false)
{
  // Nothing to do
}

ClassifierImplementation * ClassifierImplementation::clone() const
{
  return new ClassifierImplementation(*this);
}


/* Classify a sample */
UnsignedInteger ClassifierImplementation::classify(const NumericalPoint & inP) const
{
  throw NotYetImplementedException(HERE) << "In ClassifierImplementation::classify(const NumericalPoint & inP) const";
}

/* Classify a sample */
Indices ClassifierImplementation::classify(const NumericalSample & inS) const
{
  const UnsignedInteger size(inS.getSize());
  Indices prediction(size);
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    prediction[i] = classify(inS[i]);
  }
  return prediction;
}

/* Grade a point */
NumericalScalar ClassifierImplementation::grade(const NumericalPoint & inP, const UnsignedInteger hClass) const
{
  throw NotYetImplementedException(HERE) << "In ClassifierImplementation::grade(const NumericalPoint & inP, const UnsignedInteger hClass) const";
}

/* Grade a sample */
NumericalPoint ClassifierImplementation::grade(const NumericalSample & inS, const Indices & hClass) const
{
  const UnsignedInteger size(inS.getSize());
  if ( size != hClass.getSize() )
    throw InvalidDimensionException(HERE) << "Input sample dimension (=" << size << ") and classes dimension (=" << hClass.getSize() << ") do not match.";
  NumericalPoint grades(size);
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    grades[i] = grade(inS[i], hClass[i]);
  }
  return grades;
}

/* String converter */
String ClassifierImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " x=" << inputSample_
         << " y=" << classes_
         << " verbose=" << verbose_ ;
}


String ClassifierImplementation::__str__(const String & offset) const
{
  return __repr__();
}

/* Verbosity accessor */
void ClassifierImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

Bool ClassifierImplementation::getVerbose() const
{
  return verbose_;
}

/* Input space dimension accessor */
UnsignedInteger ClassifierImplementation::getDimension() const
{
  return inputSample_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void ClassifierImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "classes_", classes_ );
  adv.saveAttribute( "verbose_", verbose_ );
}


/* Method load() reloads the object from the StorageManager */
void ClassifierImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "classes_", classes_ );
  adv.loadAttribute( "verbose_", verbose_ );
}


END_NAMESPACE_OPENTURNS
