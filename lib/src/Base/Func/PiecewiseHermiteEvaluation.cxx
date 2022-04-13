//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a hermite piecewise scalar function
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseHermiteEvaluation)

static const Factory<PiecewiseHermiteEvaluation> Factory_PiecewiseHermiteEvaluation;


/* Default constructor */
PiecewiseHermiteEvaluation::PiecewiseHermiteEvaluation()
  : EvaluationImplementation()
  , locations_(1)
  , values_(1, 1)
  , derivatives_(1, 1)
{
  // Nothing to do
}


/* Parameters constructor */
PiecewiseHermiteEvaluation::PiecewiseHermiteEvaluation(const Point & locations,
    const Point & values,
    const Point & derivatives)
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
  , derivatives_(0, 0)
{
  const UnsignedInteger sizeValues = values.getSize();
  Sample sampleValues(sizeValues, 1);
  for (UnsignedInteger i = 0; i < sizeValues; ++i) sampleValues(i, 0) = values[i];
  const UnsignedInteger sizeDerivatives = derivatives.getSize();
  Sample sampleDerivatives(sizeDerivatives, 1);
  for (UnsignedInteger i = 0; i < sizeDerivatives; ++i) sampleDerivatives(i, 0) = derivatives[i];
  // Check the input
  setLocationsValuesAndDerivatives(locations, sampleValues, sampleDerivatives);
}


/* Parameters constructor */
PiecewiseHermiteEvaluation::PiecewiseHermiteEvaluation(const Point & locations,
    const Sample & values,
    const Sample & derivatives)
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
  , derivatives_(0, 0)
{
  // Check the input
  setLocationsValuesAndDerivatives(locations, values, derivatives);
}


/* Virtual constructor */
PiecewiseHermiteEvaluation * PiecewiseHermiteEvaluation::clone() const
{
  return new PiecewiseHermiteEvaluation(*this);
}


/* String converter */
String PiecewiseHermiteEvaluation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " locations=" << locations_
         << " values=" << values_
         << " derivatives=" << derivatives_;
}

String PiecewiseHermiteEvaluation::__str__(const String & ) const
{
  return OSS(false) << GetClassName() << "("
         << "locations=" << locations_
         << ", values=" << values_
         << ", derivatives=" << derivatives_ << ")";
}

namespace
{
Bool computeRegularHermite(const Point & locations)
{
  const UnsignedInteger size = locations.getSize();
  if (size < 2) return true;
  const Scalar step = locations[1] - locations[0];
  const Scalar relativeEpsilon = ResourceMap::GetAsScalar("PiecewiseHermiteEvaluation-EpsilonRegular") * std::abs(step);
  for (UnsignedInteger i = 2; i < size; ++i)
  {
    if (!(std::abs(locations[i] - locations[0] - i * step) < relativeEpsilon))
      return false;
  }
  return true;
}

// Find the segment containing value by bisection
UnsignedInteger findSegmentIndexHermite(const Point & locations, const Scalar value, const UnsignedInteger start)
{
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
  return  iLeft;
}
} // anonymous namespace


/* Evaluation operator */
Point PiecewiseHermiteEvaluation::operator () (const Point & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const Scalar x = inP[0];
  UnsignedInteger iLeft = 0;
  if (x <= locations_[iLeft]) return values_[iLeft];
  const UnsignedInteger iRight = locations_.getSize() - 1;
  if (x >= locations_[iRight]) return values_[iRight];
  if (isRegular_)
    iLeft = static_cast<UnsignedInteger>(floor((x - locations_[0]) / (locations_[1] - locations_[0])));
  else
    iLeft = findSegmentIndexHermite(locations_, x, 0);

  const Scalar h = locations_[iLeft + 1] - locations_[iLeft];
  const Scalar theta = (x - locations_[iLeft]) / h;
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  const Scalar alpha = 1.0 - theta;
  const Scalar beta = theta * alpha;
  const Scalar gamma = 2.0 * theta - 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = alpha * values_(iLeft, i) + theta * values_(iLeft + 1, i) + beta * (gamma * (values_(iLeft + 1, i) - values_(iLeft, i)) + h * (alpha * derivatives_(iLeft, i) - theta * derivatives_(iLeft + 1, i)));
  return value;
}

Sample PiecewiseHermiteEvaluation::operator () (const Sample & inSample) const
{
  if (inSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input sample of dimension 1, got dimension=" << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger dimension = getOutputDimension();
  Sample output(size, dimension);
  const UnsignedInteger iRight = locations_.getSize() - 1;
  UnsignedInteger iLeft = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = inSample(i, 0);
    if (x <= locations_[0])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = values_(0, j);
      continue;
    }
    if (x >= locations_[iRight])
    {
      for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = values_(iRight, j);
      continue;
    }
    if (isRegular_)
      iLeft = static_cast<UnsignedInteger>(floor((x - locations_[0]) / (locations_[1] - locations_[0])));
    else
      iLeft = findSegmentIndexHermite(locations_, x, iLeft);

    const Scalar h = locations_[iLeft + 1] - locations_[iLeft];
    const Scalar theta = (x - locations_[iLeft]) / h;
    const Scalar alpha = 1.0 - theta;
    const Scalar beta = theta * alpha;
    const Scalar gamma = 2.0 * theta - 1.0;
    for (UnsignedInteger j = 0; j < dimension; ++j) output(i, j) = alpha * values_(iLeft, j) + theta * values_(iLeft + 1, j) + beta * (gamma * (values_(iLeft + 1, j) - values_(iLeft, j)) + h * (alpha * derivatives_(iLeft, j) - theta * derivatives_(iLeft + 1, j)));
  }
  return output;
}

