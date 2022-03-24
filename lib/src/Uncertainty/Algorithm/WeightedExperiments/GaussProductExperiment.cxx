//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an GaussProductExperiment
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ComposedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussProductExperiment)

static const Factory<GaussProductExperiment> Factory_GaussProductExperiment;

typedef Collection< Point > PointCollection;

/* Default constructor */
GaussProductExperiment::GaussProductExperiment()
  : GaussProductExperiment(Indices(1, 1))
{
  // Nothing to do
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Indices & marginalSizes)
  : WeightedExperimentImplementation()
  , collection_(0)
  , marginalSizes_(0)
  , nodes_(0, 0)
{
  // Here we have to set a distribution of dimension compatible with the marginal sizes
  setDistributionAndMarginalSizes(ComposedDistribution(ComposedDistribution::DistributionCollection(marginalSizes.getSize())), marginalSizes);
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Distribution & distribution)
  : WeightedExperimentImplementation()
  , collection_(0)
  , marginalSizes_(0)
  , nodes_(0, 0)
{
  setDistributionAndMarginalSizes(distribution, Indices(distribution.getDimension(), ResourceMap::GetAsUnsignedInteger( "GaussProductExperiment-DefaultMarginalSize" )));
}

/* Constructor with parameters */
GaussProductExperiment::GaussProductExperiment(const Distribution & distribution,
    const Indices & marginalSizes)
  : WeightedExperimentImplementation()
  , collection_(0)
  , marginalSizes_(0)
  , nodes_(0, 0)
{
  setDistributionAndMarginalSizes(distribution, marginalSizes);
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
      << " marginal sizes=" << marginalSizes_;
  return oss;
}

/* Distribution accessor */
void GaussProductExperiment::setDistribution(const Distribution & distribution)
{
  if (!distribution.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: the GaussProductExperiment can only be used with distributions having an independent copula.";
  const UnsignedInteger dimension = distribution.getDimension();
  if (dimension != marginalSizes_.getSize()) throw InvalidArgumentException(HERE) << "Error: the given distribution has a dimension=" << dimension << "different from the number of marginal sizes =" << marginalSizes_.getSize() << ".";
  collection_ = OrthogonalUniVariatePolynomialFamilyCollection(0);
  // Here we use the StandardDistributionPolynomialFactory class directly in order to benefit from the possible mapping to dedicated factories
  // The affine transform between the marginals and their standard representatives will be applied in computeNodesAndWeights()
  for (UnsignedInteger i = 0; i < dimension; ++i) collection_.add(StandardDistributionPolynomialFactory(distribution.getMarginal(i)));
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

/** Marginal sizes accessor */
void GaussProductExperiment::setMarginalSizes(const Indices & marginalSizes)
{
  const UnsignedInteger dimension = distribution_.getDimension();
  if (marginalSizes.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the marginal sizes number must match the distribution dimension. Here, the sizes are " << marginalSizes << " and the dimension is " << dimension;
  if (marginalSizes != marginalSizes_)
  {
    marginalSizes_ = marginalSizes;
    isAlreadyComputedNodesAndWeights_ = false;
  }
}

/** Marginal sizes accessor */
void GaussProductExperiment::setDistributionAndMarginalSizes(const Distribution & distribution,
    const Indices & marginalSizes)
{
  // Set the marginal sizes here then the distribution with checks
  marginalSizes_ = marginalSizes;
  setDistribution(distribution);

  const UnsignedInteger dimension = distribution_.getDimension();
  size_ = 1;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    const UnsignedInteger dI = marginalSizes_[i];
    size_ *= dI;
  }
}

Indices GaussProductExperiment::getMarginalSizes() const
{
  return marginalSizes_;
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
    const UnsignedInteger dI = marginalSizes_[i];
    const Distribution marginalI(distribution_.getMarginal(i));
    const Distribution standardMarginalI(marginalI.getStandardRepresentative());
    // Here we compute the affine transform mapping the standard marginal into the marginal
    // alpha -> a
    // beta  -> b
    // x     -> y
    // (y - a) / (b - a) = (x - alpha) / (beta - alpha)
    // y = a + (x - alpha) * (b - a) / (beta - alpha)
    const Scalar alpha = standardMarginalI.getRange().getLowerBound()[0];
    const Scalar beta  = standardMarginalI.getRange().getUpperBound()[0];
    const Scalar a     = marginalI.getRange().getLowerBound()[0];
    const Scalar b     = marginalI.getRange().getUpperBound()[0];
    const Scalar m = (b - a) / (beta - alpha);
    marginalNodes[i] = collection_[i].getNodesAndWeights(dI, marginalWeights[i]);
    // Transform the nodes if needed
    if (!(a == 0.0 && m == 1.0))
      for (UnsignedInteger j = 0; j < marginalNodes[i].getSize(); ++j)
        marginalNodes[i][j] = a + m * (marginalNodes[i][j] - alpha);
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
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j + 1] += (indices[j] == marginalSizes_[j]);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j] = indices[j] % marginalSizes_[j];
  } // Loop over the n-D nodes
  isAlreadyComputedNodesAndWeights_ = true;
}

/* Set size
void GaussProductExperiment::setSize(const UnsignedInteger size)
{
  const Indices marginalDegrees = {size};
  setMarginalDegrees(marginalDegrees);
}*/

/* Method save() stores the object through the StorageManager */
void GaussProductExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("marginalSizes_", marginalSizes_);
}

/* Method load() reloads the object from the StorageManager */
void GaussProductExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  if (adv.hasAttribute("marginalSizes_"))
    // new name
    adv.loadAttribute("marginalSizes_", marginalSizes_);
  else
    // old name
    adv.loadAttribute("marginalDegrees_", marginalSizes_);
  setDistributionAndMarginalSizes(distribution_, marginalSizes_);
}


END_NAMESPACE_OPENTURNS
