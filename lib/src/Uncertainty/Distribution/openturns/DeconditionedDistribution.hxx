//                                               -*- C++ -*-
/**
 *  @brief The DeconditionedDistribution distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DECONDITIONEDDISTRIBUTION_HXX
#define OPENTURNS_DECONDITIONEDDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Function.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DeconditionedDistribution
 *
 * The DeconditionedDistribution distribution.
 */
class OT_API DeconditionedDistribution
  : public Mixture
{
  CLASSNAME

  /** The PosteriorDistribution class is closely linked with the DeconditionedDistribution class
      as they are the two parts of the Bayesian modeling using distributions */
  friend class PosteriorDistribution;

public:

  /** Default constructor */
  DeconditionedDistribution();

  /** Parameters constructor */
  DeconditionedDistribution(const Distribution & conditionedDistribution,
                            const Distribution & conditioningDistribution);

  DeconditionedDistribution(const Distribution & conditionedDistribution,
                            const Distribution & conditioningDistribution,
                            const Function & linkFunction);

  /** Comparison operator */
  using Mixture::operator ==;
  Bool operator ==(const DeconditionedDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:


  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  DeconditionedDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to DeconditionedDistribution */

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** set both parameters */
  void setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
      const Distribution & conditioningDistribution,
      const Function & linkFunction);

  /** Compute the expectation of f(\theta)1_{\theta\leq \theta^*} with respect to the prior distribution of \theta */
  Point computeExpectation(const Function & f,
                           const Point & thetaStar) const;

  /** The conditioned distribution, i.e L(X|Theta) */
  Distribution conditionedDistribution_;

  /** The conditioning distribution, i.e L(Theta) */
  Distribution conditioningDistribution_;

  /** The link function */
  Function linkFunction_;

  /** Discrete marginals indices */
  Indices discreteMarginalsIndices_;

  /** Dirac marginals indices */
  Indices diracMarginalsIndices_;

  /** Continuous marginals indices */
  Indices continuousMarginalsIndices_;

  /** Lower bounds of the continuous marginals */
  Point continuousLowerBounds_;

  /** Upper bounds of the continuous marginals */
  Point continuousUpperBounds_;

  /** Standard continuous integration nodes */
  Sample continuousNodes_;

  /** Standard continuous weights */
  Point continuousWeights_;

  /** Discrete integration nodes */
  Sample discreteNodes_;

  /** Values of the Dirac marginals */
  Point diracValues_;

}; /* class DeconditionedDistribution */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DECONDITIONEDDISTRIBUTION_HXX */
