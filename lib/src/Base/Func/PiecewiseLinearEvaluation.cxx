//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <algorithm>
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseLinearEvaluation)

static const Factory<PiecewiseLinearEvaluation> Factory_PiecewiseLinearEvaluation;


/* Default constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation(const Point & locations,
    const Point & values)
  : EvaluationImplementation(),
    enableExtrapolation_(ResourceMap::GetAsBool("PiecewiseLinearEvaluation-DefaultEnableExtrapolation"))
{
  // Convert the values into a sample
  const UnsignedInteger size = values.getSize();
  SampleImplementation sampleValues(size, 1);
  sampleValues.setData(values);
  // Check the input
  setLocationsAndValues(locations, sampleValues);
}

/* Parameters constructor */
PiecewiseLinearEvaluation::PiecewiseLinearEvaluation(const Point & locations,
    const Sample & values)
  : EvaluationImplementation(),
    enableExtrapolation_(ResourceMap::GetAsBool("PiecewiseLinearEvaluation-DefaultEnableExtrapolation"))
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

String PiecewiseLinearEvaluation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the locations grid is regular to the given tolerance */
Bool PiecewiseLinearEvaluation::IsRegular(const Point & locations,
    const Scalar & epsilon)
{
  const UnsignedInteger size = locations.getSize();
  if (size >= 2)
  {
    const Scalar step = locations[1] - locations[0];
    const Scalar relativeEpsilon = epsilon * std::abs(step);
    for (UnsignedInteger i = 2; i < size; ++i)
    {
      if (!(std::abs(locations[i] - locations[0] - i * step) < relativeEpsilon))
        return false;
    }
  }
  return true;
}

/* Find the segment containing value by bisection */
UnsignedInteger PiecewiseLinearEvaluation::FindSegmentIndex(const Point & locations,
    const Scalar value,
    const UnsignedInteger start,
    const Bool isRegular)
{
  if (isRegular)
    return static_cast<UnsignedInteger>(floor((value - locations[0]) / (locations[1] - locations[0])));
  UnsignedInteger iRight = locations.getSize() - 1;
  UnsignedInteger iLeft  = start;
  if (value >= locations[start])
  {
    // Shortcuts for the most common cases when looping over a Sample
    if (start == iRight || value < locations[start + 1])
      return start;
    else if (start + 1 == iRight || value < locations[start + 2])
      return start + 1 ;
  }
  else
    iLeft = 0;

  while (iRight > iLeft + 1)
  {
    const UnsignedInteger im = (iRight + iLeft) / 2;
    if (value < locations[im]) iRight = im;
    else iLeft = im;
  }
  return iLeft;
}

/* Evaluation operator */
Point PiecewiseLinearEvaluation::operator () (const Point & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  if (values_.getSize() == 1) return values_[0];
  const Scalar x = inP[0];
  UnsignedInteger iLeft = 0;

  if (x <= locations_[iLeft])
  {
    if (enableExtrapolation_)
    {
      return values_[iLeft];
    }
    else
    {
      throw InvalidArgumentException(HERE) << "Error : input point is less than the lower bound of the locations=" << locations_[iLeft];
    }
  }

  UnsignedInteger iRight = locations_.getSize() - 1;
  if (x >= locations_[iRight])
  {
    if (enableExtrapolation_)
    {
      return values_[iRight];
    }
    else
    {
      throw InvalidArgumentException(HERE) << "Error : input point is greater than the upper bound of the locations=" << values_[iRight];
    }
  }
  iLeft = FindSegmentIndex(locations_, x, 0, isRegular_);

  const Scalar xLeft = locations_[iLeft];
  const Scalar xRight = locations_[iLeft + 1];
  const Scalar dx = xLeft - xRight;
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  const Scalar alpha = (x - xRight) / dx;
  const Scalar beta = (xLeft - x) / dx;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = alpha * values_(iLeft, i) + beta * values_(iLeft + 1, i);
  return value;
}

Sample PiecewiseLinearEvaluation::operator () (const Sample & inSample) const
{
  if (inSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input sample of dimension 1, got dimension=" << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  if (values_.getSize() == 1) return Sample(size, values_[0]);
  const UnsignedInteger dimension = getOutputDimension();
  Sample output(size, dimension);
  const UnsignedInteger maxIndex = locations_.getSize() - 1;
  UnsignedInteger iLeft = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = inSample(i, 0);

    if (x <= locations_[0])
    {
      if (enableExtrapolation_)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = values_(0, j);
        continue;
      }
      else
      {
        throw InvalidArgumentException(HERE) << "Error : input point is less than the lower bound of the locations=" << locations_[0];
      }
    }

    UnsignedInteger iRight = maxIndex;
    if (x >= locations_[iRight])
    {
      if (enableExtrapolation_)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = values_(iRight, j);
        continue;
      }
      else
      {
        throw InvalidArgumentException(HERE) << "Error : input point is greater than the upper bound of the locations=" << locations_[iRight];
      }
    }
    iLeft = FindSegmentIndex(locations_, x, iLeft, isRegular_);

    const Scalar xLeft = locations_[iLeft];
    const Scalar xRight = locations_[iLeft + 1];
    const Scalar dx = xLeft - xRight;
    const Scalar alpha = (x - xRight) / dx;
    const Scalar beta = (xLeft - x) / dx;
    for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = alpha * values_(iLeft, j) + beta * values_(iLeft + 1, j);
  }
  return output;
}

