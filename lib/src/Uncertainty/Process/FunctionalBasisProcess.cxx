//                                               -*- C++ -*-
/**
 *  @brief A class which implements the FunctionalBasisProcess process
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FunctionalBasisProcess.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Point.hxx"
#include "openturns/Collection.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/LinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FunctionalBasisProcess)

static const Factory<FunctionalBasisProcess> Factory_FunctionalBasisProcess;

/* Standard constructor */
FunctionalBasisProcess::FunctionalBasisProcess()
  : ProcessImplementation()
  , distribution_(Normal())
  , basis_(1, SymbolicFunction("t", "t"))
  , state_(1, 0.0)
{
  // Set the dimension of the process
  setOutputDimension(1);
  setDescription(basis_[0].getOutputDescription());
}

/* Standard constructor */
FunctionalBasisProcess::FunctionalBasisProcess(const Distribution & distribution,
    const FunctionCollection & basis)
  : ProcessImplementation()
  , distribution_(distribution)
  , basis_()
  , state_(distribution.getDimension(), 0.0)
{
  // Check the given basis against the distribution
  if (basis.getSize() > 0) setMesh(Mesh(basis[0].getInputDimension()));
  setBasis(basis);
}

/* Standard constructor */
FunctionalBasisProcess::FunctionalBasisProcess(const Distribution & distribution,
    const FunctionCollection & basis,
    const Mesh & mesh)
  : ProcessImplementation()
  , distribution_(distribution)
  , basis_()
  , state_(distribution.getDimension(), 0.0)
{
  // Check the given basis against the distribution
  setMesh(mesh);
  setBasis(basis);
}

/* Virtual constructor */
FunctionalBasisProcess * FunctionalBasisProcess::clone() const
{
  return new FunctionalBasisProcess(*this);
}

/* String converter */
String FunctionalBasisProcess::__repr__() const
{
  OSS oss;
  oss << "class=" << FunctionalBasisProcess::GetClassName()
      << " distribution=" << distribution_
      << " basis=" << basis_;
  return oss;
}

/* String converter */
String FunctionalBasisProcess::__str__(const String & offset) const
{
  OSS oss;
  oss << "FunctionalBasisProcess(distribution=" << distribution_.__str__(offset)
      << ", basis=" << basis_.__str__(offset)
      << ")";
  return oss;
}

/* Covariance model accessor */
CovarianceModel FunctionalBasisProcess::getCovarianceModel() const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  if (distribution_.hasIndependentCopula())
  {
    // We use the standard deviation as it is an O(dim) computation
    // in the general case, while covariance is an O(n^2) computation
    Point coefficients(distribution_.getStandardDeviation());
    for (UnsignedInteger i = 0; i < dimension; ++i)
      coefficients[i] *= coefficients[i];
    return RankMCovarianceModel(coefficients, basis_);
  }
  return RankMCovarianceModel(distribution_.getCovariance(), basis_);
}

/* Is the process stationary ? */
Bool FunctionalBasisProcess::isStationary() const
{
  return false;
}

/* Is the process gaussian ? */
Bool FunctionalBasisProcess::isNormal() const
{
  // The easy case: the distribution is an interface to
  // a Normal distribution
  if (distribution_.getImplementation()->getClassName() == "Normal") return true;
  // The hard case: the distribution has the properties of a Normal distribution
  return (distribution_.isElliptical() && distribution_.getStandardDistribution().hasIndependentCopula());
}

/* Discrete realization accessor */
Field FunctionalBasisProcess::getRealization() const
{
  state_ = distribution_.getRealization();
  const UnsignedInteger timeGridSize = mesh_.getVerticesNumber();
  const UnsignedInteger basisSize = basis_.getSize();
  // Loop over the time stamps
  Sample result(timeGridSize, Point(getOutputDimension(), 0.0));
  // Loop over the basis
  for (UnsignedInteger j = 0; j < basisSize; ++j)
  {
    Sample currentBasisContribution(basis_[j](mesh_.getVertices()));
    currentBasisContribution *= Point(getOutputDimension(), state_[j]);
    result += currentBasisContribution;
  }
  result.setDescription(getDescription());
  return Field(mesh_, result);
}


