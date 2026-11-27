//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/EfficientGlobalOptimizationCriterion.hxx"
#include "openturns/ExpectedImprovement.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class EfficientGlobalOptimization
 *
 * Efficient global optimization driven by a pluggable enrichment criterion.
 *
 * The algorithm owns the loop: growing design, Gaussian process refit, inner
 * solver and multi-start, stopping rules, callbacks and result. The enrichment
 * rule itself is a EfficientGlobalOptimizationCriterion, the expected
 * improvement by default, and can be replaced or subclassed from Python.
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
                              const GaussianProcessRegressionResult & gprResult);

  /** Virtual constructor */
  EfficientGlobalOptimization * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Performs the actual computation. */
  void run() override;

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

  /** correlation length stopping criterion factor accessor */
  void setCorrelationLengthFactor(const Scalar b);
  Scalar getCorrelationLengthFactor() const;

  /** Enrichment criterion accessor */
  void setCriterion(const EfficientGlobalOptimizationCriterion & criterion);
  EfficientGlobalOptimizationCriterion getCriterion() const;

  /**
   * AEI tradeoff constant accessor.
   * These two accessors apply to the expected improvement criterion only: they
   * throw when another criterion has been set, since the tradeoff has no
   * meaning outside the augmented expected improvement.
   */
  void setAEITradeoff(const Scalar c);
  Scalar getAEITradeoff() const;

  /** Improvement noise function accessor */
  void setNoiseFunction(const Function & noiseFunction);
  Function getNoiseFunction() const;

  /** Value of the criterion at each iteration */
  Sample getCriterionValues() const;

  /**
   * Value of the criterion at each iteration.
   * @deprecated Kept for compatibility: with a pluggable criterion the values
   * are no longer necessarily an expected improvement. Use getCriterionValues().
   */
  Sample getExpectedImprovement() const;

  /** Whether the loop ended on the criterion convergence test */
  Bool getHasConverged() const;

  /** GPR result accessor (especially useful after run() has been called) */
  GaussianProcessRegressionResult getGaussianProcessRegressionResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Check whether this problem can be solved by this solver. */
  void checkProblem(const OptimizationProblem & problem) const override;

private:
  GaussianProcessRegressionResult gprResult_;
  OptimizationAlgorithm solver_;

  // whether the solver was set
  Bool useDefaultSolver_ = true;

  // Size of the design to draw starting points
  UnsignedInteger multiStartExperimentSize_ = 0;

  // number of starting points for criterion optim
  UnsignedInteger multiStartNumber_ = 0;

  // relearn kriging parameters every X iterations
  UnsignedInteger parameterEstimationPeriod_ = 0;

  // correlation length stopping criterion factor
  Scalar correlationLengthFactor_ = 0.0;

  // noise function of the objective, propagated to the criterion
  Function noiseFunction_;

  // enrichment criterion, expected improvement by default
  EfficientGlobalOptimizationCriterion criterion_;

  // value of the criterion at each iteration
  Sample criterionValues_;

  // whether the loop ended on the criterion convergence test
  Bool hasConverged_ = false;

} ; /* class EfficientGlobalOptimization */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX */
