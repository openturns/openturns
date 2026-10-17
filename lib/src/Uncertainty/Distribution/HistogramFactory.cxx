//                                               -*- C++ -*-
/**
 *  @brief Factory for Histogram distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include <algorithm>
#include <cmath>
#include <limits>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HistogramFactory)

static const Factory<HistogramFactory> Factory_HistogramFactory;

/* Check weights and return their sum */
static Scalar CheckWeights(const Sample & sample,
                           const Point & weights)
{
  const UnsignedInteger size = sample.getSize();
  if (weights.getDimension() != size)
    throw InvalidArgumentException(HERE) << "Error: the number of weights=" << weights.getDimension() << " does not match the sample size=" << size;
  Scalar totalWeight = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // An infinite weight would propagate NaN through the normalized heights
    if (!(weights[i] >= 0.0) || !std::isfinite(weights[i]))
      throw InvalidArgumentException(HERE) << "Error: the weights must be finite and non negative, here weight[" << i << "]=" << weights[i];
    totalWeight += weights[i];
  }
  if (!std::isfinite(totalWeight) || !(totalWeight > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the sum of the weights must be positive, here sum=" << totalWeight;
  return totalWeight;
}

/* Weighted quantile of a 1D sample by linear interpolation of the cumulative weights */
static Scalar ComputeWeightedQuantile(const Sample & sample,
                                      const Point & weights,
                                      const Scalar prob)
{
  const UnsignedInteger size = sample.getSize();
  // Sort the values with their weights in ascending order
  Collection< std::pair<Scalar, UnsignedInteger> > sortedPairs(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    sortedPairs[i] = std::pair<Scalar, UnsignedInteger>(sample(i, 0), i);
  std::sort(sortedPairs.begin(), sortedPairs.end());
  Scalar totalWeight = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i) totalWeight += weights[i];
  const Scalar threshold = prob * totalWeight;
  Scalar cumulativeWeight = 0.0;
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const UnsignedInteger i = sortedPairs[k].second;
    cumulativeWeight += weights[i];
    if (cumulativeWeight >= threshold)
    {
      if (k == 0) return sample(i, 0);
      // Interpolate between the two bracketing values
      const Scalar previousWeight = cumulativeWeight - weights[i];
      const Scalar alpha = (threshold - previousWeight) / weights[i];
      return (1.0 - alpha) * sample(sortedPairs[k - 1].second, 0) + alpha * sample(i, 0);
    }
  }
  return sample(sortedPairs[size - 1].second, 0);
}

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
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (!(bandwidth > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive bandwidth, got bandwidth=" << bandwidth;
  // Construct the histogram
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  // It will extends from min to max.
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (!(xMax > xMin))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a Histogram distribution from a constant sample, here max value is " << xMax << " and min value is " << xMin;
  const UnsignedInteger binNumber = static_cast<UnsignedInteger>(ceil((xMax - xMin) / bandwidth + 0.5));
  return buildAsHistogram(sample, binNumber);
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample,
    const UnsignedInteger binNumber) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive number of bin, got 0.";
  // Construct the histogram
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  // It will extends from min to max.
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (!(xMax > xMin))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a Histogram distribution from a constant sample, here max value is " << xMax << " and min value is " << xMin;
  // Adjust the bin with in order to match the bin number. Add a small adjustment in order to have bins defined as [x_k, x_k+1[ intervals
  const Scalar delta = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon") * (xMax - xMin);
  const Scalar hOpt = ((xMax - xMin) + delta) / binNumber;
  Point heights(binNumber, 0.0);
  const Scalar step = 1.0 / hOpt;
  // Aggregate the realizations into the bins
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    // The index takes values in [[0, binNumber-1]] because min <= sample(i, 0) <= max and step < binNumber / (max - min)
    const UnsignedInteger index = static_cast<UnsignedInteger>(floor((sample(i, 0) - xMin) * step));
    heights[index] += 1.0;
  }
  const Scalar inverseArea = 1.0 / (hOpt * size);
  Histogram result(xMin, Point(binNumber, hOpt), heights * inverseArea);
  result.setDescription(sample.getDescription());
  return result;
}

Histogram HistogramFactory::buildAsHistogram(const Sample & sample,
    const Scalar first,
    const Point & width) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  const UnsignedInteger binNumber = width.getSize();
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive number of bin, got 0.";
  // Construct the histogram
  Point heights(binNumber);
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  // It will extends from min to max.
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot estimate an Histogram distribution from a constant sample.";
  if (xMin < first) throw InvalidArgumentException(HERE) << "Error: the minimum of the sample is smaller than first";
  // Compute the right bound of the histogram = first + the sum of the widths
  Scalar rightBound = first;
  for(UnsignedInteger j = 0; j < binNumber; ++j)
  {
    if (width[j] <= 0.) throw InvalidArgumentException(HERE) << "Error: at least one width is nonpositive";
    rightBound += width[j];
  }
  if (xMax >= rightBound) throw InvalidArgumentException(HERE) << "Error: the maximum of the sample is greater than the right boundary of the histogram";
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

