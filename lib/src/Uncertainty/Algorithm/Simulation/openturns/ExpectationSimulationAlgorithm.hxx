//                                               -*- C++ -*-
/**
 *  @brief Simulation algorithm to estimate an expectation
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
#ifndef OPENTURNS_EXPECTATIONSIMULATIONALGORITHM_HXX
#define OPENTURNS_EXPECTATIONSIMULATIONALGORITHM_HXX

#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/ExpectationSimulationResult.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/HistoryStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExpectationSimulationAlgorithm
 */

class OT_API ExpectationSimulationAlgorithm
  : public SimulationAlgorithm
{

  CLASSNAME
public:
  /** Default constructor */
  ExpectationSimulationAlgorithm();

  /** Constructor with parameters */
  explicit ExpectationSimulationAlgorithm(const RandomVector & randomVector);

  /** Virtual constructor */
  ExpectationSimulationAlgorithm * clone() const override;

  /** RandomVector accessor */
  RandomVector getRandomVector() const;

  /** Result accessor */
  ExpectationSimulationResult getResult() const;

  /** Criterion type on coefficient of variation accessor */
  void setCoefficientOfVariationCriterionType(const String & criterionType);
  String getCoefficientOfVariationCriterionType() const;

  /** Criterion type on standard deviation accessor */
  void setStandardDeviationCriterionType(const String & criterionType);
  String getStandardDeviationCriterionType() const;

  /** Maximum of standard deviation on each component */
  void setMaximumStandardDeviationPerComponent(const Point & maximumStandardDeviation);
  Point getMaximumStandardDeviationPerComponent() const;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run() override;

  /** Draw the probability convergence at the given level */
  Graph drawExpectationConvergence(const UnsignedInteger marginalIndex = 0,
                                   const Scalar level = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel")) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // compute criterion on a quantity
  Scalar computeCriterion(const String & criterionType, const Point & values);

  /** Result accessor */
  void setResult(const ExpectationSimulationResult & result);

  // The vector to study
  RandomVector randomVector_;

  // Result of the simulation
  ExpectationSimulationResult result_;

  // Criterion type to apply on coefficient of variation / standard deviation: NONE, MAX, NORM1, NORM2
  String coefficientOfVariationCriterionType_;
  String standardDeviationCriterionType_;

  // Maximum of standard deviation on each component
  Point maximumStandardDeviationPerComponent_;

} ; /* class ExpectationSimulationAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPECTATIONSIMULATIONALGORITHM_HXX */
