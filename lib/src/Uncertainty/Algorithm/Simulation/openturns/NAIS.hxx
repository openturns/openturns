//                                               -*- C++ -*-
/**
 *  @brief NAIS implement Non Parametric Adaptive Importance Sampling algorithm
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
                 const Scalar quantileLevel = ResourceMap::GetAsScalar("NAIS-DefaultQuantileLevel"));

  /** Virtual constructor */
  NAIS * clone() const override;

  /** Get quantileLevel */
  Scalar getQuantileLevel() const;

  /** Set quantileLevel */
  void setQuantileLevel(const Scalar & quantileLevel);

  /** Main function that computes the failure probability */
  void run() override;

  /** Accessor to results */
  NAISResult getResult() const;
  
  
  /** Stepwise result accessors */
  Point getThresholdPerStep() const;
  
  /** Accessor to the achieved number of steps */
  UnsignedInteger getStepsNumber() const;
  
  /** Keep event sample */
  void setKeepSample(const Bool keepSample);
   
  /** Input/output sample accessor according to select flag */
  enum SelectSample {EVENT0, EVENT1, BOTH};
  Sample getInputSample(const UnsignedInteger step, const UnsignedInteger select = BOTH) const;
  Sample getOutputSample(const UnsignedInteger step, const UnsignedInteger select = BOTH) const;

private:

  /** Function computing the auxiliary distribution as a function of current samples and associated weights */
  Distribution computeAuxiliaryDistribution(const Sample & sample,
      const Point & weights);

  /** Function computing weigths  of sample */
  Point computeWeights(const Sample & sample,
                       const Sample & respectiveSamples,
                       const Scalar eventThresholdLocal,
                       const Distribution & AuxiliaryDistribution);


  /** Select sample indices according to status */
  Indices getSampleIndices(const UnsignedInteger step, const Bool status) const;

  // Initial distribution
  Distribution initialDistribution_;

  // Quantile
  Scalar quantileLevel_ = 0.0;

  // Result of NAIS algorithm
  NAISResult naisResult_;
  
  // some results
  UnsignedInteger numberOfSteps_ = 0;// number of subset steps
  Point thresholdPerStep_;// intermediate thresholds
<<<<<<< HEAD
  
=======


>>>>>>> be868a6659c83a6f1c872830a124a4b62369d01a
  // keep samples generated at each step
  Bool keepSample_ = false;
  PersistentCollection<Sample> inputSample_;
  PersistentCollection<Sample> outputSample_;
  
  
}; /* class NAIS */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIS_HXX */
