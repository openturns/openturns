//                                               -*- C++ -*-
/**
 *  @brief NAIS implement Non Parametric Adaptive Importance Sampling algorithm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NAIS_HXX
#define OPENTURNS_NAIS_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/NAISResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAIS
 *
 * NAIS is some nais type to illustrate how to add some classes in OpenTURNS
 */
class OT_API NAIS
  : public EventSimulation
{

  CLASSNAME
public:

  /** Default constructor */
  NAIS();

  /** Default constructor */
  NAIS(const RandomVector & event,
       const Scalar rho_quantile = 0.7);
       
  /** Virtual constructor */
  NAIS * clone() const override;

  /** Get weights */
  Point getWeights() const;
  
  /** Get auxiliary input sample */
  Sample getAuxiliaryInputSample() const;

  /** Get rhoQuantile */
  Scalar getRhoQuantile() const;
  
  /** Set rhoQuantile */
  void setRhoQuantile(const Scalar & rhoQuantile);

  /** Get outputsamples */
  Sample getAuxiliaryOutputSample() const;

  /** Main function that computes the failure probability */
  void run() override;

  /** Accessor to results */
  NAISResult getResult() const;

private:

  /** Function computing the auxiliary distribution as a function of current samples and associated weights */
  Distribution computeAuxiliaryDistribution(const Sample & sample,
      const Point & weights);

  /** Function computing weigths  of sample */
  Point computeWeights(const Sample & sample,
                       const Sample & respectiveSamples,
                       const Scalar eventThresholdLocal,
                       const Distribution & AuxiliaryDistribution);
                       
  // Current weights samples
  Point weights_;

  // Current Sample
  Sample auxiliaryInputSample_;

  // Current auxiliary output sample
  Sample auxiliaryOutputSample_;

  // Initial distribution
  Distribution initialDistribution_;

  // Quantile
  Scalar rhoQuantile_;

  // Result of NAIS algorithm
  NAISResult naisResult_;
  
  // Not Implemented
  Graph drawProbabilityConvergence(const Scalar level) const;
  HistoryStrategy getConvergenceStrategy() const;
  Scalar getMaximumCoefficientOfVariation() const;
  Scalar getMaximumStandardDeviation() const;
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy) const;
  void setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation) const;  

}; /* class NAIS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIS_HXX */
