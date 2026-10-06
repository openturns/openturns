//                                               -*- C++ -*-
/**
 *  @brief A factory of orthonormal functions from a finite set of functions.
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
#include "openturns/BasisOrthonormalizationFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/GaussLPQuadrature.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BasisOrthonormalizationFactory)

static const Factory<BasisOrthonormalizationFactory> Factory_BasisOrthonormalizationFactory;

BasisOrthonormalizationFactory::BasisOrthonormalizationFactory()
  : OrthogonalFunctionFactory()
  , orthoAlgo_()
{
}

BasisOrthonormalizationFactory::BasisOrthonormalizationFactory(const FunctionCollection & functions,
    const Distribution & measure)
  : OrthogonalFunctionFactory(measure)
  , orthoAlgo_(functions, measure)
{
}

BasisOrthonormalizationFactory::BasisOrthonormalizationFactory(const FunctionCollection & functions,
    const Distribution & measure,
    const WeightedExperiment & experiment)
  : OrthogonalFunctionFactory(measure)
  , orthoAlgo_(functions, measure, experiment)
{
}

BasisOrthonormalizationFactory::BasisOrthonormalizationFactory(const FunctionCollection & functions,
    const Distribution & measure,
    const IntegrationAlgorithm & integrationAlgorithm)
  : OrthogonalFunctionFactory(measure)
  , orthoAlgo_(functions, measure, integrationAlgorithm)
{
}

BasisOrthonormalizationFactory * BasisOrthonormalizationFactory::clone() const
{
  return new BasisOrthonormalizationFactory(*this);
}

Function BasisOrthonormalizationFactory::build(const UnsignedInteger index) const
{
  const FunctionPersistentCollection & orthoFunctions = orthoAlgo_.getOrthonormalFunctions();
  if (index >= orthoFunctions.getSize()) throw InvalidArgumentException(HERE) << "Error: the given index=" << index << " is greater than the size of the orthonormal set=" << orthoFunctions.getSize();
  return orthoFunctions[index];
}

Sample BasisOrthonormalizationFactory::buildQuadrature(const UnsignedInteger n,
    Point & weightsOut) const
{
  const GaussLPQuadrature quad(orthoAlgo_.getOrthonormalFunctions(), measure_);
  return quad.build(n, weightsOut);
}

void BasisOrthonormalizationFactory::setFunctionsCollection(const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
  orthoAlgo_.setFunctionsCollection(functions);
}

void BasisOrthonormalizationFactory::setMeasureAndFunctions(const Distribution & measure,
    const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
  measure_ = measure;
  orthoAlgo_.setMeasure(measure);
  orthoAlgo_.setFunctionsCollection(functions);
}

BasisOrthonormalizationFactory::FunctionCollection BasisOrthonormalizationFactory::getFunctionsCollection() const
{
  return orthoAlgo_.getFunctionsCollection();
}

void BasisOrthonormalizationFactory::setExperiment(const WeightedExperiment & experiment)
{
  orthoAlgo_.setExperiment(experiment);
}

WeightedExperiment BasisOrthonormalizationFactory::getExperiment() const
{
  return orthoAlgo_.getExperiment();
}

void BasisOrthonormalizationFactory::setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm)
{
  orthoAlgo_.setIntegrationAlgorithm(integrationAlgorithm);
}

IntegrationAlgorithm BasisOrthonormalizationFactory::getIntegrationAlgorithm() const
{
  return orthoAlgo_.getIntegrationAlgorithm();
}

SquareMatrix BasisOrthonormalizationFactory::getCoefficients() const
{
  return orthoAlgo_.getCoefficients();
}

BasisOrthonormalizationAlgorithm BasisOrthonormalizationFactory::getOrthonormalizationAlgorithm() const
{
  return orthoAlgo_;
}

String BasisOrthonormalizationFactory::__repr__() const
{
  const FunctionPersistentCollection functions(orthoAlgo_.getFunctionsCollection());
  OSS oss(true);
  oss << "class=" << getClassName()
      << " functions=" << functions
      << " measure=" << measure_
      << " experiment=" << orthoAlgo_.getExperiment();
  return oss;
}

String BasisOrthonormalizationFactory::__str__(const String & /*offset*/) const
{
  const FunctionPersistentCollection functions(orthoAlgo_.getFunctionsCollection());
  OSS oss(false);
  oss << getClassName()
      << "(functions=" << functions
      << ", measure=" << measure_
      << ", experiment=" << orthoAlgo_.getExperiment()
      << ")";
  return oss;
}

void BasisOrthonormalizationFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute("orthoAlgo_", orthoAlgo_);
}

void BasisOrthonormalizationFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute("orthoAlgo_", orthoAlgo_);
}

END_NAMESPACE_OPENTURNS
