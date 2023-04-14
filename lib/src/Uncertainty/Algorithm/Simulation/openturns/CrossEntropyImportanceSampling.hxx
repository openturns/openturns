//                                               -*- C++ -*-
/**
 *  @brief CrossEntropyImportanceSampling implement Importance Sampling by Cross Entropy algorithm
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
                 const Scalar rhoQuantile = ResourceMap::GetAsScalar("CrossEntropyImportanceSampling-DefaultRhoQuantile"));

  /** Virtual constructor */
  CrossEntropyImportanceSampling * clone() const override;

  /** Get rhoQuantile */
  Scalar getRhoQuantile() const;

  /** Set rhoQuantile */
  void setRhoQuantile(const Scalar & rhoQuantile);
  
  /** Main function that computes the failure probability */
  void run() override;

  /** Accessor to results */
  CrossEntropyResult getResult() const;

protected:

  /** Function computing the output samples from the input samples*/
  virtual Sample computeOutputSamples(const Sample & inputSamples) const;
  
  /** Function updating the auxiliary distribution as a function of auxiliary distribution parameters */
  virtual void updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters);
  
  /** Function optimizing the auxiliary distribution parameters*/
  virtual Point optimizeAuxiliaryDistributionParameters(const Sample & auxiliaryCriticInputSamples) const;

  // Initial distribution
  Distribution initialDistribution_;
  
  // Auxiliary distribution
  Distribution auxiliaryDistribution_;
  
  // Current auxiliary distribution parameters
  Point currentAuxiliaryDistributionParameters;

  // Quantile
  Scalar rhoQuantile_ = 0.0;

  // Result of CrossEntropyImportanceSampling algorithm
  CrossEntropyResult crossEntropyResult_;

}; /* class CrossEntropyImportanceSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CROSSENTROPYIMPORTANCESAMPLING_HXX */
