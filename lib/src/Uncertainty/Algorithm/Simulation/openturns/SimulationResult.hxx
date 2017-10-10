//                                               -*- C++ -*-
/**
 *  @brief SimulationResult stores the simulation result
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_SIMULATIONRESULT_HXX
#define OPENTURNS_SIMULATIONRESULT_HXX

#include "openturns/SimulationResultImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SimulationResult
 * SimulationResult stores the simulation result
 */
class OT_API SimulationResult
  : public TypedInterfaceObject<SimulationResultImplementation>
{

  CLASSNAME
public:

  typedef Pointer<SimulationResultImplementation>                       Implementation;

  /** Default constructor */
  SimulationResult();

  /** Standard constructor */
  SimulationResult(const Event & event,
                   const Scalar probabilityEstimate,
                   const Scalar varianceEstimate,
                   const UnsignedInteger outerSampling,
                   const UnsignedInteger blockSize);

  /** Constructor with parameters */
  SimulationResult(const SimulationResultImplementation & implementation);

  /** Constructor with parameters */
  SimulationResult(Implementation & p_implementation);

  /** Event accessor */
  Event getEvent() const;

  /** Probability estimate accessor */
  Scalar getProbabilityEstimate() const;
  void setProbabilityEstimate(const Scalar probabilityEstimate);

  /** Variance estimate accessor */
  Scalar getVarianceEstimate() const;
  void setVarianceEstimate(const Scalar varianceEstimate);

  /** Coefficient of variation estimate accessor */
  Scalar getCoefficientOfVariation() const;

  /** Standard deviation estimate accessor */
  Scalar getStandardDeviation() const;

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
  Scalar getConfidenceLength(const Scalar level = ResourceMap::GetAsScalar("SimulationResult-DefaultConfidenceLevel")) const;

  /** String converter */
  String __repr__() const;

private:

}; // class Result

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SIMULATIONRESULT_HXX */
