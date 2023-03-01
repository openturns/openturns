//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/GaussianProcessRegression.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/KrigingEvaluation.hxx"
#include "openturns/KrigingGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/GeneralLinearModelResult.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessRegression)

static const Factory<GaussianProcessRegression> Factory_GaussianProcessRegression;


/* Default constructor */
GaussianProcessRegression::GaussianProcessRegression()
  : MetaModelAlgorithm()
  , inputSample_(0, 0)
  , outputSample_(0, 0)
  , covarianceModel_()
  , basis_()
  , gamma_(0)
  , result_()
{
  // Nothing to do
}

GaussianProcessRegression::GaussianProcessRegression(const GaussianProcessFitterResult &result)
  : MetaModelAlgorithm(result.getInputSample(), result.getOutputSample())
  , inputSample_(result.getInputSample())
  , outputSample_(result.getOutputSample())
  , covarianceModel_(result.getCovarianceModel())
  , basis_(result.getBasis())
  , gamma_(0)
  , gaussianProcessFitterResult_(result)
  , result_()
{ 
  // TODO
}

/* Constructor */
GaussianProcessRegression::GaussianProcessRegression(const Sample & inputSample,
                                                     const Sample & outputSample,
                                                     const CovarianceModel & covarianceModel,
                                                     const Function & trend)
  : MetaModelAlgorithm(inputSample, outputSample)
  , inputSample_(inputSample)
  , outputSample_(outputSample)
  , covarianceModel_(covarianceModel)
  , basis_()
  , gamma_(0)
  , gaussianProcessFitterResult_()
  , result_()
{
  // check in/out samples
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression input sample size (" << inputSample.getSize() << ") does not match output sample size (" << outputSample.getSize() << ")";

  const UnsignedInteger inputDimension = inputSample.getDimension();
  const UnsignedInteger outputDimension = outputSample.getDimension();

  // covariance model checking
  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : Covariance model input dimension is " << covarianceModel.getInputDimension() << ", expected " << inputDimension;
  if (covarianceModel.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : Covariance model output dimension is " << covarianceModel.getOutputDimension() << ", expected " << outputDimension;

  // trend checking
  if (trend.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : trend input dimension is " << trend.getInputDimension() << ", expected " << inputDimension;
  if (trend.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : trend output dimension is " << trend.getOutputDimension() << ", expected " << outputDimension;
  
  // TODO : implement the computation
  // TODO Delegate to GaussianProcessFitter
  //const Sample residual(outputSample_ - trend(inputSample_));
}

/* Virtual constructor */
GaussianProcessRegression * GaussianProcessRegression::clone() const
{
  return new GaussianProcessRegression(*this);
}

void GaussianProcessRegression::computeGamma()
{
  // Get cholesky factor & rho from glm
  LOGINFO("Solve L^t.gamma = rho");

  const Point rho(gaussianProcessFitterResult_.getRho());
  const UnsignedInteger algebraMethod = gaussianProcessFitterResult_.getLinearAlgebraMethod();

  if (algebraMethod)
  {
    gamma_ = gaussianProcessFitterResult_.getHMatCholeskyFactor().solveLower(rho, true);
  }
  else
  {
    // Arguments are keepIntact=true, matrix_lower=true & solving_transposed=true
    gamma_ = gaussianProcessFitterResult_.getCholeskyFactor().getImplementation()->solveLinearSystemTri(rho, true, true);
  }
}

/* Perform regression */
void GaussianProcessRegression::run()
{

  // Covariance coefficients are computed once, ever if optimizer is fixed
  LOGINFO("Compute the interpolation part");
  computeGamma();
  LOGINFO("Store the estimates");
  LOGINFO("Build the output meta-model");
  Function metaModel;

  // We use directly the points
  const Basis basis(gaussianProcessFitterResult_.getBasis());
  const CovarianceModel conditionalCovarianceModel(gaussianProcessFitterResult_.getCovarianceModel());
  const Point beta(gaussianProcessFitterResult_.getTrendCoefficients());
  const UnsignedInteger outputDimension = outputSample_.getDimension();
  Sample covarianceCoefficients(inputSample_.getSize(), outputDimension);
  covarianceCoefficients.getImplementation()->setData(gamma_);
  // Meta model definition
  metaModel.setEvaluation(new KrigingEvaluation(basis, inputSample_, conditionalCovarianceModel, beta, covarianceCoefficients));
  metaModel.setGradient(new KrigingGradient(basis, inputSample_, conditionalCovarianceModel, beta, covarianceCoefficients));
  metaModel.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));

  // compute residual, relative error
  const Point outputVariance(outputSample_.computeVariance());
  const Sample mY(metaModel(inputSample_));
  //const Sample mY(outputSample_.getSize(), outputSample_.getDimension());
  const Point squaredResiduals((outputSample_ - mY).computeRawMoment(2));

  const UnsignedInteger size = inputSample_.getSize();
  Point residuals(outputDimension);
  Point relativeErrors(outputDimension);
  for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++ outputIndex)
  {
    residuals[outputIndex] = sqrt(squaredResiduals[outputIndex] / size);
    relativeErrors[outputIndex] = squaredResiduals[outputIndex] / outputVariance[outputIndex];
  }
  //TODO Gamma as a Sample
  //result_ = GaussianProcessRegressionResult(gaussianProcessFitterResult_, gamma_);
}


/* String converter */
String GaussianProcessRegression::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


GaussianProcessRegressionResult GaussianProcessRegression::getResult()
{
  return result_;
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessRegression::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "basis_", basis_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessRegression::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
