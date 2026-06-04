//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "openturns/GaussianProcessRegression.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GaussianProcessEvaluation.hxx"
#include "openturns/GaussianProcessGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/GaussianProcessFitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessRegression)

static const Factory<GaussianProcessRegression> Factory_GaussianProcessRegression;


/* Default constructor */
GaussianProcessRegression::GaussianProcessRegression()
  : MetaModelAlgorithm()
  , covarianceModel_()
  , basis_()
  , beta_(0)
  , gaussianProcessFitterResult_()
  , result_()
{
  // Nothing to do
}

GaussianProcessRegression::GaussianProcessRegression(const GaussianProcessFitterResult & result)
  : MetaModelAlgorithm(result.getInputSample(), result.getOutputSample())
  , covarianceModel_(result.getCovarianceModel())
  , basis_(result.getBasis())
  , beta_(0)
  , gaussianProcessFitterResult_(result)
  , result_()
{
  basis_ = gaussianProcessFitterResult_.getBasis();
  beta_ = gaussianProcessFitterResult_.getTrendCoefficients();
}

/* Constructor */
GaussianProcessRegression::GaussianProcessRegression(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Function & trendFunction)
  : MetaModelAlgorithm(inputSample, outputSample),
    covarianceModel_(covarianceModel),
    basis_(),
    beta_(0),
    gaussianProcessFitterResult_(),
    result_()
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
  if (trendFunction.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : trend input dimension is " << trendFunction.getInputDimension() << ", expected " << inputDimension;
  if (trendFunction.getOutputDimension() != outputDimension)
    throw InvalidArgumentException(HERE) << "GaussianProcessRegression : trend output dimension is " << trendFunction.getOutputDimension() << ", expected " << outputDimension;

  const Sample detrended(outputSample_ - trendFunction(inputSample_));
  // Launch a fitter without actually fitting, just discretize
  GaussianProcessFitter algo(inputSample_, detrended, covarianceModel_);
  algo.setKeepCholeskyFactor(true);
  algo.setOptimizeParameters(false);
  algo.run();
  gaussianProcessFitterResult_ = algo.getResult();
  // we do not estimate any trend but the trend function should be stored in the Kriging eval
  basis_ = Basis(1);
  basis_[0] = trendFunction;
  beta_ = Point(outputDimension, 1.0);
}

/* Virtual constructor */
GaussianProcessRegression * GaussianProcessRegression::clone() const
{
  return new GaussianProcessRegression(*this);
}

Point GaussianProcessRegression::computeGamma() const
{
  // Get cholesky factor & rho from gaussian fitter result
  LOGDEBUG("Solve L^t.gamma = rho");

  const Point rho(gaussianProcessFitterResult_.getStandardizedOutput());

  const GaussianProcessFitterResult::LinearAlgebra algebraMethod = gaussianProcessFitterResult_.getLinearAlgebraMethod();
  if (algebraMethod == GaussianProcessFitterResult::HMAT)
  {
    return gaussianProcessFitterResult_.getHMatCholeskyFactor().solveLower(rho, true);
  }
  else
  {
    // Arguments are keepIntact=true, matrix_lower=true & solving_transposed=true
    return gaussianProcessFitterResult_.getCholeskyFactor().getImplementation()->solveLinearSystemTri(rho, true, true);
  }
}

/* Perform regression */
void GaussianProcessRegression::run()
{

  // Covariance coefficients are computed once, ever if optimizer is fixed
  LOGDEBUG("Compute the interpolation part");
  const Point gamma(computeGamma());
  LOGDEBUG("Store the estimates");
  LOGDEBUG("Build the output meta-model");
  Function metaModel;

  // We use directly the points
  const CovarianceModel conditionalCovarianceModel(gaussianProcessFitterResult_.getCovarianceModel());
  const Sample inputSample(getInputSample());
  const Sample outputSample(getOutputSample());
  const UnsignedInteger outputDimension = outputSample.getDimension();
  Sample covarianceCoefficients(inputSample.getSize(), outputDimension);
  covarianceCoefficients.getImplementation()->setData(gamma);

  // Meta model definition
  metaModel.setEvaluation(new GaussianProcessEvaluation(basis_, inputSample, conditionalCovarianceModel, beta_, covarianceCoefficients));
  metaModel.setGradient(new GaussianProcessGradient(basis_, inputSample, conditionalCovarianceModel, beta_, covarianceCoefficients));
  metaModel.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), metaModel.getEvaluation()));

  result_ = GaussianProcessRegressionResult(gaussianProcessFitterResult_, covarianceCoefficients);
  result_.setMetaModel(metaModel);
}


/* String converter */
String GaussianProcessRegression::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


GaussianProcessRegressionResult GaussianProcessRegression::getResult() const
{
  return result_;
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessRegression::save(Advocate & adv) const
{
  MetaModelAlgorithm::save(adv);
  adv.saveAttribute( "covarianceModel_", covarianceModel_ );
  adv.saveAttribute( "basis_", basis_);
  adv.saveAttribute( "result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessRegression::load(Advocate & adv)
{
  MetaModelAlgorithm::load(adv);
  adv.loadAttribute( "covarianceModel_", covarianceModel_ );
  adv.loadAttribute( "basis_", basis_);
  adv.loadAttribute( "result_", result_);
}

END_NAMESPACE_OPENTURNS
