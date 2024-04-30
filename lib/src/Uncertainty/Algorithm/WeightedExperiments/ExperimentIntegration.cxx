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
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExperimentIntegration)

typedef Collection<Function>                              FunctionCollection;

/* Default constructor */
ExperimentIntegration::ExperimentIntegration()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
ExperimentIntegration::ExperimentIntegration(const WeightedExperiment & weightedExperiment)
  : PersistentObject()
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

Point ExperimentIntegration::integrate(const Function & g) const
{
  const UnsignedInteger outputDimension = g.getOutputDimension();
  Point weights(0);
  const Sample inputSample(weightedExperiment_.generateWithWeights(weights));
  if (g.getInputDimension() != inputSample.getDimension())
    throw NotYetImplementedException(HERE) << "The input dimension of g is : " 
                                           << g.getInputDimension() 
                                           << " which is inconsistent with the experiment dimension : "
                                           << inputSample.getDimension();
  Point approximateIntegral(outputDimension);
  const Sample outputSample(g(inputSample));
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const Point functionSampleMarginal(outputSample.getMarginal(i).asPoint());
    approximateIntegral[i] = functionSampleMarginal.dot(weights);
  }
  return approximateIntegral;
}

Point ExperimentIntegration::computeL2Norm(const Function & g) const
{
  const UnsignedInteger outputDimension = g.getOutputDimension();
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
  const Function integrand(ComposedFunction(squareFunction, g));
  const Point functionSquaredNorm(integrate(integrand));
  Point functionNorm(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    functionNorm[i] = std::sqrt(functionSquaredNorm[i]);
  return functionNorm;
}

/* Method save() stores the object through the StorageManager */
void ExperimentIntegration::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("weightedExperiment_", weightedExperiment_);
}

/* Method load() reloads the object from the StorageManager */
void ExperimentIntegration::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("weightedExperiment_", weightedExperiment_);
}

END_NAMESPACE_OPENTURNS