/* Locations accessor */
Point PiecewiseLinearEvaluation::getLocations() const
{
  return locations_;
}

void PiecewiseLinearEvaluation::setLocations(const Point & locations)
{
  const UnsignedInteger size = locations.getSize();
  if (locations.getSize() != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of locations=" << size << " must match the number of previously set values=" << values_.getSize();
  if (locations.isNonDecreasing())
  {
    locations_ = locations;
  }
  else
  {
    // Sort the data in increasing order according to the locations
    Collection< std::pair<Scalar, UnsignedInteger> > locationAndIndex(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      locationAndIndex[i] = std::pair<Scalar, UnsignedInteger>(locations[i], i);
    std::stable_sort(locationAndIndex.begin(), locationAndIndex.end());
    for (UnsignedInteger i = 0; i < size; ++i)
      locations_[i] = locationAndIndex[i].first;
    const UnsignedInteger dimension = values_.getDimension();
    const Sample oldValues(values_);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        values_(i, j) = oldValues(locationAndIndex[i].second, j);
    }
  }
  isRegular_ = IsRegular(locations_, ResourceMap::GetAsScalar("PiecewiseLinearEvaluation-EpsilonRegular"));
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
  SampleImplementation sampleValues(size, 1);
  sampleValues.setData(values);
  values_ = sampleValues;
}

void PiecewiseLinearEvaluation::setValues(const Sample & values)
{
  const UnsignedInteger size = values.getSize();
  if (!(size >= 1)) throw InvalidArgumentException(HERE) << "Error: there must be at least 1 point to build a piecewise linear interpolation function, but size=" << size;
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  values_ = values;
}

void PiecewiseLinearEvaluation::setLocationsAndValues(const Point & locations,
    const Sample & values)
{
  const UnsignedInteger size = locations.getSize();
  if (!(size >= 1)) throw InvalidArgumentException(HERE) << "Error: there must be at least 1 point to build a piecewise linear interpolation function, but size=" << size;
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << values.getSize() << " must match the number of locations=" << size;
  // Sort the data in increasing order according to the locations
  Collection< std::pair<Scalar, UnsignedInteger> > locationAndIndex(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    locationAndIndex[i] = std::pair<Scalar, UnsignedInteger>(locations[i], i);
  std::stable_sort(locationAndIndex.begin(), locationAndIndex.end());
  const UnsignedInteger dimension = values.getDimension();
  locations_ = Point(size);
  values_ = Sample(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    locations_[i] = locationAndIndex[i].first;
    for (UnsignedInteger j = 0; j < dimension; ++j)
      values_(i, j) = values(locationAndIndex[i].second, j);
  }
  isRegular_ = IsRegular(locations_, ResourceMap::GetAsScalar("PiecewiseLinearEvaluation-EpsilonRegular"));
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

/* enableExtrapolation accessor */
Bool PiecewiseLinearEvaluation::getEnableExtrapolation() const
{
  return enableExtrapolation_;
}

void PiecewiseLinearEvaluation::setEnableExtrapolation(const Bool & enableExtrapolation)
{
  enableExtrapolation_ = enableExtrapolation;
}

/* Method save() stores the object through the StorageManager */
void PiecewiseLinearEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "locations_", locations_ );
  adv.saveAttribute( "values_", values_ );
  adv.saveAttribute( "enableExtrapolation_", enableExtrapolation_ );
}


/* Method load() reloads the object from the StorageManager */
void PiecewiseLinearEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "locations_", locations_ );
  adv.loadAttribute( "values_", values_ );
  adv.loadAttribute( "enableExtrapolation_", enableExtrapolation_ );
  if (adv.hasAttribute("enableExtrapolation_"))
    adv.loadAttribute( "enableExtrapolation_", enableExtrapolation_ );
  isRegular_ = IsRegular(locations_, ResourceMap::GetAsScalar("PiecewiseLinearEvaluation-EpsilonRegular"));
}


END_NAMESPACE_OPENTURNS
