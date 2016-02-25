//                                               -*- C++ -*-
/**
 *  @brief A class which implements the AggregatedProcess process
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/AggregatedProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/WhiteNoise.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< Process >);
CLASSNAMEINIT(AggregatedProcess);

static const Factory<AggregatedProcess> RegisteredFactory;
static const Factory<PersistentCollection<Process> > RegisteredFactory1;

AggregatedProcess::AggregatedProcess()
  : ProcessImplementation()
  , processCollection_(ProcessCollection(1, WhiteNoise()))
{
  // Nothing to do
}

/* Standard constructor */
AggregatedProcess::AggregatedProcess(const ProcessCollection & coll)
  : ProcessImplementation()
  , processCollection_()
{
  setProcessCollection(coll);
}

/* Virtual constructor */
AggregatedProcess * AggregatedProcess::clone() const
{
  return new AggregatedProcess(*this);
}

/* String converter */
String AggregatedProcess::__repr__() const
{
  OSS oss(true);
  oss << "class=" << AggregatedProcess::GetClassName()
      << " process collection=" << processCollection_;
  return oss;
}

String AggregatedProcess::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "AggregatedProcess(" << processCollection_ << ")";
  return oss;
}

/* Realization accessor */
Field AggregatedProcess::getRealization() const
{
  NumericalSample values(processCollection_[0].getRealization().getValues());
  for (UnsignedInteger i = 1; i < processCollection_.getSize(); ++i)
    values.stack(processCollection_[i].getRealization().getValues());
  return Field(getMesh(), values);
}

/* Continuous realization accessor */
NumericalMathFunction AggregatedProcess::getContinuousRealization() const
{
  const UnsignedInteger size(processCollection_.getSize());
  Collection<NumericalMathFunction> continuousRealizations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    continuousRealizations[i] = processCollection_[i].getContinuousRealization();
  return NumericalMathFunction(continuousRealizations);
}

/* Compute the next steps of a random walk */
TimeSeries AggregatedProcess::getFuture(const UnsignedInteger stepNumber) const
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
  const NumericalScalar timeStep(timeGrid.getStep());
  const RegularGrid futurTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  NumericalSample values(processCollection_[0].getFuture(stepNumber).getValues());
  for (UnsignedInteger i = 1; i < processCollection_.getSize(); ++i) values.stack(processCollection_[i].getFuture(stepNumber).getValues());

  return TimeSeries(futurTimeGrid, values);
}

/* Get the random vector corresponding to the i-th marginal component */
AggregatedProcess::Implementation AggregatedProcess::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "Error: the index must be less than the output dimension";
  UnsignedInteger lastBlock(0);
  for (UnsignedInteger k = 0; k < processCollection_.getSize(); ++k)
  {
    const UnsignedInteger block(lastBlock + processCollection_[k].getDimension());
    if (i < block) return processCollection_[k].getMarginal(i - lastBlock).getImplementation();
    lastBlock = block;
  }
  throw InternalException(HERE) << "AggregatedProcess::getMarginal: should never go there";
}

/* Get the marginal process corresponding to indices dimensions
   Some additional restriction occur for this copula: the indices must be of the form:
   [i_1^1,...,i_k1^1,i_1^2,...,i_k2^2,...,i_1^n,...,i_kn^n]
   where:
   i_1^1,...,i_k1^1 is a subset of {0,...,dim_1-1},
   i_1^2,...,i_k2^2 is a subset of {0,...,dim_2-1}+dim_1,
   i_1^n,...,i_kn^n is a subset of {0,...,dim_n-1}+dim_1+...+dim_(n-1),
   dim_1 = dimension(copula_1) etc.
*/
AggregatedProcess::Implementation AggregatedProcess::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal process must be in the range [0, dim-1] and  must be different";
  ProcessCollection marginalProcesses(0);
  const UnsignedInteger indicesSize(indices.getSize());
  const UnsignedInteger size(processCollection_.getSize());
  // For each copula, see if there is something to extract
  UnsignedInteger currentPosition(0);
  UnsignedInteger currentIndex(indices[currentPosition]);
  // Lower bound of indices related to the current copula
  UnsignedInteger lowerIndex(0);
  // Upper bound of indices related to the current copula plus 1
  UnsignedInteger upperIndex(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Process process(processCollection_[i]);
    // Update index range for the current copula
    lowerIndex = upperIndex;
    upperIndex += process.getDimension();
    Indices processIndices(0);
    // Find the indices related to the current copula
    while ((currentPosition < indicesSize) && (currentIndex >= lowerIndex) && (currentIndex < upperIndex))
    {
      processIndices.add(currentIndex - lowerIndex);
      // Go to next index
      ++currentPosition;
      if (currentPosition == indicesSize) break;
      currentIndex = indices[currentPosition];
    }
    // If there is something to extract
    if (processIndices.getSize() > 0) marginalProcesses.add(processCollection_[i].getMarginal(processIndices));
    // All the indices have been taken into account
    if (currentPosition == indicesSize) break;
    // Check if a bad case occurs: one index related to copula i is found after indices related to copula j, with j > i
    if (currentIndex < lowerIndex) throw InvalidArgumentException(HERE) << "Error: one index related to the ith process has been found after indices related to the jth process, with j > i";
  }
  return new AggregatedProcess(marginalProcesses);
}

/* Process collection accessor */
void AggregatedProcess::setProcessCollection(const ProcessCollection & coll)
{
  const UnsignedInteger size(coll.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an aggregated process based on an empty process collection.";
  processCollection_ = ProcessCollection(size);
  processCollection_[0] = coll[0];
  const UnsignedInteger spatialDimension(processCollection_[0].getSpatialDimension());
  UnsignedInteger dimension(processCollection_[0].getDimension());
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (coll[i].getSpatialDimension() != spatialDimension) throw InvalidArgumentException(HERE) << "Error: expected a spatial dimension=" << spatialDimension << ", got process " << i << " with a spatial dimension=" << coll[i].getSpatialDimension();
    processCollection_[i] = coll[i];
    dimension += coll[i].getDimension();
  }
  setDimension(dimension);
  setMesh(processCollection_[0].getMesh());
}

AggregatedProcess::ProcessCollection AggregatedProcess::getProcessCollection() const
{
  return processCollection_;
}

/* Mesh accessor */
void AggregatedProcess::setMesh(const Mesh & mesh)
{
  // We know that an AggregatedProcess cannot be built with an empty process collection
  if (mesh.getDimension() != processCollection_[0].getSpatialDimension()) throw InvalidArgumentException(HERE) << "Error: the given mesh has a dimension=" << mesh.getDimension() << " which is different from the spatial dimension of the aggregated process spatial dimension=" << processCollection_[0].getSpatialDimension();
  for (UnsignedInteger i = 0; i < processCollection_.getSize(); ++i)
    processCollection_[i].setMesh(mesh);
  ProcessImplementation::setMesh(mesh);
}

/* Time grid accessor */
void AggregatedProcess::setTimeGrid(const RegularGrid & timeGrid)
{
  for (UnsignedInteger i = 0; i < processCollection_.getSize(); ++i)
    processCollection_[i].setTimeGrid(timeGrid);
  ProcessImplementation::setTimeGrid(timeGrid);
}

/* Method save() stores the object through the StorageManager */
void AggregatedProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "processCollection_", processCollection_ );
}

/* Method load() reloads the object from the StorageManager */
void AggregatedProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "processCollection_", processCollection_ );
}

END_NAMESPACE_OPENTURNS
