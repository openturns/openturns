//                                               -*- C++ -*-
/**
 *  @brief SimulationAlgorithm implementation base class
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SIMULATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_SIMULATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/HistoryStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationAlgorithmImplementation
 */

class OT_API SimulationAlgorithmImplementation
  : public PersistentObject
{

  CLASSNAME
public:

  /** Constructor with parameters */
  SimulationAlgorithmImplementation();

  /** Virtual constructor */
  SimulationAlgorithmImplementation * clone() const override;

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
  void setProgressCallback(ProgressCallback callBack, void * state = nullptr);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  void setStopCallback(StopCallback callBack, void * state = nullptr);

  /** Maximum time accessor */
  void setMaximumTimeDuration(const Scalar maximumTimeDuration);
  Scalar getMaximumTimeDuration() const;

  using PersistentObject::operator==;
  /** Comparison operator */
  Bool operator ==(const PersistentObject & other) const override;

  using PersistentObject::operator!=;
  /** Comparison operator */
  Bool operator !=(const PersistentObject & other) const override;

protected:
  // Size of the atomic blocks of computation
  UnsignedInteger blockSize_ = 0;

  // callbacks
  std::pair< ProgressCallback, void *> progressCallback_;
  std::pair< StopCallback, void *> stopCallback_;

  /** History strategy for the probability and variance estimate */
  HistoryStrategy convergenceStrategy_;

private:

  // Maximum number of outer iteration allowed
  UnsignedInteger maximumOuterSampling_ = 0;

  // Maximum coefficient of variation allowed for convergence
  Scalar maximumCoefficientOfVariation_ = 0.0;

  // Maximum standard deviation allowed for convergence
  Scalar maximumStandardDeviation_ = 0.0;

  Scalar maximumTimeDuration_ = -1.0;
} ; /* class SimulationAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONALGORITHMIMPLEMENTATION_HXX */
