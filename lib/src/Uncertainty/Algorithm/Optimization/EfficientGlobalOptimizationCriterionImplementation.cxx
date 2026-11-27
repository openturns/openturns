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
#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EfficientGlobalOptimizationCriterionImplementation)

static const Factory<EfficientGlobalOptimizationCriterionImplementation> Factory_EfficientGlobalOptimizationCriterionImplementation;

/* Default constructor */
EfficientGlobalOptimizationCriterionImplementation::EfficientGlobalOptimizationCriterionImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
EfficientGlobalOptimizationCriterionImplementation * EfficientGlobalOptimizationCriterionImplementation::clone() const
{
  return new EfficientGlobalOptimizationCriterionImplementation(*this);
}

/* Gaussian process surrogate accessor */
void EfficientGlobalOptimizationCriterionImplementation::setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult)
{
  if (gprResult.getMetaModel().getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "The Gaussian process metamodel must have output dimension 1, here outputDimension=" << gprResult.getMetaModel().getOutputDimension();
  gprResult_ = gprResult;
}

GaussianProcessRegressionResult EfficientGlobalOptimizationCriterionImplementation::getGaussianProcessRegression() const
{
  return gprResult_;
}

/* Current design of experiments accessor */
void EfficientGlobalOptimizationCriterionImplementation::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
}

Sample EfficientGlobalOptimizationCriterionImplementation::getInputSample() const
{
  return inputSample_;
}

/* Best objective value observed so far accessor */
void EfficientGlobalOptimizationCriterionImplementation::setOptimalValue(const Scalar optimalValue)
{
  if (std::isnan(optimalValue))
    throw InvalidArgumentException(HERE) << "The optimal value must not be NaN";
  optimalValue_ = optimalValue;
}

Scalar EfficientGlobalOptimizationCriterionImplementation::getOptimalValue() const
{
  return optimalValue_;
}

/* Direction of the problem accessor */
void EfficientGlobalOptimizationCriterionImplementation::setMinimization(const Bool minimization)
{
  minimization_ = minimization;
}

Bool EfficientGlobalOptimizationCriterionImplementation::getMinimization() const
{
  return minimization_;
}

/* Noise variance function accessor */
void EfficientGlobalOptimizationCriterionImplementation::setNoiseFunction(const Function & noiseFunction)
{
  if (noiseFunction.getOutputDimension() > 1)
    throw InvalidArgumentException(HERE) << "The noise function must be 1-d, here outputDimension=" << noiseFunction.getOutputDimension();
  noiseFunction_ = noiseFunction;
}

Function EfficientGlobalOptimizationCriterionImplementation::getNoiseFunction() const
{
  return noiseFunction_;
}

Bool EfficientGlobalOptimizationCriterionImplementation::hasNoiseFunction() const
{
  return noiseFunction_.getEvaluation().getImplementation()->isActualImplementation();
}

/* The objective to maximize, to be implemented by concrete criteria */
Function EfficientGlobalOptimizationCriterionImplementation::buildObjective() const
{
  throw NotYetImplementedException(HERE) << "In EfficientGlobalOptimizationCriterionImplementation::buildObjective(): "
                                         << "a concrete criterion must return the 1-d function maximized by the algorithm";
}

/* Convergence test on the criterion value at the new point */
Bool EfficientGlobalOptimizationCriterionImplementation::checkConvergence(const Scalar) const
{
  return false;
}

/* String converter */
String EfficientGlobalOptimizationCriterionImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << EfficientGlobalOptimizationCriterionImplementation::GetClassName()
      << " name=" << getName()
      << " minimization=" << (minimization_ ? "true" : "false")
      << " optimalValue=" << optimalValue_
      << " designSize=" << inputSample_.getSize()
      << " withNoise=" << (hasNoiseFunction() ? "true" : "false");
  return oss;
}

/* Method save() stores the object through the StorageManager */
void EfficientGlobalOptimizationCriterionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("optimalValue_", optimalValue_);
  adv.saveAttribute("minimization_", minimization_);
  adv.saveAttribute("noiseFunction_", noiseFunction_);
}

/* Method load() reloads the object from the StorageManager */
void EfficientGlobalOptimizationCriterionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("optimalValue_", optimalValue_);
  adv.loadAttribute("minimization_", minimization_);
  adv.loadAttribute("noiseFunction_", noiseFunction_);
}

END_NAMESPACE_OPENTURNS
