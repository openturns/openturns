//                                               -*- C++ -*-
/**
 *  @brief
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
#include "UserDefinedSpectralModel.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< HermitianMatrix >);
static const Factory<PersistentCollection<HermitianMatrix> > RegisteredFactory1;


CLASSNAMEINIT(UserDefinedSpectralModel);
static const Factory<UserDefinedSpectralModel> RegisteredFactory;
/* Constructor with parameters */
UserDefinedSpectralModel::UserDefinedSpectralModel()
  : SpectralModelImplementation()
  , DSPCollection_(0)
{
  dimension_ = 0;
}

UserDefinedSpectralModel::UserDefinedSpectralModel(const RegularGrid & frequencyGrid,
    const HermitianMatrixCollection & spectralFunction)
  : SpectralModelImplementation(),
    DSPCollection_(0)
{
  const UnsignedInteger N(frequencyGrid.getN());
  if (N != spectralFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: Frequency grid and spectral functions have different sizes";
  if (frequencyGrid.getStart() < 0.0)
    throw InvalidArgumentException(HERE) << "Error: The frequency grid must contains only nonnegative values";
  setFrequencyGrid(frequencyGrid);
  DSPCollection_ = HermitianMatrixCollection(N);
  // put the first element
  DSPCollection_[0] = spectralFunction[0];
  DSPCollection_[0].checkHermitian();
  setDimension(DSPCollection_[0].getDimension());
  // put the next elements if dimension is ok
  for (UnsignedInteger k = 1; k < N; ++k)
  {
    if (spectralFunction[k].getDimension() != dimension_)
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
HermitianMatrix UserDefinedSpectralModel::operator() (const NumericalScalar frequency) const
{
  Bool nonNegative(frequency >= 0.0);
  // If the grid size is one , return the spectral function
  // else find in the grid the nearest frequency value
  if (getFrequencyGrid().getN() == 1) return DSPCollection_[0];
  const NumericalScalar frequencyStep(getFrequencyGrid().getStep());
  const UnsignedInteger nFrequency(getFrequencyGrid().getN());
  const UnsignedInteger index(std::min<UnsignedInteger>( nFrequency - 1, static_cast<UnsignedInteger>( std::max<NumericalScalar>( 0.0, nearbyint( ( fabs(frequency) - frequencyGrid_.getStart() ) / frequencyStep) ) ) ));
  // Use the relation S(-f) = conjugate(S(f))
  return (nonNegative ? DSPCollection_[index] : DSPCollection_[index].conjugate());
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
      << " dimension=" << dimension_
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
