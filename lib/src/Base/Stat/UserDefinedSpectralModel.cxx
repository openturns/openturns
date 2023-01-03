//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UserDefinedSpectralModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< HermitianMatrix >)
static const Factory<PersistentCollection<HermitianMatrix> > Factory_PersistentCollection_HermitianMatrix;


CLASSNAMEINIT(UserDefinedSpectralModel)
static const Factory<UserDefinedSpectralModel> Factory_UserDefinedSpectralModel;
/* Constructor with parameters */
UserDefinedSpectralModel::UserDefinedSpectralModel()
  : SpectralModelImplementation()
  , DSPCollection_(0)
{
  outputDimension_ = 0;
}

UserDefinedSpectralModel::UserDefinedSpectralModel(const RegularGrid & frequencyGrid,
    const HermitianMatrixCollection & spectralFunction)
  : SpectralModelImplementation(),
    DSPCollection_(0)
{
  const UnsignedInteger N = frequencyGrid.getN();
  if (N != spectralFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: Frequency grid and spectral functions have different sizes";
  if (!(frequencyGrid.getStart() >= 0.0))
    throw InvalidArgumentException(HERE) << "Error: The frequency grid must contain only nonnegative values";
  setFrequencyGrid(frequencyGrid);
  DSPCollection_ = HermitianMatrixCollection(N);
  // put the first element
  DSPCollection_[0] = spectralFunction[0];
  DSPCollection_[0].checkHermitian();
  setOutputDimension(DSPCollection_[0].getDimension());
  // put the next elements if dimension is ok
  for (UnsignedInteger k = 1; k < N; ++k)
  {
    if (spectralFunction[k].getDimension() != outputDimension_)
      throw InvalidArgumentException(HERE) << " Error with dimension; the spectral matrices should be of same dimension";
    DSPCollection_[k] = spectralFunction[k];
    DSPCollection_[k].checkHermitian();
  }
}

/* Virtual constructor */
UserDefinedSpectralModel * UserDefinedSpectralModel::clone() const
{
  return new UserDefinedSpectralModel(*this);
}


/* Computation of the spectral density function */
HermitianMatrix UserDefinedSpectralModel::operator() (const Scalar frequency) const
{
  Bool nonNegative = frequency >= 0.0;
  // If the grid size is one , return the spectral function
  // else find in the grid the nearest frequency value
  if (getFrequencyGrid().getN() == 1) return DSPCollection_[0];
  const Scalar frequencyStep = getFrequencyGrid().getStep();
  const UnsignedInteger nFrequency = getFrequencyGrid().getN();
  const UnsignedInteger index = std::min<UnsignedInteger>( nFrequency - 1, static_cast<UnsignedInteger>( std::max<Scalar>( 0.0, nearbyint( ( std::abs(frequency) - frequencyGrid_.getStart() ) / frequencyStep) ) ) );
  // Use the relation S(-f) = conjugate(S(f))
  return (nonNegative ? DSPCollection_[index] : DSPCollection_[index].conjugate());
}


/* Frequency grid accessors */
RegularGrid UserDefinedSpectralModel::getFrequencyGrid() const
{
  return frequencyGrid_;
}

void UserDefinedSpectralModel::setFrequencyGrid(const RegularGrid & frequencyGrid)
{
  frequencyGrid_ = frequencyGrid;
}

/* String converter */
String UserDefinedSpectralModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UserDefinedSpectralModel::GetClassName()
      << " DSPCollection=" << DSPCollection_
      << " frequencyGrid=" << getFrequencyGrid();
  return oss;
}

/* String converter */
String UserDefinedSpectralModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "frequency=" << frequencyGrid_.__str__(offset)
      << " dimension=" << outputDimension_
      << " DSPCollection=" << DSPCollection_.__str__(offset);
  return oss;
}

/* Method save() stores the object through the StorageManager */
void UserDefinedSpectralModel::save(Advocate & adv) const
{
  SpectralModelImplementation::save(adv);
  adv.saveAttribute( "DSPCollection_", DSPCollection_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedSpectralModel::load(Advocate & adv)
{
  SpectralModelImplementation::load(adv);
  adv.loadAttribute( "DSPCollection_", DSPCollection_);
}

END_NAMESPACE_OPENTURNS
