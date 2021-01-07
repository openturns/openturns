//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#include <cmath>
#include <fstream>
#include "openturns/HypothesisTest.hxx"
#include "openturns/Path.hxx"
#include "openturns/Log.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Log.hxx"
#include "openturns/LinearModelTest.hxx"

BEGIN_NAMESPACE_OPENTURNS

HypothesisTest::HypothesisTest()
{
}


/* Independence test between 2 scalar scalar samples for discrete distributions
 * The samples here are issued from discrete distributions. Thus there is a transformation
 * from sample to a contingency table in order to apply the Chi square independence test
 * We rely on UserDefined distribution to get unique value samples and their proporprtions
*/
TestResult HypothesisTest::ChiSquared(const Sample & firstSample,
                                      const Sample & secondSample,
                                      const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the ChiSquared test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the ChiSquared test can be performed only between two samples of same size.";

  // Ensure that more than 80 % of classes contain at least nMin points
  const UnsignedInteger nMin = ResourceMap::GetAsUnsignedInteger("FittingTest-ChiSquaredMinimumBinCount");
  if (size < 2 * nMin)
    throw InvalidArgumentException(HERE) << "Error: ChiSquared test cannot be used with a sample size smaller than " << 2 * nMin
                                         << ". Reduce the value of \"FittingTest-ChiSquaredMinimumBinCount below " << size / 2 << " if you really want to use this test.";

  // Derive unique values and frequencies for each 1D sample
  // first sample --> table
  const UserDefined discreteXDistribution(firstSample);
  const Sample tableX(discreteXDistribution.getSupport());
  const Point frequenciesX(discreteXDistribution.computePDF(tableX).asPoint());

  // Define new classes ensuring we get nMin elements / class
  UnsignedInteger binNumberX = 0;
  // If the bin number was too high to get only nMin points per bin on average, reduce the number of bins to get only nonempty bins
  Point ticksX(0);
  Point binX(0);
  const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  UnsignedInteger cumulatedPoints = 0;
  ticksX.add(tableX(0, 0) - epsilon * (1.0 + std::abs(tableX(0, 0))));

  for (UnsignedInteger i = 0; i < tableX.getSize(); ++i)
  {
    cumulatedPoints += (frequenciesX[i] + epsilon) * size;
    if (cumulatedPoints >= nMin)
    {
      binX.add(cumulatedPoints);
      cumulatedPoints = 0;
      if (i == tableX.getSize() - 1)
        ticksX.add(tableX(i, 0) + epsilon * (1.0 + std::abs(tableX(i, 0))));
      else
        ticksX.add(0.5 * (tableX(i, 0) + tableX(i + 1, 0)));
    }
  } // i
  binNumberX = ticksX.getSize() - 1;
  // Check if there is a pending bin
  if (cumulatedPoints > 0)
  {
    binX[binNumberX - 1] += cumulatedPoints;
    ticksX[binNumberX] = tableX(tableX.getSize() - 1, 0) + epsilon * (1.0 + std::abs(tableX(tableX.getSize() - 1, 0)));
  }
  if (binNumberX < 2)
    throw InvalidArgumentException(HERE) << "Error: the adjusted bin number=" << binNumberX << " must be at least equal to 2.";


  // second sample table
  const UserDefined discreteYDistribution(secondSample);
  const Sample tableY(discreteYDistribution.getSupport());
  const Point frequenciesY(discreteYDistribution.computePDF(tableY).asPoint());

  // Classes for second sample
  UnsignedInteger binNumberY = 0;
  // If the bin number was too high to get only nMin points per bin on average, reduce the number of bins to get only nonempty bins
  Point ticksY(0);
  Point binY(0);
  cumulatedPoints = 0;
  ticksY.add(tableY(0, 0) - epsilon * (1.0 + std::abs(tableY(0, 0))));
  for (UnsignedInteger i = 0; i < tableY.getSize(); ++i)
  {
    cumulatedPoints += (frequenciesY[i] + epsilon) * size;
    if (cumulatedPoints >= nMin)
    {
      binY.add(cumulatedPoints);
      cumulatedPoints = 0;
      if (i == tableY.getSize() - 1)
        ticksY.add(tableY(i, 0) + epsilon * (1.0 + std::abs(tableY(i, 0))));
      else
        ticksY.add(0.5 * (tableY(i, 0) + tableY(i + 1, 0)));
    }
  } // i
  binNumberY = ticksY.getSize() - 1;
  // Check if there is a pending bin
  if (cumulatedPoints > 0)
  {
    binY[binNumberY - 1] += cumulatedPoints;
    ticksY[binNumberY] = tableY(tableY.getSize() - 1, 0) + epsilon * (1.0 + std::abs(tableY(tableY.getSize() - 1, 0)));
  }
  if (binNumberY < 2)
    throw InvalidArgumentException(HERE) << "Error: the adjusted bin number=" << binNumberY << " must be at least equal to 2.";

  // Now we define the table with elements E_{i,j} where E[i,j] counts the occurence of the element [xi, yj]
  // Bivariate sample
  Sample data(firstSample);
  data.stack(secondSample);
  const UserDefined bivariateDiscreteDistribution(data);
  const Sample table(bivariateDiscreteDistribution.getSupport());
  const Point frequencies(bivariateDiscreteDistribution.getProbabilities());

  Point classes(binNumberX * binNumberY);
  Point pointsInClasses(binNumberX * binNumberY);

  for (UnsignedInteger k = 0; k < table.getSize(); ++k)
  {
    const Scalar xi = table(k, 0);
    const Scalar yj = table(k, 1);
    Bool condition = true;
    UnsignedInteger indexI = 0;
    while (condition)
    {
      indexI++;
      condition = (xi > ticksX[indexI]) && (indexI < binNumberX);
    }
    //
    condition = true;
    indexI--;
    UnsignedInteger indexJ = 0;
    while (condition)
    {
      indexJ++;
      condition = (yj > ticksY[indexJ]) && (indexJ < binNumberY);
    }
    indexJ--;

    // We get the corresponding class for the kth point
    pointsInClasses[indexI * binNumberY + indexJ] += (frequencies[k] + epsilon) * size;
  }

  // Theoretical value under H0 assumption
  Scalar theoretical = 0.0;
  // Let us compute the statistic
  Scalar squaredSum = 0.0;

  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < binNumberX; ++i)
  {
    for (UnsignedInteger j = 0; j < binNumberY; ++j)
    {
      theoretical = binX[i] * binY[j] / size;
      squaredSum += std::pow( pointsInClasses[index]  - theoretical, 2) / theoretical;
      index++;
    }
  }
  const UnsignedInteger df = (binNumberX - 1) * (binNumberY - 1);
  const Scalar pValue =  DistFunc::pGamma(0.5 * df, 0.5 * squaredSum, true);
  Log::Debug ( OSS() << "In ChiSquared independence test : df = " << df << ", statistic = " << squaredSum << ", pValue = " << pValue );
  return TestResult("ChiSquared", pValue > level, pValue, level, squaredSum);
}

