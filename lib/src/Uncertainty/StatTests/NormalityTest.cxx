//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include <cmath>
#include <fstream>
#include "openturns/NormalityTest.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* Anderson Darling normality test for normal distributions. */
TestResult NormalityTest::AndersonDarlingNormal(const Sample & sample,
    const Scalar level)
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot perform an Anderson Darling normality test with sample of dimension > 1.";
  if (sample.getSize() < 8) throw InvalidArgumentException(HERE) << "Error: cannot perform an Anderson Darling normality test with sample of size < 8.";
  Sample sortedSample(sample.sort(0));
  Scalar mean = sortedSample.computeMean()[0];
  Scalar sd = sortedSample.computeStandardDeviation()[0];
  const UnsignedInteger size = sample.getSize();
  Scalar testStatistic = 0.0;
  UnsignedInteger effectiveIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar yi = DistFunc::pNormal((sortedSample(i, 0) - mean) / sd);
    Scalar yni = 1.0 - DistFunc::pNormal((sortedSample(size - i - 1, 0) - mean) / sd);
    if ((yi > 0.0) && (yni > 0.0))
    {
      testStatistic += (2.0 * effectiveIndex + 1) * (log(yi) + log(yni));
      ++effectiveIndex;
    }
  }
  testStatistic /= effectiveIndex;
  testStatistic = (-Scalar(effectiveIndex) - testStatistic);
  // Corrective factor for small sample size
  Scalar adjustedStatistic = testStatistic * (1.0 + 0.75 / effectiveIndex + 2.25 / (effectiveIndex * effectiveIndex));
  // Compute approximate p-value
  Scalar pValue = 1.0;
  if (adjustedStatistic >= -1.38)
  {
    pValue = 1 - exp(-13.436 + 101.14 * adjustedStatistic - 223.73 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.2)
  {
    pValue = 1 - exp(-8.318 + 42.796 * adjustedStatistic - 59.938 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.34)
  {
    pValue = exp(0.9177 - 4.279 * adjustedStatistic - 1.38 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.6)
  {
    pValue = exp(1.2937 - 5.709 * adjustedStatistic + 0.0186 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 153.0)
  {
    pValue = 0.0;
  }
  return TestResult("AndersonDarlingNormal", pValue > level, pValue, level, adjustedStatistic);
}

/* Cramer Von Mises normality test for normal distributions. */
TestResult NormalityTest::CramerVonMisesNormal(const Sample & sample,
    const Scalar level)
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot perform a Cramer Von-Mises normality test with sample of dimension > 1.";
  if (sample.getSize() < 8) throw InvalidArgumentException(HERE) << "Error: cannot perform a Cramer Von-Mises normality test with sample of size < 8.";
  Sample sortedSample(sample.sort(0));
  Scalar mean = sortedSample.computeMean()[0];
  Scalar sd = sortedSample.computeStandardDeviation()[0];
  const UnsignedInteger size = sample.getSize();
  Scalar testStatistic = 1.0 / (12.0 * size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar yi = DistFunc::pNormal((sortedSample(i, 0) - mean) / sd);
    Scalar delta = yi - (2.0 * i + 1.0) / (2.0 * size);
    testStatistic += delta * delta;
  }
  // Corrective factor for small sample size
  Scalar adjustedStatistic = testStatistic * (1.0 + 0.5 / size);
  // Compute approximate p-value
  Scalar pValue = 1.0;
  if (adjustedStatistic >= -0.2)
  {
    pValue = 1 - exp(-13.953 + 775.5 * adjustedStatistic - 12542.61 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.0275)
  {
    pValue = 1 - exp(-5.903 + 179.546 * adjustedStatistic - 1515.29 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.051)
  {
    pValue = exp(0.886 - 31.62 * adjustedStatistic + 10.897 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 0.092)
  {
    pValue = exp(1.111 - 34.242 * adjustedStatistic + 12.832 * adjustedStatistic * adjustedStatistic);
  }
  if (adjustedStatistic >= 2.636)
  {
    pValue = 0.0;
  }
  return TestResult("CramerVonMisesNormal", pValue > level, pValue, level, adjustedStatistic);
}

END_NAMESPACE_OPENTURNS
