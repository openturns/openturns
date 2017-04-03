//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a hermite piecewise scalar function
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
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseHermiteEvaluation);

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
PiecewiseHermiteEvaluation::PiecewiseHermiteEvaluation(const NumericalPoint & locations,
    const NumericalPoint & values,
    const NumericalPoint & derivatives)
  : EvaluationImplementation()
  , locations_(0)
  , values_(0, 0)
  , derivatives_(0, 0)
{
  const UnsignedInteger sizeValues = values.getSize();
  NumericalSample sampleValues(sizeValues, 1);
  for (UnsignedInteger i = 0; i < sizeValues; ++i) sampleValues[i][0] = values[i];
  const UnsignedInteger sizeDerivatives = derivatives.getSize();
  NumericalSample sampleDerivatives(sizeDerivatives, 1);
  for (UnsignedInteger i = 0; i < sizeDerivatives; ++i) sampleDerivatives[i][0] = derivatives[i];
  // Check the input
  setLocationsValuesAndDerivatives(locations, sampleValues, sampleDerivatives);
}


/* Parameters constructor */
PiecewiseHermiteEvaluation::PiecewiseHermiteEvaluation(const NumericalPoint & locations,
    const NumericalSample & values,
    const NumericalSample & derivatives)
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

String PiecewiseHermiteEvaluation::__str__(const String & offset) const
{
  return OSS(false) << GetClassName() << "("
         << "locations=" << locations_
         << ", values=" << values_
         << ", derivatives=" << derivatives_ << ")";
}


/* Evaluation operator */
NumericalPoint PiecewiseHermiteEvaluation::operator () (const NumericalPoint & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const NumericalScalar x = inP[0];
  UnsignedInteger iLeft = 0;
  if (x <= locations_[iLeft]) return values_[iLeft];
  UnsignedInteger iRight = locations_.getSize() - 1;
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
      const UnsignedInteger im = (iRight + iLeft) / 2;
      if (x < locations_[im]) iRight = im;
      else iLeft = im;
    }

  const NumericalScalar h = locations_[iRight] - locations_[iLeft];
  const NumericalScalar theta = (x - locations_[iLeft]) / h;
  const NumericalPoint vLeft(values_[iLeft]);
  const NumericalPoint vRight(values_[iRight]);
  const NumericalPoint dvLeft(derivatives_[iLeft]);
  const NumericalPoint dvRight(derivatives_[iRight]);
  const UnsignedInteger dimension = getOutputDimension();
  NumericalPoint value(dimension);
  const NumericalScalar alpha = 1.0 - theta;
  const NumericalScalar beta = theta * alpha;
  const NumericalScalar gamma = 2.0 * theta - 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = alpha * vLeft[i] + theta * vRight[i] + beta * (gamma * (vRight[i] - vLeft[i]) + h * (alpha * dvLeft[i] - theta * dvRight[i]));
  return value;
}

