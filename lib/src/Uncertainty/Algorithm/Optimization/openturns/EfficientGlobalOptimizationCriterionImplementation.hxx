//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimizationCriterionImplementation is the base class
 *  of the enrichment criterion of the EGO algorithm
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
#ifndef OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERIONIMPLEMENTATION_HXX
#define OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EfficientGlobalOptimizationCriterionImplementation
 *
 * Base class of the enrichment criterion of the EfficientGlobalOptimization
 * algorithm.
 *
 * The algorithm drives the whole loop: it grows the design, refits the Gaussian
 * process, maximizes the objective returned by buildObjective() with its inner
 * solver, evaluates the true model at the new point and checks the stopping
 * rules. The criterion only answers two questions, at each iteration:
 *
 * - which scalar function must the inner solver maximize?
 * - does the criterion value at the new point say that the enrichment is over?
 *
 * The algorithm pushes its current state (the Gaussian process, the design, the
 * best observed value, the optimization direction, the noise function) into the
 * criterion before calling buildObjective(), so an implementation never has to
 * guess what the current iteration looks like.
 *
 * buildObjective() must return a 1-d function defined on the problem domain,
 * which the algorithm MAXIMIZES whatever the direction of the problem: a
 * minimization criterion returns the opposite of its quantity of interest.
 */
class OT_API EfficientGlobalOptimizationCriterionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  EfficientGlobalOptimizationCriterionImplementation();

  /** Virtual constructor */
  EfficientGlobalOptimizationCriterionImplementation * clone() const override;

  /** Gaussian process surrogate accessor */
  virtual void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult);

  GaussianProcessRegressionResult getGaussianProcessRegression() const;

  /** Current design of experiments accessor */
  virtual void setInputSample(const Sample & inputSample);

  Sample getInputSample() const;

  /** Best objective value observed so far accessor */
  virtual void setOptimalValue(const Scalar optimalValue);

  Scalar getOptimalValue() const;

  /** Direction of the problem accessor */
  virtual void setMinimization(const Bool minimization);

  Bool getMinimization() const;

  /** Noise variance function accessor, empty when the objective is noiseless */
  virtual void setNoiseFunction(const Function & noiseFunction);

  Function getNoiseFunction() const;

  /** Whether a noise function was provided */
  Bool hasNoiseFunction() const;

  /**
   * The scalar function the inner solver of the algorithm maximizes.
   * Must be overridden by every concrete criterion.
   */
  virtual Function buildObjective() const;

  /**
   * Convergence test on the criterion value reached at the new point.
   * The default never stops: the algorithm keeps its own stopping rules.
   */
  virtual Bool checkConvergence(const Scalar criterionValue) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Gaussian process surrogate of the current iteration */
  GaussianProcessRegressionResult gprResult_;

  /** Design of experiments of the current iteration */
  Sample inputSample_;

  /** Best objective value observed so far */
  Scalar optimalValue_ = 0.0;

  /** Whether the problem is a minimization */
  Bool minimization_ = true;

  /** Noise variance function, empty when the objective is noiseless */
  Function noiseFunction_;

} ; /* class EfficientGlobalOptimizationCriterionImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERIONIMPLEMENTATION_HXX */
