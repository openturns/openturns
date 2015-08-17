//                                               -*- C++ -*-
/**
 *  @brief NearestPointAlgorithm implements an algorithm for finding the
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
#include "NearestPointAlgorithm.hxx"
#include "CobylaObsolete.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NearestPointAlgorithm);

/* Default constructor */
NearestPointAlgorithm::NearestPointAlgorithm() :
  TypedInterfaceObject<NearestPointAlgorithmImplementation>(new NearestPointAlgorithmImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
NearestPointAlgorithm::NearestPointAlgorithm(const NearestPointAlgorithmImplementation & implementation) :
  TypedInterfaceObject<NearestPointAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
NearestPointAlgorithm::NearestPointAlgorithm(const Implementation & p_implementation) :
  TypedInterfaceObject<NearestPointAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued function)
 *         and a level value
 */
NearestPointAlgorithm::NearestPointAlgorithm(const NumericalMathFunction & levelFunction):
  TypedInterfaceObject<NearestPointAlgorithmImplementation>(new CobylaObsolete(levelFunction))
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint NearestPointAlgorithm::getStartingPoint() const
{
  return getImplementation()->getStartingPoint();
}

/* Starting point accessor */
void NearestPointAlgorithm::setStartingPoint(const NumericalPoint & startingPoint)
{
  copyOnWrite();
  getImplementation()->setStartingPoint(startingPoint);
}

/* Level value accessor */
NumericalScalar NearestPointAlgorithm::getLevelValue() const
{
  return getImplementation()->getLevelValue();
}

/* Level value accessor */
void NearestPointAlgorithm::setLevelValue(const NumericalScalar levelValue)
{
  copyOnWrite();
  getImplementation()->setLevelValue(levelValue);
}

/* Result accessor */
NearestPointAlgorithm::Result NearestPointAlgorithm::getResult() const
{
  return getImplementation()->getResult();
}

/* Result accessor */
void NearestPointAlgorithm::setResult(const Result & result)
{
  copyOnWrite();
  getImplementation()->setResult(result);
}

/* Maximum iterations number accessor */
UnsignedInteger NearestPointAlgorithm::getMaximumIterationsNumber() const
{
  return getImplementation()->getMaximumIterationsNumber();
}

/* Maximum iterations number accessor */
void NearestPointAlgorithm::setMaximumIterationsNumber(const UnsignedInteger maximumIterationsNumber)
{
  copyOnWrite();
  getImplementation()->setMaximumIterationsNumber(maximumIterationsNumber);
}

/* Maximum absolute error accessor */
NumericalScalar NearestPointAlgorithm::getMaximumAbsoluteError() const
{
  return getImplementation()->getMaximumAbsoluteError();
}

/* Maximum absolute error accessor */
void NearestPointAlgorithm::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  copyOnWrite();
  getImplementation()->setMaximumAbsoluteError(maximumAbsoluteError);
}

/* Maximum relative error accessor */
NumericalScalar NearestPointAlgorithm::getMaximumRelativeError() const
{
  return getImplementation()->getMaximumRelativeError();
}

/* Maximum relative error accessor */
void NearestPointAlgorithm::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  copyOnWrite();
  getImplementation()->setMaximumRelativeError(maximumRelativeError);
}

/* Maximum residual error accessor */
NumericalScalar NearestPointAlgorithm::getMaximumResidualError() const
{
  return getImplementation()->getMaximumResidualError();
}

/* Maximum residual error accessor */
void NearestPointAlgorithm::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  copyOnWrite();
  getImplementation()->setMaximumResidualError(maximumResidualError);
}

/* Maximum constraint error accessor */
NumericalScalar NearestPointAlgorithm::getMaximumConstraintError() const
{
  return getImplementation()->getMaximumConstraintError();
}

/* Maximum constraint error accessor */
void NearestPointAlgorithm::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  copyOnWrite();
  getImplementation()->setMaximumConstraintError(maximumConstraintError);
}

/* String converter */
String NearestPointAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NearestPointAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String NearestPointAlgorithm::__str__(const String & offset) const
{
  return __repr__();
}

/* Level function accessor */
NumericalMathFunction NearestPointAlgorithm::getLevelFunction() const
{
  return getImplementation()->getLevelFunction();
}

/* Level function accessor */
void NearestPointAlgorithm::setLevelFunction(const NumericalMathFunction & levelFunction)
{
  copyOnWrite();
  getImplementation()->setLevelFunction(levelFunction);
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void NearestPointAlgorithm::run()
{
  getImplementation()->run();
}

/* Verbose accessor */
Bool NearestPointAlgorithm::getVerbose() const
{
  return getImplementation()->getVerbose();
}

/* Verbose accessor */
void NearestPointAlgorithm::setVerbose(const Bool verbose)
{
  copyOnWrite();
  getImplementation()->setVerbose(verbose);
}

END_NAMESPACE_OPENTURNS
