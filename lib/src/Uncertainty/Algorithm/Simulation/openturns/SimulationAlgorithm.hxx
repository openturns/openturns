//                                               -*- C++ -*-
/**
 *  @brief SimulationAlgorithm algorithms base class
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
#ifndef OPENTURNS_SIMULATIONALGORITHM_HXX
#define OPENTURNS_SIMULATIONALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SimulationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationAlgorithm
 *
 * SimulationAlgorithm is the interface to SimulationAlgorithmImplementation.
 */

class OT_API SimulationAlgorithm
  : public TypedInterfaceObject<SimulationAlgorithmImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  SimulationAlgorithm();

  /** Constructor from implementation */
  SimulationAlgorithm(const SimulationAlgorithmImplementation & implementation);

  /** Constructor from implementation pointer */
#ifndef SWIG
  SimulationAlgorithm(SimulationAlgorithmImplementation * p_implementation);
#endif

  /** Constructor from implementation */
  SimulationAlgorithm(const Implementation & p_implementation);

  /** Comparison operator */
  using TypedInterfaceObject<SimulationAlgorithmImplementation>::operator ==;
  Bool operator ==(const SimulationAlgorithm & other) const;

  /** Comparison operator */
  using TypedInterfaceObject<SimulationAlgorithmImplementation>::operator !=;
  Bool operator !=(const SimulationAlgorithm & other) const;

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
  void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run();

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** Progress callback */
  typedef void (*ProgressCallback)(Scalar, void * state);
  void setProgressCallback(ProgressCallback callBack, void * state = nullptr);

  /** Stop callback */
  typedef Bool (*StopCallback)(void * state);
  void setStopCallback(StopCallback callBack, void * state = nullptr);

  /** Maximum time accessor */
  void setMaximumTimeDuration(const Scalar maximumTimeDuration);
  Scalar getMaximumTimeDuration() const;

}; // class SimulationAlgorithm


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONALGORITHM_HXX */
