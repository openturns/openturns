//                                               -*- C++ -*-
/**
 *  @brief The JointByConditioningDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_JOINTBYCONDITIONINGDISTRIBUTION_HXX
#define OPENTURNS_JOINTBYCONDITIONINGDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Function.hxx"
#include "openturns/DeconditionedDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class JointByConditioningDistribution
 *
 * The JointByConditioningDistribution distribution.
 */
class OT_API JointByConditioningDistribution
  : public DistributionImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  JointByConditioningDistribution();

  /** Parameters constructor */
  JointByConditioningDistribution(const Distribution & conditionedDistribution,
                                  const Distribution & conditioningDistribution,
                                  const Function & linkFunction = Function());

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const JointByConditioningDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  JointByConditioningDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Compute the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /* Interface specific to JointByConditioningDistribution */
  /** Compute the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Conditioned distribution accessor */
  void setConditionedDistribution(const Distribution & conditionedDistribution);
  Distribution getConditionedDistribution() const;

  /** Conditioning distribution accessor */
  void setConditioningDistribution(const Distribution & conditioningDistribution);
  Distribution getConditioningDistribution() const;

  /** Link function accessor */
  void setLinkFunction(const Function & linkFunction);
  Function getLinkFunction() const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Compute the numerical range of the distribution */
  void computeRange() override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Method to set simultaneously the conditioning distribution, the conditioned distribution and the link function */
  void setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
      const Distribution & conditioningDistribution,
      const Function & linkFunction);

  /** The conditioned distribution, i.e L(X|Theta) */
  Distribution conditionedDistribution_;

  /** The conditioning distribution, i.e L(Theta) */
  Distribution conditioningDistribution_;

  /** Values of the Dirac marginals */
  Function linkFunction_;

  /** Deconditioned */
  DeconditionedDistribution deconditioned_;
}; /* class JointByConditioningDistribution */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JOINTBYCONDITIONINGDISTRIBUTION_HXX */
