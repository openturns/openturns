//                                               -*- C++ -*-
/**
 * @brief SensitivityAnalysis implements the sensitivity analysis methods based on Sobol coefficients
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
#include "openturns/SensitivityAnalysis.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/Pie.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SensitivityAnalysis)

/* Constructor with input samples and model */
SensitivityAnalysis::SensitivityAnalysis (const NumericalSample & inputSample1,
    const NumericalSample & inputSample2,
    const NumericalMathFunction & model)
  : Object()
  , inputSample1_(inputSample1)
  , inputSample2_(inputSample2)
  , model_(model)
  , alreadyComputedOrder_(0)
{
  if (inputSample1_.getDimension() != inputSample2_.getDimension()) throw InvalidDimensionException(HERE) << "Input samples must have the same dimension";
  if (inputSample1_.getSize() != inputSample2_.getSize()) throw InvalidArgumentException(HERE) << "Input samples must have the same size";
  if (inputSample1_.getDimension() != model_.getInputDimension()) throw InvalidDimensionException(HERE) << "Input samples must have the same dimension as the model";

  // initialize blockSize
  const UnsignedInteger size = inputSample1_.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Input sample is empty";
  blockSize_ = std::min(size, ResourceMap::GetAsUnsignedInteger("SensitivityAnalysis-DefaultBlockSize"));
}

/* Compute all the Sobol indices */
void SensitivityAnalysis::computeSobolIndices(const UnsignedInteger order) const
{
  const UnsignedInteger spatialDimension = inputSample1_.getDimension();
  const UnsignedInteger outputDimension = model_.getOutputDimension();
  const UnsignedInteger size = inputSample1_.getSize();

  // allocate indices
  firstOrderIndice_ = NumericalSample(outputDimension, spatialDimension);
  totalOrderIndice_ = NumericalSample(outputDimension, spatialDimension);
  if (order >= 2) secondOrderIndice_ = SymmetricTensor(spatialDimension, outputDimension);

  // this avoids to store huge input samples while allowing for multi-threading
  const UnsignedInteger maximumOuterSampling = static_cast<UnsignedInteger>(ceil(1.0 * size / blockSize_));
  const UnsignedInteger modulo = size % blockSize_;
  const UnsignedInteger lastBlockSize = modulo == 0 ? blockSize_ : modulo;

  // 2*N evaluations
  const NumericalSample outputSample1(model_(inputSample1_));
  const NumericalSample outputSample2(model_(inputSample2_));

  NumericalPoint sample1SquareMean(outputSample1.computeMean());// mean over one sample, used for total order indices
  for (UnsignedInteger j = 0; j < outputDimension; ++ j) sample1SquareMean[j] *=  sample1SquareMean[j];
  NumericalPoint crossSquareMean(outputDimension, 0.0);// cross-mean over both samples, used for 1st order indices
  for (UnsignedInteger i = 0; i < size; ++ i)
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      crossSquareMean[j] += outputSample1[i][j] * outputSample2[i][j] / size;

  const NumericalPoint sample1Variance(outputSample1.computeVariance());

  // for each block ...
  for (UnsignedInteger outerSampling = 0; outerSampling < maximumOuterSampling; ++ outerSampling)
  {
    // the last block can be smaller
    const UnsignedInteger effectiveBlockSize = outerSampling < (maximumOuterSampling - 1) ? blockSize_ : lastBlockSize;

    // k*N evaluations
    NumericalSample scrambledAllOutputBlock1(0, outputDimension);

    // k*N evaluations (only 2nd order)
    NumericalSample scrambledAllOutputBlock2(0, outputDimension);

    for (UnsignedInteger k1 = 0; k1 < spatialDimension; ++ k1)
    {
      NumericalSample scrambledInputBlock1(effectiveBlockSize, spatialDimension);
      for (UnsignedInteger blockIndex = 0; blockIndex < effectiveBlockSize; ++ blockIndex)
      {
        scrambledInputBlock1[blockIndex] = inputSample1_[outerSampling * blockSize_ + blockIndex];
        scrambledInputBlock1[blockIndex][k1] = inputSample2_[outerSampling * blockSize_ + blockIndex][k1];
      }
      const NumericalSample scrambledOutputBlock1(model_(scrambledInputBlock1));
      scrambledAllOutputBlock1.add(scrambledOutputBlock1);

      if (order >= 2)
      {
        NumericalSample scrambledInputBlock2(effectiveBlockSize , spatialDimension);
        for (UnsignedInteger blockIndex = 0; blockIndex < effectiveBlockSize; ++ blockIndex)
        {
          scrambledInputBlock2[blockIndex] = inputSample2_[outerSampling * blockSize_ + blockIndex];
          scrambledInputBlock2[blockIndex][k1] = inputSample1_[outerSampling * blockSize_ + blockIndex][k1];
        }
        const NumericalSample scrambledOutputBlock2(model_(scrambledInputBlock2));
        scrambledAllOutputBlock2.add(scrambledOutputBlock2);
      }

      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      {
        for (UnsignedInteger blockIndex = 0; blockIndex < effectiveBlockSize; ++ blockIndex)
        {
          // this is the core idea of the Saltelli 2002 algorithm
          firstOrderIndice_[j][k1] += scrambledOutputBlock1[blockIndex][j] * outputSample2[outerSampling * blockSize_ + blockIndex][j] / (size - 1);
          totalOrderIndice_[j][k1] += scrambledOutputBlock1[blockIndex][j] * outputSample1[outerSampling * blockSize_ + blockIndex][j] / (size - 1);
          if (order >= 2)
          {
            for (UnsignedInteger k2 = 0; k2 < k1; ++ k2)
            {
              // here we could also get the closed second order effects with the same trick as above without any further computation
              secondOrderIndice_(k1, k2, j) += scrambledAllOutputBlock1[k1 * effectiveBlockSize + blockIndex][j] * scrambledAllOutputBlock2[k2 * effectiveBlockSize + blockIndex][j] / (size - 1);
            }
          }
        }
      }
    }
  }

  // normalize indices
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    for (UnsignedInteger k = 0; k < spatialDimension; ++ k)
    {
      const NumericalScalar FOIjk(firstOrderIndice_[j][k]);
      const NumericalScalar CSMj(crossSquareMean[j]);
      const NumericalScalar sample1Variancej(sample1Variance[j]);
      firstOrderIndice_[j][k] = (FOIjk - CSMj) / sample1Variancej;
      if ((firstOrderIndice_[j][k] < 0.0) || (firstOrderIndice_[j][k] > 1.0)) LOGWARN(OSS() << "The estimated first order total Sobol index (" << k << ") is not in the range [0, 1]. You may increase the sampling size. HERE we have: FOIjk=" << FOIjk << ", CSMj=" << CSMj << ", sample1Variancej=" << sample1Variancej);

      totalOrderIndice_[j][k] = 1.0 - (totalOrderIndice_[j][k] - sample1SquareMean[j]) / sample1Variance[j];
      if ((totalOrderIndice_[j][k] < 0.0) || (totalOrderIndice_[j][k] > 1.0)) LOGWARN(OSS() << "The estimated total order total Sobol index (" << k << ") is not in the range [0, 1]. You may increase the sampling size.");
    }
    if (order >= 2)
    {
      for (UnsignedInteger k1 = 0; k1 < spatialDimension; ++ k1)
      {
        for (UnsignedInteger k2 = 0; k2 < k1; ++ k2)
        {
          secondOrderIndice_(k1, k2, j) = (secondOrderIndice_(k1, k2, j) - crossSquareMean[j]) / sample1Variance[j] - firstOrderIndice_[j][k1] - firstOrderIndice_[j][k2];
          if ((secondOrderIndice_(k1, k2, j) < 0.0) || (secondOrderIndice_(k1, k2, j) > 1.0)) LOGWARN(OSS() << "The estimated second order Sobol index (" << k1 << ", " << k2 << ") is not in the range [0, 1]. You may increase the sampling size.");
        }
      }
    }
  }

  alreadyComputedOrder_ = order;
}