/* Independence Pearson test between 2 scalar samples which form a gaussian vector: test the linear relation  */
TestResult HypothesisTest::Pearson(const Sample & firstSample,
                                   const Sample & secondSample,
                                   const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Pearson test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the Pearson test can be performed only between two samples of same size.";
  if (size < 3)  throw InvalidArgumentException(HERE) << "Error: the Pearson test can not be performed with small samples.";
  // Implement the test using basic rho statistic
  Sample fullSample(firstSample);
  fullSample.stack(secondSample);
  const Scalar rho = fullSample.computePearsonCorrelation()(0, 1);
  // statistic value
  Scalar statistic;
  if ((rho <= -1.0 + SpecFunc::Precision) || (rho >=  1.0 - SpecFunc::Precision))
    statistic = SpecFunc::MaxScalar;
  else
    statistic = rho * std::sqrt((size - 2.0) / (1.0 - rho * rho));
  // Here we check if rho is significantly different from 0
  const Scalar pValue = 2.0 * DistFunc::pPearsonCorrelation(size, rho, true);
  return TestResult("Pearson", pValue > level, pValue, level, statistic);
}

/* Two-sample Kolmogorov–Smirnov test */
TestResult HypothesisTest::TwoSamplesKolmogorov(const Sample & sample1,
    const Sample & sample2,
    const Scalar level)
{
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Error: level must be in ]0, 1[, here level=" << level;
  if ((sample1.getDimension() != 1) || (sample2.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: Kolmogorov test works only with 1D samples";
  if ((sample1.getSize() == 0) || (sample2.getSize() == 0)) throw InvalidArgumentException(HERE) << "Error: the sample is empty";
  const UnsignedInteger size1 = sample1.getSize();
  const UnsignedInteger size2 = sample2.getSize();
  Sample sampleAllSorted(sample1.sort());
  sampleAllSorted.add(sample2.sort());
  Scalar value = 0.0;
  for (UnsignedInteger i = 0; i < size1 + size2; ++ i)
  {
    const Scalar sampleAllSorted_i = sampleAllSorted(i, 0);
    Scalar cdf1 = 0.0;
    for (UnsignedInteger j = 0; j < size1; ++ j)
    {
      if (sampleAllSorted(j, 0) <= sampleAllSorted_i)
      {
        cdf1 = (j + 1.0) / size1;
      }
      else
        break;
    }
    Scalar cdf2 = 0.0;
    for (UnsignedInteger j = 0; j < size2; ++ j)
    {
      if (sampleAllSorted(size1 + j, 0) <= sampleAllSorted_i)
      {
        cdf2 = (j + 1.0) / size2;
      }
      else
        break;
    }
    value = std::max(value, std::abs(cdf1 - cdf2));
  }
  const Scalar pValue = DistFunc::pKolmogorov((size1 * size2) / (size1 + size2), value, true);
  TestResult result(OSS(false) << "TwoSamplesKolmogorov", pValue > level, pValue, level, value);
  result.setDescription(Description(1, String(OSS() << "sample" << sample1.getName() << " vs sample " << sample2.getName())));
  LOGDEBUG(OSS() << result);
  return result;
}

/* Spearman test between 2 samples of dimension 1: it tests for null rank correlation between the two samples.*/
TestResult HypothesisTest::Spearman(const Sample & firstSample,
                                    const Sample & secondSample,
                                    const Scalar level)
{
  if ((firstSample.getDimension() != 1) || (secondSample.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the Spearman test can be performed only between two 1D samples.";
  const UnsignedInteger size = firstSample.getSize();
  if (secondSample.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the Spearman test can be performed only between two samples of same size.";

  const Bool ties = (firstSample.sortUnique().getSize() < size) || (secondSample.sortUnique().getSize() < size);
  Sample fullSample(firstSample);
  fullSample.stack(secondSample);
  const Scalar rho = fullSample.computeSpearmanCorrelation()(0, 1);
  // statistic value
  Scalar statistic;
  if ((rho <= -1.0 + SpecFunc::Precision) || (rho >=  1.0 - SpecFunc::Precision))
    statistic = SpecFunc::MaxScalar;
  else
    statistic = rho * std::sqrt((size - 2.0) / (1.0 - rho * rho));
  // Here we check if rho is significantly different from 0
  const Scalar pValue = 2.0 * DistFunc::pSpearmanCorrelation(size, std::abs(rho), true, ties);
  return TestResult("Spearman", pValue > level, pValue, level, statistic);
}

/* Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numeriacl sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample, for i in the selection. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
HypothesisTest::TestResultCollection HypothesisTest::PartialPearson(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the partial Pearson test can be performed only with an 1-d ouput sample.";
  if (!selection.check(firstSample.getDimension())) throw InvalidArgumentException(HERE) << "Error: invalid selection, repeated indices or values out of bound";
  const UnsignedInteger size = selection.getSize();
  TestResultCollection results(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    results[i] = Pearson(firstSample.getMarginal(selection[i]), secondSample, level);
  return results;
}

/* Spearman test between 2 samples : firstSample if of dimension n and secondSample of dimension 1. PartialSpearman tests for null rank correlation between the selected marginals of the first sample wrt the second sample.*/
HypothesisTest::TestResultCollection HypothesisTest::PartialSpearman(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level)
{
  if (secondSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the partial Spearman test can be performed only with an 1-d ouput sample.";
  if (!selection.check(firstSample.getDimension())) throw InvalidArgumentException(HERE) << "Error: invalid selection, repeated indices or values out of bound";
  const UnsignedInteger size = selection.getSize();
  TestResultCollection results(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    results[i] = Spearman(firstSample.getMarginal(selection[i]), secondSample, level);
  return results;
}

/* Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullPearson performs the Independence Pearson test simultaneously on all firstSample[i] and secondSample. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector. */
HypothesisTest::TestResultCollection HypothesisTest::FullPearson(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialPearson(firstSample, secondSample, selection, level);
}

/* Spearman test between 2 samples : firstSample if of dimension n and secondSample of dimension 1. FullSpearman tests for null rank correlation between the marginals of the first sample wrt the second sample.*/
HypothesisTest::TestResultCollection HypothesisTest::FullSpearman(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Indices selection(dimension);
  selection.fill();
  return PartialSpearman(firstSample, secondSample, selection, level);
}


END_NAMESPACE_OPENTURNS