/* Compute the derivative */
Point PiecewiseHermiteEvaluation::derivate(const Point & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const Scalar x = inP[0];
  UnsignedInteger iLeft = 0;
  if (x <= locations_[iLeft]) return values_[iLeft];
  UnsignedInteger iRight = locations_.getSize() - 1;
  if (x >= locations_[iRight]) return values_[iRight];
  if (isRegular_)
    iLeft = static_cast<UnsignedInteger>(trunc((x - locations_[0]) / (locations_[1] - locations_[0])));
  else
    iLeft = findSegmentIndexHermite(locations_, x, 0);

  const Scalar h = locations_[iLeft + 1] - locations_[iLeft];
  const Scalar theta = (x - locations_[iLeft]) / h;
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  const Scalar alpha = 1.0 - theta;
  const Scalar beta = theta * alpha;
  const Scalar gamma = 2.0 * theta - 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = (-values_(iLeft, i) + values_(iLeft + 1, i) + alpha * (gamma * (values_(iLeft + 1, i) - values_(iLeft, i)) + h * (alpha * derivatives_(iLeft, 1) - theta * derivatives_(iLeft + 1, i))) + beta * (2.0 * (values_(iLeft + 1, i) - values_(iLeft, i)) + h * (- derivatives_(iLeft, 1) - derivatives_(iLeft + 1, i)))) / h;
  return value;
}

/* Locations accessor */
Point PiecewiseHermiteEvaluation::getLocations() const
{
  return locations_;
}

void PiecewiseHermiteEvaluation::setLocations(const Point & locations)
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
    const UnsignedInteger outputDimension = values_.getDimension();
    const Sample oldValues(values_);
    const Sample oldDerivatives(derivatives_);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < outputDimension; ++j)
      {
        values_(i, j) = oldValues(locationAndIndex[i].second, j);
        derivatives_(i, j) = oldDerivatives(locationAndIndex[i].second, j);
      }
    }
  }
  isRegular_ = computeRegularHermite(locations_);
}

/* Values accessor */
Sample PiecewiseHermiteEvaluation::getValues() const
{
  return values_;
}

void PiecewiseHermiteEvaluation::setValues(const Sample & values)
{
  const UnsignedInteger size = values.getSize();
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  values_ = values;
}

/* Derivatives accessor */
Sample PiecewiseHermiteEvaluation::getDerivatives() const
{
  return derivatives_;
}

void PiecewiseHermiteEvaluation::setDerivatives(const Sample & derivatives)
{
  const UnsignedInteger size = derivatives.getSize();
  if (!(size >= 2)) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function, but size=" << size;
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of derivatives=" << size << " must match the number of previously set locations=" << locations_.getSize();
  derivatives_ = derivatives;
}


void PiecewiseHermiteEvaluation::setLocationsValuesAndDerivatives(const Point & locations,
    const Sample & values,
    const Sample & derivatives)
{
  const UnsignedInteger size = locations.getSize();
  if (!(size >= 2)) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function, but size=" << size;
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << values.getSize() << " must match the number of locations=" << size;
  if (size != derivatives.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of derivatives=" << derivatives.getSize() << " must match the number of locations=" << size;
  const UnsignedInteger outputDimension = values.getDimension();
  if (outputDimension != derivatives.getDimension()) throw InvalidArgumentException(HERE) << "Error: the dimension of the derivatives=" << derivatives.getDimension() << " must match the dimension of the locations=" << outputDimension;
  // Sort the data in increasing order according to the locations
  Collection< std::pair<Scalar, UnsignedInteger> > locationAndIndex(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    locationAndIndex[i] = std::pair<Scalar, UnsignedInteger>(locations[i], i);
  std::stable_sort(locationAndIndex.begin(), locationAndIndex.end());
  locations_ = Point(size);
  values_ = Sample(size, outputDimension);
  derivatives_ = Sample(size, outputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    locations_[i] = locationAndIndex[i].first;
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
    {
      values_(i, j) = values(locationAndIndex[i].second, j);
      derivatives_(i, j) = derivatives(locationAndIndex[i].second, j);
    }
  }
  isRegular_ = computeRegularHermite(locations_);
}

/* Input dimension accessor */
UnsignedInteger PiecewiseHermiteEvaluation::getInputDimension() const
{
  return 1;
}

/* Output dimension accessor */
UnsignedInteger PiecewiseHermiteEvaluation::getOutputDimension() const
{
  return values_.getDimension();
}


/* Method save() stores the object through the StorageManager */
void PiecewiseHermiteEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "locations_", locations_ );
  adv.saveAttribute( "values_", values_ );
  adv.saveAttribute( "derivatives_", derivatives_ );
}


/* Method load() reloads the object from the StorageManager */
void PiecewiseHermiteEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "locations_", locations_ );
  adv.loadAttribute( "values_", values_ );
  adv.loadAttribute( "derivatives_", derivatives_ );
  isRegular_ = computeRegularHermite(locations_);
}


END_NAMESPACE_OPENTURNS
