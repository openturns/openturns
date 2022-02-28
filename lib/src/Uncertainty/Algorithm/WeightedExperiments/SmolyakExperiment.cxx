//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an Smolyak experiment
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SmolyakExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmolyakExperiment)

static const Factory<SmolyakExperiment> Factory_SmolyakExperiment;

typedef Collection< Point > PointCollection;

/* Default constructor */
SmolyakExperiment::SmolyakExperiment()
  : WeightedExperimentImplementation()
  , collection_(0)
  , level_(0)
  , nodes_(0, 0)
{
  // Nothing to do
}

/* Virtual constructor */
SmolyakExperiment * SmolyakExperiment::clone() const
{
  return new SmolyakExperiment(*this);
}

/* String converter */
String TensorProductExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
    << " name=" << getName()
    << " level=" << level_
    << " collection=" << collection_;
  return oss;
}

Bool SmolyakExperiment::hasUniformWeights() const
{
  return false;
}

/* Sample generation */
Sample SmolyakExperiment::generateWithWeights(Point & weights) const
{
  if (!isAlreadyComputedNodesAndWeights_) computeNodesAndWeights();
  weights = weights_;
  return nodes_;
}

/* Compute the nodes and weights */
void SmolyakExperiment::computeNodesAndWeights() const
{
  // TODO
}

/* Method save() stores the object through the StorageManager */
void SmolyakExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("level_", level_);
}

/* Method load() reloads the object from the StorageManager */
void SmolyakExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  adv.loadAttribute("level_", level_);
}


END_NAMESPACE_OPENTURNS
