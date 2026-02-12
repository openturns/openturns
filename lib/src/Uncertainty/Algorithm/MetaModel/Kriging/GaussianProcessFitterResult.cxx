//                                               -*- C++ -*-
/**
 *  @brief The result of a gaussian process fitter
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
#include "openturns/GaussianProcessFitterResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/GaussianProcess.hxx"
#include "openturns/WhiteNoise.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessFitterResult)
static const Factory<GaussianProcessFitterResult> Factory_GaussianProcessFitterResult;

/* Default constructor */
GaussianProcessFitterResult::GaussianProcessFitterResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters & Cholesky factor */
GaussianProcessFitterResult::GaussianProcessFitterResult(const Sample & inputSample,
    const Sample & outputSample,
    const Function & metaModel,
    const Matrix & regressionMatrix,
    const Basis & basis,
    const Point & trendCoefficients,
    const CovarianceModel & covarianceModel,
    const Scalar optimalLogLikelihood,
    const LinearAlgebra linearAlgebraMethod)
  : MetaModelResult(inputSample, outputSample, metaModel),
    regressionMatrix_(regressionMatrix),
    basis_(basis),
    beta_(trendCoefficients),
    covarianceModel_(covarianceModel),
    optimalLogLikelihood_(optimalLogLikelihood),
    linearAlgebraMethod_(linearAlgebraMethod),
    hasCholeskyFactor_(false),
    covarianceCholeskyFactor_(),
    covarianceHMatrix_()
{
  const UnsignedInteger size = inputSample.getSize();
  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "In GaussianProcessFitterResult::GaussianProcessFitterResult, input & output sample have different size. input sample size = " << size << ", output sample size = " << outputSample.getSize();
}


/* Virtual constructor */
GaussianProcessFitterResult * GaussianProcessFitterResult::clone() const
{
  return new GaussianProcessFitterResult(*this);
}


/* String converter */
String GaussianProcessFitterResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", covariance model=" << covarianceModel_
         << ", basis=" << basis_
         << ", trend coefficients=" << beta_;
}

String GaussianProcessFitterResult::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << "covariance model=" << covarianceModel_.__str__(offset)
      << ", basis=" << basis_.__str__(offset)
      << ", trend coefficients=" << beta_.__str__(offset) << ")";
  return oss;
}

/* Basis accessor */
Basis GaussianProcessFitterResult::getBasis() const
{
  return basis_;
}

/* Trend coefficients accessor */
Point GaussianProcessFitterResult::getTrendCoefficients() const
{
  return beta_;
}

/* Covariance models accessor */
CovarianceModel GaussianProcessFitterResult::getCovarianceModel() const
{
  return covarianceModel_;
}

/** Regression matrix accessor */
Matrix GaussianProcessFitterResult::getRegressionMatrix() const
{
  return regressionMatrix_;
}

Scalar GaussianProcessFitterResult::getOptimalLogLikelihood() const
{
  return optimalLogLikelihood_;
}

GaussianProcessFitterResult::LinearAlgebra GaussianProcessFitterResult::getLinearAlgebraMethod() const
{
  return linearAlgebraMethod_;
}

/* process accessor */
Process GaussianProcessFitterResult::getNoise() const
{
  // Define noise process
  if (getCovarianceModel().getClassName() == "DiracCovarianceModel")
  {
    // Here it is assumed that the covariance model parameters are the
    // marginal amplitude.
    const Point sigma(getCovarianceModel().getParameter());
    const CorrelationMatrix R(getCovarianceModel().getOutputCorrelation());
    const Normal dist(Point(sigma.getSize(), 0.0), sigma, R);
    const WhiteNoise noise(dist);
    return noise;
  }
  // Other covariance models
  const GaussianProcess noise(getCovarianceModel(), Mesh(getInputSample()));
  return noise;
}

/* Method that returns the covariance factor - Lapack */
TriangularMatrix GaussianProcessFitterResult::getCholeskyFactor() const
{
  return covarianceCholeskyFactor_;
}

/** rho accessor */
Point GaussianProcessFitterResult::getStandardizedOutput() const
{
  return rho_;
}

/** rho accessor */
void GaussianProcessFitterResult::setStandardizedOutput(const Point & rho)
{
  rho_ = rho;
}

void GaussianProcessFitterResult::setCholeskyFactor(const TriangularMatrix & covarianceCholeskyFactor,
    const HMatrix & covarianceHMatrix)
{
  const UnsignedInteger size = getInputSample().getSize();
  const UnsignedInteger outputDimension = getMetaModel().getOutputDimension();
  if (covarianceCholeskyFactor_.getDimension() != 0 && covarianceCholeskyFactor_.getDimension() != size * outputDimension)
    throw InvalidArgumentException(HERE) << "In GaussianProcessFitterResult::setCholeskyFactor, Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceCholeskyFactor_.getDimension();
  if (covarianceHMatrix_.getNbRows() != 0)
  {
    if (covarianceHMatrix_.getNbRows() != covarianceHMatrix_.getNbColumns())
      throw InvalidArgumentException(HERE) << "In GaussianProcessFitterResult::setCholeskyFactor, HMAT Cholesky factor is not square. Its dimension is " << covarianceHMatrix_.getNbRows() << "x" << covarianceHMatrix_.getNbColumns();
    if (covarianceHMatrix_.getNbRows() != size * outputDimension)
      throw InvalidArgumentException(HERE) << "In GaussianProcessFitterResult::setCholeskyFactor, HMAT Cholesky factor has unexpected dimensions. Its dimension should be " << size * outputDimension << ". Here dimension = " << covarianceHMatrix_.getNbRows();
  }
  covarianceCholeskyFactor_ = covarianceCholeskyFactor;
  covarianceHMatrix_ = covarianceHMatrix;
}

/* Method that returns the covariance factor - hmat */
HMatrix GaussianProcessFitterResult::getHMatCholeskyFactor() const
{
  return covarianceHMatrix_;
}


/* Method save() stores the object through the StorageManager */
void GaussianProcessFitterResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute("regressionMatrix_", regressionMatrix_);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("beta_", beta_ );
  adv.saveAttribute("covarianceModel_", covarianceModel_ );
  adv.saveAttribute("rho_", covarianceModel_);
  adv.saveAttribute("optimalLogLikelihood_", optimalLogLikelihood_);
  UnsignedInteger linearAlgebraMethod  = static_cast<LinearAlgebra>(linearAlgebraMethod_);
  adv.saveAttribute("linearAlgebraMethod_", linearAlgebraMethod);
  adv.saveAttribute("hasCholeskyFactor_", hasCholeskyFactor_);
  adv.saveAttribute("covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}


/* Method load() reloads the object from the StorageManager */
void GaussianProcessFitterResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute("regressionMatrix_", regressionMatrix_);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("beta_", beta_ );
  adv.loadAttribute("covarianceModel_", covarianceModel_ );
  adv.loadAttribute("rho_", covarianceModel_);
  adv.loadAttribute("optimalLogLikelihood_", optimalLogLikelihood_);
  UnsignedInteger linearAlgebraMethod = 0;
  adv.loadAttribute( "linearAlgebraMethod_", linearAlgebraMethod );
  linearAlgebraMethod_ = static_cast<LinearAlgebra>(linearAlgebraMethod);
  adv.loadAttribute("hasCholeskyFactor_", hasCholeskyFactor_);
  adv.loadAttribute("covarianceCholeskyFactor_", covarianceCholeskyFactor_);
}



END_NAMESPACE_OPENTURNS
