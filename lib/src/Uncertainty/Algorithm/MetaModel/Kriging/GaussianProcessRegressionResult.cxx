//                                               -*- C++ -*-
/**
 *  @brief The result of a gaussian process regressions estimation
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
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessRegressionResult)
static const Factory<GaussianProcessRegressionResult> Factory_GaussianProcessRegressionResult;

/* Default constructor */
GaussianProcessRegressionResult::GaussianProcessRegressionResult()
  : MetaModelResult()
{
  // Nothing to do
}

/* Constructor with parameters  */
GaussianProcessRegressionResult::GaussianProcessRegressionResult(const GaussianProcessFitterResult & result,
    const Sample & covarianceCoefficients)
  : MetaModelResult(result)
  , gpfResult_(result)
  , covarianceCoefficients_(covarianceCoefficients)
{
  const UnsignedInteger size = covarianceCoefficients.getSize();
  if (size != getInputSample().getSize())
    throw InvalidArgumentException(HERE) << "In GaussianProcessRegressionResult::GaussianProcessRegressionResult, covariance coefficients has incorrect size. Expected size = " << getInputSample().getSize() << ", covariance coefficients size = " << size;
}

/* Virtual constructor */
GaussianProcessRegressionResult * GaussianProcessRegressionResult::clone() const
{
  return new GaussianProcessRegressionResult(*this);
}

/* Covariance coefficients accessor */
Sample GaussianProcessRegressionResult::getCovarianceCoefficients() const
{
  return covarianceCoefficients_;
}

/* String converter */
String GaussianProcessRegressionResult::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", basis=" << getBasis()
         << ", trend coefficients=" << getTrendCoefficients()
         << ", covariance models=" << getCovarianceModel()
         << ", covariance coefficients=" << covarianceCoefficients_;
}

String GaussianProcessRegressionResult::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(";
  oss << "covariance models=" << getCovarianceModel();
  oss << ", covariance coefficients=" << covarianceCoefficients_;
  oss << ", basis=" << getBasis();
  oss << ", trend coefficients=" << getTrendCoefficients() << ")";
  return oss;
}

/* Basis accessor */
Basis GaussianProcessRegressionResult::getBasis() const
{
  return gpfResult_.getBasis();
}

/* Trend coefficients accessor */
Point GaussianProcessRegressionResult::getTrendCoefficients() const
{
  return gpfResult_.getTrendCoefficients();
}

/* Covariance models accessor */
CovarianceModel GaussianProcessRegressionResult::getCovarianceModel() const
{
  return gpfResult_.getCovarianceModel();
}

/** Regression matrix accessor */
Matrix GaussianProcessRegressionResult::getRegressionMatrix() const
{
  return gpfResult_.getRegressionMatrix();
}

Scalar GaussianProcessRegressionResult::getOptimalLogLikelihood() const
{
  return gpfResult_.getOptimalLogLikelihood();
}

GaussianProcessRegressionResult::LinearAlgebra GaussianProcessRegressionResult::getLinearAlgebraMethod() const
{
  return gpfResult_.getLinearAlgebraMethod();
}

/* process accessor */
Process GaussianProcessRegressionResult::getNoise() const
{
  return gpfResult_.getNoise();
}

/* Method that returns the covariance factor - Lapack */
TriangularMatrix GaussianProcessRegressionResult::getCholeskyFactor() const
{
  return gpfResult_.getCholeskyFactor();
}

/* Method that returns the covariance factor - hmat */
HMatrix GaussianProcessRegressionResult::getHMatCholeskyFactor() const
{
  return gpfResult_.getHMatCholeskyFactor();
}

/* Fitter result accessor */
GaussianProcessFitterResult GaussianProcessRegressionResult::getGaussianProcessFitterResult() const
{
  return gpfResult_;
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessRegressionResult::save(Advocate & adv) const
{
  MetaModelResult::save(adv);
  adv.saveAttribute( "gpfResult_", gpfResult_);
  adv.saveAttribute( "covarianceCoefficients_", covarianceCoefficients_ );
}


/* Method load() reloads the object from the StorageManager */
void GaussianProcessRegressionResult::load(Advocate & adv)
{
  MetaModelResult::load(adv);
  adv.loadAttribute("gpfResult_", gpfResult_);
  adv.loadAttribute("covarianceCoefficients_", covarianceCoefficients_);
}


END_NAMESPACE_OPENTURNS