Distribution HistogramFactory::buildWeighted(const Sample & sample,
    const Point & weights) const
{
  return buildWeightedAsHistogram(sample, weights).clone();
}

Histogram HistogramFactory::buildWeightedAsHistogram(const Sample & sample,
    const Point & weights) const
{
  return buildWeightedAsHistogram(sample, computeBandwidthWeighted(sample, weights), weights);
}

Histogram HistogramFactory::buildWeightedAsHistogram(const Sample & sample,
    const Scalar bandwidth,
    const Point & weights) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (!(bandwidth > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive bandwidth, got bandwidth=" << bandwidth;
  const Scalar totalWeight = CheckWeights(sample, weights);
  // Construct the histogram
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  // It will extends from min to max.
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (!(xMax > xMin))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate an Histogram distribution from a constant sample, here max value is " << xMax << " and min value is " << xMin;
  const UnsignedInteger binNumber = static_cast<UnsignedInteger>(ceil((xMax - xMin) / bandwidth + 0.5));
  // Adjust the bin with in order to match the bin number. Add a small adjustment in order to have bins defined as [x_k, x_k+1[ intervals
  const Scalar delta = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon") * (xMax - xMin);
  const Scalar hOpt = ((xMax - xMin) + delta) / binNumber;
  Point heights(binNumber, 0.0);
  const Scalar step = 1.0 / hOpt;
  // Aggregate the weighted realizations into the bins
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    // The index takes values in [[0, binNumber-1]] because min <= sample(i, 0) <= max and step < binNumber / (max - min)
    const UnsignedInteger index = static_cast<UnsignedInteger>(floor((sample(i, 0) - xMin) * step));
    heights[index] += weights[i];
  }
  const Scalar inverseArea = 1.0 / (hOpt * totalWeight);
  Histogram result(xMin, Point(binNumber, hOpt), heights * inverseArea);
  result.setDescription(sample.getDescription());
  return result;
}

/* Compute the bandwidth according to Scott or Freedman and Diaconis rule */
Scalar HistogramFactory::computeBandwidth(const Sample & sample,
    const Bool useQuantile) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Cannot build a Histogram distribution from a sample of size < 2";

  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (!(xMax > xMin))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a Histogram distribution from a constant sample, here max value is " << xMax << " and min value is " << xMin;

  Scalar hOpt = 0.0;
  UnsignedInteger binNumber = 0;

  if (useQuantile)
  {
    // We use the robust estimation of dispersion based on inter-quartile
    hOpt = (sample.computeQuantilePerComponent(0.75)[0] - sample.computeQuantilePerComponent(0.25)[0]) * std::cbrt(24.0 * std::sqrt(M_PI) / size) / (2.0 * DistFunc::qNormal(0.75));

    // If the resulting bandwidth is zero it is because a majority of values are repeated in the sample
    if (hOpt == 0.0)
    {
      LOGWARN(OSS() << "The first and third quartiles are equal, which means that many values are repeated in the given sample. Switch to the standard deviation-based bandwidth.");
    }
    else
      binNumber = static_cast<UnsignedInteger>(ceil((xMax - xMin) / hOpt + 0.5));
  }

  // Here hOpt == 0.0 either because we asked for the standard deviation based bandwidth or because the quantile based bandwidth is zero
  if (hOpt == 0.0)
  {
    // We use the standard deviation
    const Scalar sigma = sample.computeStandardDeviation()[0];
    hOpt = sigma * std::cbrt(24.0 * std::sqrt(M_PI) / size);
    binNumber = static_cast<UnsignedInteger>(ceil((xMax - xMin) / hOpt + 0.5));
  }

  // cap the bin number
  const UnsignedInteger maximumBinNumber = ResourceMap::GetAsUnsignedInteger("HistogramFactory-MaximumBinNumber");
  if (binNumber > maximumBinNumber)
    hOpt = (xMax - xMin) / maximumBinNumber;

  return hOpt;
}

/* Compute the bandwidth of a weighted sample, using the Kish effective
   sample size in place of the sample size (see issue #3164) */
