//                                               -*- C++ -*-
/**
 *  @brief EventSimulation algorithms base class
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
#ifndef OPENTURNS_SIMULATIONALGORITHM_HXX
#define OPENTURNS_SIMULATIONALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/HistoryStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationAlgorithm
 */

class OT_API SimulationAlgorithm
  : public PersistentObject
{

  CLASSNAME
public:

  /** Constructor with parameters */
  SimulationAlgorithm();

  /** Virtual constructor */
  SimulationAlgorithm * clone() const override;

  /** Maximum sample size accessor */
  void setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling);
  UnsignedInteger getMaximumOuterSampling() const;

  /** Maximum coefficient of variation accessor */
  void setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation);
  Scalar getMaximumCoefficientOfVariation() const;

  /** Maximum standard deviation accessor */
  void setMaximumStandardDeviation(const Scalar maximumStandardDeviation);
  Scalar getMaximumStandardDeviation() const;

  /** Block size accessor */
  virtual void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

  /** Verbosity accessor */
  void setVerbose(const Bool verbose);
  Bool getVerbose() const;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  virtual void run();

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Progress callback */
  typedef void (*ProgressCallback)(Scalar, void * state);
  void setProgressCallback(ProgressCallback callBack, void * state = 0);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  void setStopCallback(StopCallback callBack, void * state = 0);

protected:
  // Size of the atomic blocks of computation
  UnsignedInteger blockSize_;

  // callbacks
  std::pair< ProgressCallback, void *> progressCallback_;
  std::pair< StopCallback, void *> stopCallback_;

  /** History strategy for the probability and variance estimate */
  HistoryStrategy convergenceStrategy_;

private:

  // Maximum number of outer iteration allowed
  UnsignedInteger maximumOuterSampling_;

  // Maximum coefficient of variation allowed for convergence
  Scalar maximumCoefficientOfVariation_;

  // Maximum standard deviation allowed for convergence
  Scalar maximumStandardDeviation_;

  // Do we have to echo all the intermediate results?
  Bool verbose_;

} ; /* class SimulationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONALGORITHM_HXX */
