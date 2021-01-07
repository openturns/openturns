//                                               -*- C++ -*-
/**
 *  @brief Factory for Histogram distribution
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
#include "openturns/HistogramFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HistogramFactory)

static const Factory<HistogramFactory> Factory_HistogramFactory;

/* Default constructor */
HistogramFactory::HistogramFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
HistogramFactory * HistogramFactory::clone() const
{
  return new HistogramFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution HistogramFactory::build(const Sample & sample) const
{
  return buildAsHistogram(sample).clone();
}

Distribution HistogramFactory::build(const Sample & sample,
                                     const Scalar bandwidth) const
{
  return buildAsHistogram(sample, bandwidth).clone();
}

Distribution HistogramFactory::build(const Sample & sample,
                                     const UnsignedInteger binNumber) const
{
  return buildAsHistogram(sample, binNumber).clone();
}

Distribution HistogramFactory::build(const Sample & sample,
                                     const Scalar first,
                                     const Point & width) const
{
  return buildAsHistogram(sample, first, width).clone();
}

Distribution HistogramFactory::build() const
{
  return buildAsHistogram().clone();
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample) const
{
  return buildAsHistogram(sample, computeBandwidth(sample));
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample,
    const Scalar bandwidth) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram based on an empty sample.";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (!(bandwidth > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive bandwidth, got bandwidth=" << bandwidth;
  // Construct the histogram
  // It will extends from min to max.
  const Scalar min = sample.getMin()[0];
  const Scalar max = sample.getMax()[0];
  if (!SpecFunc::IsNormal(min) || !SpecFunc::IsNormal(max)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  if (max == min)
  {
    Histogram result(min - 0.5 * bandwidth, Point(1, bandwidth), Point(1, 1.0));
    result.setDescription(sample.getDescription());
    return result;
  }
  const UnsignedInteger binNumber = static_cast<UnsignedInteger>(ceil((max - min) / bandwidth + 0.5));
  return buildAsHistogram(sample, binNumber);
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample,
    const UnsignedInteger binNumber) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram based on an empty sample.";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive number of bin, got 0.";
  // Construct the histogram
  // It will extends from min to max.
  const Scalar min = sample.getMin()[0];
  const Scalar max = sample.getMax()[0];
  if (!SpecFunc::IsNormal(min) || !SpecFunc::IsNormal(max)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  if (max == min)
  {
    const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-DefaultCDFEpsilon");
    const Scalar delta = std::max(std::abs(min), 10.0) * epsilon;
    Histogram result(min - 0.5 * delta, Point(1, delta), Point(1, 1.0));
    result.setDescription(sample.getDescription());
    return result;
  }
  // Adjust the bin with in order to match the bin number. Add a small adjustment in order to have bins defined as [x_k, x_k+1[ intervals
  const Scalar delta = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon") * (max - min);
  const Scalar hOpt = ((max - min) + delta) / binNumber;
  Point heights(binNumber, 0.0);
  const Scalar step = 1.0 / hOpt;
  // Aggregate the realizations into the bins
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    // The index takes values in [[0, binNumber-1]] because min <= sample(i, 0) <= max and step < binNumber / (max - min)
    const UnsignedInteger index = static_cast<UnsignedInteger>(floor((sample(i, 0) - min) * step));
    heights[index] += 1.0;
  }
  const Scalar inverseArea = 1.0 / (hOpt * size);
  Histogram result(min, Point(binNumber, hOpt), heights * inverseArea);
  result.setDescription(sample.getDescription());
  return result;
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample,
    const Scalar first,
    const Point & width) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram based on an empty sample.";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  const UnsignedInteger binNumber = width.getSize();
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive number of bin, got 0.";
  // Construct the histogram
  Point heights(binNumber);
  // It will extends from min to max.
  const Scalar minvalue = sample.getMin()[0];
  const Scalar maxvalue = sample.getMax()[0];
  if (!SpecFunc::IsNormal(minvalue) || !SpecFunc::IsNormal(maxvalue)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  if (maxvalue == minvalue)
  {
    const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-DefaultCDFEpsilon");
    const Scalar delta = std::max(std::abs(minvalue), 10.0) * epsilon;
    Histogram result(minvalue - 0.5 * delta, Point(1, delta), Point(1, 1.0));
    result.setDescription(sample.getDescription());
    return result;
  }
  if (minvalue < first) throw InvalidArgumentException(HERE) << "Error: the minimum of the sample is smaller than first";
  // Compute the right bound of the histogram = first + the sum of the widths
  Scalar rightBound = first;
  for(UnsignedInteger j = 0; j < binNumber; ++j)
  {
    if (width[j] <= 0.) throw InvalidArgumentException(HERE) << "Error: at least one width is nonpositive";
    rightBound += width[j];
  }
  if (maxvalue >= rightBound) throw InvalidArgumentException(HERE) << "Error: the maximum of the sample is greater than the right boundary of the histogram";
  // Count the number of points in each class with a naive loop
  Scalar left = first;
  Scalar right;
  for(UnsignedInteger j = 0; j < binNumber; ++j) // Loop over the bins
  {
    right = left + width[j];
    for(UnsignedInteger i = 0; i < size; ++i)    // Loop over the values
    {
      if ((sample(i, 0) >= left) && (sample(i, 0) < right))
      {
        heights[j] += 1.0;
      }
    }
    left = right;
  }
  // Compute the area
  Scalar area = 0.;
  for(UnsignedInteger j = 0; j < binNumber; ++j) // Loop over the bins
  {
    area += width[j] * heights[j];
  }
  const Scalar inverseArea = 1.0 / area;
  Histogram result(first, width, heights * inverseArea);
  result.setDescription(sample.getDescription());
  return result;
}

Histogram HistogramFactory::buildAsHistogram() const
{
  return Histogram();
}

/* Compute the bandwidth according to Scott or Freedman and Diaconis rule */
Scalar HistogramFactory::computeBandwidth(const Sample & sample,
    const Bool useQuantile) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot compute the bandwidth based on an empty sample.";
  Scalar hOpt = 0;
  if (useQuantile)
  {
    // We use the robust estimation of dispersion based on inter-quartile
    hOpt = (sample.computeQuantilePerComponent(0.75)[0] - sample.computeQuantilePerComponent(0.25)[0]) * std::pow(24.0 * std::sqrt(M_PI) / size, 1.0 / 3.0) / (2.0 * DistFunc::qNormal(0.75));
    // If the resulting bandwidth is zero it is because a majority of values are repeated in the sample
    if (hOpt == 0.0) LOGWARN(OSS() << "The first and third quartiles are equal, which means that many values are repeated in the given sample. Switch to the standard deviation-based bandwidth.");
  }
  // Here hOpt == 0.0 either because we asked for the standard deviation based bandwidth or because the quantile based bandwidth is zero
  if (hOpt == 0.0)
  {
    // We use the standard deviation
    hOpt = sample.computeStandardDeviation()[0] * std::pow(24.0 * std::sqrt(M_PI) / size, 1.0 / 3.0);
    // If we get zero it is due to a constant sample
    if (hOpt == 0.0)
    {
      LOGWARN(OSS() << "All the values are equal in the given sample. We switch to a bandwidth equal to QuantileEpsilon.");
      hOpt =  ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
    }
  }
  return hOpt;
}

END_NAMESPACE_OPENTURNS
