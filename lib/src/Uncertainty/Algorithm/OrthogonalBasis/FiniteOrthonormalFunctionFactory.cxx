//                                               -*- C++ -*-
/**
 *  @brief A finite orthonormal set of functions wrt a given distribution.
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
#include "openturns/FiniteOrthonormalFunctionFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/QROrthonormalizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteOrthonormalFunctionFactory)

static const Factory<FiniteOrthonormalFunctionFactory> Factory_FiniteOrthonormalFunctionFactory;

FiniteOrthonormalFunctionFactory::FiniteOrthonormalFunctionFactory()
  : OrthogonalFunctionFactory()
  , initialFunctions_()
  , experiment_()
  , orthoAlgo_()
  , quad_()
  , isAlreadyComputed_(false)
{
}

FiniteOrthonormalFunctionFactory::FiniteOrthonormalFunctionFactory(const FunctionCollection & functions,
    const Distribution & measure,
    const WeightedExperiment & experiment)
  : OrthogonalFunctionFactory(measure)
  , initialFunctions_()
  , experiment_(experiment)
  , orthoAlgo_()
  , quad_()
  , isAlreadyComputed_(false)
{
  setMeasureAndFunctions(measure, functions);
}

FiniteOrthonormalFunctionFactory * FiniteOrthonormalFunctionFactory::clone() const
{
  return new FiniteOrthonormalFunctionFactory(*this);
}

Function FiniteOrthonormalFunctionFactory::build(const UnsignedInteger index) const
{
  computeOrthonormalBasis();
  const FunctionPersistentCollection & orthoFunctions = orthoAlgo_.getOrthonormalFunctions();
  if (index >= orthoFunctions.getSize()) throw InvalidArgumentException(HERE) << "Error: the given index=" << index << " is greater than the size of the orthonormal set=" << orthoFunctions.getSize();
  return orthoFunctions[index];
}

Sample FiniteOrthonormalFunctionFactory::buildQuadrature(const UnsignedInteger n,
    Point & weightsOut) const
{
  computeOrthonormalBasis();
  quad_.setOrthonormalFunctions(orthoAlgo_.getOrthonormalFunctions());
  quad_.setMeasure(measure_);
  return quad_.build(n, weightsOut);
}

void FiniteOrthonormalFunctionFactory::setFunctionsCollection(const FunctionCollection & functions)
{
  const UnsignedInteger size = functions.getSize();
  const UnsignedInteger dimension = measure_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (functions[i].getInputDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an input dimension=" << functions[i].getInputDimension() << ", expected an input dimension=" << dimension;
    if (functions[i].getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the function=" << functions[i] << " at index=" << i << " has an output dimension=" << functions[i].getOutputDimension() << ", expected an output dimension=1";
  }
  initialFunctions_ = functions;
  isAlreadyComputed_ = false;
}

void FiniteOrthonormalFunctionFactory::setMeasureAndFunctions(const Distribution & measure,
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
  initialFunctions_ = functions;
  isAlreadyComputed_ = false;
}

FiniteOrthonormalFunctionFactory::FunctionCollection FiniteOrthonormalFunctionFactory::getFunctionsCollection() const
{
  return initialFunctions_;
}

void FiniteOrthonormalFunctionFactory::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isAlreadyComputed_ = false;
}

WeightedExperiment FiniteOrthonormalFunctionFactory::getExperiment() const
{
  return experiment_;
}

SquareMatrix FiniteOrthonormalFunctionFactory::getCoefficients() const
{
  computeOrthonormalBasis();
  return orthoAlgo_.getCoefficients();
}

void FiniteOrthonormalFunctionFactory::setOrthonormalizationAlgorithm(const QROrthonormalizationAlgorithm & algo)
{
  orthoAlgo_ = algo;
  isAlreadyComputed_ = false;
}

QROrthonormalizationAlgorithm FiniteOrthonormalFunctionFactory::getOrthonormalizationAlgorithm() const
{
  return orthoAlgo_;
}

void FiniteOrthonormalFunctionFactory::setGaussLPQuadrature(const GaussLPQuadrature & quad)
{
  quad_ = quad;
}

GaussLPQuadrature FiniteOrthonormalFunctionFactory::getGaussLPQuadrature() const
{
  return quad_;
}

String FiniteOrthonormalFunctionFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " functions=" << initialFunctions_
      << " measure=" << measure_
      << " experiment=" << experiment_;
  return oss;
}

String FiniteOrthonormalFunctionFactory::__str__(const String & /*offset*/) const
{
  OSS oss(false);
  oss << getClassName()
      << "(functions=" << initialFunctions_
      << ", measure=" << measure_
      << ", experiment=" << experiment_
      << ")";
  return oss;
}

void FiniteOrthonormalFunctionFactory::computeOrthonormalBasis() const
{
  if (isAlreadyComputed_) return;
  orthoAlgo_.setMeasure(measure_);
  orthoAlgo_.setFunctionsCollection(initialFunctions_);
  orthoAlgo_.setExperiment(experiment_);
  orthoAlgo_.run();
  isAlreadyComputed_ = true;
}

void FiniteOrthonormalFunctionFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute("initialFunctions_", initialFunctions_);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("orthoAlgo_", orthoAlgo_);
  adv.saveAttribute("quad_", quad_);
  adv.saveAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

void FiniteOrthonormalFunctionFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute("initialFunctions_", initialFunctions_);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("orthoAlgo_", orthoAlgo_);
  adv.loadAttribute("quad_", quad_);
  adv.loadAttribute("isAlreadyComputed_", isAlreadyComputed_);
}

END_NAMESPACE_OPENTURNS
