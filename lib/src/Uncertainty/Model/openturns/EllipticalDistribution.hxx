//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for elliptical distributions
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
#ifndef OPENTURNS_ELLIPTICALDISTRIBUTIONIMPLEMENTATION_HXX
#define OPENTURNS_ELLIPTICALDISTRIBUTIONIMPLEMENTATION_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/CorrelationMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EllipticalDistribution
 *
 * A subclass for elliptical usual distributions.
 */
class OT_API EllipticalDistribution
  : public ContinuousDistribution
{
  CLASSNAME;

public:

  // Numerical precision for computing the quantile

  /** Default constructor */
  EllipticalDistribution();

  /** Parameter constructor */
  EllipticalDistribution(const NumericalPoint & mean,
                         const NumericalPoint & sigma,
                         const CorrelationMatrix & R,
                         const NumericalScalar covarianceNormalizationFactor);

  /** Virtual copy constructor */
  virtual EllipticalDistribution * clone() const;

  /** Comparison operator */
  Bool operator ==(const EllipticalDistribution & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;

  /** Tell if the distribution is elliptical */
  Bool isElliptical() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Get the DDF of the distribution */
  using ContinuousDistribution::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the PDF gradient of the distribution */
  using ContinuousDistribution::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Compute the density generator of the elliptical distribution, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R^{-1}(x-mu))                      */
  virtual NumericalScalar computeDensityGenerator(const NumericalScalar betaSquare) const;
  virtual NumericalScalar computeLogDensityGenerator(const NumericalScalar betaSquare) const;

  /** Compute the derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const;

  /** Compute the second derivative of the density generator */
  virtual NumericalScalar computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const;

  /** Compute the survival function */
  using ContinuousDistribution::computeSurvivalFunction;
  virtual NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Mean point accessor */
  void setMean(const NumericalPoint & mean);

  /** Sigma vector accessor */
  void setSigma(const NumericalPoint & sigma);

  /** Sigma vector accessor */
  NumericalPoint getSigma() const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Correlation matrix accessor */
  void setCorrelation(const CorrelationMatrix & R);

  /** Correlation matrix accessor */
  CorrelationMatrix getCorrelation() const;

protected:
  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

public:
  /** Normalize the given point u_i = (x_i - mu_i) / sigma_i */
  NumericalPoint normalize(const NumericalPoint & x) const;

  /** Denormalize the given point x_i = mu_i + sigma_i * x_i */
  NumericalPoint denormalize(const NumericalPoint & u) const;

  /** Inverse correlation matrix accessor */
  SquareMatrix getInverseCorrelation() const;

  /** Cholesky factor of the correlation matrix accessor */
  TriangularMatrix getCholesky() const;

  /** Inverse of the Cholesky factor of the correlation matrix accessor */
  TriangularMatrix getInverseCholesky() const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution, i.e. a distribution of the same kind but with zero mean,
   * unit marginal standard distribution and identity correlation */
  Implementation getStandardDistribution() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The sigma vector of the distribution */
  mutable NumericalPoint sigma_;

  /** The correlation matrix (Rij) of the distribution */
  mutable CorrelationMatrix R_;

  /** The shape matrix of the distribution = Diag(sigma_) * R_ * Diag(sigma_) */
  mutable CovarianceMatrix shape_;

  /** The inverse of the correlation matrix of the distribution */
  SymmetricMatrix inverseR_;

  /** The Cholesky factor of the shape matrix shape_ = cholesky_ * cholesky_.transpose() */
  TriangularMatrix cholesky_;

  /** The inverse Cholesky factor of the covariance matrix */
  TriangularMatrix inverseCholesky_;

  /** The normalization factor of the distribution */
  NumericalScalar normalizationFactor_;

  /** The scaling factor of the covariance matrix covariance = covarianceScalingFactor_ * shape_*/
  NumericalScalar covarianceScalingFactor_;

private:
  /** Compute the value of the auxiliary attributes */
  void update();

}; /* class EllipticalDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ELLIPTICALDISTRIBUTIONIMPLEMENTATION_HXX */
