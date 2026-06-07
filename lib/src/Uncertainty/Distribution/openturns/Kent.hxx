//                                               -*- C++ -*-
/**
 *  @brief The Kent (Fisher-Bingham) distribution
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
#ifndef OPENTURNS_KENT_HXX
#define OPENTURNS_KENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API Kent
  : public DistributionImplementation
{
  CLASSNAME
public:

  Kent();

  Kent(const Scalar kappa,
       const Scalar beta,
       const SquareMatrix & gamma,
       const Scalar epsilon = ResourceMap::GetAsScalar("Kent-OrthogonalityThreshold"));

  using DistributionImplementation::operator ==;
  Bool operator ==(const Kent & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  Kent * clone() const override;

  Point getRealization() const override;

  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  Description getParameterDescription() const override;

  /** Kappa accessor */
  void setKappa(const Scalar kappa);
  Scalar getKappa() const;

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** Gamma accessor */
  void setGamma(const SquareMatrix & gamma);
  SquareMatrix getGamma() const;

  /** Epsilon accessor */
  Scalar getEpsilon() const;

  /** Compute the first two moments r1=E[x1], r2=E[x2^2-x3^2] for given kappa,beta */
  static Point ComputeMoments(const Scalar kappa,
                              const Scalar beta);

  Bool isContinuous() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

protected:

  void computeRange() override;

private:

  void computeMean() const override;
  void computeNormalization();
  void updateSampler();

  Scalar kappa_;
  Scalar beta_;
  SquareMatrix gamma_;
  Scalar epsilon_;
  Scalar logNormalization_;
  Scalar optimalB_;

}; /* class Kent */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KENT_HXX */
