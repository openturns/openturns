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

#include "kendall.h"

BEGIN_NAMESPACE_OPENTURNS

static void CheckSampleLayout(const DataContainer & sample)
{
  if (sample.getLayout() != DataContainer::ROW_MAJOR)
    throw InvalidArgumentException(HERE) << "Sample must be in ROW_MAJOR layout";
}

/* Interpolate one quantile level from a sorted column.
 * With type7 = false, reproduce the historical SampleImplementation rule
 * (s = q * n - 0.5 clamped to [min, max]); with type7 = true, use the
 * Hyndman-Fan Type-7 rule (pos = q * (n - 1) clamped to [min, max]). */
static Scalar InterpolateQuantile(const std::vector<Scalar> & sortedColumn,
                                  Scalar q,
                                  Bool type7)
{
  const UnsignedInteger size = static_cast<UnsignedInteger>(sortedColumn.size());
  const Scalar n = 1.0 * size;
  UnsignedInteger lo = 0;
  UnsignedInteger hi = 0;
  Scalar frac = 0.0;
  if (type7)
  {
    const Scalar pos = std::min(std::max(q * (n - 1.0), 0.0), n - 1.0);
    lo = static_cast<UnsignedInteger>(pos);
    hi = std::min(lo + 1, size - 1);
    frac = pos - static_cast<Scalar>(lo);
  }
  else
  {
    const Scalar scalarIndex = q * n - 0.5;
    if (scalarIndex <= 0.0)
    {
      lo = 0;
      hi = 0;
    }
    else if (scalarIndex >= n - 1.0)
    {
      lo = size - 1;
      hi = size - 1;
    }
    else
    {
      lo = static_cast<UnsignedInteger>(scalarIndex);
      hi = lo + 1;
      frac = scalarIndex - static_cast<Scalar>(lo);
    }
  }
  return sortedColumn[lo] + frac * (sortedColumn[hi] - sortedColumn[lo]);
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
  // Note: denominators are taken from the diagonal of the same covariance
  // matrix, not from a separate variance computation. This preserves exact
  // unit (anti-)correlations for identical (opposite) columns, which
  // downstream 1 / (1 - R^2) statistics rely on (e.g. FullPearson of a
  // sample with itself must be infinite, not merely large).
  const DataContainer cov = ComputeCovariance(sample);
  const UnsignedInteger dimension = cov.getDimension();

  DataContainer result(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    result(i, i) = 1.0;
    const Scalar sdi = std::sqrt(cov(i, i));
    for (UnsignedInteger j = i + 1; j < dimension; ++j)
    {
      const Scalar sdj = std::sqrt(cov(j, j));
      const Scalar denom = sdi * sdj;
      Scalar corr = (denom > 0.0) ? cov(i, j) / denom : 0.0;
      // Cauchy-Schwarz: round-off alone can push |corr| past 1
      corr = std::min(std::max(corr, -1.0), 1.0);
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

/* ===== ComputeKendallTau ===== */

struct KendallPairBody
{
  const Scalar * data;
  UnsignedInteger size;
  UnsignedInteger dimension;
  const UnsignedInteger * indX;
  const UnsignedInteger * indY;
  Scalar * taus;
  Bool smallCase;
  KendallPairBody(const Scalar * d, UnsignedInteger n, UnsignedInteger dim,
                  const UnsignedInteger * ix, const UnsignedInteger * iy,
                  Scalar * t, Bool small)
    : data(d), size(n), dimension(dim), indX(ix), indY(iy), taus(t), smallCase(small) {}
  inline void operator()(const tbb::blocked_range<UnsignedInteger> & r) const
  {
    std::vector<Scalar> x(size);
    std::vector<Scalar> y(size);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger ix = indX[i];
      const UnsignedInteger iy = indY[i];
      for (UnsignedInteger k = 0; k < size; ++k)
      {
        x[k] = data[k * dimension + ix];
        y[k] = data[k * dimension + iy];
      }
      if (smallCase) taus[i] = kendallSmallN(x.data(), y.data(), size);
      else taus[i] = kendallNlogN(x.data(), y.data(), size);
    }
  }
};

DataContainer StatisticsEngine::ComputeKendallTau(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  DataContainer result(dimension, dimension, 0.0, DataContainer::ROW_MAJOR);
  for (UnsignedInteger i = 0; i < dimension; ++i) result(i, i) = 1.0;
  if (dimension < 2 || size == 0) return result;

  // Same threshold as the historical Sample implementation
  const Bool smallCase = size < ResourceMap::GetAsUnsignedInteger("Sample-SmallKendallTau");
  const UnsignedInteger caseNumber = (dimension * (dimension - 1)) / 2;
  std::vector<UnsignedInteger> indX(caseNumber);
  std::vector<UnsignedInteger> indY(caseNumber);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < dimension - 1; ++i)
    for (UnsignedInteger j = i + 1; j < dimension; ++j)
    {
      indX[index] = i;
      indY[index] = j;
      ++index;
    }
  DataContainer taus(1, caseNumber, 0.0, DataContainer::ROW_MAJOR);
  const KendallPairBody body(sample.data(), size, dimension, indX.data(), indY.data(), taus.data(), smallCase);
  TBBImplementation::ParallelFor(0, caseNumber, body);
  index = 0;
  for (UnsignedInteger i = 0; i < dimension - 1; ++i)
    for (UnsignedInteger j = i + 1; j < dimension; ++j)
    {
      const Scalar tau = taus[index];
      result(i, j) = tau;
      result(j, i) = tau;
      ++index;
    }
  return result;
}

/* ===== ComputeQuantile ===== */

DataContainer StatisticsEngine::ComputeQuantile(const DataContainer & sample,
    Scalar q,
    Bool type7)
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
    result[j] = InterpolateQuantile(col, q, type7);
  }
  return result;
}

