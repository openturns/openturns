//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include <algorithm>
#include "openturns/PiecewiseLinearEvaluationImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseLinearEvaluationImplementation);

static const Factory<PiecewiseLinearEvaluationImplementation> RegisteredFactory;


/* Default constructor */
PiecewiseLinearEvaluationImplementation::PiecewiseLinearEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
{
  // Nothing to do
}


/* Parameters constructor */
PiecewiseLinearEvaluationImplementation::PiecewiseLinearEvaluationImplementation(const NumericalPoint & locations,
    const NumericalPoint & values)
  : NumericalMathEvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
{
  // Convert the values into a sample
  const UnsignedInteger size(values.getSize());
  NumericalSample sampleValues(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) sampleValues[i][0] = values[i];
  // Check the input
  setLocationsAndValues(locations, sampleValues);
}

/* Parameters constructor */
PiecewiseLinearEvaluationImplementation::PiecewiseLinearEvaluationImplementation(const NumericalPoint & locations,
    const NumericalSample & values)
  : NumericalMathEvaluationImplementation()
  , locations_(0)
  , values_(0, values.getDimension())
{
  setLocationsAndValues(locations, values);
}


/* Virtual constructor */
PiecewiseLinearEvaluationImplementation * PiecewiseLinearEvaluationImplementation::clone() const
{
  return new PiecewiseLinearEvaluationImplementation(*this);
}


/* String converter */
String PiecewiseLinearEvaluationImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " locations=" << locations_
         << " values=" << values_;
}

String PiecewiseLinearEvaluationImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}


/* Evaluation operator */
NumericalPoint PiecewiseLinearEvaluationImplementation::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const NumericalScalar x(inP[0]);
  UnsignedInteger iLeft(0);
  if (x <= locations_[iLeft]) return values_[iLeft];
  UnsignedInteger iRight(locations_.getSize() - 1);
  if (x >= locations_[iRight]) return values_[iRight];
  if (isRegular_)
  {
    iLeft = static_cast<UnsignedInteger>(floor((x - locations_[0]) / (locations_[1] - locations_[0])));
    iRight = iLeft + 1;
  }
  else
    // Find the segment containing x by bisection
    while (iRight - iLeft > 1)
    {
      const UnsignedInteger im((iRight + iLeft) / 2);
      if (x < locations_[im]) iRight = im;
      else iLeft = im;
    }

  const NumericalScalar xLeft(locations_[iLeft]);
  const NumericalScalar xRight(locations_[iRight]);
  const NumericalScalar dx(xLeft - xRight);
  const NumericalPoint vLeft(values_[iLeft]);
  const NumericalPoint vRight(values_[iRight]);
  const UnsignedInteger dimension(getOutputDimension());
  NumericalPoint value(dimension);
  const NumericalScalar alpha((x - xRight) / dx);
  const NumericalScalar beta((xLeft - x) / dx);
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = alpha * vLeft[i] + beta * vRight[i];
  return value;
}

/* Locations accessor */
NumericalPoint PiecewiseLinearEvaluationImplementation::getLocations() const
{
  return locations_;
}

void PiecewiseLinearEvaluationImplementation::setLocations(const NumericalPoint & locations)
{
  const UnsignedInteger size(locations.getSize());
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (locations.getSize() != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of locations=" << size << " must match the number of previously set values=" << values_.getSize();
  const NumericalScalar step(locations_[0] - locations_[0]);
  const NumericalScalar epsilon(ResourceMap::GetAsNumericalScalar("PiecewiseHermiteEvaluationImplementation-EpsilonRegular") * std::abs(step));
  isRegular_ = true;
  for (UnsignedInteger i = 0; i < size; ++i) isRegular_ = isRegular_ && (std::abs(locations[i] - locations[0] - i * step) < epsilon);
  locations_ = locations;
  std::stable_sort(locations_.begin(), locations_.end());
}

/* Values accessor */
NumericalSample PiecewiseLinearEvaluationImplementation::getValues() const
{
  return values_;
}

void PiecewiseLinearEvaluationImplementation::setValues(const NumericalPoint & values)
{
  const UnsignedInteger size(values.getSize());
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  NumericalSample sampleValues(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) sampleValues[i][0] = values[i];
  values_ = sampleValues;
}

void PiecewiseLinearEvaluationImplementation::setValues(const NumericalSample & values)
{
  const UnsignedInteger size(values.getSize());
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  values_ = values;
}

void PiecewiseLinearEvaluationImplementation::setLocationsAndValues(const NumericalPoint & locations,
    const NumericalSample & values)
{
  const UnsignedInteger size(locations.getSize());
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << values.getSize() << " must match the number of locations=" << size;
  // Sort the data in increasing order according to the locations
  const UnsignedInteger dimension(values.getDimension());
  NumericalSample data(size, 1 + dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i][0] = locations[i];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      data[i][j + 1] = values[i][j];
  }
  data = data.sortAccordingToAComponent(0);
  locations_ = NumericalPoint(size);
  values_ = NumericalSample(size, dimension);
  const NumericalScalar step(data[1][0] - data[0][0]);
  const NumericalScalar epsilon(ResourceMap::GetAsNumericalScalar("PiecewiseLinearEvaluationImplementation-EpsilonRegular") * std::abs(step));
  isRegular_ = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    locations_[i] = data[i][0];
    isRegular_ = isRegular_ && (std::abs(locations_[i] - locations_[0] - i * step) < epsilon);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      values_[i][j] = data[i][j + 1];
  }
}

/* Input dimension accessor */
UnsignedInteger PiecewiseLinearEvaluationImplementation::getInputDimension() const
{
  return 1;
}

/* Output dimension accessor */
UnsignedInteger PiecewiseLinearEvaluationImplementation::getOutputDimension() const
{
  return values_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void PiecewiseLinearEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "locations_", locations_ );
  adv.saveAttribute( "values_", values_ );
}


/* Method load() reloads the object from the StorageManager */
void PiecewiseLinearEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "locations_", locations_ );
  adv.loadAttribute( "values_", values_ );
}


END_NAMESPACE_OPENTURNS
