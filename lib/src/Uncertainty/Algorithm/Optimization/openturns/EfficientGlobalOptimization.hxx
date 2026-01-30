//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimization or EGO algorithm
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
#ifndef OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX
#define OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"

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

  /** AEI tradeoff constant accessor */
  void setAEITradeoff(const Scalar c);
  Scalar getAEITradeoff() const;

  /** Improvement noise function accessor */
  void setNoiseFunction(const Function & noiseFunction);
  Function getNoiseFunction() const;

  /** Expected improvement function */
  Sample getExpectedImprovement() const;

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

  // AEI tradeoff constant u(x)=mk(x)+c*sk(x)
  Scalar aeiTradeoff_ = 0.0;

  // noise function
  Function noiseFunction_;
  
  Sample expectedImprovement_;

} ; /* class EfficientGlobalOptimization */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EFFICIENTGLOBALOPTIMIZATION_HXX */
