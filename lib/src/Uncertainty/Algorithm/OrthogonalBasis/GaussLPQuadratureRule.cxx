//                                               -*- C++ -*-
/**
 *  @brief Gauss-LP quadrature rule
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
#include "openturns/GaussLPQuadratureRule.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussLPQuadratureRule)

static const Factory<GaussLPQuadratureRule> Factory_GaussLPQuadratureRule;

GaussLPQuadratureRule::GaussLPQuadratureRule()
  : WeightedExperimentImplementation()
  , nodes_()
  , weights_()
{
  // Nothing to do
}

GaussLPQuadratureRule::GaussLPQuadratureRule(const Sample & nodes,
    const Point & weights,
    const Distribution & measure)
  : WeightedExperimentImplementation(measure, nodes.getSize())
  , nodes_(nodes)
  , weights_(weights)
{
  if (nodes_.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the quadrature rule must contain at least one node";
  if (weights_.getDimension() != nodes_.getSize())
    throw InvalidArgumentException(HERE) << "Error: the quadrature rule has " << nodes_.getSize()
                                         << " nodes but " << weights_.getDimension() << " weights";
  if (measure.getDimension() != nodes_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: the measure dimension=" << measure.getDimension()
                                         << " does not match the node dimension=" << nodes_.getDimension();
  for (UnsignedInteger i = 0; i < nodes_.getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < nodes_.getDimension(); ++j)
      if (!std::isfinite(nodes_(i, j)))
        throw InvalidArgumentException(HERE) << "Error: the node at index=" << i << " is not finite";
    if (!std::isfinite(weights_[i]))
      throw InvalidArgumentException(HERE) << "Error: the weight at index=" << i << " is not finite: " << weights_[i];
    if (weights_[i] < 0.0)
      throw InvalidArgumentException(HERE) << "Error: the weight at index=" << i << " is negative: " << weights_[i];
  }
}

GaussLPQuadratureRule * GaussLPQuadratureRule::clone() const
{
  return new GaussLPQuadratureRule(*this);
}

Sample GaussLPQuadratureRule::generateWithWeights(Point & weightsOut) const
{
  weightsOut = weights_;
  return nodes_;
}

Bool GaussLPQuadratureRule::hasUniformWeights() const
{
  return weights_ == Point(nodes_.getSize(), 1.0 / nodes_.getSize());
}

Bool GaussLPQuadratureRule::isRandom() const
{
  return false;
}

void GaussLPQuadratureRule::setSize(const UnsignedInteger size)
{
  if (size != nodes_.getSize())
    throw InvalidArgumentException(HERE) << "Error: the size of a quadrature rule is fixed to " << nodes_.getSize()
                                         << ", cannot change it to " << size;
}

void GaussLPQuadratureRule::setDistribution(const Distribution &)
{
  throw InvalidArgumentException(HERE) << "Error: the measure of a quadrature rule is fixed to " << getDistribution()
                                       << ", it cannot be changed";
}

String GaussLPQuadratureRule::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " size=" << getSize()
      << " measure=" << getDistribution();
  return oss;
}

void GaussLPQuadratureRule::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("nodes_", nodes_);
  adv.saveAttribute("weights_", weights_);
}

void GaussLPQuadratureRule::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("nodes_", nodes_);
  adv.loadAttribute("weights_", weights_);
}

END_NAMESPACE_OPENTURNS
