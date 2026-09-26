//                                               -*- C++ -*-
/**
 *  @brief StatisticsEngine implements free functions for statistical operations
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
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "openturns/StatisticsEngine.hxx"
#include "openturns/Exception.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

static void CheckSampleLayout(const DataContainer & sample)
{
  if (sample.getLayout() != DataContainer::ROW_MAJOR)
    throw InvalidArgumentException(HERE) << "Sample must be in ROW_MAJOR layout";
}

/* ===== ComputeSum ===== */

DataContainer StatisticsEngine::ComputeSum(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  struct SumBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    std::vector<Scalar> partialSums;
    SumBody(const Scalar *d, UnsignedInteger dim)
      : data(d), dimension(dim), partialSums(dim, 0.0) {}
    SumBody(SumBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), partialSums(other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          partialSums[j] += row[j];
      }
    }
    void join(SumBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        partialSums[j] += other.partialSums[j];
    }
  };

  SumBody body(pData, dimension);
  TBBImplementation::ParallelReduce(0, size, body);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialSums[j];
  return result;
}

/* ===== ComputeMean ===== */

DataContainer StatisticsEngine::ComputeMean(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  struct MeanBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    std::vector<Scalar> partialSums;
    MeanBody(const Scalar *d, UnsignedInteger dim)
      : data(d), dimension(dim), partialSums(dim, 0.0) {}
    MeanBody(MeanBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), partialSums(other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          partialSums[j] += row[j];
      }
    }
    void join(MeanBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        partialSums[j] += other.partialSums[j];
    }
  };

  MeanBody body(pData, dimension);
  TBBImplementation::ParallelReduce(0, size, body);

  const Scalar invN = 1.0 / static_cast<Scalar>(size);
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialSums[j] * invN;
  return result;
}

/* ===== ComputeVariance ===== */

DataContainer StatisticsEngine::ComputeVariance(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size < 2) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  const DataContainer mean = ComputeMean(sample);

  struct VarBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    const Scalar *meanPtr;
    std::vector<Scalar> partialSumSq;
    VarBody(const Scalar *d, UnsignedInteger dim, const Scalar *m)
      : data(d), dimension(dim), meanPtr(m), partialSumSq(dim, 0.0) {}
    VarBody(VarBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), meanPtr(other.meanPtr), partialSumSq(other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          const Scalar diff = row[j] - meanPtr[j];
          partialSumSq[j] += diff * diff;
        }
      }
    }
    void join(VarBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        partialSumSq[j] += other.partialSumSq[j];
    }
  };

  VarBody body(pData, dimension, mean.data());
  TBBImplementation::ParallelReduce(0, size, body);

  const Scalar invNm1 = 1.0 / static_cast<Scalar>(size - 1);
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialSumSq[j] * invNm1;
  return result;
}

/* ===== ComputeStandardDeviation ===== */

DataContainer StatisticsEngine::ComputeStandardDeviation(const DataContainer & sample)
{
  const DataContainer variance = ComputeVariance(sample);
  const UnsignedInteger dimension = variance.getDimension();
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = std::sqrt(variance[j]);
  return result;
}

/* ===== ComputeCovariance ===== */

DataContainer StatisticsEngine::ComputeCovariance(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size < 2) return DataContainer(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  const DataContainer mean = ComputeMean(sample);

  struct CovBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    const Scalar *meanPtr;
    std::vector<Scalar> partialSum;
    CovBody(const Scalar *d, UnsignedInteger dim, const Scalar *m)
      : data(d), dimension(dim), meanPtr(m), partialSum(dim * dim, 0.0) {}
    CovBody(CovBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), meanPtr(other.meanPtr), partialSum(other.dimension * other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          const Scalar dj = row[j] - meanPtr[j];
          for (UnsignedInteger k = j; k < dimension; ++k)
          {
            const Scalar dk = row[k] - meanPtr[k];
            partialSum[j * dimension + k] += dj * dk;
          }
        }
      }
    }
    void join(CovBody & other)
    {
      for (UnsignedInteger idx = 0; idx < dimension * dimension; ++idx)
        partialSum[idx] += other.partialSum[idx];
    }
  };

  CovBody body(pData, dimension, mean.data());
  TBBImplementation::ParallelReduce(0, size, body);

  const Scalar invNm1 = 1.0 / static_cast<Scalar>(size - 1);
  DataContainer result(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    result(j, j) = body.partialSum[j * dimension + j] * invNm1;
    for (UnsignedInteger k = j + 1; k < dimension; ++k)
    {
      const Scalar cov = body.partialSum[j * dimension + k] * invNm1;
      result(j, k) = cov;
      result(k, j) = cov;
    }
  }
  return result;
}

