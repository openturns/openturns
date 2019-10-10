//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX
#define OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Event.hxx"
#include "openturns/SimulationResult.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilitySimulationResult
 * Implementation of SimulationResult
 */
class OT_API ProbabilitySimulationResult
  : public SimulationResult
{

  CLASSNAME
public:


  /** Default constructor */
  ProbabilitySimulationResult();

  /** Standard constructor */
  ProbabilitySimulationResult(const RandomVector & event,
                              const Scalar probabilityEstimate,
                              const Scalar varianceEstimate,
                              const UnsignedInteger outerSampling,
                              const UnsignedInteger blockSize);

  /** Virtual constructor */
  virtual ProbabilitySimulationResult * clone() const;

  /** Event accessor */
  RandomVector getEvent() const;
  void setEvent(const RandomVector & event);

  /** Probability estimate accessor */
  Scalar getProbabilityEstimate() const;
  void setProbabilityEstimate(const Scalar probabilityEstimate);

  /** Variance estimate accessor */
  Scalar getVarianceEstimate() const;
  void setVarianceEstimate(const Scalar varianceEstimate);

  /** Probability estimate distribution accessor */
  Normal getProbabilityDistribution() const;

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

  /** Confidence length */
  virtual Scalar getConfidenceLength(const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /* The event we are interested in */
  RandomVector event_;
  Scalar probabilityEstimate_;
  Scalar varianceEstimate_;

}; // class ProbabilitySimulationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROBABILITYSIMULATIONRESULT_HXX */
