//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LOWDISCREPANCYEXPERIMENT_HXX
#define OPENTURNS_LOWDISCREPANCYEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/LowDiscrepancySequence.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LowDiscrepancyExperiment
 *
 * The class describes the probabilistic concept of monteCarloExperiment plan
 */
class OT_API LowDiscrepancyExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  typedef Collection<Distribution> DistributionCollection;

  /** Default constructor */
  LowDiscrepancyExperiment();

  /** Parameters constructor */
  explicit LowDiscrepancyExperiment(const UnsignedInteger size,
                                    const Bool restart = true);

  /** Parameters constructor */
  LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
                           const UnsignedInteger size,
                           const Bool restart = true);

  /** Parameters constructor */
  LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
                           const Distribution & distribution,
                           const UnsignedInteger size,
                           const Bool restart = true);

  /** Virtual constructor */
  virtual LowDiscrepancyExperiment * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Distribution accessor */
  virtual void setDistribution(const Distribution & distribution);

  /** Sequence accessor */
  LowDiscrepancySequence getSequence() const;

  /** Restart accessor */
  Bool getRestart() const;
  void setRestart(const Bool restart);

  /** Randomization flag accessor */
  Bool getRandomize() const;
  void setRandomize(const Bool randomize);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Sample generateWithWeights(Point & weightsOut) const;

protected:

private:

  // Low discrepancy sequence
  LowDiscrepancySequence sequence_;

  // Initialization flag
  Bool restart_;

  // Randomization flag
  Bool randomize_;

  // Transformation from uniform space to distribution space
  Function transformation_;

}; /* class LowDiscrepancyExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOWDISCREPANCYEXPERIMENT_HXX */