/* ===== ComputePearsonCorrelation ===== */

DataContainer StatisticsEngine::ComputePearsonCorrelation(const DataContainer & sample)
{
  const DataContainer cov = ComputeCovariance(sample);
  const UnsignedInteger dimension = cov.getDimension();
  const DataContainer stdDev = ComputeStandardDeviation(sample);

  DataContainer result(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    result(i, i) = 1.0;
    for (UnsignedInteger j = i + 1; j < dimension; ++j)
    {
      const Scalar denom = stdDev[i] * stdDev[j];
      const Scalar corr = (denom > 0.0) ? cov(i, j) / denom : 0.0;
      result(i, j) = corr;
      result(j, i) = corr;
    }
  }
  return result;
}

/* ===== ComputeSpearmanCorrelation ===== */

DataContainer StatisticsEngine::ComputeSpearmanCorrelation(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);

  DataContainer ranked(size, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    std::vector<std::pair<Scalar, UnsignedInteger>> pairs(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      pairs[i] = std::make_pair(sample(i, j), i);

    std::sort(pairs.begin(), pairs.end());

    UnsignedInteger idx = 0;
    while (idx < size)
    {
      UnsignedInteger start = idx;
      const Scalar val = pairs[start].first;
      while (idx < size && pairs[idx].first == val)
        ++idx;
      const Scalar avgRank = 0.5 * static_cast<Scalar>(start + idx + 1);
      for (UnsignedInteger k = start; k < idx; ++k)
        ranked(pairs[k].second, j) = avgRank;
    }
  }

  return ComputePearsonCorrelation(ranked);
}

/* ===== ComputeQuantile ===== */

DataContainer StatisticsEngine::ComputeQuantile(const DataContainer & sample, Scalar q)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    std::vector<Scalar> col(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      col[i] = sample(i, j);
    std::sort(col.begin(), col.end());

    const Scalar pos = q * static_cast<Scalar>(size - 1);
    const UnsignedInteger lo = static_cast<UnsignedInteger>(pos);
    const UnsignedInteger hi = std::min(lo + 1, size - 1);
    const Scalar frac = pos - static_cast<Scalar>(lo);
    result[j] = col[lo] + frac * (col[hi] - col[lo]);
  }
  return result;
}

/* ===== Sort ===== */

DataContainer StatisticsEngine::Sort(const DataContainer & sample, UnsignedInteger columnIndex)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0 || dimension == 0) return DataContainer(0, dimension, 0.0, DataContainer::ROW_MAJOR);
  if (columnIndex >= dimension)
    throw InvalidDimensionException(HERE) << "Column index " << columnIndex << " >= dimension " << dimension;

  std::vector<UnsignedInteger> indices(size);
  std::iota(indices.begin(), indices.end(), 0);

  const Scalar *pData = sample.data();
  std::sort(indices.begin(), indices.end(),
    [pData, dimension, columnIndex](UnsignedInteger a, UnsignedInteger b)
    {
      return pData[a * dimension + columnIndex] < pData[b * dimension + columnIndex];
    });

  DataContainer sorted(size, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar *row = pData + indices[i] * dimension;
    Scalar *outRow = sorted.data() + i * dimension;
    for (UnsignedInteger j = 0; j < dimension; ++j)
      outRow[j] = row[j];
  }
  return sorted;
}

/* ===== ComputeMin ===== */

DataContainer StatisticsEngine::ComputeMin(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  struct MinBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    std::vector<Scalar> partialMin;
    MinBody(const Scalar *d, UnsignedInteger dim)
      : data(d), dimension(dim), partialMin(dim, std::numeric_limits<Scalar>::max()) {}
    MinBody(MinBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), partialMin(other.dimension, std::numeric_limits<Scalar>::max()) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          if (row[j] < partialMin[j])
            partialMin[j] = row[j];
      }
    }
    void join(MinBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if (other.partialMin[j] < partialMin[j])
          partialMin[j] = other.partialMin[j];
    }
  };

  MinBody body(pData, dimension);
  TBBImplementation::ParallelReduce(0, size, body);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialMin[j];
  return result;
}

/* ===== ComputeMax ===== */

