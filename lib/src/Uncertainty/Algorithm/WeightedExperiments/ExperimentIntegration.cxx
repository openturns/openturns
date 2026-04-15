//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an ExperimentIntegration
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/ExperimentIntegration.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExperimentIntegration)

static const Factory<ExperimentIntegration> Factory_ExperimentIntegration;

/* Default constructor */
ExperimentIntegration::ExperimentIntegration()
  : IntegrationAlgorithmImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ExperimentIntegration::ExperimentIntegration(const WeightedExperiment & weightedExperiment)
  : IntegrationAlgorithmImplementation()
  , weightedExperiment_(weightedExperiment)
{
  // Nothing to do
}

/* Virtual constructor */
ExperimentIntegration * ExperimentIntegration::clone() const
{
  return new ExperimentIntegration(*this);
}

/* String converter */
String ExperimentIntegration::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " weightedExperiment=" << weightedExperiment_;
  return oss;
}

/* Compute the weighted integral of the function */
Point ExperimentIntegration::integrate(const Function & function,
				       const Interval & interval) const
{
  const UnsignedInteger intervalDimension = interval.getDimension();
  if (function.getInputDimension() != intervalDimension)
    throw InvalidArgumentException(HERE) << "The input dimension of the function is : "
					 << function.getInputDimension()
					 << " which is inconsistent with the interval dimension : "
					 << intervalDimension;
  // Build the affine transformation which maps the given interval I into the
  // range I0 of the weighted experiment distribution
  // x = T(u)
  // \int_I f(x)p(x)dx = \int_I0 f(T(u))p(T(u))|det(T)|du
  // with |det(T)|p(T(u))=p0(u) the PDF of the weighted experiment distribution
  const Interval distributionRange(weightedExperiment_.getDistribution().getRange());
  // If no transformation
  if (interval == distributionRange)
    return integrate(function);
  // If transformation
  const Point lb(distributionRange.getLowerBound());
  const Point ub(distributionRange.getUpperBound());
  const Point lbTilde(interval.getLowerBound());
  const Point ubTilde(interval.getUpperBound());
  Point center(intervalDimension);
  Point constant(intervalDimension);
  SquareMatrix linear(intervalDimension);
  for (UnsignedInteger i = 0; i < intervalDimension; ++ i)
  {
    center[i] = 0.5 * (lb[i] + ub[i]);
    constant[i] = 0.5 * (lbTilde[i] + ubTilde[i]);
    linear(i, i) = (ubTilde[i] - lbTilde[i]) / (ub[i] - lb[i]);
  }
  return integrate(ComposedFunction(function, LinearFunction(center, constant, linear)));
}

Point ExperimentIntegration::integrate(const Function & function) const
{
  const UnsignedInteger experimentDimension = weightedExperiment_.getDistribution().getDimension();
  if (function.getInputDimension() != experimentDimension)
    throw InvalidArgumentException(HERE) << "The input dimension of the function is : "
					 << function.getInputDimension()
					 << " which is inconsistent with the experiment dimension : "
					 << experimentDimension;
  const UnsignedInteger outputDimension = function.getOutputDimension();
  Point weights(0);
  const Sample inputSample(weightedExperiment_.generateWithWeights(weights));
  Point approximateIntegral(outputDimension);
  const Sample outputSample(function(inputSample));
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const Point functionSampleMarginal(outputSample.getMarginal(i).asPoint());
    approximateIntegral[i] = functionSampleMarginal.dot(weights);
  }
  return approximateIntegral;
}

Point ExperimentIntegration::computeL2Norm(const Function & function) const
{
  const UnsignedInteger outputDimension = function.getOutputDimension();
  // Create the squared integrand
  Description inputVariables(outputDimension);
  Description formula(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const String variableName = (OSS() << "x" << i);
    inputVariables[i] = variableName;
    formula[i] = (OSS() << variableName << "^2");
  }
  const SymbolicFunction squareFunction(inputVariables, formula);
  const Function integrand(ComposedFunction(squareFunction, function));
  const Point functionSquaredNorm(integrate(integrand));
  Point functionNorm(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    functionNorm[i] = std::sqrt(functionSquaredNorm[i]);
  return functionNorm;
}

/* Method save() stores the object through the StorageManager */
void ExperimentIntegration::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("weightedExperiment_", weightedExperiment_);
}

/* Method load() reloads the object from the StorageManager */
void ExperimentIntegration::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("weightedExperiment_", weightedExperiment_);
}

END_NAMESPACE_OPENTURNS
