//                                               -*- C++ -*-
/**
 *  @brief NearestPointAlgorithmImplementationResult stores the result of a NearestPointAlgorithmImplementationResult
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
#include "NearestPointAlgorithmImplementationResult.hxx"
#include "PersistentObjectFactory.hxx"
#include "Curve.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NearestPointAlgorithmImplementationResult);

static const Factory<NearestPointAlgorithmImplementationResult> RegisteredFactory;

/* Default constructor */
NearestPointAlgorithmImplementationResult::NearestPointAlgorithmImplementationResult()
  : PersistentObject()
  , minimizer_(NumericalPoint(0))
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
NearestPointAlgorithmImplementationResult::NearestPointAlgorithmImplementationResult(const NumericalPoint & minimizer,
    const UnsignedInteger iterationsNumber,
    const NumericalScalar absoluteError,
    const NumericalScalar relativeError,
    const NumericalScalar residualError,
    const NumericalScalar constraintError)
  : PersistentObject()
  , minimizer_(minimizer)
  , iterationsNumber_(iterationsNumber)
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

/* Virtual constructor */
NearestPointAlgorithmImplementationResult * NearestPointAlgorithmImplementationResult::clone() const
{
  return new NearestPointAlgorithmImplementationResult(*this);
}

/* Minimizer accessors */
NumericalPoint NearestPointAlgorithmImplementationResult::getMinimizer() const
{
  return minimizer_;
}

/* Minimizer accessors */
void NearestPointAlgorithmImplementationResult::setMinimizer(const NumericalPoint & minimizer)
{
  minimizer_ = minimizer;
}

/* Iterations number accessor */
UnsignedInteger NearestPointAlgorithmImplementationResult::getIterationsNumber() const
{
  return iterationsNumber_;
}

/* Iterations Number accessor */
void NearestPointAlgorithmImplementationResult::setIterationsNumber(const UnsignedInteger iterationsNumber)
{
  iterationsNumber_ = iterationsNumber;
}

/* Absolute error accessor */
NumericalScalar NearestPointAlgorithmImplementationResult::getAbsoluteError() const
{
  return absoluteError_;
}

NumericalSample NearestPointAlgorithmImplementationResult::getAbsoluteErrorHistory() const
{
  return absoluteErrorHistory_.getSample();
}

/* Absolute error accessor */
void NearestPointAlgorithmImplementationResult::setAbsoluteError(const NumericalScalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
NumericalScalar NearestPointAlgorithmImplementationResult::getRelativeError() const
{
  return relativeError_;
}

NumericalSample NearestPointAlgorithmImplementationResult::getRelativeErrorHistory() const
{
  return relativeErrorHistory_.getSample();
}

/* Relative error accessor */
void NearestPointAlgorithmImplementationResult::setRelativeError(const NumericalScalar relativeError)
{
  relativeError_ = relativeError;
}

/* Residual error accessor */
NumericalScalar NearestPointAlgorithmImplementationResult::getResidualError() const
{
  return residualError_;
}

NumericalSample NearestPointAlgorithmImplementationResult::getResidualErrorHistory() const
{
  return residualErrorHistory_.getSample();
}

/* Residual error accessor */
void NearestPointAlgorithmImplementationResult::setResidualError(const NumericalScalar residualError)
{
  residualError_ = residualError;
}

/* Constraint error accessor */
NumericalScalar NearestPointAlgorithmImplementationResult::getConstraintError() const
{
  return constraintError_;
}

NumericalSample NearestPointAlgorithmImplementationResult::getConstraintErrorHistory() const
{
  return constraintErrorHistory_.getSample();
}

/* Constraint error accessor */
void NearestPointAlgorithmImplementationResult::setConstraintError(const NumericalScalar constraintError)
{
  constraintError_ = constraintError;
}

NumericalSample NearestPointAlgorithmImplementationResult::getInputSample() const
{
  return inputHistory_.getSample();
}

NumericalSample NearestPointAlgorithmImplementationResult::getOutputSample() const
{
  return outputHistory_.getSample();
}

/* String converter */
String NearestPointAlgorithmImplementationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << NearestPointAlgorithmImplementationResult::GetClassName()
      << " minimizer=" << minimizer_
      << " iterationsNumber=" << iterationsNumber_
      << " absoluteError=" << getAbsoluteError()
      << " relativeError=" << getRelativeError()
      << " residualError=" << getResidualError()
      << " constraintError=" << getConstraintError();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NearestPointAlgorithmImplementationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "minimizer_", minimizer_ );
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
void NearestPointAlgorithmImplementationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "minimizer_", minimizer_ );
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
void NearestPointAlgorithmImplementationResult::update(const NumericalPoint & minimizer, UnsignedInteger iterationNumber)
{
  setMinimizer( minimizer );
  setIterationsNumber( iterationNumber );
}

/* Incremental history storage */
void NearestPointAlgorithmImplementationResult::store(const NumericalPoint & x,
    const NumericalPoint & y,
    const NumericalScalar absoluteError,
    const NumericalScalar relativeError,
    const NumericalScalar residualError,
    const NumericalScalar constraintError)
{
  // assume the last point stored is the optimum
  minimizer_ = x;

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

Graph NearestPointAlgorithmImplementationResult::drawErrorHistory() const
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
