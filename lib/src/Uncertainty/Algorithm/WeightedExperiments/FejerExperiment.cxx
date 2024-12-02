//                                               -*- C++ -*-
/**
 *  @brief This class exposes the Fejer experiments
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FejerExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerExperiment
 */


CLASSNAMEINIT(FejerExperiment)

static const Factory<FejerExperiment> Factory_FejerExperiment;


/* Parameters constructor */
FejerExperiment::FejerExperiment()
  : WeightedExperimentImplementation()
{
}

FejerExperiment::FejerExperiment(const FejerAlgorithm & algorithm,
                                 const Interval & bounds)
  : WeightedExperimentImplementation()
  , algorithm_(algorithm)
  , bounds_(bounds)
{
  if (algorithm.getDiscretization().getSize() != bounds.getDimension())
    throw InvalidArgumentException(HERE) << "FejerExperiment discretization and bounds dimension do not match.";
  if (algorithm.getDiscretization().getSize() != 1)
    throw NotYetImplementedException(HERE) << "FejerExperiment only allows 1-d discretization for now";
}

/* Virtual constructor */
FejerExperiment * FejerExperiment::clone() const
{
  return new FejerExperiment(*this);
}


Bool FejerExperiment::hasUniformWeights() const
{
  return algorithm_.getDiscretization().getSize() < 2;
}

/* Sample generation */
Sample FejerExperiment::generate() const
{
  return algorithm_.getNodes();
}

Sample FejerExperiment::generateWithWeights(Point & weightsOut) const
{
  weightsOut = algorithm_.getWeights();
  // Adapt the nodes to the bounds of the interval
  const Point halfDelta((bounds_.getUpperBound() - bounds_.getLowerBound()) * 0.5);
  return algorithm_.getNodes() * halfDelta + halfDelta + bounds_.getLowerBound();
}

/* Size accessor */
void FejerExperiment::setSize(const UnsignedInteger /*size*/)
{
  throw NotDefinedException(HERE) << "in FejerExperiment::setSize";
}

UnsignedInteger FejerExperiment::getSize() const
{
  // only known at generation time
  return generate().getSize();
}

/* Return isRandom flag */
Bool FejerExperiment::isRandom() const
{
  return false;
}

void FejerExperiment::setLevel(const UnsignedInteger level)
{
  if (level == 0)
    throw InvalidArgumentException(HERE) << "FejerExperiment level cannot be null";
  const UnsignedInteger size = (1 << level) - 1;
  // TODO: move the logic from FejerAlgorithm into FejerExperiment, save method
  algorithm_ = FejerAlgorithm(Indices({size}), FejerAlgorithm::CLENSHAWCURTIS);
}

/* String converter */
String FejerExperiment::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FejerExperiment::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FejerExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
  adv.saveAttribute("bounds_", bounds_);
}

/* Method load() reloads the object from the StorageManager */
void FejerExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
  adv.loadAttribute("bounds_", bounds_);
}

END_NAMESPACE_OPENTURNS