/* First Order indices accessor */
NumericalPoint SensitivityAnalysis::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (alreadyComputedOrder_ < 1) computeSobolIndices(1);
  if (marginalIndex >= firstOrderIndice_.getSize()) throw InvalidArgumentException(HERE) << "Output dimension is " << firstOrderIndice_.getSize();
  return firstOrderIndice_[marginalIndex];
}

/* Second order indices accessor */
SymmetricMatrix SensitivityAnalysis::getSecondOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (alreadyComputedOrder_ < 2) computeSobolIndices(2);
  if (marginalIndex >= secondOrderIndice_.getNbSheets()) throw InvalidArgumentException(HERE) << "Output dimension is " << secondOrderIndice_.getNbSheets();
  return secondOrderIndice_.getSheet(marginalIndex);
}

/* Total Order indices accessor */
NumericalPoint SensitivityAnalysis::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (alreadyComputedOrder_ < 1) computeSobolIndices(1);
  if (marginalIndex >= totalOrderIndice_.getSize()) throw InvalidArgumentException(HERE) << "Output dimension is " << totalOrderIndice_.getSize();
  return totalOrderIndice_[marginalIndex];
}

/* Block size accessor */
void SensitivityAnalysis::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

UnsignedInteger SensitivityAnalysis::getBlockSize() const
{
  return blockSize_;
}

/* ImportanceFactors graph */
Graph SensitivityAnalysis::DrawImportanceFactors(const NumericalPointWithDescription & importanceFactors,
    const String & title)
{
  return DrawImportanceFactors(importanceFactors, importanceFactors.getDescription(), title);
}

/* ImportanceFactors graph */
Graph SensitivityAnalysis::DrawImportanceFactors(const NumericalPoint & values,
    const Description & names,
    const String & title)
{
  /* build data for the pie */
  const UnsignedInteger dimension = values.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on empty data.";
  if ((names.getSize() != 0) && (names.getSize() != dimension)) throw InvalidArgumentException(HERE) << "Error: the names size must match the value dimension.";
  NumericalScalar l1Norm(0.0);
  for (UnsignedInteger i = 0; i < dimension; ++i) l1Norm += std::abs(values[i]);
  if (l1Norm == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on null data.";
  NumericalPoint data(dimension);
  /* Normalization */
  for (UnsignedInteger i = 0; i < dimension; ++i) data[i] = values[i] / l1Norm;
  /* we build the pie */
  Pie importanceFactorsPie(data);

  /* build labels and colors for the pie */
  Description palette(dimension);
  Description labels(dimension);
  Description description(names);
  // If no description has been given for the input distribution components, give standard ones
  if (description.getSize() != dimension)
  {
    description = Description(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) description[i] = String(OSS() << "Component " << i);
  }
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    OSS oss(false);
    oss << description[i] << " : " << std::fixed;
    oss.setPrecision(1);
    oss << 100.0 * data[i] << "%";
    labels[i] = oss;
  }
  /* we complete the pie */
  importanceFactorsPie.setLabels(labels);
  importanceFactorsPie.buildDefaultPalette();
  /* we build the graph with a title */
  Graph importanceFactorsGraph(title);
  /* we embed the pie into the graph */
  importanceFactorsGraph.add(importanceFactorsPie);
  return importanceFactorsGraph;
}

END_NAMESPACE_OPENTURNS


