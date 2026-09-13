//                                               -*- C++ -*-
/**
 *  @brief The Bingham distribution
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
#ifndef OPENTURNS_BINGHAM_HXX
#define OPENTURNS_BINGHAM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API Bingham
  : public DistributionImplementation
{
  CLASSNAME
public:

  Bingham();

  Bingham(const Point & zeta,
          const SquareMatrix & gamma,
          const Scalar epsilon = ResourceMap::GetAsScalar("Bingham-OrthogonalityThreshold"));

  using DistributionImplementation::operator ==;
  Bool operator ==(const Bingham & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  Bingham * clone() const override;

  Point getRealization() const override;

  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  Description getParameterDescription() const override;

  /** Zeta (concentration parameters) accessor */
  void setZeta(const Point & zeta);
  Point getZeta() const;

  /** Gamma (orientation matrix) accessor */
  void setGamma(const SquareMatrix & gamma);
  SquareMatrix getGamma() const;

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

  friend class BinghamFactory;

  /** Compute the normalization constant using an exact series expansion */
  void computeNormalization();

  void computeMean() const override;
  void computeCovariance() const override;

  /** Compute the normalization constant associated with the given
      concentration vector on the log scale */
  Scalar computeLogNormalizationConstant(const Point & zeta) const;

  /** Compute the expectation of the squared coordinates in the
      diagonalizing frame using a finite difference of the series */
  Point computeSecondMoments() const;

  /** Update the sampler parameters */
  void updateSampler();

  /** Dimension of the sphere (S^{dimension-1}) */
  UnsignedInteger dimension_;

  /** Concentration parameters (diagonal of Z) */
  Point zeta_;

  /** Orthogonal orientation matrix */
  SquareMatrix gamma_;

  Scalar epsilon_;
  Scalar logNormalization_;
  Scalar optimalB_;

}; /* class Bingham */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BINGHAM_HXX */