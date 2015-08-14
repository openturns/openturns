//                                               -*- C++ -*-
/**
 *  @brief BoundConstrainedAlgorithmImplementationResult stores the result of a BoundConstrainedAlgorithmImplementation algorithm
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
#include "BoundConstrainedAlgorithmImplementationResult.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BoundConstrainedAlgorithmImplementationResult);

static Factory<BoundConstrainedAlgorithmImplementationResult> RegisteredFactory("BoundConstrainedAlgorithmImplementationResult");

/* Default constructor */
BoundConstrainedAlgorithmImplementationResult::BoundConstrainedAlgorithmImplementationResult():
  PersistentObject(),
  optimizer_(NumericalPoint(1)),
  optimalValue_(0.0),
  optimization_(MINIMIZATION),
  evaluationsNumber_(0),
  absoluteError_(0.0),
  relativeError_(0.0),
  objectiveError_(0.0),
  constraintError_(0.0)
{
  // Nothing to do
}

/* Standard constructor */
BoundConstrainedAlgorithmImplementationResult::BoundConstrainedAlgorithmImplementationResult(const NumericalPoint & optimizer,
    const NumericalScalar optimalValue,
    const OptimizationProblem optimization,
    const UnsignedInteger evaluationsNumber,
    const NumericalScalar absoluteError,
    const NumericalScalar relativeError,
    const NumericalScalar objectiveError,
    const NumericalScalar constraintError):
  PersistentObject(),
  optimizer_(optimizer),
  optimalValue_(optimalValue),
  optimization_(optimization),
  evaluationsNumber_(evaluationsNumber),
  absoluteError_(absoluteError),
  relativeError_(relativeError),
  objectiveError_(objectiveError),
  constraintError_(constraintError)
{
  // Nothing to do
}

/* Virtual constructor */
BoundConstrainedAlgorithmImplementationResult * BoundConstrainedAlgorithmImplementationResult::clone() const
{
  return new BoundConstrainedAlgorithmImplementationResult(*this);
}

/* Optimizer accessors */
NumericalPoint BoundConstrainedAlgorithmImplementationResult::getOptimizer() const
{
  return optimizer_;
}

void BoundConstrainedAlgorithmImplementationResult::setOptimizer(const NumericalPoint & optimizer)
{
  optimizer_ = optimizer;
}

/* Optimal value accessors */
NumericalScalar BoundConstrainedAlgorithmImplementationResult::getOptimalValue() const
{
  return optimalValue_;
}

void BoundConstrainedAlgorithmImplementationResult::setOptimalValue(const NumericalScalar optimalValue)
{
  optimalValue_ = optimalValue;
}

/* Optimization problem accessors */
BoundConstrainedAlgorithmImplementationResult::OptimizationProblem BoundConstrainedAlgorithmImplementationResult::getOptimizationProblem() const
{
  return optimization_;
}

void BoundConstrainedAlgorithmImplementationResult::setOptimizationProblem(const OptimizationProblem optimization)
{
  optimization_ = optimization;
}

/* Iterations number accessor */
UnsignedInteger BoundConstrainedAlgorithmImplementationResult::getEvaluationsNumber() const
{
  return evaluationsNumber_;
}

void BoundConstrainedAlgorithmImplementationResult::setEvaluationsNumber(const UnsignedInteger evaluationsNumber)
{
  evaluationsNumber_ = evaluationsNumber;
}

/* Absolute error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementationResult::getAbsoluteError() const
{
  return absoluteError_;
}

void BoundConstrainedAlgorithmImplementationResult::setAbsoluteError(const NumericalScalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementationResult::getRelativeError() const
{
  return relativeError_;
}

void BoundConstrainedAlgorithmImplementationResult::setRelativeError(const NumericalScalar relativeError)
{
  relativeError_ = relativeError;
}

/* Objective error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementationResult::getObjectiveError() const
{
  return objectiveError_;
}

void BoundConstrainedAlgorithmImplementationResult::setObjectiveError(const NumericalScalar objectiveError)
{
  objectiveError_ = objectiveError;
}

/* Constraint error accessor */
NumericalScalar BoundConstrainedAlgorithmImplementationResult::getConstraintError() const
{
  return constraintError_;
}

void BoundConstrainedAlgorithmImplementationResult::setConstraintError(const NumericalScalar constraintError)
{
  constraintError_ = constraintError;
}

/* String converter */
String BoundConstrainedAlgorithmImplementationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << BoundConstrainedAlgorithmImplementationResult::GetClassName()
      << " optimizer="            << optimizer_
      << " optimal value="        << optimalValue_
      << " optimization problem=" << optimization_
      << " evaluationsNumber="    << evaluationsNumber_
      << " absoluteError="        << absoluteError_
      << " relativeError="        << relativeError_
      << " objectiveError="       << objectiveError_
      << " constraintError="      << constraintError_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void BoundConstrainedAlgorithmImplementationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "optimizer_", optimizer_ );
  adv.saveAttribute( "optimalValue_", optimalValue_ );
  UnsignedInteger optimization = optimization_;
  adv.saveAttribute( "optimization_", optimization );
  adv.saveAttribute( "evaluationsNumber_", evaluationsNumber_ );
  adv.saveAttribute( "absoluteError_",     absoluteError_ );
  adv.saveAttribute( "relativeError_",     relativeError_ );
  adv.saveAttribute( "objectiveError_",    objectiveError_ );
  adv.saveAttribute( "constraintError_",   constraintError_ );
}


/* Method load() reloads the object from the StorageManager */
void BoundConstrainedAlgorithmImplementationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "optimizer_", optimizer_ );
  adv.loadAttribute( "optimalValue_", optimalValue_ );
  UnsignedInteger optimization;
  adv.loadAttribute( "optimization_", optimization );
  optimization_ = static_cast<OptimizationProblem>( optimization);
  adv.loadAttribute( "evaluationsNumber_", evaluationsNumber_ );
  adv.loadAttribute( "absoluteError_",     absoluteError_ );
  adv.loadAttribute( "relativeError_",     relativeError_ );
  adv.loadAttribute( "objectiveError_",    objectiveError_ );
  adv.loadAttribute( "constraintError_",   constraintError_ );
}


END_NAMESPACE_OPENTURNS
