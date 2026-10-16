//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunction is the handle of the
 *  active learning criterion used in reliability algorithms
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

#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ActiveLearningReliabilityFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunction
 *
 * Handle of the active learning (infill) criterion for reliability
 * analysis. The implementation hierarchy
 * (ActiveLearningReliabilityFunctionImplementation, ActiveLearningUFunction,
 * ActiveLearningEFFFunction, ActiveLearningGMMFunction) carries the
 * scoring behavior; this handle provides value semantics so owners
 * such as ActiveLearningReliabilityAlgorithm hold it by value, without
 * Pointer members, null checks or manual persistence dispatch.
 * Concrete criteria convert implicitly to this handle.
 */

class OT_API ActiveLearningReliabilityFunction
  : public TypedInterfaceObject<ActiveLearningReliabilityFunctionImplementation>
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityFunction();

  /** Constructor from implementation */
  ActiveLearningReliabilityFunction(const ActiveLearningReliabilityFunctionImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  ActiveLearningReliabilityFunction(const Implementation & p_implementation);
#endif

  /** Gaussian process surrogate accessor */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult);

  GaussianProcessRegressionResult getGaussianProcessRegression() const;

  /** Reliability threshold accessor */
  void setReliabilityThreshold(const Scalar reliabilityThreshold);

  Scalar getReliabilityThreshold() const;

  /** Learning threshold accessor */
  void setLearningThreshold(const Scalar learningThreshold);

  Scalar getLearningThreshold() const;

  /** Score the candidate pool against the current design */
  Sample operator()(const Sample & inputSample, const Sample & inputDoE) const;

  /** Select the infill sample to evaluate on the true function */
  Sample getInfillSample(const Sample & inputSample,
                         const Sample & criterionValues) const;

  /** Test convergence of active learning */
  Bool checkConvergenceLearning(const Sample & criterionValues) const;

  /** Selection direction: true if the infill point maximizes the criterion (EFF, GMM), false if it minimizes it (U) */
  Bool isMaximization() const;

  /** String converter */
  String __repr__() const override;

} ; /* class ActiveLearningReliabilityFunction */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX */
