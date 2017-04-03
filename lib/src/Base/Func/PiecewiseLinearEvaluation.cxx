//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
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
#include <algorithm>
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseLinearEvaluation);

static const Factory<PiecewiseLinearEvaluation> Factory_PiecewiseLinearEvaluation;


/* Default constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation()
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
{
  // Nothing to do
}


/* Parameters constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation(const Point & locations,
    const Point & values)
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
{
  // Convert the values into a sample
  const UnsignedInteger size = values.getSize();
  Sample sampleValues(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) sampleValues[i][0] = values[i];
  // Check the input
  setLocationsAndValues(locations, sampleValues);
}

/* Parameters constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation(const Point & locations,
    const Sample & values)
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, values.getDimension())
{
  setLocationsAndValues(locations, values);
}


/* Virtual constructor */
PiecewiseLinearEvaluation * PiecewiseLinearEvaluation::clone() const
{
  return new PiecewiseLinearEvaluation(*this);
}


/* String converter */
String PiecewiseLinearEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " locations=" << locations_
         << " values=" << values_;
}

String PiecewiseLinearEvaluation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}


/* Evaluation operator */
Point PiecewiseLinearEvaluation::operator () (const Point & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const NumericalScalar x = inP[0];
  UnsignedInteger iLeft = 0;
  if (x <= locations_[iLeft])
    return values_[iLeft];
  UnsignedInteger iRight = locations_.getSize() - 1;
  if (x >= locations_[iRight])
    return values_[iRight];
  if (isRegular_)
  {
    iLeft = static_cast<UnsignedInteger>(floor((x - locations_[0]) / (locations_[1] - locations_[0])));
    iRight = iLeft + 1;
  }
  else
    // Find the segment containing x by bisection
    while (iRight - iLeft > 1)
    {
      const UnsignedInteger im = (iRight + iLeft) / 2;
      if (x < locations_[im]) iRight = im;
      else iLeft = im;
    }

  const NumericalScalar xLeft = locations_[iLeft];
  const NumericalScalar xRight = locations_[iRight];
  const NumericalScalar dx = xLeft - xRight;
  const Point vLeft(values_[iLeft]);
  const Point vRight(values_[iRight]);
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  const NumericalScalar alpha = (x - xRight) / dx;
  const NumericalScalar beta = (xLeft - x) / dx;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = alpha * vLeft[i] + beta * vRight[i];
  return value;
}

/* Locations accessor */
Point PiecewiseLinearEvaluation::getLocations() const
{
  return locations_;
}

void PiecewiseLinearEvaluation::setLocations(const Point & locations)
{
  const UnsignedInteger size = locations.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise linear interpolation function.";
  if (locations.getSize() != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of locations=" << size << " must match the number of previously set values=" << values_.getSize();
  Collection< std::pair<NumericalScalar, Point> > locationsAndValues(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    locationsAndValues[i] = std::pair<NumericalScalar, Point>(locations[i], values_[i]);
  std::stable_sort(locationsAndValues.begin(), locationsAndValues.end());
  locations_ = Point(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      locations_[i] = locationsAndValues[i].first;
      values_[i] = locationsAndValues[i].second;      
    }
  const NumericalScalar step = locations_[1] - locations_[0];
  const NumericalScalar epsilon = ResourceMap::GetAsScalar("PiecewiseLinearEvaluation-EpsilonRegular") * std::abs(step);
  isRegular_ = true;
  for (UnsignedInteger i = 0; i < size; ++i) isRegular_ = isRegular_ && (std::abs(locations_[i] - locations_[0] - i * step) < epsilon);
}

/* Values accessor */
Sample PiecewiseLinearEvaluation::getValues() const
{
  return values_;
}

void PiecewiseLinearEvaluation::setValues(const Point & values)
{
  const UnsignedInteger size = values.getSize();
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  Sample sampleValues(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) sampleValues[i][0] = values[i];
  values_ = sampleValues;
}

void PiecewiseLinearEvaluation::setValues(const Sample & values)
{
  const UnsignedInteger size = values.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise linear interpolation function.";
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  values_ = values;
}

void PiecewiseLinearEvaluation::setLocationsAndValues(const Point & locations,
    const Sample & values)
{
  const UnsignedInteger size = locations.getSize();
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << values.getSize() << " must match the number of locations=" << size;
  // Sort the data in increasing order according to the locations
  const UnsignedInteger dimension = values.getDimension();
  Sample data(size, 1 + dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i][0] = locations[i];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      data[i][j + 1] = values[i][j];
  }
  data = data.sortAccordingToAComponent(0);
  locations_ = Point(size);
  values_ = Sample(size, dimension);
  const NumericalScalar step = data[1][0] - data[0][0];
  const NumericalScalar epsilon = ResourceMap::GetAsScalar("PiecewiseLinearEvaluation-EpsilonRegular") * std::abs(step);
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
UnsignedInteger PiecewiseLinearEvaluation::getInputDimension() const
{
  return 1;
}

/* Output dimension accessor */
UnsignedInteger PiecewiseLinearEvaluation::getOutputDimension() const
{
  return values_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void PiecewiseLinearEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "locations_", locations_ );
  adv.saveAttribute( "values_", values_ );
}


/* Method load() reloads the object from the StorageManager */
void PiecewiseLinearEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "locations_", locations_ );
  adv.loadAttribute( "values_", values_ );
}


END_NAMESPACE_OPENTURNS
