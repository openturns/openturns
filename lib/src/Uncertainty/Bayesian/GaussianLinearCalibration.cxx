//                                               -*- C++ -*-
/**
 *  @brief Default GaussianLinearCalibration
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
#include "openturns/GaussianLinearCalibration.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(GaussianLinearCalibration)

static const Factory<GaussianLinearCalibration> Factory_GaussianLinearCalibration;

/* Default constructor */
GaussianLinearCalibration::GaussianLinearCalibration()
  : CalibrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
GaussianLinearCalibration::GaussianLinearCalibration(const Function & model,
    const Sample & inputObservations,
    const Sample & outputObservations,
    const Point & parameterMean,
    const CovarianceMatrix & parameterCovariance,
    const CovarianceMatrix & errorCovariance,
    const String & methodName)
  : CalibrationAlgorithmImplementation(model, inputObservations, outputObservations, Normal(parameterMean, parameterCovariance))
  , modelObservations_(0, 0)
  , gradientObservations_(0, 0)
  , errorCovariance_(errorCovariance)
  , globalErrorCovariance_(false)
  , methodName_(methodName)
{
  // Check the input
  const UnsignedInteger parameterDimension = parameterMean.getDimension();
  if (model.getParameterDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of parameter dimension=" << parameterDimension << ", got parameter dimension=" << model.getParameterDimension();
  if (parameterCovariance.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a parameter covariance of dimension=" << parameterDimension << ", got dimension=" << parameterCovariance.getDimension();
  const UnsignedInteger inputDimension = inputObservations_.getDimension();
  if (model.getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of input dimension=" << inputDimension << ", got input dimension=" << model.getInputDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  if (model.getOutputDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model of output dimension=" << outputDimension << ", got output dimension=" << model.getOutputDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (inputObservations.getDimension() && inputObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected an input sample of size=" << size << ", got size=" << inputObservations.getSize();
  globalErrorCovariance_ = errorCovariance.getDimension() != outputDimension;
  if (globalErrorCovariance_ && !(errorCovariance.getDimension() == outputDimension * size)) throw InvalidArgumentException(HERE) << "Error: expected an error covariance either of dimension=" << outputDimension << " or dimension=" << outputDimension * size << ", got dimension=" << errorCovariance.getDimension();
}

/* Parameter constructor */
GaussianLinearCalibration::GaussianLinearCalibration(const Sample & modelObservations,
    const Matrix & gradientObservations,
    const Sample & outputObservations,
    const Point & parameterMean,
    const CovarianceMatrix & parameterCovariance,
    const CovarianceMatrix & errorCovariance,
    const String & methodName)
  : CalibrationAlgorithmImplementation(Function(), Sample(), outputObservations, Normal(parameterMean, parameterCovariance))
  , modelObservations_(modelObservations)
  , gradientObservations_(gradientObservations)
  , errorCovariance_(errorCovariance)
  , globalErrorCovariance_(false)
  , methodName_(methodName)
{
  // Check the input
  const UnsignedInteger parameterDimension = parameterMean.getDimension();
  if (parameterCovariance.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a parameter covariance of dimension=" << parameterDimension << ", got dimension=" << parameterCovariance.getDimension();
  const UnsignedInteger outputDimension = outputObservations.getDimension();
  const UnsignedInteger size = outputObservations.getSize();
  if (modelObservations.getSize() != size) throw InvalidArgumentException(HERE) << "Error: expected a model observation sample of size=" << size << ", got size=" << modelObservations.getSize();
  if (modelObservations.getDimension() != outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a model observation sample of dimension=" << outputDimension << ", got dimension=" << modelObservations.getDimension();
  if (gradientObservations.getNbColumns() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of columns number=" << parameterDimension << ", got columns number=" << gradientObservations.getNbColumns();
  if (gradientObservations.getNbRows() != size * outputDimension) throw InvalidArgumentException(HERE) << "Error: expected a gradient parameter of rows number=" << size * outputDimension << ", got rows number=" << gradientObservations.getNbRows();
  globalErrorCovariance_ = errorCovariance.getDimension() != outputDimension;
  if (globalErrorCovariance_ && !(errorCovariance.getDimension() == outputDimension * size)) throw InvalidArgumentException(HERE) << "Error: expected an error covariance either of dimension=" << outputDimension << " or dimension=" << outputDimension * size << ", got dimension=" << errorCovariance.getDimension();
}


Matrix GaussianLinearCalibration::ComputeDesignMatrix(const UnsignedInteger parameterDimension,
    const UnsignedInteger outputDimension,
    const UnsignedInteger size,
    const Distribution & parameterPrior,
    const Matrix & gradientObservations,
    const Bool globalErrorCovariance,
    const CovarianceMatrix & errorCovariance,
    TriangularMatrix & errorInverseCholesky)
{
  const UnsignedInteger dimension = errorCovariance.getDimension();
  // Compute inverse of the Cholesky decomposition of the covariance matrix of the parameter
  const TriangularMatrix parameterInverseCholesky(parameterPrior.getInverseCholesky());

  // Compute the covariance matrix R
  CovarianceMatrix R(size * outputDimension);
  if (globalErrorCovariance) R = errorCovariance;
  else
  {
    if (dimension == 1) R = (R * errorCovariance(0, 0)).getImplementation();
    else
    {
      for (UnsignedInteger i = 0; i < size; ++i)
        for (UnsignedInteger j = 0; j < dimension; ++j)
          for (UnsignedInteger k = 0; k < dimension; ++k)
            R(i * dimension + j, i * dimension + k) = errorCovariance(j, k);
    }
  }

  // Compute the inverse of the Cholesky decomposition of R
  const Normal error(Point(R.getDimension()), R);
  errorInverseCholesky = TriangularMatrix(error.getInverseCholesky());
  // Compute errorInverseCholesky*J, the second part of the extended design matrix
  const Matrix invLRJ(errorInverseCholesky * gradientObservations);
  // Create the extended design matrix of the linear least squares problem
  Matrix Abar (parameterDimension + size * outputDimension, parameterDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++i)
    for (UnsignedInteger j = 0; j < parameterDimension; ++j)
      Abar(i, j) = parameterInverseCholesky(i, j);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      for (UnsignedInteger k = 0; k < parameterDimension; ++k)
        Abar(i * outputDimension + j + parameterDimension, k) = -invLRJ(i * outputDimension + j, k);
  return Abar;
}


/* Performs the actual computation. Must be overloaded by the actual calibration algorithm */
void GaussianLinearCalibration::run()
{
  const UnsignedInteger parameterDimension = getParameterPrior().getDimension();
  const UnsignedInteger outputDimension = getOutputObservations().getDimension();
  const UnsignedInteger size = getOutputObservations().getSize();
  if (getModel().getEvaluation().getImplementation()->isActualImplementation())
  {
    // Compute the linearization
    Function parametrizedModel(getModel());
    parametrizedModel.setParameter(getParameterPrior().getMean());
    // Flatten everything related to the model evaluations over the input observations
    modelObservations_ = parametrizedModel(getInputObservations());
    gradientObservations_ = MatrixImplementation(parameterDimension, size * outputDimension);
    UnsignedInteger shift = 0;
    UnsignedInteger skip = parameterDimension * outputDimension;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Matrix parameterGradient(parametrizedModel.parameterGradient(getInputObservations()[i]));
      std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), gradientObservations_.getImplementation()->begin() + shift);
      shift += skip;
    }
    gradientObservations_ = gradientObservations_.transpose();
    parameterPrior_.setDescription(getModel().getParameterDescription());
  }

  TriangularMatrix errorInverseCholesky;
  const Matrix Abar(ComputeDesignMatrix(parameterDimension, outputDimension, size, getParameterPrior(), gradientObservations_,
                                        globalErrorCovariance_, errorCovariance_, errorInverseCholesky));

  // Compute the difference of output observations and output predictions
  const Point deltaY(modelObservations_.getImplementation()->getData() - outputObservations_.getImplementation()->getData());
  // Compute errorInverseCholesky*deltay, the right hand size of the extended residual
  const Point invLRz = errorInverseCholesky * deltaY;
  // Create the extended right hand side of the extended linear least squares system : ybar = -invLRz
  Point ybar(parameterDimension + size * outputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      ybar[i * outputDimension + j + parameterDimension] = invLRz[i * outputDimension + j];
  // Solve the linear least squares problem
  Distribution parameterPosterior;
  Point thetaStar;
  if (bounds_.getDimension() == 0)
  {
    LeastSquaresMethod method(LeastSquaresMethod::Build(methodName_, Abar));
    const Point deltaTheta(method.solve(ybar));
    for (UnsignedInteger i = 0; i < deltaTheta.getDimension(); ++ i)
      if (!std::isfinite(deltaTheta[i])) throw InvalidArgumentException(HERE) << "The calibration problem is not identifiable";

    thetaStar = getParameterMean() + deltaTheta;
    const CovarianceMatrix covarianceThetaStar(method.getGramInverse().getImplementation());
    parameterPosterior = Normal(thetaStar, covarianceThetaStar);
  }
  else
  {
    // Constrained MAP: minimize ||Abar.theta - (ybar + Abar.parameterMean)||^2
    // subject to the parameter bounds. In the increment variable
    // theta-parameterMean the right-hand side is ybar. When the unconstrained
    // optimum is feasible, the constrained MAP is equal to it.
    const Point deltaTheta(SolveBoundedLeastSquaresIncrement(Abar, ybar, bounds_, getParameterMean()));
    thetaStar = getParameterMean() + deltaTheta;

    const String boundsHandling(ResourceMap::GetAsString("GaussianLinearCalibration-BoundsHandling"));
    if (boundsHandling == "ActiveSet")
    {
      // Variables at an active bound are considered fixed: the posterior
      // covariance is computed on the free block Abar_F^T Abar_F only, and it
      // is zero in the directions fixed at a bound. This is the usual
      // active-set approximation, which treats the active bounds as equality
      // constraints and applies the unconstrained Gaussian approximation to
      // the remaining free parameters. See
      // S. Huet, A. Bouvier, M.-A. Poursat and E. Jolivet, Statistical Tools for
      // Nonlinear Regression: A Practical Guide with S-PLUS and R Examples,
      // 2nd ed., Springer, New York, 2004, for the Gaussian approximation, and
      // P. E. Gill, W. Murray and M. H. Wright, Practical Optimization,
      // Academic Press, London, 1981, Section 5.2, for the active-set treatment
      // of bound constraints.
      const Indices freeIndices(ComputeFreeParameterIndices(thetaStar, bounds_));
      const CovarianceMatrix covarianceThetaStar(ComputeFreeGramInverse(Abar, freeIndices, methodName_));
      try
      {
        parameterPosterior = Normal(thetaStar, covarianceThetaStar);
      }
      catch (...)
      {
        throw InternalException(HERE) << "Error: the covariance of the posterior distribution is not definite positive. The problem may be not identifiable.";
      }
    }
    else if (boundsHandling == "Truncated")
    {
      // Unconstrained Gaussian posterior.
      LeastSquaresMethod method(LeastSquaresMethod::Build(methodName_, Abar));
      const Point unconstrainedDeltaTheta(method.solve(ybar));
      for (UnsignedInteger i = 0; i < unconstrainedDeltaTheta.getDimension(); ++ i)
        if (!std::isfinite(unconstrainedDeltaTheta[i])) throw InvalidArgumentException(HERE) << "The calibration problem is not identifiable";
      const Point unconstrainedMean(getParameterMean() + unconstrainedDeltaTheta);
      const CovarianceMatrix unconstrainedCovariance(method.getGramInverse().getImplementation());

      // Exact posterior of the linear-Gaussian model with bounds: the
      // unconstrained Gaussian posterior truncated to the parameter bounds.
      // The reported MAP is the mode of the truncated distribution, which is
      // the constrained minimizer computed above; the mean of the truncated
      // distribution differs from the MAP. Beware that the current
      // TruncatedDistribution only computes its moments in reasonable time
      // for dimensions up to about three. See G. M. Tallis, The moment
      // generating function of the truncated multi-normal distribution,
      // JRSS B 23(1):223-229, 1961, and W. C. Horrace, Some results on the
      // multivariate truncated normal distribution, J. Multivariate Anal.
      // 94(1):209-221, 2005.
      parameterPosterior = ComputeTruncatedPosterior(unconstrainedMean, unconstrainedCovariance, bounds_);
    }
    else
      throw InvalidArgumentException(HERE) << "Error: unknown value " << boundsHandling << " for the GaussianLinearCalibration-BoundsHandling key, expected \"ActiveSet\" or \"Truncated\"";
  }
  parameterPosterior.setDescription(parameterPrior_.getDescription());
  const LinearFunction residualFunction(getParameterMean(), deltaY, gradientObservations_);
  result_ = CalibrationResult(parameterPrior_, parameterPosterior, thetaStar, Normal(Point(errorCovariance_.getDimension()), errorCovariance_), inputObservations_, outputObservations_, residualFunction, true);
}

/* Model observations accessor */
Sample GaussianLinearCalibration::getModelObservations() const
{
  return modelObservations_;
}


/* Model gradient wrt the parameter accessor */
Matrix GaussianLinearCalibration::getGradientObservations() const
{
  return gradientObservations_;
}

/* ParameterMean accessor */
Point GaussianLinearCalibration::getParameterMean() const
{
  // The parameterMean is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getMean();
}

/* Parameter covariance accessor */
CovarianceMatrix GaussianLinearCalibration::getParameterCovariance() const
{
  // The parameter covariance is stored in the prior distribution, which is a Normal distribution
  return getParameterPrior().getCovariance();
}

/* Error covariance accessor */
CovarianceMatrix GaussianLinearCalibration::getErrorCovariance() const
{
  return errorCovariance_;
}

/* Global error covariance accessor */
Bool GaussianLinearCalibration::getGlobalErrorCovariance() const
{
  return globalErrorCovariance_;
}

/* Least squares method name accessor */
String GaussianLinearCalibration::getMethodName() const
{
  return methodName_;
}

/* String converter */
String GaussianLinearCalibration::__repr__() const
{
  return OSS() << "class=" << GaussianLinearCalibration::GetClassName()
         << " name=" << getName();
}


GaussianLinearCalibration * GaussianLinearCalibration::clone() const
{
  return new GaussianLinearCalibration(*this);
}


/* Method save() stores the object through the StorageManager */
void GaussianLinearCalibration::save(Advocate & adv) const
{
  CalibrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("modelObservations_", modelObservations_);
  adv.saveAttribute("gradientObservations_", gradientObservations_);
  adv.saveAttribute("errorCovariance_", errorCovariance_);
  adv.saveAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.saveAttribute("methodName_", methodName_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianLinearCalibration::load(Advocate & adv)
{
  CalibrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("modelObservations_", modelObservations_);
  adv.loadAttribute("gradientObservations_", gradientObservations_);
  adv.loadAttribute("errorCovariance_", errorCovariance_);
  adv.loadAttribute("globalErrorCovariance_", globalErrorCovariance_);
  adv.loadAttribute("methodName_", methodName_);
}


END_NAMESPACE_OPENTURNS
