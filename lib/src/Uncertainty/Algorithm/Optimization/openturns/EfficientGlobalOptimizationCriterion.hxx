//                                               -*- C++ -*-
/**
 *  @brief EfficientGlobalOptimizationCriterion is the handle of the enrichment
 *  criterion of the EGO algorithm
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
#ifndef OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX
#define OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EfficientGlobalOptimizationCriterion
 *
 * Handle of the enrichment criterion of the EfficientGlobalOptimization
 * algorithm. The implementation hierarchy carries the enrichment logic, this
 * handle gives it value semantics so the algorithm holds it by value, without
 * pointer members, null checks or manual persistence dispatch. Concrete
 * criteria, such as ExpectedImprovement or ProbabilityImprovement, convert
 * implicitly to this handle.
 */
class OT_API EfficientGlobalOptimizationCriterion
  : public TypedInterfaceObject<EfficientGlobalOptimizationCriterionImplementation>
{

  CLASSNAME
public:

  /** Default constructor */
  EfficientGlobalOptimizationCriterion();

  /** Constructor from implementation */
  EfficientGlobalOptimizationCriterion(const EfficientGlobalOptimizationCriterionImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  EfficientGlobalOptimizationCriterion(const Implementation & p_implementation);
#endif

  /** Gaussian process surrogate accessor */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult);

  GaussianProcessRegressionResult getGaussianProcessRegression() const;

  /** Current design of experiments accessor */
  void setInputSample(const Sample & inputSample);

  Sample getInputSample() const;

  /** Best objective value observed so far accessor */
  void setOptimalValue(const Scalar optimalValue);

  Scalar getOptimalValue() const;

  /** Direction of the problem accessor */
  void setMinimization(const Bool minimization);

  Bool getMinimization() const;

  /** Noise variance function accessor */
  void setNoiseFunction(const Function & noiseFunction);

  Function getNoiseFunction() const;

  /** Whether a noise function was provided */
  Bool hasNoiseFunction() const;

  /** The scalar function the inner solver maximizes */
  Function buildObjective() const;

  /** Convergence test on the criterion value reached at the new point */
  Bool checkConvergence(const Scalar criterionValue) const;

  /** String converter */
  String __repr__() const override;

} ; /* class EfficientGlobalOptimizationCriterion */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EFFICIENTGLOBALOPTIMIZATIONCRITERION_HXX */
