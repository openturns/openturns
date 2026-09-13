//                                               -*- C++ -*-
/**
 *  @brief The Wrapped Normal distribution on Torus T^n and Circle S^1
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
#ifndef OPENTURNS_WRAPPEDNORMAL_HXX
#define OPENTURNS_WRAPPEDNORMAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API WrappedNormal
  : public DistributionImplementation
{
  CLASSNAME
public:

  WrappedNormal();

  WrappedNormal(const Point & mu,
                const CovarianceMatrix & sigma,
                const Scalar period = 2.0 * M_PI);

  using DistributionImplementation::operator ==;
  Bool operator ==(const WrappedNormal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  WrappedNormal * clone() const override;

  Point getRealization() const override;

  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  Description getParameterDescription() const override;

  /** Mean (on torus) accessor */
  void setMu(const Point & mu);
  Point getMu() const;

  /** Covariance accessor */
  void setSigma(const CovarianceMatrix & sigma);
  CovarianceMatrix getSigma() const;

  /** Period accessor */
  void setPeriod(const Scalar period);
  Scalar getPeriod() const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  Bool isContinuous() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

protected:

  void computeRange() override;

private:

  friend class WrappedNormalFactory;

  /** Compute the normalization constant */
  void computeNormalization();

  void computeMean() const override;
  void computeCovariance() const override;

  /** Wrap angle to [-period/2, period/2] */
  Point wrap(const Point & x) const;

  /** Dimension of the torus */
  UnsignedInteger dimension_;

  /** Mean vector on torus */
  Point mu_;

  /** Covariance matrix (in tangent space) */
  CovarianceMatrix sigma_;

  /** Period (2*pi for standard torus) */
  Scalar period_;

  Scalar logNormalization_;
  SquareMatrix sigmaInv_;
  Scalar sigmaDet_;

}; /* class WrappedNormal */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WRAPPEDNORMAL_HXX */