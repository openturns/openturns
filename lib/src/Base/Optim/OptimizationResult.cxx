//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult stores the result of a OptimizationAlgorithmImplementation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OptimizationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationResult)

static const Factory<OptimizationResult> Factory_OptimizationResult;

/* Default constructor */
OptimizationResult::OptimizationResult()
  : PersistentObject()
  , optimalPoint_(0)
  , optimalValue_(0)
  , evaluationNumber_(0)
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
  absoluteErrorHistory_.setDimension(1);
  relativeErrorHistory_.setDimension(1);
  residualErrorHistory_.setDimension(1);
  constraintErrorHistory_.setDimension(1);
}

/* Default constructor */
OptimizationResult::OptimizationResult(const UnsignedInteger inputDimension, const UnsignedInteger outputDimension)
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
  absoluteErrorHistory_.setDimension(1);
  relativeErrorHistory_.setDimension(1);
  residualErrorHistory_.setDimension(1);
  constraintErrorHistory_.setDimension(1);
  inputHistory_.setDimension(inputDimension);
  outputHistory_.setDimension(outputDimension);
}

/* Standard constructor */
OptimizationResult::OptimizationResult(const Point & optimalPoint,
                                       const Point &  optimalValue,
                                       const UnsignedInteger evaluationNumber,
                                       const Scalar absoluteError,
                                       const Scalar relativeError,
                                       const Scalar residualError,
                                       const Scalar constraintError,
                                       const OptimizationProblem & problem)
  : PersistentObject()
  , optimalPoint_(optimalPoint)
  , optimalValue_(optimalValue)
  , evaluationNumber_(evaluationNumber)
  , iterationNumber_(0)
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
  absoluteErrorHistory_.setDimension(1);
  relativeErrorHistory_.setDimension(1);
  residualErrorHistory_.setDimension(1);
  constraintErrorHistory_.setDimension(1);
  inputHistory_.setDimension(optimalPoint.getDimension());
  outputHistory_.setDimension(optimalValue.getDimension());
}

/* Virtual constructor */
OptimizationResult * OptimizationResult::clone() const
{
  return new OptimizationResult(*this);
}

/* OptimalPoint accessors */
Point OptimizationResult::getOptimalPoint() const
{
  return optimalPoint_;
}

void OptimizationResult::setOptimalPoint(const Point & optimalPoint)
{
  optimalPoint_ = optimalPoint;
}

/* Optimal value accessors */
Point OptimizationResult::getOptimalValue() const
{
  return optimalValue_;
}

void OptimizationResult::setOptimalValue(const Point &  optimalValue)
{
  optimalValue_ = optimalValue;
}

/* Evaluation number accessor */
UnsignedInteger OptimizationResult::getEvaluationNumber() const
{
  return evaluationNumber_;
}

