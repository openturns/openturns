//                                               -*- C++ -*-
/**
 *  @brief NearestPointAlgorithmImplementation implements an algorithm for finding the
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

#include "NearestPointAlgorithmImplementation.hxx"
#include "ResourceMap.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NearestPointAlgorithmImplementation);

static Factory<NearestPointAlgorithmImplementation> RegisteredFactory("NearestPointAlgorithmImplementation");

/* Default constructor */
NearestPointAlgorithmImplementation::NearestPointAlgorithmImplementation() :
  PersistentObject(),
  levelFunction_(NumericalMathFunction()),
  startingPoint_(NumericalPoint(0)),
  levelValue_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultLevelValue" )),
  maximumIterationsNumber_(ResourceMap::GetAsUnsignedInteger( "NearestPointAlgorithmImplementation-DefaultMaximumIteration" )),
  maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumAbsoluteError" )),
  maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumRelativeError" )),
  maximumResidualError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumResidualError" )),
  maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumConstraintError" )),
  verbose_(false)
{
  result_ = Result(startingPoint_, 0, -1.0, -1.0, -1.0, -1.0);
}

/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued function)
 *         and a level value
 */
NearestPointAlgorithmImplementation::NearestPointAlgorithmImplementation(const NumericalMathFunction & levelFunction,
    const Bool verbose):
  PersistentObject(),
  levelFunction_(levelFunction),
  startingPoint_(NumericalPoint(levelFunction.getInputDimension(), 0.0)),
  levelValue_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultLevelValue" )),
  maximumIterationsNumber_(ResourceMap::GetAsUnsignedInteger( "NearestPointAlgorithmImplementation-DefaultMaximumIteration" )),
  maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumAbsoluteError" )),
  maximumRelativeError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumRelativeError" )),
  maximumResidualError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumResidualError" )),
  maximumConstraintError_(ResourceMap::GetAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumConstraintError" )),
  verbose_(verbose)
{
  result_ = Result(startingPoint_, 0, -1.0, -1.0, -1.0, -1.0);
}

/* Starting point accessor */
NumericalPoint NearestPointAlgorithmImplementation::getStartingPoint() const
{
  return startingPoint_;
}

/* Starting point accessor */
void NearestPointAlgorithmImplementation::setStartingPoint(const NumericalPoint & startingPoint)
{
  startingPoint_ = startingPoint;
}

/* Level value accessor */
NumericalScalar NearestPointAlgorithmImplementation::getLevelValue() const
{
  return levelValue_;
}

/* Level value accessor */
void NearestPointAlgorithmImplementation::setLevelValue(const NumericalScalar levelValue)
{
  levelValue_ = levelValue;
}

/* Result accessor */
NearestPointAlgorithmImplementation::Result NearestPointAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Result accessor */
void NearestPointAlgorithmImplementation::setResult(const Result & result)
{
  result_ = result;
}

/* Maximum iterations number accessor */
UnsignedInteger NearestPointAlgorithmImplementation::getMaximumIterationsNumber() const
{
  return maximumIterationsNumber_;
}

/* Maximum iterations number accessor */
void NearestPointAlgorithmImplementation::setMaximumIterationsNumber(const UnsignedInteger maximumIterationsNumber)
{
  maximumIterationsNumber_ = maximumIterationsNumber;
}

/* Maximum absolute error accessor */
NumericalScalar NearestPointAlgorithmImplementation::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

/* Maximum absolute error accessor */
void NearestPointAlgorithmImplementation::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* Maximum relative error accessor */
NumericalScalar NearestPointAlgorithmImplementation::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

/* Maximum relative error accessor */
void NearestPointAlgorithmImplementation::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

/* Maximum residual error accessor */
NumericalScalar NearestPointAlgorithmImplementation::getMaximumResidualError() const
{
  return maximumResidualError_;
}

/* Maximum residual error accessor */
void NearestPointAlgorithmImplementation::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}

/* Maximum constraint error accessor */
NumericalScalar NearestPointAlgorithmImplementation::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}

/* Maximum constraint error accessor */
void NearestPointAlgorithmImplementation::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}

/* String converter */
String NearestPointAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << NearestPointAlgorithmImplementation::GetClassName()
      << " startingPoint=" << startingPoint_
      << " levelFunction=" << levelFunction_
      << " levelValue=" << levelValue_
      << " maximumIterationsNumber=" << maximumIterationsNumber_
      << " maximumAbsoluteError=" << maximumAbsoluteError_
      << " maximumRelativeError=" << maximumRelativeError_
      << " maximumResidualError=" << maximumResidualError_
      << " maximumConstraintError=" << maximumConstraintError_
      << " verbose=" << (verbose_ ? "true" : "false");
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NearestPointAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("levelFunction_", levelFunction_);
  adv.saveAttribute("startingPoint_", startingPoint_);
  adv.saveAttribute("levelValue_", levelValue_);
  adv.saveAttribute("maximumIterationsNumber_", maximumIterationsNumber_);
  adv.saveAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute("maximumResidualError_", maximumResidualError_);
  adv.saveAttribute("maximumConstraintError_", maximumConstraintError_);
  adv.saveAttribute("verbose_", verbose_);
}

/* Method load() reloads the object from the StorageManager */
void NearestPointAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("levelFunction_", levelFunction_);
  adv.loadAttribute("startingPoint_", startingPoint_);
  adv.loadAttribute("levelValue_", levelValue_);
  adv.loadAttribute("maximumIterationsNumber_", maximumIterationsNumber_);
  adv.loadAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.loadAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.loadAttribute("maximumResidualError_", maximumResidualError_);
  adv.loadAttribute("maximumConstraintError_", maximumConstraintError_);
  adv.loadAttribute("verbose_", verbose_);
}

/* Level function accessor */
NumericalMathFunction NearestPointAlgorithmImplementation::getLevelFunction() const
{
  return levelFunction_;
}

/* Level function accessor */
void NearestPointAlgorithmImplementation::setLevelFunction(const NumericalMathFunction & levelFunction)
{
  levelFunction_ = levelFunction;
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void NearestPointAlgorithmImplementation::run()
{
  // To be overloaded
}

/* Virtual constructor */
NearestPointAlgorithmImplementation * NearestPointAlgorithmImplementation::clone() const
{
  return new NearestPointAlgorithmImplementation(*this);
}

/* Verbose accessor */
Bool NearestPointAlgorithmImplementation::getVerbose() const
{
  return verbose_;
}

/* Verbose accessor */
void NearestPointAlgorithmImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}


END_NAMESPACE_OPENTURNS
