//                                               -*- C++ -*-
/**
 *  @brief A class which implements the AggregatedProcess process
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

#include "openturns/AggregatedProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/WhiteNoise.hxx"
#include "openturns/TensorizedCovarianceModel.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< Process >)
CLASSNAMEINIT(AggregatedProcess)

static const Factory<AggregatedProcess> Factory_AggregatedProcess;
static const Factory<PersistentCollection<Process> > Factory_PersistentCollection_Process;

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

String AggregatedProcess::__str__(const String & ) const
{
  OSS oss(false);
  oss << "AggregatedProcess(" << processCollection_ << ")";
  return oss;
}

/* Realization accessor */
Field AggregatedProcess::getRealization() const
{
  Sample values(processCollection_[0].getRealization().getValues());
  for (UnsignedInteger i = 1; i < processCollection_.getSize(); ++i)
    values.stack(processCollection_[i].getRealization().getValues());
  return Field(getMesh(), values);
}

/* Continuous realization accessor */
Function AggregatedProcess::getContinuousRealization() const
{
  const UnsignedInteger size = processCollection_.getSize();
  Collection<Function> continuousRealizations(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    continuousRealizations[i] = processCollection_[i].getContinuousRealization();
  return AggregatedFunction(continuousRealizations);
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
  catch (const InvalidArgumentException &)
  {
    throw InternalException(HERE) << "Error: can extend the realization of a process only if defined on a regular grid.";
  }
  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  /* TimeGrid associated with the possible future */
  const Scalar timeStep = timeGrid.getStep();
  const RegularGrid futurTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  Sample values(processCollection_[0].getFuture(stepNumber).getValues());
  for (UnsignedInteger i = 1; i < processCollection_.getSize(); ++i) values.stack(processCollection_[i].getFuture(stepNumber).getValues());

  return TimeSeries(futurTimeGrid, values);
}

/* Get the random vector corresponding to the i-th marginal component */
Process AggregatedProcess::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index must be less than the output dimension";
  UnsignedInteger lastBlock = 0;
  for (UnsignedInteger k = 0; k < processCollection_.getSize(); ++k)
  {
    const UnsignedInteger block = lastBlock + processCollection_[k].getOutputDimension();
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
Process AggregatedProcess::getMarginal(const Indices & indices) const
{
  const UnsignedInteger outputDimension = getOutputDimension();
  if (!indices.check(outputDimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal process must be in the range [0, dim-1] and must be different";
  ProcessCollection marginalProcesses(0);
  const UnsignedInteger indicesSize = indices.getSize();
  const UnsignedInteger size = processCollection_.getSize();
  // For each copula, see if there is something to extract
  UnsignedInteger currentPosition = 0;
  UnsignedInteger currentIndex = indices[currentPosition];
  // Lower bound of indices related to the current copula
  UnsignedInteger lowerIndex = 0;
  // Upper bound of indices related to the current copula plus 1
  UnsignedInteger upperIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Process process(processCollection_[i]);
    // Update index range for the current copula
    lowerIndex = upperIndex;
    upperIndex += process.getOutputDimension();
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
  const UnsignedInteger size = coll.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an aggregated process based on an empty process collection.";
  processCollection_ = ProcessCollection(size);
  processCollection_[0] = coll[0];
  const UnsignedInteger inputDimension = processCollection_[0].getInputDimension();
  UnsignedInteger outputDimension = processCollection_[0].getOutputDimension();
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (coll[i].getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected an input dimension=" << inputDimension << ", got process " << i << " with an input dimension=" << coll[i].getInputDimension();
    processCollection_[i] = coll[i];
    outputDimension += coll[i].getOutputDimension();
  }
  setOutputDimension(outputDimension);
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
  if (mesh.getDimension() != processCollection_[0].getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the given mesh has a dimension=" << mesh.getDimension() << " which is different from the input dimension of the aggregated process input dimension=" << processCollection_[0].getInputDimension();
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

/* Is the underlying a gaussian process ? */
Bool AggregatedProcess::isNormal() const
{
  for (UnsignedInteger i = 0; i < processCollection_.getSize(); ++i)
    if (!processCollection_[i].isNormal()) return false;
  return true;
}

/* Is the underlying a stationary process ? */
Bool AggregatedProcess::isStationary() const
{
  for (UnsignedInteger i = 0; i < processCollection_.getSize(); ++i)
    if (!processCollection_[i].isStationary()) return false;
  return true;
}

/* Covariance model accessor */
CovarianceModel AggregatedProcess::getCovarianceModel() const
{
  Collection<CovarianceModel> coll(processCollection_.getSize());
  for (UnsignedInteger i = 0; i < processCollection_.getSize(); ++i)
    coll[i] = processCollection_[i].getCovarianceModel();
  return TensorizedCovarianceModel(coll);
}

/* Trend accessor */
TrendTransform AggregatedProcess::getTrend() const
{
  throw NotYetImplementedException(HERE) << "AggregatedProcess::getTrend()";
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
