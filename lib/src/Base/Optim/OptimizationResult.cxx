//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult stores the result of a OptimizationSolverImplementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OptimizationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationResult);

static const Factory<OptimizationResult> Factory_OptimizationResult;

/* Default constructor */
OptimizationResult::OptimizationResult()
  : PersistentObject()
  , optimalPoint_(0)
  , optimalValue_(0)
  , iterationNumber_(0)
  , absoluteError_(-1.0)
  , relativeError_(-1.0)
  , residualError_(-1.0)
  , constraintError_(-1.0)
  , lagrangeMultipliers_(0)
  , absoluteErrorHistory_()
  , relativeErrorHistory_()
  , residualErrorHistory_()
  , constraintErrorHistory_()
  , inputHistory_()
  , outputHistory_()
  , problem_()
{
  // Nothing to do
}

/* Standard constructor */
OptimizationResult::OptimizationResult(const NumericalPoint & optimalPoint,
                                       const NumericalPoint &  optimalValue,
                                       const UnsignedInteger iterationNumber,
                                       const NumericalScalar absoluteError,
                                       const NumericalScalar relativeError,
                                       const NumericalScalar residualError,
                                       const NumericalScalar constraintError,
                                       const OptimizationProblem & problem)
  : PersistentObject()
  , optimalPoint_(optimalPoint)
  , optimalValue_(optimalValue)
  , iterationNumber_(iterationNumber)
  , absoluteError_(absoluteError)
  , relativeError_(relativeError)
  , residualError_(residualError)
  , constraintError_(constraintError)
  , lagrangeMultipliers_(0)
  , absoluteErrorHistory_()
  , relativeErrorHistory_()
  , residualErrorHistory_()
  , constraintErrorHistory_()
  , inputHistory_()
  , outputHistory_()
  , problem_(problem)
{
  // Nothing to do
}

OptimizationResult::OptimizationResult(const NumericalPoint & optimalPoint,
                                       const NumericalPoint &  optimalValue,
                                       const UnsignedInteger iterationNumber,
                                       const NumericalScalar absoluteError,
                                       const NumericalScalar relativeError,
                                       const NumericalScalar residualError,
                                       const NumericalScalar constraintError,
                                       const OptimizationProblem & problem,
                                       const NumericalPoint & lagrangeMultipliers)
  : PersistentObject()
  , optimalPoint_(optimalPoint)
  , optimalValue_(optimalValue)
  , iterationNumber_(iterationNumber)
  , absoluteError_(absoluteError)
  , relativeError_(relativeError)
  , residualError_(residualError)
  , constraintError_(constraintError)
  , lagrangeMultipliers_(lagrangeMultipliers)
  , absoluteErrorHistory_()
  , relativeErrorHistory_()
  , residualErrorHistory_()
  , constraintErrorHistory_()
  , inputHistory_()
  , outputHistory_()
  , problem_(problem)
{
  // Nothing to do
}

/* Virtual constructor */
OptimizationResult * OptimizationResult::clone() const
{
  return new OptimizationResult(*this);
}

/* OptimalPoint accessors */
NumericalPoint OptimizationResult::getOptimalPoint() const
{
  return optimalPoint_;
}

void OptimizationResult::setOptimalPoint(const NumericalPoint & optimalPoint)
{
  optimalPoint_ = optimalPoint;
}

/* Optimal value accessors */
NumericalPoint OptimizationResult::getOptimalValue() const
{
  return optimalValue_;
}

void OptimizationResult::setOptimalValue(const NumericalPoint &  optimalValue)
{
  optimalValue_ = optimalValue;
}

/* Iteration number accessor */
UnsignedInteger OptimizationResult::getIterationNumber() const
{
  return iterationNumber_;
}

void OptimizationResult::setIterationNumber(const UnsignedInteger iterationNumber)
{
  iterationNumber_ = iterationNumber;
}

/* Absolute error accessor */
NumericalScalar OptimizationResult::getAbsoluteError() const
{
  return absoluteError_;
}

NumericalSample OptimizationResult::getAbsoluteErrorHistory() const
{
  return absoluteErrorHistory_.getSample();
}

