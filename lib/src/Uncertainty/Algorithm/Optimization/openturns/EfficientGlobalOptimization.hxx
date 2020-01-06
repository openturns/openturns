//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
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
#ifndef OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX
#define OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/KrigingResult.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class EfficientGlobalOptimization
*/

class OT_API EfficientGlobalOptimization
  : public OptimizationAlgorithmImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  EfficientGlobalOptimization();

  /** Constructor with parameters */
  EfficientGlobalOptimization(const OptimizationProblem & problem,
                              const KrigingResult & krigingResult);

  /** Virtual constructor */
  virtual EfficientGlobalOptimization * clone() const;

  /** String converter */
  String __repr__() const;

  /** Performs the actual computation. */
  void run();

  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Size of the design to draw starting points */
  UnsignedInteger getMultiStartExperimentSize() const;
  void setMultiStartExperimentSize(const UnsignedInteger multiStartExperimentSize);

  /** Number of starting points for the criterion optim */
  UnsignedInteger getMultiStartNumber() const;
  void setMultiStartNumber(const UnsignedInteger multiStartNumberSize);

  /** Parameter estimation period accessor */
  UnsignedInteger getParameterEstimationPeriod() const;
  void setParameterEstimationPeriod(const UnsignedInteger parameterEstimationPeriod);

  /** improvement stopping criterion factor accessor */
  void setImprovementFactor(const Scalar improvementFactor);
  Scalar getImprovementFactor() const;

  /** correlation length stopping criterion factor accessor */
  void setCorrelationLengthFactor(const Scalar b);
  Scalar getCorrelationLengthFactor() const;

  /** AEI tradeoff constant accessor */
  void setAEITradeoff(const Scalar c);
  Scalar getAEITradeoff() const;

  /** Improvement noise function accessor */
  void setNoiseModel(const Function & noiseModel);
  Function getNoiseModel() const;

  /** Expected improvement function */
  Sample getExpectedImprovement() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Check whether this problem can be solved by this solver. */
  void checkProblem(const OptimizationProblem & problem) const;

private:
  KrigingResult krigingResult_;
  OptimizationAlgorithm solver_;

  // whether the solver was set
  mutable Bool useDefaultSolver_;

  // Size of the design to draw starting points
  UnsignedInteger multiStartExperimentSize_;

  // number of starting points for criterion optim
  UnsignedInteger multiStartNumber_;

  // relearn kriging parameters every X iterations
  UnsignedInteger parameterEstimationPeriod_;

  // improvement stopping criterion factor
  Scalar improvementFactor_;

  // correlation length stopping criterion factor
  Scalar correlationLengthFactor_;

  // AEI tradeoff constant u(x)=mk(x)+c*sk(x)
  Scalar aeiTradeoff_;

  // optional noise model for improvement optimization only
  // for the metamodel noise the 2nd marginal of the objective is used
  Function noiseModel_;

  mutable Sample expectedImprovement_;

} ; /* class EfficientGlobalOptimization */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX */
