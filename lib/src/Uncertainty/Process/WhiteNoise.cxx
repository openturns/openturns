//                                               -*- C++ -*-
/**
 *  @brief A class which implements the WhiteNoise process
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/WhiteNoise.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/DatabaseFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WhiteNoise);

static const Factory<WhiteNoise> Factory_WhiteNoise;

/* Standard constructor */
WhiteNoise::WhiteNoise()
  : ProcessImplementation()
  , distribution_()
{
  setDistribution(Normal());
}

/* Standard constructor */
WhiteNoise::WhiteNoise(const Distribution & distribution)
/* throw (InvalidArgumentException) */
  : ProcessImplementation()
  , distribution_()
{
  // Check if the given distribution has a null mean
  setDistribution(distribution);
}

/* Standard constructor */
WhiteNoise::WhiteNoise(const Distribution & distribution,
                       const Mesh & mesh)
  : ProcessImplementation()
  , distribution_()
{
  // Set the dimension of the process
  // Check if the given distribution has a null mean
  setDistribution(distribution);
  setMesh(mesh);
}

/* Virtual constructor */
WhiteNoise * WhiteNoise::clone() const
{
  return new WhiteNoise(*this);
}

/* String converter */
String WhiteNoise::__repr__() const
{
  OSS oss;
  oss << "class=" << WhiteNoise::GetClassName()
      << " distribution=" << distribution_ ;
  return oss;
}

String WhiteNoise::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(" << distribution_.__str__() << ")";
  return oss;
}

/* Is the underlying stationary ? */
Bool WhiteNoise::isStationary() const
{
  return true;
}

/* Is the underlying gaussian ? */
Bool WhiteNoise::isNormal() const
{
  // The easy case: the distribution is an interface to
  // a Normal distribution
  if (distribution_.getImplementation()->getClassName() == "Normal") return true;
  // The hard case: the distribution has the properties of a Normal distribution
  return (distribution_.isElliptical() && distribution_.getStandardDistribution().hasIndependentCopula());
}

/* Realization accessor */
Field WhiteNoise::getRealization() const
{
  NumericalSample values(distribution_.getSample(mesh_.getVerticesNumber()));
  values.setDescription(getDescription());
  return Field(mesh_, values);
}

/* Continuous realization accessor */
Function WhiteNoise::getContinuousRealization() const
{
  return DatabaseFunction(mesh_.getVertices(), distribution_.getSample(mesh_.getVerticesNumber()));
}

/* Compute the next steps of a random walk */
TimeSeries WhiteNoise::getFuture(const UnsignedInteger stepNumber) const
{
  /* TimeGrid of the process */
  RegularGrid timeGrid;
  try
  {
    timeGrid = getTimeGrid();
  }
  catch (...)
  {
    throw InternalException(HERE) << "Error: can extend the realization of a process only if defined on a regular grid.";
  }
  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  /* TimeGrid associated with the possible future */
  const NumericalScalar timeStep = timeGrid.getStep();
  const RegularGrid futurTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  return TimeSeries(futurTimeGrid, distribution_.getSample(stepNumber));
}

/* Get the marginal process corresponding to the i-th marginal component */
WhiteNoise::Implementation WhiteNoise::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal process must be in the range [0, dim-1]";
  return WhiteNoise(distribution_.getMarginal(i), mesh_).clone();
}

/* Get the marginal random vector corresponding to indices components */
WhiteNoise::Implementation WhiteNoise::getMarginal(const Indices & indices) const
{
  if (!indices.check(getDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal process must be in the range [0, dim-1] and must be different";
  return new WhiteNoise(distribution_.getMarginal(indices), mesh_);
}

/* Distribution accessor */
Distribution WhiteNoise::getDistribution() const
{
  return distribution_;
}

/* Distribution accessor */
void WhiteNoise::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
  setDimension(distribution.getDimension());
  setDescription(distribution.getDescription());
}

/* Method save() stores the object through the StorageManager */
void WhiteNoise::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
}

/* Method load() reloads the object from the StorageManager */
void WhiteNoise::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
}
END_NAMESPACE_OPENTURNS
