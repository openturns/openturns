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
#include "openturns/EfficientGlobalOptimizationCriterion.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EfficientGlobalOptimizationCriterion)

/* Default constructor */
EfficientGlobalOptimizationCriterion::EfficientGlobalOptimizationCriterion()
  : TypedInterfaceObject<EfficientGlobalOptimizationCriterionImplementation>(new EfficientGlobalOptimizationCriterionImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
EfficientGlobalOptimizationCriterion::EfficientGlobalOptimizationCriterion(const EfficientGlobalOptimizationCriterionImplementation & implementation)
  : TypedInterfaceObject<EfficientGlobalOptimizationCriterionImplementation>(implementation.clone())
{
  // Nothing to do
}

#ifndef SWIG
/* Constructor from implementation pointer */
EfficientGlobalOptimizationCriterion::EfficientGlobalOptimizationCriterion(const Implementation & p_implementation)
  : TypedInterfaceObject<EfficientGlobalOptimizationCriterionImplementation>(p_implementation)
{
  // Nothing to do
}
#endif

/* Gaussian process surrogate accessor */
void EfficientGlobalOptimizationCriterion::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  copyOnWrite();
  getImplementation()->setGaussianProcessRegression(gprResult);
}

GaussianProcessRegressionResult EfficientGlobalOptimizationCriterion::getGaussianProcessRegression() const
{
  return getImplementation()->getGaussianProcessRegression();
}

/* Current design of experiments accessor */
void EfficientGlobalOptimizationCriterion::setInputSample(const Sample & inputSample)
{
  copyOnWrite();
  getImplementation()->setInputSample(inputSample);
}

Sample EfficientGlobalOptimizationCriterion::getInputSample() const
{
  return getImplementation()->getInputSample();
}

/* Best objective value observed so far accessor */
void EfficientGlobalOptimizationCriterion::setOptimalValue(const Scalar optimalValue)
{
  copyOnWrite();
  getImplementation()->setOptimalValue(optimalValue);
}

Scalar EfficientGlobalOptimizationCriterion::getOptimalValue() const
{
  return getImplementation()->getOptimalValue();
}

/* Direction of the problem accessor */
void EfficientGlobalOptimizationCriterion::setMinimization(const Bool minimization)
{
  copyOnWrite();
  getImplementation()->setMinimization(minimization);
}

Bool EfficientGlobalOptimizationCriterion::getMinimization() const
{
  return getImplementation()->getMinimization();
}

/* Noise variance function accessor */
void EfficientGlobalOptimizationCriterion::setNoiseFunction(const Function & noiseFunction)
{
  copyOnWrite();
  getImplementation()->setNoiseFunction(noiseFunction);
}

Function EfficientGlobalOptimizationCriterion::getNoiseFunction() const
{
  return getImplementation()->getNoiseFunction();
}

Bool EfficientGlobalOptimizationCriterion::hasNoiseFunction() const
{
  return getImplementation()->hasNoiseFunction();
}

/* The scalar function the inner solver maximizes */
Function EfficientGlobalOptimizationCriterion::buildObjective() const
{
  return getImplementation()->buildObjective();
}

/* Convergence test on the criterion value reached at the new point */
Bool EfficientGlobalOptimizationCriterion::checkConvergence(const Scalar criterionValue) const
{
  return getImplementation()->checkConvergence(criterionValue);
}

/* String converter */
String EfficientGlobalOptimizationCriterion::__repr__() const
{
  return getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
