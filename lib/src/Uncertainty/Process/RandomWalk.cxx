//                                               -*- C++ -*-
/**
 *  @brief A class which implements the RandomWalk process
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/RandomWalk.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RandomWalk)

static const Factory<RandomWalk> Factory_RandomWalk;

/* Standard constructor */
RandomWalk::RandomWalk()
  : ProcessImplementation()
  , origin_(1)
  , distribution_(Normal())
  , currentPosition_(1)
{
  setDescription(distribution_.getDescription());
}

/* Standard constructor */
RandomWalk::RandomWalk(const Point & origin,
                       const Distribution & distribution)
  : ProcessImplementation()
  , origin_(origin)
  , distribution_()
  , currentPosition_(origin)
{
  // Set the dimension of the process
  setOutputDimension(origin.getDimension());
  setDistribution(distribution);
}

/* Standard constructor */
RandomWalk::RandomWalk(const Point & origin,
                       const Distribution & distribution,
                       const RegularGrid & timeGrid)
  : ProcessImplementation()
  , origin_(origin)
  , distribution_()
  , currentPosition_(origin)
{
  // Set the dimension of the process
  setOutputDimension(origin.getDimension());
  setDistribution(distribution);
  setTimeGrid(timeGrid);
}

/* Virtual constructor */
RandomWalk * RandomWalk::clone() const
{
  return new RandomWalk(*this);
}

/* String converter */
String RandomWalk::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomWalk::GetClassName()
      << ", origin=" << origin_
      << ", distribution=" << distribution_;
  return oss;
}

/* String converter */
String RandomWalk::__str__(const String & offset) const
{
  OSS oss;
  oss << "RandomWalk(origin=" << origin_.__str__(offset)
      << ", distribution=" << distribution_.__str__(offset)
      << ")";
  return oss;
}

/* Is the underlying stationary ? */
Bool RandomWalk::isStationary() const
{
  return false;
}

/* Is the underlying gaussian ? */
Bool RandomWalk::isNormal() const
{
  // The easy case: the distribution is an interface to
  // a Normal distribution
  if (distribution_.getImplementation()->getClassName() == "Normal") return true;
  // The hard case: the distribution has the properties of a Normal distribution
  return (distribution_.isElliptical() && distribution_.getStandardDistribution().hasIndependentCopula());
}

/* Realization accessor */
Field RandomWalk::getRealization() const
{
  const UnsignedInteger size = mesh_.getVerticesNumber();
  Sample data(size, origin_);
  for (UnsignedInteger i = 1; i < size; ++i) data[i] = data[i - 1] + distribution_.getRealization();
  currentPosition_ = data[data.getSize() - 1];
  data.setDescription(getDescription());
  return Field(mesh_, data);
}

/* Compute the next steps of a random walk */
TimeSeries RandomWalk::getFuture(const UnsignedInteger stepNumber) const
{
  /* TimeGrid of the process */
  RegularGrid timeGrid(getTimeGrid());

  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";

  /* TimeGrid associated with the possible future */
  const Scalar timeStep = RegularGrid(mesh_).getStep();
  const RegularGrid futurTimeGrid(RegularGrid(mesh_).getEnd(), timeStep, stepNumber);
  Sample data(stepNumber, getOutputDimension());
  Point previous(currentPosition_);
  for (UnsignedInteger i = 0; i < stepNumber; ++i)
  {
    data[i] = previous + distribution_.getRealization();
    previous = data[i];
  }
  return TimeSeries(futurTimeGrid, data);
}

/* Get the marginal process corresponding to indices components */
Process RandomWalk::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal process must be in the range [0, dim-1] and must be different";
  const UnsignedInteger size = indices.getSize();
  Point marginalOrigin(size);
  for (UnsignedInteger i = 0; i < size; ++i) marginalOrigin[i] = origin_[indices[i]];
  return new RandomWalk(marginalOrigin, distribution_.getMarginal(indices), mesh_);
}

/* Distribution accessor */
Distribution RandomWalk::getDistribution() const
{
  return distribution_;
}

/* Distribution accessor */
void RandomWalk::setDistribution(const Distribution & distribution)
{
  if (distribution.getDimension() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the given distribution has a dimension=" << distribution.getDimension() << " incompatible with the process dimension=" << getOutputDimension();
  distribution_ = distribution;
  setDescription(distribution_.getDescription());
}

/* Origin accessor */
Point RandomWalk::getOrigin() const
{
  return origin_;
}

void RandomWalk::setOrigin(const Point & origin)
{
  if (origin.getDimension() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the given origin has a dimension=" << origin.getDimension() << " incompatible with the process dimension=" << getOutputDimension();
  origin_ = origin;
}

/* Mesh accessor */
void RandomWalk::setMesh(const Mesh & mesh)
{
  if(!mesh.isRegular()) throw InvalidArgumentException(HERE) << "Error: the mesh must be regular.";
  ProcessImplementation::setMesh(mesh);
}

/* Method save() stores the object through the StorageManager */
void RandomWalk::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "origin_", origin_ );
  adv.saveAttribute( "currentPosition_", currentPosition_ );
}

/* Method load() reloads the object from the StorageManager */
void RandomWalk::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "origin_", origin_ );
  adv.loadAttribute( "currentPosition_", currentPosition_ );
}

END_NAMESPACE_OPENTURNS