/* Continuous realization accessor */
Function FunctionalBasisProcess::getContinuousRealization() const
{
  state_ = distribution_.getRealization();
  return LinearCombinationFunction(basis_, state_);
}

/* Compute the next steps of the process */
TimeSeries FunctionalBasisProcess::getFuture(const UnsignedInteger stepNumber) const
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
  const Scalar timeStep = timeGrid.getStep();
  const RegularGrid futureTimeGrid(timeGrid.getEnd(), timeStep, stepNumber);
  const UnsignedInteger basisSize = basis_.getSize();
  // Loop over the time stamps
  Sample result(stepNumber, Point(getOutputDimension(), 0.0));
  for (UnsignedInteger i = 0; i  < stepNumber; ++i)
  {
    const Point t(1, futureTimeGrid.getValue(i));
    // Loop over the basis using the previous state
    for (UnsignedInteger j = 0; j < basisSize; ++j) result[i] += basis_[j](t) * state_[j];
  }
  return TimeSeries(futureTimeGrid, result);
}

/* Get the marginal random vector corresponding to indices components */
Process FunctionalBasisProcess::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "The indices of a marginal process must be in the range [0, dim-1] and must be different";
  // First the marginal distribution
  Distribution marginalDistribution(distribution_.getMarginal(indices));
  // Second the marginal basis
  const UnsignedInteger basisSize = basis_.getSize();
  FunctionCollection marginalBasis(basisSize);
  for (UnsignedInteger i = 0; i < basisSize; ++i) marginalBasis[i] = basis_[i].getMarginal(indices);
  // Return the associated FunctionalBasisProcess
  return new FunctionalBasisProcess(marginalDistribution, marginalBasis, mesh_);
}

/* Distribution accessor */
Distribution FunctionalBasisProcess::getDistribution() const
{
  return distribution_;
}

/* Distribution accessor */
void FunctionalBasisProcess::setDistribution(const Distribution & distribution)
{
  // Check the distribution against the basis
  if (distribution.getDimension() != basis_.getSize()) throw InvalidArgumentException(HERE) << "Error: the given distribution has a dimension=" << distribution.getDimension() << " that does not match the basis size=" << basis_.getSize();
  distribution_ = distribution;
}

/* Basis accessor */
FunctionalBasisProcess::FunctionCollection FunctionalBasisProcess::getBasis() const
{
  return basis_;
}

/* Basis accessor */
void FunctionalBasisProcess::setBasis(const FunctionCollection & basis)
{
  const UnsignedInteger size = basis.getSize();
  // Check if the basis is not empty
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given basis is empty.";
  // Check the basis against the distribution
  if (size != distribution_.getDimension() ) throw InvalidArgumentException(HERE) << "Error: the given basis has a size=" << size << " that does not match the distribution dimension=" << distribution_.getDimension();
  // Check if the functions in the basis are from R to R^n for the same n
  setOutputDimension(basis[0].getOutputDimension());
  const UnsignedInteger inputDimension = mesh_.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Check the input dimension
    if (basis[i].getInputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the function at index=" << i << " has an input dimension=" << basis[i].getInputDimension() << " which is not equal to " << inputDimension << ".";
    // Check the output dimension
    if (basis[i].getOutputDimension() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the function at index=" << i << " has an output dimension=" << basis[i].getOutputDimension() << " which is not equal to the process dimension=" << getOutputDimension();
  }
  basis_ = basis;
  setDescription(basis[0].getOutputDescription());
}

/* Method save() stores the object through the StorageManager */
void FunctionalBasisProcess::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "state_", state_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalBasisProcess::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "state_", state_ );
}
END_NAMESPACE_OPENTURNS