DataContainer StatisticsEngine::ComputeMax(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  const Scalar *pData = sample.data();

  struct MaxBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    std::vector<Scalar> partialMax;
    MaxBody(const Scalar *d, UnsignedInteger dim)
      : data(d), dimension(dim), partialMax(dim, std::numeric_limits<Scalar>::lowest()) {}
    MaxBody(MaxBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), partialMax(other.dimension, std::numeric_limits<Scalar>::lowest()) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          if (row[j] > partialMax[j])
            partialMax[j] = row[j];
      }
    }
    void join(MaxBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        if (other.partialMax[j] > partialMax[j])
          partialMax[j] = other.partialMax[j];
    }
  };

  MaxBody body(pData, dimension);
  TBBImplementation::ParallelReduce(0, size, body);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialMax[j];
  return result;
}

/* ===== ComputeRawMoment ===== */

DataContainer StatisticsEngine::ComputeRawMoment(const DataContainer & sample, UnsignedInteger k)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  if (k == 0) return ComputeSum(sample);
  const Scalar *pData = sample.data();

  struct MomentBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    UnsignedInteger order;
    std::vector<Scalar> partialSums;
    MomentBody(const Scalar *d, UnsignedInteger dim, UnsignedInteger k)
      : data(d), dimension(dim), order(k), partialSums(dim, 0.0) {}
    MomentBody(MomentBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), order(other.order), partialSums(other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
          partialSums[j] += std::pow(row[j], static_cast<Scalar>(order));
      }
    }
    void join(MomentBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        partialSums[j] += other.partialSums[j];
    }
  };

  MomentBody body(pData, dimension, k);
  TBBImplementation::ParallelReduce(0, size, body);

  const Scalar invN = 1.0 / static_cast<Scalar>(size);
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialSums[j] * invN;
  return result;
}

/* ===== ComputeCentralMoment ===== */

DataContainer StatisticsEngine::ComputeCentralMoment(const DataContainer & sample, UnsignedInteger k)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0) return DataContainer(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  if (k == 0)
  {
    DataContainer result(1, dimension, 1.0, DataContainer::ROW_MAJOR);
    return result;
  }
  const Scalar *pData = sample.data();

  const DataContainer mean = ComputeMean(sample);

  struct CentMomBody
  {
    const Scalar *data;
    UnsignedInteger dimension;
    UnsignedInteger order;
    const Scalar *meanPtr;
    std::vector<Scalar> partialSums;
    CentMomBody(const Scalar *d, UnsignedInteger dim, UnsignedInteger k, const Scalar *m)
      : data(d), dimension(dim), order(k), meanPtr(m), partialSums(dim, 0.0) {}
    CentMomBody(CentMomBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), order(other.order), meanPtr(other.meanPtr), partialSums(other.dimension, 0.0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i < r.end(); ++i)
      {
        const Scalar *row = data + i * dimension;
        for (UnsignedInteger j = 0; j < dimension; ++j)
        {
          const Scalar diff = row[j] - meanPtr[j];
          partialSums[j] += std::pow(diff, static_cast<Scalar>(order));
        }
      }
    }
    void join(CentMomBody & other)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
        partialSums[j] += other.partialSums[j];
    }
  };

  CentMomBody body(pData, dimension, k, mean.data());
  TBBImplementation::ParallelReduce(0, size, body);

  const Scalar invN = 1.0 / static_cast<Scalar>(size);
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
    result[j] = body.partialSums[j] * invN;
  return result;
}

/* ===== ComputeSkewness ===== */

DataContainer StatisticsEngine::ComputeSkewness(const DataContainer & sample)
{
  const DataContainer var = ComputeVariance(sample);
  const UnsignedInteger dimension = var.getDimension();
  const DataContainer m3 = ComputeCentralMoment(sample, 3);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar sd = std::sqrt(var[j]);
    if (sd > 0.0)
      result[j] = m3[j] / (sd * sd * sd);
  }
  return result;
}

/* ===== ComputeKurtosis ===== */

DataContainer StatisticsEngine::ComputeKurtosis(const DataContainer & sample)
{
  const DataContainer var = ComputeVariance(sample);
  const UnsignedInteger dimension = var.getDimension();
  const DataContainer m4 = ComputeCentralMoment(sample, 4);

  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar v = var[j];
    if (v > 0.0)
      result[j] = m4[j] / (v * v);
  }
  return result;
}


