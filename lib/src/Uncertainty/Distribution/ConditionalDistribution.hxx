//                                               -*- C++ -*-
/**
 *  @brief The ConditionalDistribution distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_CONDITIONALDISTRIBUTION_HXX
#define OPENTURNS_CONDITIONALDISTRIBUTION_HXX

#include "OTprivate.hxx"
#include "Mixture.hxx"
#include "ResourceMap.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConditionalDistribution
 *
 * The ConditionalDistribution distribution.
 */
class OT_API ConditionalDistribution
  : public Mixture
{
  CLASSNAME;

  /** The PosteriorDistribution class is closely linked with the ConditionalDistribution class
      as they are the two parts of the Bayesian modeling using distributions */
  friend class PosteriorDistribution;

public:

  /** Default constructor */
  ConditionalDistribution();

  /** Parameters constructor */
  ConditionalDistribution(const Distribution & conditionedDistribution,
                          const Distribution & conditioningDistribution);

  ConditionalDistribution(const Distribution & conditionedDistribution,
                          const Distribution & conditioningDistribution,
                          const NumericalMathFunction & linkFunction);

  /** Comparison operator */
  Bool operator ==(const ConditionalDistribution & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual ConditionalDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Parameters value accessors */
  void setParameters(const NumericalPoint & parameters);
  NumericalPoint getParameters() const;

  /** Parameters description accessor */
  Description getParametersDescription() const;

  /* Interface specific to ConditionalDistribution */

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


private:

  /** set both parameters */
  void setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
      const Distribution & conditioningDistribution,
      const NumericalMathFunction & linkFunction);

  /** Compute the expectation of f(\theta)1_{\theta\leq \theta^*} with respect to the prior distribution of \theta */
  NumericalPoint computeExpectation(const NumericalMathFunction & f,
                                    const NumericalPoint & thetaStar) const;

  /** The conditioned distribution, i.e L(X|Theta) */
  Distribution conditionedDistribution_;

  /** The conditioning distribution, i.e L(Theta) */
  Distribution conditioningDistribution_;

  /** The link function */
  NumericalMathFunction linkFunction_;

  /** Discrete marginals indices */
  Indices discreteMarginalsIndices_;

  /** Dirac marginals indices */
  Indices diracMarginalsIndices_;

  /** Continuous marginals indices */
  Indices continuousMarginalsIndices_;

  /** Lower bounds of the continuous marginals */
  NumericalPoint continuousLowerBounds_;

  /** Upper bounds of the continuous marginals */
  NumericalPoint continuousUpperBounds_;

  /** Standard continuous integration nodes */
  NumericalSample continuousNodes_;

  /** Standard continuous weights */
  NumericalPoint continuousWeights_;

  /** Discrete integration nodes */
  NumericalSample discreteNodes_;

  /** Values of the Dirac marginals */
  NumericalPoint diracValues_;

}; /* class ConditionalDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONDITIONALDISTRIBUTION_HXX */
