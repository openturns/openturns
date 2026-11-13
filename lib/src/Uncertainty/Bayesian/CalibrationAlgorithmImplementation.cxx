//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CalibrationAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"

#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CalibrationAlgorithmImplementation)

static const Factory<CalibrationAlgorithmImplementation> Factory_CalibrationAlgorithmImplementation;

/* Default constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Parameter constructor */
CalibrationAlgorithmImplementation::CalibrationAlgorithmImplementation(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Distribution & parameterPrior)
  : PersistentObject()
  , model_(model)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , parameterPrior_(parameterPrior)
  , result_()
{
  // Allow for a dim-0 sample but it should have the right size internally
  if (!inputObservations_.getDimension())
    inputObservations_ = Sample(outputObservations.getSize(), 0);
}

/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void CalibrationAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In CalibrationAlgorithmImplementation::run()";
}

/* Indices of the parameters that are not fixed at a bound: all the
 * directions when bounds is empty, the free ones otherwise. */
Indices CalibrationAlgorithmImplementation::ComputeFreeParameterIndices(const Point & thetaStar,
    const Interval & bounds)
{
  const UnsignedInteger parameterDimension = thetaStar.getDimension();
  if (bounds.getDimension() == 0)
  {
    Indices freeIndices(parameterDimension);
    freeIndices.iota();
    return freeIndices;
  }
  if (bounds.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  // Absolute tolerance used to detect a parameter lying on one of its bounds.
  // It is deliberately larger than the default primal feasibility tolerance of
  // the quadratic solver (1e-7) so that a parameter reported slightly outside
  // its bound is still detected. It is not scaled by the bound magnitude:
  // scaling by a large bound would classify a parameter lying well inside the
  // feasible interval as fixed.
  const Scalar tolerance = ResourceMap::GetAsScalar("LinearLeastSquaresCalibration-ActiveBoundTolerance");
  if (!(tolerance > 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE) << "Error: the active bound tolerance must be finite and strictly positive, got " << tolerance;
  const Point lowerBound(bounds.getLowerBound());
  const Point upperBound(bounds.getUpperBound());
  const Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
  Indices freeIndices(0);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
  {
    Bool active = false;
    if (finiteLowerBound[i] && (std::abs(thetaStar[i] - lowerBound[i]) <= tolerance))
      active = true;
    if (finiteUpperBound[i] && (std::abs(thetaStar[i] - upperBound[i]) <= tolerance))
      active = true;
    if (!active)
      freeIndices.add(i);
  }
  return freeIndices;
}

/* Inverse of the Gram matrix on the free directions of a design matrix,
 * padded with zeros in the directions fixed at a bound. */
CovarianceMatrix CalibrationAlgorithmImplementation::ComputeFreeGramInverse(const Matrix & design,
    const Indices & freeIndices,
    const String & methodName,
    const Bool checkRank)
{
  const UnsignedInteger parameterDimension = design.getNbColumns();
  const UnsignedInteger freeDimension = freeIndices.getSize();
  // The covariance is zero in the directions fixed at a bound: start from a
  // zero matrix instead of the identity matrix used by CovarianceMatrix(dim).
  CovarianceMatrix gramInverse(parameterDimension, Point(parameterDimension));
  if (freeDimension == 0)
    return gramInverse;
  Matrix freeDesign(design.getNbRows(), freeDimension);
  for (UnsignedInteger j = 0; j < freeDimension; ++j)
    for (UnsignedInteger i = 0; i < design.getNbRows(); ++i)
      freeDesign(i, j) = design(i, freeIndices[j]);
  // When requested, a rank-deficient free block gives a small positive
  // singular value whose inverse can still be finite: check the numerical rank
  // before relying on the factorization. The unbounded non-linear path keeps
  // the unconstrained behaviour and lets the caller detect a non-finite
  // covariance, so that regularized non-identifiable problems keep being
  // calibrated.
  if (checkRank)
  {
    const Point singularValues(freeDesign.computeSingularValues());
    const UnsignedInteger singularValuesSize = singularValues.getSize();
    const Scalar largestSingularValue = (singularValuesSize > 0) ? singularValues[0] : 0.0;
    const Scalar rankTolerance = std::max(freeDesign.getNbRows(), freeDimension) * SpecFunc::Precision * largestSingularValue;
    if ((singularValuesSize < freeDimension) || !(largestSingularValue > 0.0) || (singularValues[singularValuesSize - 1] <= rankTolerance))
      throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
  }
  LeastSquaresMethod freeMethod(LeastSquaresMethod::Build(methodName, freeDesign));
  // Initialize the decomposition before accessing the Gram inverse
  freeMethod.update(Indices(), freeMethod.getCurrentIndices(), Indices(), false);
  const CovarianceMatrix freeGramInverse(freeMethod.getGramInverse());
  for (UnsignedInteger i = 0; i < freeDimension; ++i)
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
    {
      const Scalar value = freeGramInverse(i, j);
      if (!std::isfinite(value))
        throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
      gramInverse(freeIndices[i], freeIndices[j]) = value;
    }
  return gramInverse;
}

/* Inverse of a Hessian matrix on the free directions, padded with zeros in
 * the directions fixed at a bound. */
CovarianceMatrix CalibrationAlgorithmImplementation::ComputeFreeHessianInverse(const Matrix & hessian,
    const Indices & freeIndices)
{
  const UnsignedInteger parameterDimension = hessian.getNbRows();
  const UnsignedInteger freeDimension = freeIndices.getSize();
  CovarianceMatrix hessianInverse(parameterDimension, Point(parameterDimension));
  if (freeDimension == 0)
    return hessianInverse;
  Matrix freeHessian(freeDimension, freeDimension);
  for (UnsignedInteger i = 0; i < freeDimension; ++i)
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
      freeHessian(i, j) = hessian(freeIndices[i], freeIndices[j]);
  try
  {
    const Matrix freeInverse(freeHessian.solveLinearSystem(IdentityMatrix(freeDimension)));
    const CovarianceMatrix freeCovariance(*freeInverse.getImplementation());
    for (UnsignedInteger i = 0; i < freeDimension; ++i)
      for (UnsignedInteger j = 0; j < freeDimension; ++j)
      {
        const Scalar value = freeCovariance(i, j);
        if (!std::isfinite(value))
          throw InvalidArgumentException(HERE) << "Error: the calibration problem is not identifiable in the free parameter directions";
        hessianInverse(freeIndices[i], freeIndices[j]) = value;
      }
  }
  catch (const InvalidArgumentException &)
  {
    throw;
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot compute the Laplace approximation: the Hessian of the residuals at the optimum is not positive definite in the free parameter directions.";
  }
  return hessianInverse;
}

/* String converter */
String CalibrationAlgorithmImplementation::__repr__() const
{
  return OSS() << "class=" << CalibrationAlgorithmImplementation::GetClassName()
         << " output observations=" << outputObservations_
         << " parameter prior=" << parameterPrior_
         << " result=" << result_;
}

/* Model accessor */
Function CalibrationAlgorithmImplementation::getModel() const
{
  return model_;
}

/* Input observations accessor */
Sample CalibrationAlgorithmImplementation::getInputObservations() const
{
  return inputObservations_;
}

/* Output observations accessor */
Sample CalibrationAlgorithmImplementation::getOutputObservations() const
{
  return outputObservations_;
}

/* Parameter prior accessor */
Distribution CalibrationAlgorithmImplementation::getParameterPrior() const
{
  return parameterPrior_;
}

/* Bounds accessor */
Interval CalibrationAlgorithmImplementation::getBounds() const
{
  return bounds_;
}

void CalibrationAlgorithmImplementation::setBounds(const Interval & bounds)
{
  // An empty interval means no bounds; otherwise the dimension must match
  // the calibration parameter dimension when it is already known.
  const UnsignedInteger parameterDimension = parameterPrior_.getDimension();
  if ((bounds.getDimension() != 0) && (parameterDimension != 0) && (bounds.getDimension() != parameterDimension))
    throw InvalidArgumentException(HERE) << "Error: expected bounds of dimension=" << parameterDimension << ", got dimension=" << bounds.getDimension();
  bounds_ = bounds;
}

/* Result accessor */
void CalibrationAlgorithmImplementation::setResult(const CalibrationResult & result)
{
  result_ = result;
}

CalibrationResult CalibrationAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Virtual copy constructor */
CalibrationAlgorithmImplementation* CalibrationAlgorithmImplementation::clone() const
{
  return new CalibrationAlgorithmImplementation(*this);
}

/* Method save() stores the object through the StorageManager */
void CalibrationAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("inputObservations_", inputObservations_);
  adv.saveAttribute("outputObservations_", outputObservations_);
  adv.saveAttribute("parameterPrior_", parameterPrior_);
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void CalibrationAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("inputObservations_", inputObservations_);
  adv.loadAttribute("outputObservations_", outputObservations_);
  adv.loadAttribute("parameterPrior_", parameterPrior_);
  if (adv.hasAttribute("bounds_"))
    adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
