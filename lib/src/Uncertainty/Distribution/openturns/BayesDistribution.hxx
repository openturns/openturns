//                                               -*- C++ -*-
/**
 *  @brief The BayesDistribution distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BAYESDISTRIBUTION_HXX
#define OPENTURNS_BAYESDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BayesDistribution
 *
 * The BayesDistribution distribution.
 */
class OT_API BayesDistribution
  : public ContinuousDistribution
{
  CLASSNAME

public:

  /** Default constructor */
  BayesDistribution();

  /** Parameters constructor */
  BayesDistribution(const Distribution & conditionedDistribution,
                    const Distribution & conditioningDistribution,
                    const Function & linkFunction);

  /** Parameters constructor */
  BayesDistribution(const Distribution & conditionedDistribution,
                    const Distribution & conditioningDistribution);

  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const BayesDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  BayesDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Compute the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /* Interface specific to BayesDistribution */

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

}; /* class BayesDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BAYESDISTRIBUTION_HXX */
