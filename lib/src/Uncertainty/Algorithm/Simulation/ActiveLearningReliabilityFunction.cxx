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

#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/ActiveLearningReliabilityFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunction
 */

CLASSNAMEINIT(ActiveLearningReliabilityFunction)

/* Default constructor */
ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction()
  : TypedInterfaceObject<ActiveLearningReliabilityFunctionImplementation>(new ActiveLearningReliabilityFunctionImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction(const ActiveLearningReliabilityFunctionImplementation & implementation)
  : TypedInterfaceObject<ActiveLearningReliabilityFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

#ifndef SWIG
/* Constructor from implementation pointer */
ActiveLearningReliabilityFunction::ActiveLearningReliabilityFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<ActiveLearningReliabilityFunctionImplementation>(p_implementation)
{
  // Nothing to do
}
#endif

/* Gaussian process surrogate accessor */
void ActiveLearningReliabilityFunction::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  copyOnWrite();
  getImplementation()->setGaussianProcessRegression(gprResult);
}

GaussianProcessRegressionResult ActiveLearningReliabilityFunction::getGaussianProcessRegression() const
{
  return getImplementation()->getGaussianProcessRegression();
}

/* Reliability threshold accessor */
void ActiveLearningReliabilityFunction::setReliabilityThreshold(const Scalar reliabilityThreshold)
{
  copyOnWrite();
  getImplementation()->setReliabilityThreshold(reliabilityThreshold);
}

Scalar ActiveLearningReliabilityFunction::getReliabilityThreshold() const
{
  return getImplementation()->getReliabilityThreshold();
}

/* Learning threshold accessor */
void ActiveLearningReliabilityFunction::setLearningThreshold(const Scalar learningThreshold)
{
  copyOnWrite();
  getImplementation()->setLearningThreshold(learningThreshold);
}

Scalar ActiveLearningReliabilityFunction::getLearningThreshold() const
{
  return getImplementation()->getLearningThreshold();
}

/* Score the candidate pool against the current design */
Sample ActiveLearningReliabilityFunction::operator()(const Sample & inputSample, const Sample & inputDoE) const
{
  return getImplementation()->operator()(inputSample, inputDoE);
}

/* Select the infill sample to evaluate on the true function */
Sample ActiveLearningReliabilityFunction::getInfillSample(const Sample & inputSample,
                                                          const Sample & criterionValues) const
{
  return getImplementation()->getInfillSample(inputSample, criterionValues);
}

/* Test convergence of active learning */
Bool ActiveLearningReliabilityFunction::checkConvergenceLearning(const Sample & criterionValues) const
{
  return getImplementation()->checkConvergenceLearning(criterionValues);
}

/* Selection direction */
Bool ActiveLearningReliabilityFunction::isMaximization() const
{
  return getImplementation()->isMaximization();
}

/* String converter */
String ActiveLearningReliabilityFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << ActiveLearningReliabilityFunction::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
