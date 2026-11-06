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
  // Nothing to do
}

BasisOrthonormalizationFactory::BasisOrthonormalizationFactory(const OrthogonalBasis & orthogonalBasis,
    const UnsignedInteger size)
  : OrthogonalFunctionFactory(orthogonalBasis.getMeasure())
  , orthoAlgo_(orthogonalBasis, size)
{
  // Nothing to do
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

UnsignedInteger BasisOrthonormalizationFactory::getSize() const
{
  return orthoAlgo_.getSize();
}

Sample BasisOrthonormalizationFactory::buildQuadrature(const UnsignedInteger n,
    Point & weightsOut) const
{
  const WeightedExperiment rule(buildQuadratureRule(n));
  return rule.generateWithWeights(weightsOut);
}

WeightedExperiment BasisOrthonormalizationFactory::buildQuadratureRule(const UnsignedInteger n) const
{
  const UnsignedInteger size = getSize();
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
  if (n > size) throw InvalidArgumentException(HERE) << "Error: n=" << n << " is greater than the size of the orthonormal set=" << size;
  // Build the rule from the first n functions of this orthonormal basis and
  // their measure.
  GaussLPQuadrature::FunctionCollection functions(n);
  for (UnsignedInteger k = 0; k < n; ++k)
    functions[k] = build(k);
  const GaussLPQuadrature quadrature(functions, getMeasure());
  return quadrature.buildRule(n);
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
  OSS oss(true);
  oss << "class=" << getClassName()
      << " basis=" << orthoAlgo_.getOrthogonalBasis()
      << " size=" << orthoAlgo_.getSize();
  return oss;
}

String BasisOrthonormalizationFactory::__str__(const String & /*offset*/) const
{
  OSS oss(false);
  oss << getClassName()
      << "(basis=" << orthoAlgo_.getOrthogonalBasis()
      << ", size=" << orthoAlgo_.getSize()
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