/* ===== ComputeEmpiricalCDF ===== */

Scalar StatisticsEngine::ComputeEmpiricalCDF(const DataContainer & sample,
    const DataContainer & location,
    Bool tail)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (location.getSize() * location.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Location has incorrect dimension";
  if (size == 0) return 0.0;

  struct ECDFBody
  {
    const Scalar * data;
    UnsignedInteger dimension;
    const Scalar * point;
    Bool tail;
    UnsignedInteger count;
    ECDFBody(const Scalar * d, UnsignedInteger dim, const Scalar * p, Bool t)
      : data(d), dimension(dim), point(p), tail(t), count(0) {}
    ECDFBody(ECDFBody & other, TBBImplementation::Split)
      : data(other.data), dimension(other.dimension), point(other.point), tail(other.tail), count(0) {}
    void operator()(const tbb::blocked_range<UnsignedInteger> & r)
    {
      for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
      {
        const Scalar * row = data + i * dimension;
        UnsignedInteger j = 0;
        while ((j < dimension) && (tail ^ (row[j] <= point[j]))) ++j;
        if (j == dimension) ++count;
      }
    }
    void join(ECDFBody & other)
    {
      count += other.count;
    }
  };

  ECDFBody body(sample.data(), dimension, location.data(), tail);
  TBBImplementation::ParallelReduce(0, size, body, 1024);
  return (1.0 * body.count) / size;
}

/* ===== ComputeQuantile at multiple probability levels ===== */

DataContainer StatisticsEngine::ComputeQuantile(const DataContainer & sample,
    const DataContainer & probabilities,
    Bool type7)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  const UnsignedInteger levelCount = probabilities.getSize() * probabilities.getDimension();
  DataContainer result(levelCount, dimension, 0.0, DataContainer::ROW_MAJOR);
  if (size == 0 || levelCount == 0) return result;

  const Scalar * pLevels = probabilities.data();
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    std::vector<Scalar> col(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      col[i] = sample(i, j);
    std::sort(col.begin(), col.end());
    for (UnsignedInteger p = 0; p < levelCount; ++p)
      result(p, j) = InterpolateQuantile(col, pLevels[p], type7);
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

/* ===== SortLexicographic ===== */

DataContainer StatisticsEngine::SortLexicographic(const DataContainer & sample)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  if (size == 0 || dimension == 0) return DataContainer(0, dimension, 0.0, DataContainer::ROW_MAJOR);

  std::vector<UnsignedInteger> indices(size);
  std::iota(indices.begin(), indices.end(), 0);

  const Scalar *pData = sample.data();
  std::sort(indices.begin(), indices.end(),
    [pData, dimension](UnsignedInteger a, UnsignedInteger b)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        const Scalar va = pData[a * dimension + j];
        const Scalar vb = pData[b * dimension + j];
        if (va < vb) return true;
        if (vb < va) return false;
      }
      return false;
    });

  DataContainer sorted(size, dimension, 0.0, DataContainer::ROW_MAJOR);
  Scalar *pSorted = sorted.data();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar *row = pData + indices[i] * dimension;
    Scalar *outRow = pSorted + i * dimension;
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
  if (k == 0)
  {
    DataContainer result(1, dimension, 1.0, DataContainer::ROW_MAJOR);
    return result;
  }
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

DataContainer StatisticsEngine::ComputeSkewness(const DataContainer & sample,
    Bool adjusted)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  // Match SampleImplementation::computeSkewness guards: 0 at size 2, nothing below
  if (size <= 2) return result;
  const DataContainer m2 = ComputeCentralMoment(sample, 2);
  const DataContainer m3 = ComputeCentralMoment(sample, 3);

  const Scalar n = 1.0 * size;
  const Scalar factor = adjusted ? std::sqrt(n * (n - 1.0)) / (n - 2.0) : 1.0;
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar moment2 = m2[j];
    if (moment2 > 0.0)
    {
      const Scalar sd = std::sqrt(moment2);
      result[j] = factor * m3[j] / (sd * sd * sd);
    }
  }
  return result;
}

/* ===== ComputeKurtosis ===== */

DataContainer StatisticsEngine::ComputeKurtosis(const DataContainer & sample,
    Bool adjusted)
{
  CheckSampleLayout(sample);
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger dimension = sample.getDimension();
  DataContainer result(1, dimension, 0.0, DataContainer::ROW_MAJOR);
  // Match SampleImplementation::computeKurtosis guards: 0 at size 3, nothing below
  if (size <= 3) return result;
  const DataContainer m2 = ComputeCentralMoment(sample, 2);
  const DataContainer m4 = ComputeCentralMoment(sample, 4);

  const Scalar n = 1.0 * size;
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    const Scalar moment2 = m2[j];
    if (moment2 > 0.0)
    {
      const Scalar b2 = m4[j] / (moment2 * moment2);
      if (adjusted)
        result[j] = ((n * n - 1.0) / ((n - 2.0) * (n - 3.0))) * b2 - 3.0 * (3.0 * n - 5.0) / ((n - 2.0) * (n - 3.0));
      else
        result[j] = b2;
    }
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
