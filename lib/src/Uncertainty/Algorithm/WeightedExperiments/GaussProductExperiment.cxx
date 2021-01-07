//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
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
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussProductExperiment)

static const Factory<GaussProductExperiment> Factory_GaussProductExperiment;

typedef Collection< Point > PointCollection;

/* Default constructor */
GaussProductExperiment::GaussProductExperiment():
  WeightedExperimentImplementation(),
  collection_(0),
  marginalDegrees_(0),
  nodes_(0, 0)
{
  // Nothing to do
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Indices & marginalDegrees):
  WeightedExperimentImplementation(),
  collection_(0),
  marginalDegrees_(0),
  nodes_(0, 0)
{
  // Here we have to set a distribution of dimension compatible with the marginal degrees
  setDistributionAndMarginalDegrees(ComposedDistribution(ComposedDistribution::DistributionCollection(marginalDegrees.getSize())), marginalDegrees);
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Distribution & distribution):
  WeightedExperimentImplementation(),
  collection_(0),
  marginalDegrees_(0),
  nodes_(0, 0)
{
  setDistributionAndMarginalDegrees(distribution, Indices(distribution.getDimension(), ResourceMap::GetAsUnsignedInteger( "GaussProductExperiment-DefaultMarginalDegree" )));
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Distribution & distribution,
    const Indices & marginalDegrees):
  WeightedExperimentImplementation(),
  collection_(0),
  marginalDegrees_(0),
  nodes_(0, 0)
{
  setDistributionAndMarginalDegrees(distribution, marginalDegrees);
}

/* Virtual constructor */
GaussProductExperiment * GaussProductExperiment::clone() const
{
  return new GaussProductExperiment(*this);
}

/* String converter */
String GaussProductExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_
      << " marginal degrees=" << marginalDegrees_;
  return oss;
}

/* Distribution accessor */
void GaussProductExperiment::setDistribution(const Distribution & distribution)
{
  if (!distribution.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: the GaussProductExperiment can only be used with distributions having an independent copula.";
  const UnsignedInteger dimension = distribution.getDimension();
  if (dimension != marginalDegrees_.getSize()) throw InvalidArgumentException(HERE) << "Error: the given distribution has a dimension=" << dimension << "different from the number of marginal degrees=" << marginalDegrees_.getSize() << ".";
  collection_ = OrthogonalUniVariatePolynomialFamilyCollection(0);
  for (UnsignedInteger i = 0; i < dimension; ++i) collection_.add(StandardDistributionPolynomialFactory(AdaptiveStieltjesAlgorithm(distribution.getMarginal(i))));
  WeightedExperimentImplementation::setDistribution(distribution);
  isAlreadyComputedNodesAndWeights_ = false;
}

Bool GaussProductExperiment::hasUniformWeights() const
{
  return false;
}

/* Sample generation */
Sample GaussProductExperiment::generateWithWeights(Point & weights) const
{
  if (!isAlreadyComputedNodesAndWeights_) computeNodesAndWeights();
  weights = weights_;
  return nodes_;
}

/** Marginal degrees accessor */
void GaussProductExperiment::setMarginalDegrees(const Indices & marginalDegrees)
{
  const UnsignedInteger dimension = distribution_.getDimension();
  if (marginalDegrees.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the marginal degrees number must match the distribution dimension. Here, the degrees are " << marginalDegrees << " and the dimension is " << dimension;
  if (marginalDegrees != marginalDegrees_)
  {
    marginalDegrees_ = marginalDegrees;
    isAlreadyComputedNodesAndWeights_ = false;
  }
}

/** Marginal degrees accessor */
void GaussProductExperiment::setDistributionAndMarginalDegrees(const Distribution & distribution,
    const Indices & marginalDegrees)
{
  // Set the marginal degrees here then the distribution with checks
  marginalDegrees_ = marginalDegrees;
  setDistribution(distribution);

  const UnsignedInteger dimension = distribution_.getDimension();
  size_ = 1;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    const UnsignedInteger dI = marginalDegrees_[i];
    size_ *= dI;
  }
}

Indices GaussProductExperiment::getMarginalDegrees() const
{
  return marginalDegrees_;
}

/* Compute the tensor product nodes and weights */
void GaussProductExperiment::computeNodesAndWeights() const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  // Build the integration nodes and weights
  // First, get the marginal nodes and weights
  PointCollection marginalNodes(dimension);
  PointCollection marginalWeights(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger dI = marginalDegrees_[i];
    marginalNodes[i] = collection_[i].getNodesAndWeights(dI, marginalWeights[i]);
  }
  // Second, multiplex everything
  nodes_ = Sample(size_, dimension);
  nodes_.setDescription(distribution_.getDescription());
  weights_ = Point(size_, 1.0);
  Indices indices(dimension, 0);
  for (UnsignedInteger linearIndex = 0; linearIndex < size_; ++linearIndex)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const UnsignedInteger indiceJ = indices[j];
      nodes_[linearIndex][j] = marginalNodes[j][indiceJ];
      weights_[linearIndex] *= marginalWeights[j][indiceJ];
    }
    /* Update the indices */
    ++indices[0];
    /* Propagate the remainders */
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j + 1] += (indices[j] == marginalDegrees_[j]);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j] = indices[j] % marginalDegrees_[j];
  } // Loop over the n-D nodes
  isAlreadyComputedNodesAndWeights_ = true;
}

/* Method save() stores the object through the StorageManager */
void GaussProductExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("marginalDegrees_", marginalDegrees_);
}

/* Method load() reloads the object from the StorageManager */
void GaussProductExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  adv.loadAttribute("marginalDegrees_", marginalDegrees_);
  setDistributionAndMarginalDegrees(distribution_, marginalDegrees_);
}


END_NAMESPACE_OPENTURNS
