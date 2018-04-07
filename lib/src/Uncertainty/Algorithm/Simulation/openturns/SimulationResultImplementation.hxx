//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX
#define OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX

#include <cmath>
#include "openturns/PersistentObject.hxx"
#include "openturns/Event.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationResultImplementation
 * Implementation of SimulationResult
 */
class OT_API SimulationResultImplementation
  : public PersistentObject
{

  CLASSNAME
public:


  /** Default constructor */
  SimulationResultImplementation();

  /** Standard constructor */
  SimulationResultImplementation(const Event & event,
                                 const Scalar probabilityEstimate,
                                 const Scalar varianceEstimate,
                                 const UnsignedInteger outerSampling,
                                 const UnsignedInteger blockSize);

  /** Virtual constructor */
  virtual SimulationResultImplementation * clone() const;

  /** Event accessor */
  Event getEvent() const;
  void setEvent(const Event & event);

  /** Probability estimate accessor */
  Scalar getProbabilityEstimate() const;
  void setProbabilityEstimate(const Scalar probabilityEstimate);

  /** Variance estimate accessor */
  Scalar getVarianceEstimate() const;
  void setVarianceEstimate(const Scalar varianceEstimate);

  /** Coefficient of variation estimate accessor */
  virtual Scalar getCoefficientOfVariation() const;

  /** Standard deviation estimate accessor */
  virtual Scalar getStandardDeviation() const;

  /** Mean point conditioned to the event realization accessor */
  Point getMeanPointInEventDomain() const;

  /** Importance factors accessor */
  PointWithDescription getImportanceFactors() const;

  /** Draw the importance factors */
  Graph drawImportanceFactors() const;

  /** Sample size accessor */
  UnsignedInteger getOuterSampling() const;
  void setOuterSampling(const UnsignedInteger outerSampling);

  /** Block size accessor */
  UnsignedInteger getBlockSize() const;
  void setBlockSize(const UnsignedInteger blockSize);

  /** Confidence length */
  virtual Scalar getConfidenceLength(const Scalar level = ResourceMap::GetAsScalar("SimulationResult-DefaultConfidenceLevel")) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /* The event we are interested in */
  Event event_;
  Scalar probabilityEstimate_;
  Scalar varianceEstimate_;
  UnsignedInteger outerSampling_;
  UnsignedInteger blockSize_;

}; // class SimulationResultImplementation

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONRESULTIMPLEMENTATION_HXX */
