//                                               -*- C++ -*-
/**
 *  @brief The MatrixFisher distribution on SO(3)
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
#ifndef OPENTURNS_MATRIXFISHER_HXX
#define OPENTURNS_MATRIXFISHER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API MatrixFisher
  : public DistributionImplementation
{
  CLASSNAME
public:

  MatrixFisher();

  MatrixFisher(const SquareMatrix & F);

  using DistributionImplementation::operator ==;
  Bool operator ==(const MatrixFisher & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  MatrixFisher * clone() const override;

  Point getRealization() const override;

  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  Description getParameterDescription() const override;

  /** Parameter matrix F accessor */
  void setF(const SquareMatrix & F);
  SquareMatrix getF() const;

  /** Epsilon accessor */
  void setEpsilon(const Scalar epsilon);
  Scalar getEpsilon() const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  Bool isContinuous() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

protected:

  void computeRange() override;

private:

  friend class MatrixFisherFactory;

  /** Compute the normalization constant a_0(F) */
  void computeNormalization();

  /** Update the sampler parameters (SVD of F) */
  void updateSampler();

  /** Sample a uniform random element of SO(3) using the unit-quaternion trick */
  SquareMatrix sampleUniformRotation() const;

  void computeMean() const override;
  void computeCovariance() const override;

  /** Parameter matrix F (3x3) */
  SquareMatrix F_;

  /** SVD of F: F = U * diag(sigma) * V^T */
  SquareMatrix U_;
  SquareMatrix V_;
  Point singularValues_;

  Scalar epsilon_;
  Scalar logNormalization_;
  Scalar maxTrace_;

  /** Moments of R, stored for the mean/covariance/entropy */
  SquareMatrix expectedMatrix_;
  SquareMatrix expectedSquaredMatrix_;
  Scalar expectedTrace_;

}; /* class MatrixFisher */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIXFISHER_HXX */