void OptimizationResult::setEvaluationNumber(const UnsignedInteger evaluationNumber)
{
  evaluationNumber_ = evaluationNumber;
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
Scalar OptimizationResult::getAbsoluteError() const
{
  return absoluteError_;
}

Sample OptimizationResult::getAbsoluteErrorHistory() const
{
  return absoluteErrorHistory_.getSample();
}

/* Absolute error accessor */
void OptimizationResult::setAbsoluteError(const Scalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
Scalar OptimizationResult::getRelativeError() const
{
  return relativeError_;
}

Sample OptimizationResult::getRelativeErrorHistory() const
{
  return relativeErrorHistory_.getSample();
}

/* Relative error accessor */
void OptimizationResult::setRelativeError(const Scalar relativeError)
{
  relativeError_ = relativeError;
}

/* Residual error accessor */
Scalar OptimizationResult::getResidualError() const
{
  return residualError_;
}

Sample OptimizationResult::getResidualErrorHistory() const
{
  return residualErrorHistory_.getSample();
}

/* Residual error accessor */
void OptimizationResult::setResidualError(const Scalar residualError)
{
  residualError_ = residualError;
}

/* Constraint error accessor */
Scalar OptimizationResult::getConstraintError() const
{
  return constraintError_;
}

Sample OptimizationResult::getConstraintErrorHistory() const
{
  return constraintErrorHistory_.getSample();
}

/* Constraint error accessor */
void OptimizationResult::setConstraintError(const Scalar constraintError)
{
  constraintError_ = constraintError;
}

Sample OptimizationResult::getInputSample() const
{
  return inputHistory_.getSample();
}

Sample OptimizationResult::getOutputSample() const
{
  return outputHistory_.getSample();
}

void OptimizationResult::setProblem(const OptimizationProblem & problem)
{
  problem_ = problem;
}

OptimizationProblem OptimizationResult::getProblem() const
{
  return problem_;
}

/* Lagrange multipliers accessor */
void OptimizationResult::setLagrangeMultipliers(const Point & lagrangeMultipliers)
{
  lagrangeMultipliers_ = lagrangeMultipliers;
}

Point OptimizationResult::getLagrangeMultipliers() const
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
      << " evaluationNumber=" << evaluationNumber_
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
  adv.saveAttribute( "evaluationNumber_", evaluationNumber_ );
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
  adv.loadAttribute( "evaluationNumber_", evaluationNumber_ );
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

/* Incremental history storage */
void OptimizationResult::store(const Point & x,
                               const Point & y,
                               const Scalar absoluteError,
                               const Scalar relativeError,
                               const Scalar residualError,
                               const Scalar constraintError)
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
  absoluteErrorHistory_.store(Point(1, absoluteError));
  relativeErrorHistory_.store(Point(1, relativeError));
  residualErrorHistory_.store(Point(1, residualError));
  constraintErrorHistory_.store(Point(1, constraintError));

  inputHistory_.store(x);
  outputHistory_.store(y);
}

Graph OptimizationResult::drawErrorHistory() const
{
  Graph result("Error history", "Iteration number", "Error value", true, "topright", 1.0, GraphImplementation::LOGY);
  result.setGrid(true);
  result.setGridColor("black");
// create a sample with the iteration number to be plotted as x data
  const UnsignedInteger size = getAbsoluteErrorHistory().getSize();
  {
    Sample data(getAbsoluteErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve absoluteErrorCurve( data, "absolute error" );
    absoluteErrorCurve.setLegend("absolute error");
    absoluteErrorCurve.setColor("red");
    result.add( absoluteErrorCurve );
  }
// Relative error
  {
    Sample data(getRelativeErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve relativeErrorCurve( data, "relative error" );
    relativeErrorCurve.setLegend("relative error");
    relativeErrorCurve.setColor("blue");
    result.add( relativeErrorCurve );
  }
// Residual error
  {
    Sample data(getResidualErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve residualErrorCurve( data, "residual error" );
    residualErrorCurve.setLegend("residual error");
    residualErrorCurve.setColor("green");
    result.add( residualErrorCurve );
  }
// Constraint error
  {
    Sample data(getConstraintErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve constraintErrorCurve( data, "constraint error" );
    constraintErrorCurve.setLegend("constraint error");
    constraintErrorCurve.setColor("magenta");
    result.add( constraintErrorCurve );
  }
  result.setYMargin(0.0);// tighten the Y axis
  return result;
}

/* Draw optimal value graph */
Graph OptimizationResult::drawOptimalValueHistory() const
{
  Graph result("Optimal value history", "Iteration number", "Optimal value", true, "topright", 1.0);
  result.setGrid(true);
  result.setGridColor("black");
  Sample data(getOutputSample());
  const UnsignedInteger size = data.getSize();
  const Bool minimization = problem_.isMinimization();
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    const UnsignedInteger j = 0;
    if (!((minimization && (data(i, j) < data(i - 1, j)))
          || (!minimization && (data(i, j) > data(i - 1, j)))))
    {
      data(i, j) = data(i - 1, j);
    }
  }
  Curve optimalValueCurve(data, "optimal value");
  optimalValueCurve.setLegend("optimal value");
  optimalValueCurve.setColor("red");
  result.add(optimalValueCurve);
  return result;
}


END_NAMESPACE_OPENTURNS

