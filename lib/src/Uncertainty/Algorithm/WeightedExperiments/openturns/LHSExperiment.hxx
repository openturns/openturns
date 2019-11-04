//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an LHSExperiment plane
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LHSEXPERIMENT_HXX
#define OPENTURNS_LHSEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Collection.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LHSExperiment
 *
 * The class describes the probabilistic concept of LHSExperiment plan
 */
class OT_API LHSExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:

  typedef Collection<Distribution> DistributionCollection;

  /** Default constructor */
  LHSExperiment();

  /** Parameters constructor */
  explicit LHSExperiment(const UnsignedInteger size,
                         const Bool alwaysShuffle = false,
                         const Bool randomShift = true);

  /** Parameters constructor */
  LHSExperiment(const Distribution & distribution,
                const UnsignedInteger size,
                const Bool alwaysShuffle = false,
                const Bool randomShift = true);

  /** Virtual constructor */
  virtual LHSExperiment * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Sample generateStandard() const;
  Sample generateWithWeights(Point & weightsOut) const;

  /** Shuffle the cells. */
  static Matrix ComputeShuffle(const UnsignedInteger dimension,
                               const UnsignedInteger totalSize);

  /** Shuffle accessor */
  virtual Matrix getShuffle() const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** AlwaysShuffle accessor */
  Bool getAlwaysShuffle() const;
  void setAlwaysShuffle(const Bool alwaysShuffle);

  /** Random shift accessor */
  Bool getRandomShift() const;
  void setRandomShift(const Bool randomShift);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  // Marginal distributions
  mutable DistributionCollection marginals_;

  // Cells shuffle
  mutable Matrix shuffle_;

  // Flag to avoid redundant computation of the shuffle
  mutable Bool isAlreadyComputedShuffle_;

  // Initialization flag
  Bool alwaysShuffle_;

  // Random shift flag
  Bool randomShift_;

  // Transformation from uniform space to distribution space
  MarginalTransformationEvaluation transformation_;

}; /* class LHSExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LHSEXPERIMENT_HXX */

