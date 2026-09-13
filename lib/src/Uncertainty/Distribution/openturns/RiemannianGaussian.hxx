//                                               -*- C++ -*-
/**
 *  @brief The Riemannian Gaussian distribution on SPD manifold
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
#ifndef OPENTURNS_RIEMANNIANGAUSSIAN_HXX
#define OPENTURNS_RIEMANNIANGAUSSIAN_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API RiemannianGaussian
  : public DistributionImplementation
{
  CLASSNAME
public:

  RiemannianGaussian();

  RiemannianGaussian(const SymmetricMatrix & mean,
                     const SquareMatrix & sigma,
                     const Scalar epsilon = ResourceMap::GetAsScalar("RiemannianGaussian-PositiveDefiniteThreshold"));

  using DistributionImplementation::operator ==;
  Bool operator ==(const RiemannianGaussian & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  RiemannianGaussian * clone() const override;

  Point getRealization() const override;

  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  Description getParameterDescription() const override;

  /** Mean (Frechet mean on SPD) accessor */
  void setMean(const SymmetricMatrix & mean);
  SymmetricMatrix getMeanMatrix() const;
  Point getMean() const override;

  /** Covariance (in tangent space at mean) accessor */
  void setSigma(const SquareMatrix & sigma);
  SquareMatrix getSigma() const;

  /** Epsilon accessor */
  Scalar getEpsilon() const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  Bool isContinuous() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

protected:

  void computeRange() override;

private:

  friend class RiemannianGaussianFactory;

  /** Compute the normalization constant */
  void computeNormalization();

  void computeMean() const override;
  void computeCovariance() const override;

  /** Update the sampler parameters */
  void updateSampler();

  /** Log map on SPD manifold */
  SymmetricMatrix logMap(const SymmetricMatrix & x) const;

  /** Exp map on SPD manifold */
  SymmetricMatrix expMap(const SymmetricMatrix & v) const;

  /** Dimension of SPD manifold (n*(n+1)/2) */
  UnsignedInteger dimension_;

  /** Dimension of matrix (n) */
  UnsignedInteger n_;

  /** Frechet mean on SPD */
  SymmetricMatrix meanMatrix_;

  /** Covariance in tangent space at mean */
  SquareMatrix sigma_;

  Scalar epsilon_;
  Scalar logNormalization_;
  SquareMatrix sigmaInv_;
  Scalar sigmaDet_;

}; /* class RiemannianGaussian */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RIEMANNIANGAUSSIAN_HXX */