/* Absolute error accessor */
void OptimizationResult::setAbsoluteError(const NumericalScalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
NumericalScalar OptimizationResult::getRelativeError() const
{
  return relativeError_;
}

NumericalSample OptimizationResult::getRelativeErrorHistory() const
{
  return relativeErrorHistory_.getSample();
}

/* Relative error accessor */
void OptimizationResult::setRelativeError(const NumericalScalar relativeError)
{
  relativeError_ = relativeError;
}

/* Residual error accessor */
NumericalScalar OptimizationResult::getResidualError() const
{
  return residualError_;
}

NumericalSample OptimizationResult::getResidualErrorHistory() const
{
  return residualErrorHistory_.getSample();
}

/* Residual error accessor */
void OptimizationResult::setResidualError(const NumericalScalar residualError)
{
  residualError_ = residualError;
}

/* Constraint error accessor */
NumericalScalar OptimizationResult::getConstraintError() const
{
  return constraintError_;
}

NumericalSample OptimizationResult::getConstraintErrorHistory() const
{
  return constraintErrorHistory_.getSample();
}

/* Constraint error accessor */
void OptimizationResult::setConstraintError(const NumericalScalar constraintError)
{
  constraintError_ = constraintError;
}

NumericalSample OptimizationResult::getInputSample() const
{
  return inputHistory_.getSample();
}

NumericalSample OptimizationResult::getOutputSample() const
{
  return outputHistory_.getSample();
}

OptimizationProblem OptimizationResult::getProblem() const
{
  return problem_;
}

/* Lagrange multipliers accessor */
void OptimizationResult::setLagrangeMultipliers(const NumericalPoint & lagrangeMultipliers)
{
  lagrangeMultipliers_ = lagrangeMultipliers;
}

NumericalPoint OptimizationResult::getLagrangeMultipliers() const
{
  return lagrangeMultipliers_;
}

/* String converter */
String OptimizationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationResult::GetClassName()
      << " optimal point=" << optimalPoint_
      << " optimal value=" << optimalValue_
      << " iterationNumber=" << iterationNumber_
      << " absoluteError=" << getAbsoluteError()
      << " relativeError=" << getRelativeError()
      << " residualError=" << getResidualError()
      << " constraintError=" << getConstraintError()
      << " lagrangeMultipliers=" << lagrangeMultipliers_
      << " problem=" << problem_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimizationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "optimalPoint_", optimalPoint_ );
  adv.saveAttribute( "optimalValue_", optimalValue_ );
  adv.saveAttribute( "iterationNumber_", iterationNumber_ );
  adv.saveAttribute( "absoluteError_", absoluteError_ );
  adv.saveAttribute( "relativeError_", relativeError_ );
  adv.saveAttribute( "residualError_", residualError_ );
  adv.saveAttribute( "constraintError_", constraintError_ );
  adv.saveAttribute( "lagrangeMultipliers_", lagrangeMultipliers_ );

  adv.saveAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.saveAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.saveAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.saveAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.saveAttribute( "inputHistory_", inputHistory_ );
  adv.saveAttribute( "outputHistory_", outputHistory_ );

  adv.saveAttribute( "problem_", problem_ );
}

/* Method load() reloads the object from the StorageManager */
void OptimizationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "optimalPoint_", optimalPoint_ );
  adv.loadAttribute( "optimalValue_", optimalValue_ );
  adv.loadAttribute( "iterationNumber_", iterationNumber_ );
  adv.loadAttribute( "absoluteError_", absoluteError_ );
  adv.loadAttribute( "relativeError_", relativeError_ );
  adv.loadAttribute( "residualError_", residualError_ );
  adv.loadAttribute( "constraintError_", constraintError_ );
  adv.loadAttribute( "lagrangeMultipliers_", lagrangeMultipliers_ );

  adv.loadAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.loadAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.loadAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.loadAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.loadAttribute( "inputHistory_", inputHistory_ );
  adv.loadAttribute( "outputHistory_", outputHistory_ );

  adv.loadAttribute( "problem_", problem_ );
}

/* Update current state */
void OptimizationResult::update(const NumericalPoint & OptimalPoint, UnsignedInteger iterationNumber)
{
  setOptimalPoint(OptimalPoint);
  setIterationNumber(iterationNumber);
}

/* Incremental history storage */
void OptimizationResult::store(const NumericalPoint & x,
                               const NumericalPoint & y,
                               const NumericalScalar absoluteError,
                               const NumericalScalar relativeError,
                               const NumericalScalar residualError,
                               const NumericalScalar constraintError)
{
  store(x, y, absoluteError, relativeError, residualError, constraintError, NumericalPoint(0));
}

void OptimizationResult::store(const NumericalPoint & x,
                               const NumericalPoint & y,
                               const NumericalScalar absoluteError,
                               const NumericalScalar relativeError,
                               const NumericalScalar residualError,
                               const NumericalScalar constraintError,
                               const NumericalPoint & lagrangeMultipliers)
{
  // assume the last point stored is the optimum
  optimalPoint_ = x;
  optimalValue_ = y;
  lagrangeMultipliers_ = lagrangeMultipliers;
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

Graph OptimizationResult::drawErrorHistory() const
{
  Graph result("Error history", "Iteration number", "Error value", true, "topright", 1.0, GraphImplementation::LOGY);
  result.setGrid(true);
  result.setGridColor("black");
// create a sample with the iteration number to be plotted as x data
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

