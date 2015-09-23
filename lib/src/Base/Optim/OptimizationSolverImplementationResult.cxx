//                                               -*- C++ -*-
/**
 *  @brief OptimizationSolverImplementationResult stores the result of a OptimizationSolverImplementation
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
#include "OptimizationSolverImplementationResult.hxx"
#include "PersistentObjectFactory.hxx"
#include "Curve.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OptimizationSolverImplementationResult);

static const Factory<OptimizationSolverImplementationResult> RegisteredFactory;

/* Default constructor */
OptimizationSolverImplementationResult::OptimizationSolverImplementationResult()
  : PersistentObject()
  , optimalPoint_(NumericalPoint(0))
  , optimalValue_(NumericalPoint(0))
  , iterationsNumber_(0)
  , absoluteError_(-1.0)
  , relativeError_(-1.0)
  , residualError_(-1.0)
  , constraintError_(-1.0)
  , absoluteErrorHistory_()
  , relativeErrorHistory_()
  , residualErrorHistory_()
  , constraintErrorHistory_()
  , inputHistory_()
  , outputHistory_()
{
  // Nothing to do
}

/* Standard constructor */
OptimizationSolverImplementationResult::OptimizationSolverImplementationResult(const NumericalPoint & optimalPoint,
    const NumericalPoint &  optimalValue,
    const UnsignedInteger iterationsNumber,
    const NumericalScalar absoluteError,
    const NumericalScalar relativeError,
    const NumericalScalar residualError,
    const NumericalScalar constraintError)
  : PersistentObject()
  , optimalPoint_(optimalPoint)
  , optimalValue_(optimalValue)
  , iterationsNumber_(iterationsNumber)
  , absoluteError_(absoluteError)
  , relativeError_(relativeError)
  , residualError_(residualError)
  , constraintError_(constraintError)
  , absoluteErrorHistory_()
  , relativeErrorHistory_()
  , residualErrorHistory_()
  , constraintErrorHistory_()
  , inputHistory_()
  , outputHistory_()
{
  // Nothing to do
}

/* Virtual constructor */
OptimizationSolverImplementationResult * OptimizationSolverImplementationResult::clone() const
{
  return new OptimizationSolverImplementationResult(*this);
}

/* OptimalPoint accessors */
NumericalPoint OptimizationSolverImplementationResult::getOptimalPoint() const
{
  return optimalPoint_;
}

void OptimizationSolverImplementationResult::setOptimalPoint(const NumericalPoint & optimalPoint)
{
  optimalPoint_ = optimalPoint;
}

/* Optimal value accessors */
NumericalPoint OptimizationSolverImplementationResult::getOptimalValue() const
{
  return optimalValue_;
}

void OptimizationSolverImplementationResult::setOptimalValue(const NumericalPoint &  optimalValue)
{
  optimalValue_ = optimalValue;
}

/* Iterations number accessor */
UnsignedInteger OptimizationSolverImplementationResult::getIterationsNumber() const
{
  return iterationsNumber_;
}

void OptimizationSolverImplementationResult::setIterationsNumber(const UnsignedInteger iterationsNumber)
{
  iterationsNumber_ = iterationsNumber;
}

/* Absolute error accessor */
NumericalScalar OptimizationSolverImplementationResult::getAbsoluteError() const
{
  return absoluteError_;
}

NumericalSample OptimizationSolverImplementationResult::getAbsoluteErrorHistory() const
{
  return absoluteErrorHistory_.getSample();
}

/* Absolute error accessor */
void OptimizationSolverImplementationResult::setAbsoluteError(const NumericalScalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
NumericalScalar OptimizationSolverImplementationResult::getRelativeError() const
{
  return relativeError_;
}

NumericalSample OptimizationSolverImplementationResult::getRelativeErrorHistory() const
{
  return relativeErrorHistory_.getSample();
}

/* Relative error accessor */
void OptimizationSolverImplementationResult::setRelativeError(const NumericalScalar relativeError)
{
  relativeError_ = relativeError;
}

/* Residual error accessor */
NumericalScalar OptimizationSolverImplementationResult::getResidualError() const
{
  return residualError_;
}

NumericalSample OptimizationSolverImplementationResult::getResidualErrorHistory() const
{
  return residualErrorHistory_.getSample();
}

/* Residual error accessor */
void OptimizationSolverImplementationResult::setResidualError(const NumericalScalar residualError)
{
  residualError_ = residualError;
}

/* Constraint error accessor */
NumericalScalar OptimizationSolverImplementationResult::getConstraintError() const
{
  return constraintError_;
}

NumericalSample OptimizationSolverImplementationResult::getConstraintErrorHistory() const
{
  return constraintErrorHistory_.getSample();
}

/* Constraint error accessor */
void OptimizationSolverImplementationResult::setConstraintError(const NumericalScalar constraintError)
{
  constraintError_ = constraintError;
}

NumericalSample OptimizationSolverImplementationResult::getInputSample() const
{
  return inputHistory_.getSample();
}

NumericalSample OptimizationSolverImplementationResult::getOutputSample() const
{
  return outputHistory_.getSample();
}

/* String converter */
String OptimizationSolverImplementationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationSolverImplementationResult::GetClassName()
      << " optimal point=" << optimalPoint_
      << " optimal value="        << optimalValue_
      << " iterationsNumber=" << iterationsNumber_
      << " absoluteError=" << getAbsoluteError()
      << " relativeError=" << getRelativeError()
      << " residualError=" << getResidualError()
      << " constraintError=" << getConstraintError();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimizationSolverImplementationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "optimalPoint_", optimalPoint_ );
  adv.saveAttribute( "optimalValue_", optimalValue_ );
  adv.saveAttribute( "iterationsNumber_", iterationsNumber_ );
  adv.saveAttribute( "absoluteError_", absoluteError_ );
  adv.saveAttribute( "relativeError_", relativeError_ );
  adv.saveAttribute( "residualError_", residualError_ );
  adv.saveAttribute( "constraintError_", constraintError_ );

  adv.saveAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.saveAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.saveAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.saveAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.saveAttribute( "inputHistory_", inputHistory_ );
  adv.saveAttribute( "outputHistory_", outputHistory_ );
}

