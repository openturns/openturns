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
#include "openturns/RatioOfUniforms.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API Kent
  : public DistributionImplementation
{
  CLASSNAME
public:

  Kent();

  Kent(const Scalar kappa,
       const Scalar beta,
       const Point & gamma1,
       const Point & gamma2,
       const Point & gamma3,
       const Scalar epsilon = 1e-15);

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

  /** Gamma1 accessor */
  void setGamma1(const Point & gamma1);
  Point getGamma1() const;

  /** Gamma2 accessor */
  void setGamma2(const Point & gamma2);
  Point getGamma2() const;

  /** Gamma3 accessor */
  void setGamma3(const Point & gamma3);
  Point getGamma3() const;

  /** Epsilon accessor */
  Scalar getEpsilon() const;

  Bool isContinuous() const override;

  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

protected:

  void computeRange() override;

private:

  void computeNormalization();
  void updateSampler();

  Scalar kappa_;
  Scalar beta_;
  Point gamma1_;
  Point gamma2_;
  Point gamma3_;
  Scalar epsilon_;
  Scalar normalization_;

  RatioOfUniforms sampler_;

}; /* class Kent */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KENT_HXX */
