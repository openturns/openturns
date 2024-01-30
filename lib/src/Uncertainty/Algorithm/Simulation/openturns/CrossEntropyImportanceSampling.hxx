//                                               -*- C++ -*-
/**
 *  @brief CrossEntropyImportanceSampling implement Importance Sampling by Cross Entropy algorithm
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
#ifndef OPENTURNS_CROSSENTROPYIMPORTANCESAMPLING_HXX
#define OPENTURNS_CROSSENTROPYIMPORTANCESAMPLING_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/CrossEntropyResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CrossEntropyImportanceSampling
 *
 * CrossEntropyImportanceSampling is some CrossEntropyImportanceSampling type to illustrate how to add some classes in OpenTURNS
 */
class OT_API CrossEntropyImportanceSampling
  : public EventSimulation
{

  CLASSNAME
public:

  /** Default constructor */
  CrossEntropyImportanceSampling();

  /** Default constructor */
  explicit  CrossEntropyImportanceSampling(const RandomVector & event,
      const Scalar quantileLevel = ResourceMap::GetAsScalar("CrossEntropyImportanceSampling-DefaultQuantileLevel"));

  /** Virtual constructor */
  CrossEntropyImportanceSampling * clone() const override;

  /** Get quantileLevel */
  Scalar getQuantileLevel() const;

  /** Set quantileLevel */
  void setQuantileLevel(const Scalar & quantileLevel);

  /** Main function that computes the failure probability */
  void run() override;

  /** Accessor to results */
  CrossEntropyResult getResult() const;
  
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

protected:

  /** Function computing the output samples from the input samples*/
  virtual Sample computeOutputSamples(const Sample & inputSamples) const;

  /** Function updating the auxiliary distribution as a function of auxiliary distribution parameters */
  virtual void updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters);

  /** Function optimizing the auxiliary distribution parameters*/
  virtual Point optimizeAuxiliaryDistributionParameters(const Sample & auxiliaryCriticInputSamples) const;

  /** Function updating the auxiliary distribution with initial parameters (in case of multiple runs of algorithm) */
  virtual void resetAuxiliaryDistribution();

  /** Select sample indices according to status */
  Indices getSampleIndices(const UnsignedInteger step, const Bool status) const;

  // Initial distribution
  Distribution initialDistribution_;

  // Auxiliary distribution
  Distribution auxiliaryDistribution_;

  // Quantile
  Scalar quantileLevel_ = 0.0;

  // Result of CrossEntropyImportanceSampling algorithm
  CrossEntropyResult crossEntropyResult_;
  
  // some results
  UnsignedInteger numberOfSteps_ = 0;// number of subset steps
  Point thresholdPerStep_;// intermediate thresholds

  // keep samples generated at each step
  Bool keepSample_ = false;
  PersistentCollection<Sample> inputSample_;
  PersistentCollection<Sample> outputSample_;

}; /* class CrossEntropyImportanceSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CROSSENTROPYIMPORTANCESAMPLING_HXX */
