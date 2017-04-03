//                                               -*- C++ -*-
/**
 *  @brief Classification algorithm interface
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/Classifier.hxx"
#include "openturns/ClassifierImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Classifier);

/* Constructor from implementation */
Classifier::Classifier()
  : TypedInterfaceObject<ClassifierImplementation>( new ClassifierImplementation() )
{
  // Nothing to do
}

/* Constructor from implementation */
Classifier::Classifier(const ClassifierImplementation & implementation)
  : TypedInterfaceObject<ClassifierImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Constructor from implementation */
Classifier::Classifier(const Implementation & p_implementation)
  : TypedInterfaceObject<ClassifierImplementation>( p_implementation )
{
  // Nothing to do
}

/* Constructor from implementation */
Classifier::Classifier(const Sample & inS, const Indices & outC)
  : TypedInterfaceObject<ClassifierImplementation>( new ClassifierImplementation(inS, outC) )
{
  // Nothing to do
}

/* Classify a point */
UnsignedInteger Classifier::classify(const NumericalPoint & inP) const
{
  return getImplementation()->classify(inP);
}

/* Classify a sample */
Indices Classifier::classify(const Sample & inS) const
{
  return getImplementation()->classify(inS);
}

/* Grade a point */
NumericalScalar Classifier::grade(const NumericalPoint & inP, UnsignedInteger hClass) const
{
  return getImplementation()->grade(inP, hClass);
}

/* Grade a sample */
NumericalPoint Classifier::grade(const Sample & inS, const Indices & hClass) const
{
  return getImplementation()->grade(inS, hClass);
}

/* Verbosity accessor */
void Classifier::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

Bool Classifier::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/** Parallelization flag accessor */
void Classifier::setParallel(const Bool flag)
{
  copyOnWrite();
  getImplementation()->setParallel(flag);
}

Bool Classifier::isParallel() const
{
  return getImplementation()->isParallel();
}

/* Input space dimension */
UnsignedInteger Classifier::getDimension() const
{
  return getImplementation()->getDimension();
}

/* String converter */
String Classifier::__repr__() const
{
  return getImplementation()->__repr__();
}

String Classifier::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}


END_NAMESPACE_OPENTURNS