Scalar HistogramFactory::computeBandwidthWeighted(const Sample & sample,
    const Point & weights,
    const Bool useQuantile) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Cannot build a Histogram distribution from a sample of size < 2";

  const Scalar totalWeight = CheckWeights(sample, weights);
  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  if (!(xMax > xMin))
    throw InvalidArgumentException(HERE) << "Error: cannot estimate an Histogram distribution from a constant sample, here max value is " << xMax << " and min value is " << xMin;

  // Uniform weights reproduce the unweighted bandwidth exactly
  Bool uniformWeights = true;
  for (UnsignedInteger i = 1; i < size; ++i)
    if (weights[i] != weights[0])
    {
      uniformWeights = false;
      break;
    }
  if (uniformWeights) return computeBandwidth(sample, useQuantile);

  // Kish effective sample size, accumulated on normalized weights
  // to avoid overflow/underflow of the squared weights
  Scalar sumSquaredNormalizedWeights = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar normalizedWeight = weights[i] / totalWeight;
    sumSquaredNormalizedWeights += normalizedWeight * normalizedWeight;
  }
  const Scalar effectiveSize = 1.0 / sumSquaredNormalizedWeights;

  Scalar hOpt = 0.0;
  UnsignedInteger binNumber = 0;

  if (useQuantile)
  {
    // We use the robust estimation of dispersion based on inter-quartile
    hOpt = (ComputeWeightedQuantile(sample, weights, 0.75) - ComputeWeightedQuantile(sample, weights, 0.25)) * std::cbrt(24.0 * std::sqrt(M_PI) / effectiveSize) / (2.0 * DistFunc::qNormal(0.75));

    // If the resulting bandwidth is zero it is because a majority of values are repeated in the sample
    if (hOpt == 0.0)
    {
      LOGWARN(OSS() << "The first and third quartiles are equal, which means that many values are repeated in the given sample. Switch to the standard deviation-based bandwidth.");
    }
    else
    {
      const Scalar computedBinNumber = std::ceil((xMax - xMin) / hOpt + 0.5);
      if (!std::isfinite(hOpt) || !std::isfinite(computedBinNumber) || !(computedBinNumber > 0.0) || !(computedBinNumber < static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max())))
        throw InvalidArgumentException(HERE) << "Error: cannot estimate a finite bandwidth from the weighted sample";
      binNumber = static_cast<UnsignedInteger>(computedBinNumber);
    }
  }

  // Here hOpt == 0.0 either because we asked for the standard deviation based bandwidth or because the quantile based bandwidth is zero
  if (hOpt == 0.0)
  {
    // We use the weighted standard deviation
    Scalar weightedMean = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i) weightedMean += weights[i] * sample(i, 0);
    weightedMean /= totalWeight;
    Scalar weightedVariance = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar delta = sample(i, 0) - weightedMean;
      weightedVariance += weights[i] * delta * delta;
    }
    weightedVariance /= totalWeight;
    const Scalar sigma = std::sqrt(weightedVariance);
    if (!(sigma > 0.0) || !std::isfinite(sigma))
      throw InvalidArgumentException(HERE) << "Error: cannot estimate a finite bandwidth from the weighted sample";
    hOpt = sigma * std::cbrt(24.0 * std::sqrt(M_PI) / effectiveSize);
    const Scalar computedBinNumber = std::ceil((xMax - xMin) / hOpt + 0.5);
    if (!std::isfinite(hOpt) || !std::isfinite(computedBinNumber) || !(computedBinNumber > 0.0) || !(computedBinNumber < static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max())))
      throw InvalidArgumentException(HERE) << "Error: cannot estimate a finite bandwidth from the weighted sample";
    binNumber = static_cast<UnsignedInteger>(computedBinNumber);
  }

  // cap the bin number
  const UnsignedInteger maximumBinNumber = ResourceMap::GetAsUnsignedInteger("HistogramFactory-MaximumBinNumber");
  if (binNumber > maximumBinNumber)
    hOpt = (xMax - xMin) / maximumBinNumber;

  return hOpt;
}

Histogram HistogramFactory::buildFromQuantiles(const Scalar lowerBound, const Point & probabilities, const Point & quantiles) const
{
  const UnsignedInteger size = quantiles.getSize();
  if (probabilities.getSize() != size)
    throw InvalidArgumentException(HERE) << "Fractiles size (" << probabilities.getSize() << ") should match quantile size (" << size << ").";
  Point width(size);
  Point height(size);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    width[i] = quantiles[i] - (i > 0 ? quantiles[i - 1] : lowerBound);
    if (!(width[i] > 0.0)) // Avoid a division by zero. Histogram will make the required extra checks.
      throw InvalidArgumentException(HERE) << "Invalid histogram quantile at index " << i;
    height[i] = (probabilities[i] - (i > 0 ? probabilities[i - 1] : 0.0)) / width[i];
  }
  return Histogram(lowerBound, width, height);
}


END_NAMESPACE_OPENTURNS
