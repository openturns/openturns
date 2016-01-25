//                                               -*- C++ -*-
/**
 *  @brief The BayesDistribution distribution
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
#ifndef OPENTURNS_BAYESDISTRIBUTION_HXX
#define OPENTURNS_BAYESDISTRIBUTION_HXX

#include "OTprivate.hxx"
#include "ContinuousDistribution.hxx"
#include "Distribution.hxx"
#include "ResourceMap.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BayesDistribution
 *
 * The BayesDistribution distribution.
 */
class OT_API BayesDistribution
  : public ContinuousDistribution
{
  CLASSNAME;

public:

  /** Default constructor */
  BayesDistribution();

  /** Parameters constructor */
  BayesDistribution(const Distribution & conditionedDistribution,
                    const Distribution & conditioningDistribution,
                    const NumericalMathFunction & linkFunction);

  /** Parameters constructor */
  BayesDistribution(const Distribution & conditionedDistribution,
                    const Distribution & conditioningDistribution);

  /** Comparison operator */
  Bool operator ==(const BayesDistribution & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual BayesDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Compute the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /* Interface specific to BayesDistribution */

  /** Conditioned distribution accessor */
  void setConditionedDistribution(const Distribution & conditionedDistribution);
  Distribution getConditionedDistribution() const;

  /** Conditioning distribution accessor */
  void setConditioningDistribution(const Distribution & conditioningDistribution);
  Distribution getConditioningDistribution() const;

  /** Link function accessor */
  void setLinkFunction(const NumericalMathFunction & linkFunction);
  NumericalMathFunction getLinkFunction() const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Compute the numerical range of the distribution */
  void computeRange();

  /** Method to set simultaneously the conditioning distribution, the conditioned distribution and the link function */
  void setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
      const Distribution & conditioningDistribution,
      const NumericalMathFunction & linkFunction);

  /** The conditioned distribution, i.e L(X|Theta) */
  Distribution conditionedDistribution_;

  /** The conditioning distribution, i.e L(Theta) */
  Distribution conditioningDistribution_;

  /** Values of the Dirac marginals */
  NumericalMathFunction linkFunction_;

}; /* class BayesDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BAYESDISTRIBUTION_HXX */
