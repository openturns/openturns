//                                               -*- C++ -*-
/**
 *  @brief Factory for Histogram distribution
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
#include "openturns/HistogramFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HistogramFactory);

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

HistogramFactory::Implementation HistogramFactory::build(const NumericalSample & sample) const
{
  return buildAsHistogram(sample).clone();
}

HistogramFactory::Implementation HistogramFactory::build() const
{
  return buildAsHistogram().clone();
}

Histogram HistogramFactory::buildAsHistogram(const NumericalSample & sample) const
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  // Construct the histogram
  // It will extends from min to max.
  const NumericalScalar min = sample.getMin()[0];
  const NumericalScalar max = sample.getMax()[0];
  if (!SpecFunc::IsNormal(min) || !SpecFunc::IsNormal(max)) throw InvalidArgumentException(HERE) << "Error: cannot build an Histogram distribution if data contains NaN or Inf";
  if (max == min)
  {
    const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("Distribution-DefaultCDFEpsilon");
    const NumericalScalar delta = std::max(std::abs(min), 10.0) * epsilon;
    Histogram result(min - 0.5 * delta, NumericalPoint(1, delta), NumericalPoint(1, 1.0));
    result.setDescription(sample.getDescription());
    return result;
  }
  const UnsignedInteger size = sample.getSize();
  // First, try to use the robust estimation of dispersion based on inter-quartile
  NumericalScalar hOpt = (sample.computeQuantilePerComponent(0.75)[0] - sample.computeQuantilePerComponent(0.25)[0]) * std::pow(24.0 * std::sqrt(M_PI) / size, 1.0 / 3.0) / (2.0 * DistFunc::qNormal(0.75));
  // If we get a zero, try using standard deviation
  if (hOpt == 0.0) hOpt = sample.computeStandardDeviationPerComponent()[0] * std::pow(24.0 * std::sqrt(M_PI) / size, 1.0 / 3.0);
  const UnsignedInteger barNumber = static_cast<UnsignedInteger>(ceil((max - min) / hOpt + 0.5));
  // Adjust the bin with in order to match the bin number. Add a small adjustment in order to have bins defined as [x_k, x_k+1[ intervals
  const NumericalScalar delta = ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon") * (max - min);
  hOpt = ((max - min) + delta) / barNumber;
  NumericalPoint heights(barNumber, 0.0);
  const NumericalScalar step = 1.0 / hOpt;
  // Aggregate the realizations into the bins
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    // The index takes values in [[0, barNumber-1]] because min <= sample[i][0] <= max and step < barNumber / (max - min)
    const UnsignedInteger index = static_cast<UnsignedInteger>(floor((sample[i][0] - min) * step));
    heights[index] += 1.0;
  }
  const NumericalScalar inverseArea = 1.0 / (hOpt * size);
  Histogram result(min, NumericalPoint(barNumber, hOpt), heights * inverseArea);
  result.setDescription(sample.getDescription());
  return result;
}

Histogram HistogramFactory::buildAsHistogram() const
{
  return Histogram();
}

END_NAMESPACE_OPENTURNS