/* Compute the derivative */
NumericalPoint PiecewiseHermiteEvaluation::derivate(const NumericalPoint & inP) const
{
  if (inP.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: expected an input point of dimension 1, got dimension=" << inP.getDimension();
  const NumericalScalar x = inP[0];
  UnsignedInteger iLeft = 0;
  if (x <= locations_[iLeft]) return values_[iLeft];
  UnsignedInteger iRight = locations_.getSize() - 1;
  if (x >= locations_[iRight]) return values_[iRight];
  if (isRegular_)
  {
    iLeft = static_cast<UnsignedInteger>(trunc((x - locations_[0]) / (locations_[1] - locations_[0])));
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

  const NumericalScalar h = locations_[iRight] - locations_[iLeft];
  const NumericalScalar theta = (x - locations_[iLeft]) / h;
  const NumericalPoint vLeft(values_[iLeft]);
  const NumericalPoint vRight(values_[iRight]);
  const NumericalPoint dvLeft(derivatives_[iLeft]);
  const NumericalPoint dvRight(derivatives_[iRight]);
  const UnsignedInteger dimension = getOutputDimension();
  NumericalPoint value(dimension);
  const NumericalScalar alpha = 1.0 - theta;
  const NumericalScalar beta = theta * alpha;
  const NumericalScalar gamma = 2.0 * theta - 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = (-vLeft[i] + vRight[i] + alpha * (gamma * (vRight[i] - vLeft[i]) + h * (alpha * dvLeft[i] - theta * dvRight[i])) + beta * (2.0 * (vRight[i] - vLeft[i]) + h * (- dvLeft[i] - dvRight[i]))) / h;
  return value;
}

/* Locations accessor */
NumericalPoint PiecewiseHermiteEvaluation::getLocations() const
{
  return locations_;
}

void PiecewiseHermiteEvaluation::setLocations(const NumericalPoint & locations)
{
  const UnsignedInteger size = locations.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (locations.getSize() != values_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of locations=" << size << " must match the number of previously set values=" << values_.getSize();
  const NumericalScalar step = locations_[0] - locations_[0];
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("PiecewiseHermiteEvaluation-EpsilonRegular") * std::abs(step);
  isRegular_ = true;
  for (UnsignedInteger i = 0; i < size; ++i) isRegular_ = isRegular_ && (std::abs(locations[i] - locations[0] - i * step) < epsilon);
  locations_ = locations;
  std::stable_sort(locations_.begin(), locations_.end());
}

/* Values accessor */
NumericalSample PiecewiseHermiteEvaluation::getValues() const
{
  return values_;
}

void PiecewiseHermiteEvaluation::setValues(const NumericalSample & values)
{
  const UnsignedInteger size = values.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << size << " must match the number of previously set locations=" << locations_.getSize();
  values_ = values;
}

/* Derivatives accessor */
NumericalSample PiecewiseHermiteEvaluation::getDerivatives() const
{
  return derivatives_;
}

void PiecewiseHermiteEvaluation::setDerivatives(const NumericalSample & derivatives)
{
  const UnsignedInteger size = derivatives.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (size != locations_.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of derivatives=" << size << " must match the number of previously set locations=" << locations_.getSize();
  derivatives_ = derivatives;
}


void PiecewiseHermiteEvaluation::setLocationsValuesAndDerivatives(const NumericalPoint & locations,
    const NumericalSample & values,
    const NumericalSample & derivatives)
{
  const UnsignedInteger size = locations.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: there must be at least 2 points to build a piecewise Hermite interpolation function.";
  if (size != values.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of values=" << values.getSize() << " must match the number of locations=" << size;
  if (size != derivatives.getSize()) throw InvalidArgumentException(HERE) << "Error: the number of derivatives=" << derivatives.getSize() << " must match the number of locations=" << size;
  const UnsignedInteger outputDimension = values.getDimension();
  if (outputDimension != derivatives.getDimension()) throw InvalidArgumentException(HERE) << "Error: the dimension of the derivatives=" << derivatives.getDimension() << " must match the dimension of the locations=" << outputDimension;
  // Sort the data in increasing order according to the locations
  NumericalSample data(size, 1 + 2 * outputDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i][0] = locations[i];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      data[i][1 + j] = values[i][j];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      data[i][1 + outputDimension + j] = derivatives[i][j];
  }
  data = data.sortAccordingToAComponent(0);
  locations_ = NumericalPoint(size);
  values_ = NumericalSample(size, outputDimension);
  derivatives_ = NumericalSample(size, outputDimension);
  const NumericalScalar step = data[1][0] - data[0][0];
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("PiecewiseHermiteEvaluation-EpsilonRegular") * std::abs(step);
  isRegular_ = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    locations_[i] = data[i][0];
    isRegular_ = isRegular_ && (std::abs(locations_[i] - locations_[0] - i * step) < epsilon);
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      values_[i][j] = data[i][1 + j];
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      derivatives_[i][j] = data[i][1 + outputDimension + j];
  }
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
  adv.saveAttribute( "isRegular_", isRegular_ );
}


/* Method load() reloads the object from the StorageManager */
void PiecewiseHermiteEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "locations_", locations_ );
  adv.loadAttribute( "values_", values_ );
  adv.loadAttribute( "derivatives_", derivatives_ );
  adv.loadAttribute( "isRegular_", isRegular_ );
}


END_NAMESPACE_OPENTURNS