/* Method load() reloads the object from the StorageManager */
void OptimizationSolverImplementationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "optimalPoint_", optimalPoint_ );
  adv.loadAttribute( "optimalValue_", optimalValue_ );
  adv.loadAttribute( "iterationsNumber_", iterationsNumber_ );
  adv.loadAttribute( "absoluteError_", absoluteError_ );
  adv.loadAttribute( "relativeError_", relativeError_ );
  adv.loadAttribute( "residualError_", residualError_ );
  adv.loadAttribute( "constraintError_", constraintError_ );

  adv.loadAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.loadAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.loadAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.loadAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.loadAttribute( "inputHistory_", inputHistory_ );
  adv.loadAttribute( "outputHistory_", outputHistory_ );
}

/* Update current state */
void OptimizationSolverImplementationResult::update(const NumericalPoint & OptimalPoint, UnsignedInteger iterationNumber)
{
  setOptimalPoint( OptimalPoint );
  setIterationsNumber( iterationNumber );
}

/* Incremental history storage */
void OptimizationSolverImplementationResult::store(const NumericalPoint & x,
    const NumericalPoint & y,
    const NumericalScalar absoluteError,
    const NumericalScalar relativeError,
    const NumericalScalar residualError,
    const NumericalScalar constraintError)
{
  // assume the last point stored is the optimum
  optimalPoint_ = x;
  optimalValue_ = y;

  // update values
  absoluteError_ = absoluteError;
  relativeError_ = relativeError;
  residualError_ = residualError;
  constraintError_ = constraintError;

  // append values
  absoluteErrorHistory_.store(NumericalPoint(1, absoluteError));
  relativeErrorHistory_.store(NumericalPoint(1, relativeError));
  residualErrorHistory_.store(NumericalPoint(1, residualError));
  constraintErrorHistory_.store(NumericalPoint(1, constraintError));

  inputHistory_.store(x);
  outputHistory_.store(y);
}

Graph OptimizationSolverImplementationResult::drawErrorHistory() const
{
  Graph result("Error history", "Iteration number", "Error value", true, "topright", 1.0, GraphImplementation::LOGY);
  result.setGrid(true);
  result.setGridColor("black");
  // create a sample with the iterations number to be plotted as x data
  const UnsignedInteger size(getAbsoluteErrorHistory().getSize());
  {
    NumericalSample data(getAbsoluteErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data[i][0] <= 0.0) data[i][0] = SpecFunc::NumericalScalarEpsilon;
    Curve absoluteErrorCurve( data, "absolute error" );
    absoluteErrorCurve.setLegend("absolute error");
    absoluteErrorCurve.setColor("red");
    result.add( absoluteErrorCurve );
  }
  // Relative error
  {
    NumericalSample data(getRelativeErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data[i][0] <= 0.0) data[i][0] = SpecFunc::NumericalScalarEpsilon;
    Curve relativeErrorCurve( data, "relative error" );
    relativeErrorCurve.setLegend("relative error");
    relativeErrorCurve.setColor("blue");
    result.add( relativeErrorCurve );
  }
  // Residual error
  {
    NumericalSample data(getResidualErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data[i][0] <= 0.0) data[i][0] = SpecFunc::NumericalScalarEpsilon;
    Curve residualErrorCurve( data, "residual error" );
    residualErrorCurve.setLegend("residual error");
    residualErrorCurve.setColor("green");
    result.add( residualErrorCurve );
  }
  // Constraint error
  {
    NumericalSample data(getConstraintErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data[i][0] <= 0.0) data[i][0] = SpecFunc::NumericalScalarEpsilon;
    Curve constraintErrorCurve( data, "constraint error" );
    constraintErrorCurve.setLegend("constraint error");
    constraintErrorCurve.setColor("magenta");
    result.add( constraintErrorCurve );
  }

  return result;
}


END_NAMESPACE_OPENTURNS
