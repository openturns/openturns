//                                               -*- C++ -*-
/**
 *  @brief StatisticsEngine provides free functions for statistical operations
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
#ifndef OPENTURNS_STATISTICSENGINE_HXX
#define OPENTURNS_STATISTICSENGINE_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DataContainer.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StatisticsEngine
 *
 * StatisticsEngine provides free functions for statistical operations
 * on DataContainer objects, using TBBImplementation for parallelism.
 * These functions operate on raw pointers from DataContainer::data(),
 * enabling zero-copy interoperability between different container types.
 *
 * Sample convention: row = observation, column = variable.
 * Input data must be in ROW_MAJOR layout.
 */
class OT_API StatisticsEngine
{
public:

  /** Compute mean of each column. Returns a DataContainer of size 1 x dimension. */
  static DataContainer ComputeMean(const DataContainer & sample);

  /** Compute variance of each column. Returns 1 x dimension. */
  static DataContainer ComputeVariance(const DataContainer & sample);

  /** Compute standard deviation of each column. Returns 1 x dimension. */
  static DataContainer ComputeStandardDeviation(const DataContainer & sample);

  /** Compute covariance matrix. Returns dimension x dimension. */
  static DataContainer ComputeCovariance(const DataContainer & sample);

  /** Compute Pearson correlation matrix. Returns dimension x dimension. */
  static DataContainer ComputePearsonCorrelation(const DataContainer & sample);

  /** Compute Spearman rank correlation matrix. Returns dimension x dimension. */
  static DataContainer ComputeSpearmanCorrelation(const DataContainer & sample);

  /** Compute quantiles of each column. q is in [0,1]. Returns 1 x dimension. */
  static DataContainer ComputeQuantile(const DataContainer & sample, Scalar q);

  /** Sort sample by a given column index (returns sorted copy). */
  static DataContainer Sort(const DataContainer & sample, UnsignedInteger columnIndex = 0);

  /** Compute element-wise minimum across rows. Returns 1 x dimension. */
  static DataContainer ComputeMin(const DataContainer & sample);

  /** Compute element-wise maximum across rows. Returns 1 x dimension. */
  static DataContainer ComputeMax(const DataContainer & sample);

  /** Compute sum of each column. Returns 1 x dimension. */
  static DataContainer ComputeSum(const DataContainer & sample);

  /** Compute raw moment of order k for each column. Returns 1 x dimension. */
  static DataContainer ComputeRawMoment(const DataContainer & sample, UnsignedInteger k);

  /** Compute central moment of order k for each column. Returns 1 x dimension. */
  static DataContainer ComputeCentralMoment(const DataContainer & sample, UnsignedInteger k);

  /** Compute skewness of each column. Returns 1 x dimension. */
  static DataContainer ComputeSkewness(const DataContainer & sample);

  /** Compute kurtosis of each column. Returns 1 x dimension. */
  static DataContainer ComputeKurtosis(const DataContainer & sample);

  /**
   * Block-based statistics for out-of-core processing.
   * These methods read data in blocks of blockSize rows at a time,
   * avoiding loading the full dataset into memory.
   * Intended for use with HDF5Backend-backed DataContainers.
   */

  /** Block-based mean: processes sample in blocks of blockSize rows. */
  static DataContainer ComputeMeanBlockwise(const DataContainer & sample,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("StatisticsEngine-DefaultBlockSize"));

  /** Block-based variance: two-pass (mean then variance). */
  static DataContainer ComputeVarianceBlockwise(const DataContainer & sample,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("StatisticsEngine-DefaultBlockSize"));

  /** Block-based covariance: two-pass. */
  static DataContainer ComputeCovarianceBlockwise(const DataContainer & sample,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("StatisticsEngine-DefaultBlockSize"));

}; /* class StatisticsEngine */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STATISTICSENGINE_HXX */
