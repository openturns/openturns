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
  explicit  NAIS(const RandomVector & event,
                 const Scalar rhoQuantile = ResourceMap::GetAsScalar("NAIS-DefaultRhoQuantile"));

  /** Virtual constructor */
  NAIS * clone() const override;

  /** Get rhoQuantile */
  Scalar getRhoQuantile() const;

  /** Set rhoQuantile */
  void setRhoQuantile(const Scalar & rhoQuantile);

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

  // Initial distribution
  Distribution initialDistribution_;

  // Quantile
  Scalar rhoQuantile_ = 0.0;

  // Result of NAIS algorithm
  NAISResult naisResult_;

}; /* class NAIS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIS_HXX */