DataContainer StatisticsEngine::ComputeMeanBlockwise(const DataContainer & sample,
    UnsignedInteger blockSize)
{
  const UnsignedInteger nbRows = sample.getSize();
  const UnsignedInteger dim = sample.getDimension();
  if (nbRows == 0) return DataContainer(1, dim, 0.0, DataContainer::ROW_MAJOR);
  if (nbRows <= blockSize) return ComputeMean(sample);

  DataContainer accumulated(1, dim, 0.0, DataContainer::ROW_MAJOR);
  std::vector<Scalar> blockData(blockSize * dim);
  UnsignedInteger processed = 0;

  while (processed < nbRows)
  {
    const UnsignedInteger currentBlock = std::min(blockSize, nbRows - processed);
    const Scalar * src = sample.data() + processed * dim;
    Scalar * dst = blockData.data();
    std::copy(src, src + currentBlock * dim, dst);

    // Accumulate
    for (UnsignedInteger j = 0; j < dim; ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < currentBlock; ++i)
        sum += dst[j + i * dim];
      accumulated[j] += sum;
    }
    processed += currentBlock;
  }

  for (UnsignedInteger j = 0; j < dim; ++j)
    accumulated[j] /= nbRows;

  return accumulated;
}


DataContainer StatisticsEngine::ComputeVarianceBlockwise(const DataContainer & sample,
    UnsignedInteger blockSize)
{
  const UnsignedInteger nbRows = sample.getSize();
  const UnsignedInteger dim = sample.getDimension();
  if (nbRows <= 1) return DataContainer(1, dim, 0.0, DataContainer::ROW_MAJOR);
  if (nbRows <= blockSize) return ComputeVariance(sample);

  // Pass 1: compute mean
  const DataContainer mean = ComputeMeanBlockwise(sample, blockSize);

  // Pass 2: accumulate squared deviations
  DataContainer accumulated(1, dim, 0.0, DataContainer::ROW_MAJOR);
  std::vector<Scalar> blockData(blockSize * dim);
  UnsignedInteger processed = 0;

  while (processed < nbRows)
  {
    const UnsignedInteger currentBlock = std::min(blockSize, nbRows - processed);
    std::copy(sample.data() + processed * dim,
              sample.data() + (processed + currentBlock) * dim,
              blockData.data());

    for (UnsignedInteger j = 0; j < dim; ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < currentBlock; ++i)
      {
        const Scalar val = blockData[j + i * dim] - mean[j];
        sum += val * val;
      }
      accumulated[j] += sum;
    }
    processed += currentBlock;
  }

  for (UnsignedInteger j = 0; j < dim; ++j)
    accumulated[j] /= (nbRows - 1);

  return accumulated;
}


DataContainer StatisticsEngine::ComputeCovarianceBlockwise(const DataContainer & sample,
    UnsignedInteger blockSize)
{
  const UnsignedInteger nbRows = sample.getSize();
  const UnsignedInteger dim = sample.getDimension();
  if (nbRows <= 1) return DataContainer(dim, dim, 0.0, DataContainer::COLUMN_MAJOR);
  if (nbRows <= blockSize) return ComputeCovariance(sample);

  // Pass 1: compute mean
  const DataContainer mean = ComputeMeanBlockwise(sample, blockSize);

  // Pass 2: accumulate outer products
  DataContainer accumulated(dim, dim, 0.0, DataContainer::COLUMN_MAJOR);
  std::vector<Scalar> blockData(blockSize * dim);
  UnsignedInteger processed = 0;

  while (processed < nbRows)
  {
    const UnsignedInteger currentBlock = std::min(blockSize, nbRows - processed);
    std::copy(sample.data() + processed * dim,
              sample.data() + (processed + currentBlock) * dim,
              blockData.data());

    for (UnsignedInteger i = 0; i < dim; ++i)
    {
      for (UnsignedInteger j = i; j < dim; ++j)
      {
        Scalar sum = 0.0;
        for (UnsignedInteger k = 0; k < currentBlock; ++k)
        {
          const Scalar di = blockData[i + k * dim] - mean[i];
          const Scalar dj = blockData[j + k * dim] - mean[j];
          sum += di * dj;
        }
        accumulated(i, j) += sum;
        if (i != j)
          accumulated(j, i) += sum;
      }
    }
    processed += currentBlock;
  }

  for (UnsignedInteger i = 0; i < dim * dim; ++i)
    accumulated[i] /= (nbRows - 1);

  return accumulated;
}


END_NAMESPACE_OPENTURNS
