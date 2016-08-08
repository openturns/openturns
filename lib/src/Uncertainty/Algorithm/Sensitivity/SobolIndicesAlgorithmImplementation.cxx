//                                               -*- C++ -*-
/**
 *  @brief Implementation for sensitivity algorithms
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

#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/TBB.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Pie.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesAlgorithmImplementation);

static const Factory<SobolIndicesAlgorithmImplementation> Factory_SobolIndicesAlgorithmImplementation;

/* Default constructor */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation()
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(0)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsNumericalScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , mergedFirstOrderIndices_()
  , mergedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceInterval_()
  , totalOrderIndiceInterval_()
{
  // Nothing to do
}

/** Constructor with parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const NumericalSample & inputDesign,
                                                                         const NumericalSample & outputDesign,
                                                                         const UnsignedInteger size)
  : PersistentObject()
  , inputDesign_(inputDesign)
  , outputDesign_(outputDesign)
  , size_(size)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsNumericalScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , mergedFirstOrderIndices_()
  , mergedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceInterval_()
  , totalOrderIndiceInterval_()
{
  if (outputDesign.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, output design is empty" ;

  // center Y
  NumericalPoint muY(outputDesign_.computeMean());
  outputDesign_ -= muY;

  // Check if desing result is coherant
  if (inputDesign.getSize() != outputDesign.getSize())
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, input and output designs have different size. Input design size=" << inputDesign.getSize()
                                         << ", wheras output design size=" << outputDesign.getSize();
  // Reference sample and its variance
  NumericalSample outReference(size, outputDesign.getDimension());
  for (UnsignedInteger k = 0; k < size; ++k) outReference[k] = outputDesign[k];
  referenceVariance_ = outReference.computeVariance();
}

/* Virtual constructor */
SobolIndicesAlgorithmImplementation * SobolIndicesAlgorithmImplementation::clone() const
{
  return new SobolIndicesAlgorithmImplementation(*this);
}

/** Constructor with distribution / model parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const Distribution & distribution,
                                                                         const UnsignedInteger size,
                                                                         const NumericalMathFunction & model,
                                                                         const Bool computeSecondOrder)
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(size)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsNumericalScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , mergedFirstOrderIndices_()
  , mergedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceInterval_()
  , totalOrderIndiceInterval_()
{
  const UnsignedInteger inputDimension = model.getInputDimension();
  if (inputDimension != distribution.getDimension())
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, incompatible dimension between model and distribution. distribution dimension=" << distribution.getDimension()
                                         <<", model input dimension = " << inputDimension;
  inputDesign_ = Generate(distribution, size, computeSecondOrder);
  outputDesign_ = model(inputDesign_);

  // center Y
  NumericalPoint muY(outputDesign_.computeMean());
  outputDesign_ -= muY;

  size_ = size;
  // Reference sample and its variance
  NumericalSample outReference(size, outputDesign_.getDimension());
  for (UnsignedInteger k = 0; k < size_; ++k) outReference[k] = outputDesign_[k];
  referenceVariance_ = outReference.computeVariance();
}

/** Constructor with distribution / model parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const WeightedExperiment & experiment,
                                                                         const NumericalMathFunction & model,
                                                                         const Bool computeSecondOrder)
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_()
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsNumericalScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , mergedFirstOrderIndices_()
  , mergedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceInterval_()
  , totalOrderIndiceInterval_()
{
  const UnsignedInteger inputDimension = model.getInputDimension();
  if (inputDimension != experiment.getDistribution().getDimension())
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, incompatible dimension between model and distribution. Experiment dimension=" << experiment.getDistribution().getDimension()
                                         <<", model input dimension = " << inputDimension;
  inputDesign_ = Generate(experiment, computeSecondOrder);
  outputDesign_ = model(inputDesign_);

  // center Y
  NumericalPoint muY(outputDesign_.computeMean());
  outputDesign_ -= muY;

  size_ = experiment.getSize();
  // Reference sample and its variance
  NumericalSample outReference(size_, outputDesign_.getDimension());
  for (UnsignedInteger k = 0; k < size_; ++k) outReference[k] = outputDesign_[k];
  referenceVariance_ = outReference.computeVariance();
}

NumericalSample SobolIndicesAlgorithmImplementation::Generate(const Distribution & distribution,
                                                              const UnsignedInteger size,
                                                              const Bool computeSecondOrder)
{
  if (!distribution.hasIndependentCopula())
    throw InvalidDimensionException(HERE) << "In SensistivityAlgorithmImplementation::Generate, distribution should have independent copula";
  const NumericalSample inputSample1(distribution.getSample(size));
  const NumericalSample inputSample2(distribution.getSample(size));
  UnsignedInteger dimension = inputSample1.getDimension();

  NumericalSample design(inputSample1);
  design.add(inputSample2);
  // Compute designs of type Saltelli/Martinez for 1st order
  for (UnsignedInteger p = 0; p < dimension; ++p)
  {
    NumericalSample x(inputSample1);
    for (UnsignedInteger k = 0; k < size; ++k) x[k][p] = inputSample2[k][p];
    design.add(x);
  }
  if (computeSecondOrder)
  {
    for (UnsignedInteger p = 0; p < dimension; ++p)
    {
      NumericalSample x(inputSample2);
      for (UnsignedInteger k = 0; k < size; ++k) x[k][p] = inputSample1[k][p];
      design.add(x);
    }
  }
  // Return collection
  return design;
}

NumericalSample SobolIndicesAlgorithmImplementation::Generate(const WeightedExperiment & experiment,
                                                              const Bool computeSecondOrder)
{
  if (!experiment.getDistribution().hasIndependentCopula())
    throw InvalidDimensionException(HERE) << "In SensistivityAlgorithmImplementation::Generate, weighted's distribution should have independent copula";
  const UnsignedInteger size = experiment.getSize();
  // WeightedExperiment::generate is not const, so we should copy experiment
  WeightedExperiment weightedExperiment(experiment);
  const NumericalSample inputSample1(weightedExperiment.generate());
  const NumericalSample inputSample2(weightedExperiment.generate());
  UnsignedInteger dimension = inputSample1.getDimension();

  NumericalSample design(inputSample1);
  design.add(inputSample2);
  // Compute designs of type Saltelli/Martinez for 1st order
  for (UnsignedInteger p = 0; p < dimension; ++p)
  {
    NumericalSample x(inputSample1);
    for (UnsignedInteger k = 0; k < size; ++k) x[k][p] = inputSample2[k][p];
    design.add(x);
  }
  if (computeSecondOrder)
  {
    for (UnsignedInteger p = 0; p < dimension; ++p)
    {
      NumericalSample x(inputSample2);
      for (UnsignedInteger k = 0; k < size; ++k) x[k][p] = inputSample1[k][p];
      design.add(x);
    }
  }
  // Return collection
  return design;
}

/* First order indices accessor */
NumericalPoint SobolIndicesAlgorithmImplementation::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    // This method is defined in specific children classes
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getTotalOrderIndices, marginalIndex should be in [0," << outputDimension-1;
  // return value
  const NumericalPoint firstOrderSensitivity(varianceI_[marginalIndex] / referenceVariance_[marginalIndex]);
  for (UnsignedInteger p = 0; p < inputDesign_.getDimension(); ++p)
  {
    if ((firstOrderSensitivity[p] > 1.0) || firstOrderSensitivity[p] < 0.0)
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is not in the range [0, 1]. You may increase the sampling size. HERE we have: S_"
                    << p <<"=" <<  firstOrderSensitivity << ", ST_" << p << "=" << varianceTI_[marginalIndex][p] / referenceVariance_[marginalIndex]);
    // Another case : Si > STi
    if (varianceI_[marginalIndex][p] > varianceTI_[marginalIndex][p])
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is greater than its total order index . You may increase the sampling size. HERE we have: S_"
                    << p <<"=" <<  firstOrderSensitivity << ", ST_" << p << "=" << varianceTI_[marginalIndex][p] / referenceVariance_[marginalIndex]);
  }
  return firstOrderSensitivity;
}

struct BootstrapPolicy
{
  const SobolIndicesAlgorithmImplementation & sai_;
  const Indices & indices_;
  const UnsignedInteger size_;
  NumericalSample & bsFO_;
  NumericalSample & bsTO_;

  BootstrapPolicy( const SobolIndicesAlgorithmImplementation & sai,
                   const Indices & indices,
                   const UnsignedInteger size,
                   NumericalSample & bsFO,
                   NumericalSample & bsTO)
    : sai_(sai)
    , indices_(indices)
    , size_(size)
    , bsFO_(bsFO)
    , bsTO_(bsTO)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    Indices slice(size_);
    NumericalSample VTi;
    NumericalPoint mergedTotal;

    for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
    {
      // Extract indices
      memcpy(&slice[0], &indices_[k * size_], size_ * sizeof(UnsignedInteger));

      // Generate huge random sample using Bootstrap algorithm
      const NumericalSample randomCollection(sai_.getBootstrapDesign(slice));
      // Pseudo-Reference variance
      NumericalSample outReference(size_, sai_.outputDesign_.getDimension());
      for (UnsignedInteger i = 0; i < size_; ++i) outReference[i] = randomCollection[i];
      const NumericalPoint variance(outReference.computeVariance());
      // Compute indices using this collection
      const NumericalSample Vi(sai_.computeIndices(randomCollection, VTi));
      // Compute aggregated indices
      bsFO_[k] = sai_.computeAggregatedIndices(Vi, VTi, variance, mergedTotal);
      // Add to sample
      bsTO_[k] = mergedTotal;
    }
  }
}; /* end struct BootstrapPolicy */

/** void method that computes confidence interval */
void SobolIndicesAlgorithmImplementation::computeIndicesInterval() const
{
  if (0 != totalOrderIndiceInterval_.getDimension()) return;

  // Build interval using sample variance
  // Mean reference is the Sensitivity values
  const NumericalPoint aggregatedFirstOrder(getAggregatedFirstOrderIndices());
  const NumericalPoint aggregatedTotalOrder(getAggregatedTotalOrderIndices());
  // Compute confidence interval
  firstOrderIndiceInterval_ = Interval(aggregatedFirstOrder, aggregatedFirstOrder);
  totalOrderIndiceInterval_ = Interval(aggregatedTotalOrder, aggregatedTotalOrder);
  if (bootstrapSize_ > 0)
  {
    // Temporary samples that stores the first/total indices
    const UnsignedInteger inputDimension = inputDesign_.getDimension();
    NumericalSample bsFO(0, inputDimension);
    NumericalSample bsTO(0, inputDimension);
    const UnsignedInteger size = size_;
    // To have the exact same results with TBB, we have to precompute
    // RandomGenerator::IntegerGenerate calls and store results in a
    // variable.  This would require lots of memory when bootstrapSize_ is
    // large, thus let user specify block size.
    const UnsignedInteger blockSize = std::min(bootstrapSize_, ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBlockSize"));
    const UnsignedInteger maximumOuterSampling = bootstrapSize_ / blockSize;
    const UnsignedInteger modulo = bootstrapSize_ % blockSize;
    const UnsignedInteger lastBlockSize = modulo == 0 ? blockSize : modulo;

    for(UnsignedInteger outerSampling = 0; outerSampling < maximumOuterSampling; ++outerSampling)
    {
      // the last block can be smaller
      const UnsignedInteger effectiveBlockSize = (outerSampling + 1) < maximumOuterSampling ? blockSize : lastBlockSize;

      NumericalSample bsFOpartial(effectiveBlockSize, inputDimension);
      NumericalSample bsTOpartial(effectiveBlockSize, inputDimension);
      const RandomGenerator::UnsignedIntegerCollection randomIndices(RandomGenerator::IntegerGenerate(size * effectiveBlockSize, size));
      const Indices indices(randomIndices.begin(), randomIndices.end());
      const BootstrapPolicy policy( *this, indices, size, bsFOpartial, bsTOpartial );
      TBB::ParallelFor( 0, effectiveBlockSize, policy );

      bsFO.add(bsFOpartial);
      bsTO.add(bsTOpartial);
    }

    // Confidence interval elements
    // Sample of indices
    const NumericalScalar lowerQuantileLevel = 0.5 * (1.0 - confidenceLevel_);
    const NumericalScalar upperQuantileLevel = 0.5 * (1.0 + confidenceLevel_);
    // Compute empirical quantiles
    // 1) First order indices
    const NumericalPoint lowerQuantileFO(bsFO.computeQuantilePerComponent(lowerQuantileLevel));
    const NumericalPoint upperQuantileFO(bsFO.computeQuantilePerComponent(upperQuantileLevel));
    LOGINFO(OSS() << "First Order from Bootstrap sample:  lowerQuantile=" << lowerQuantileFO << ", upperQuantile=" << upperQuantileFO );
    // 2) Total order indices
    const NumericalPoint lowerQuantileTO(bsTO.computeQuantilePerComponent(lowerQuantileLevel));
    const NumericalPoint upperQuantileTO(bsTO.computeQuantilePerComponent(upperQuantileLevel));
    LOGINFO(OSS() << "Total Order from Bootstrap sample:  lowerQuantile=" << lowerQuantileTO << ", upperQuantile=" << upperQuantileTO );

    // First order interval
    const NumericalPoint firstOrderLowerBound(aggregatedFirstOrder * 2.0 - upperQuantileFO);
    const NumericalPoint firstOrderUpperBound(aggregatedFirstOrder * 2.0 - lowerQuantileFO);
    // Total order interval
    const NumericalPoint totalOrderLowerBound(aggregatedTotalOrder * 2.0 - upperQuantileTO);
    const NumericalPoint totalOrderUpperBound(aggregatedTotalOrder * 2.0 - lowerQuantileTO);

    // Compute confidence interval
    firstOrderIndiceInterval_ = Interval(firstOrderLowerBound, firstOrderUpperBound);
    totalOrderIndiceInterval_ = Interval(totalOrderLowerBound, totalOrderUpperBound);
    LOGINFO(OSS() << "First order interval Bootstrap with Basic method=" );
    LOGINFO(OSS() << "Total Order from Bootstrap sample:  lowerQuantile=" << lowerQuantileTO << ", upperQuantile=" << upperQuantileTO );
  }
}

/* Interval for the first order indices accessor */
Interval SobolIndicesAlgorithmImplementation::getFirstOrderIndicesInterval() const
{
  // Interval evaluation using Bootstrap
  computeIndicesInterval();
  return firstOrderIndiceInterval_;
}

/* Second order indices accessor */
SymmetricMatrix SobolIndicesAlgorithmImplementation::getSecondOrderIndices(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  if (0 == secondOrderIndices_.getNbSheets())
  {
    const UnsignedInteger inputDimension = inputDesign_.getDimension();
    // Check if is possible
    if (outputDesign_.getSize() < 2 * (inputDesign_.getDimension() + 1) * size_ )
      throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getSecondOrderIndices, second order indices designs not computed";
    // Compute second order indices
    secondOrderIndices_ = SymmetricTensor(inputDimension, outputDimension);
    // Compute cross square mean between samples yA and yB, which are located respectively at index 0 and size_
    NumericalPoint crossSquareMean(computeSumDotSamples(outputDesign_, size_, 0, size_) / size_);

    // Main loop
    for (UnsignedInteger k1 = 0; k1 < inputDimension; ++k1)
    {
      for (UnsignedInteger k2 = 0; k2 < k1; ++k2)
      {
        // Compute yEDotyC
        //const NumericalPoint yEDotyC(computeSumDotSamples(yE, yC));
        const NumericalPoint yEDotyC(computeSumDotSamples(outputDesign_, size_, (2 + k1) * size_, (2 + k2 + inputDimension) * size_));
        for (UnsignedInteger q = 0; q < outputDimension; ++q)
        {
          // Sij = (Vij - crossMean)/var - and S_{i}, S_{j}
          const NumericalPoint firstOrderIndices(getFirstOrderIndices(q));
          secondOrderIndices_(k1, k2, q) = (yEDotyC[q] / (size_ - 1.0) - crossSquareMean[q]) / referenceVariance_[q] - firstOrderIndices[k1] - firstOrderIndices[k2] ;
          if ((secondOrderIndices_(k1, k2, q) < 0.0) || (secondOrderIndices_(k1, k2, q) > 1.0))
            LOGWARN(OSS() << "The estimated second order Sobol index (" << k1 << ", " << k2 << ") is not in the range [0, 1]. You may increase the sampling size.");
        }
      }
    }
  }
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getSecondOrderIndices, marginalIndex should be in [0," << outputDimension-1 << "]";
  return secondOrderIndices_.getSheet(marginalIndex);
}

/* Total order indices accessor */
NumericalPoint SobolIndicesAlgorithmImplementation::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    // This method is defined in specific children classes
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getTotalOrderIndices, marginalIndex should be in [0," << outputDimension-1;
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // Another case : Si > STi
    if (varianceI_[marginalIndex][p] > varianceTI_[marginalIndex][p])
      LOGWARN(OSS() << "The estimated total order Sobol index (" << p << ") is lesser than first order index . You may increase the sampling size. HERE we have: S_"
                    << p <<"=" <<  varianceI_[marginalIndex][p] / referenceVariance_[marginalIndex] << ", ST_" << p << "=" << varianceTI_[marginalIndex][p] / referenceVariance_[marginalIndex]);
  }
  // return value
  return varianceTI_[marginalIndex] / referenceVariance_[marginalIndex] ;
}

/* Interval for the total order indices accessor */
Interval SobolIndicesAlgorithmImplementation::getTotalOrderIndicesInterval() const
{
  // Interval evaluation using Bootstrap
  computeIndicesInterval();
  return totalOrderIndiceInterval_;
}

/* Aggregated first order indices accessor for multivariate samples */
NumericalPoint SobolIndicesAlgorithmImplementation::getAggregatedFirstOrderIndices() const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  if (0 == mergedFirstOrderIndices_.getDimension())
  {
    // Compute aggregate indices
    mergedFirstOrderIndices_ = computeAggregatedIndices(varianceI_, varianceTI_, referenceVariance_, mergedTotalOrderIndices_);
  }
  // Indices computed
  return mergedFirstOrderIndices_;
}

/* Aggregated total order indices accessor for multivariate samples */
NumericalPoint SobolIndicesAlgorithmImplementation::getAggregatedTotalOrderIndices() const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  if (0 == mergedFirstOrderIndices_.getDimension())
  {
    // Compute aggregate indices
    mergedFirstOrderIndices_ = computeAggregatedIndices(varianceI_, varianceTI_, referenceVariance_, mergedTotalOrderIndices_);
  }
  // Indices computed
  return mergedTotalOrderIndices_;
}

// Getter for bootstrap size
UnsignedInteger SobolIndicesAlgorithmImplementation::getBootstrapSize() const
{
  return bootstrapSize_;
}

// Setter for bootstrap size
void SobolIndicesAlgorithmImplementation::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  if (bootstrapSize <= 0.0)
    throw InvalidArgumentException(HERE) << "Bootstrap sampling size should be positive. Here, bootstrapSize=" << bootstrapSize;
  bootstrapSize_ = bootstrapSize;
  firstOrderIndiceInterval_ = Interval();
  totalOrderIndiceInterval_ = Interval();
}

// Getter for bootstrap confidence level
NumericalScalar SobolIndicesAlgorithmImplementation::getBootstrapConfidenceLevel() const
{
  return confidenceLevel_;
}

// Setter for bootstrap confidence level
void SobolIndicesAlgorithmImplementation::setBootstrapConfidenceLevel(const NumericalScalar confidenceLevel)
{
  if ((confidenceLevel < 0.0) || (confidenceLevel >= 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level value should be in ]0,1[. Here, confidence level=" << confidenceLevel;
  confidenceLevel_ = confidenceLevel;
  firstOrderIndiceInterval_ = Interval();
  totalOrderIndiceInterval_ = Interval();
}

/* String converter */
String SobolIndicesAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

// Multiplication and sum of two NumericalSamples
// TODO Write method in NumericalSample ?
NumericalPoint SobolIndicesAlgorithmImplementation::computeSumDotSamples(const NumericalSample & x,
                                                                         const NumericalSample & y) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();

  const NumericalScalar * xptr(&x[0][0]);
  const NumericalScalar * yptr(&y[0][0]);


  NumericalPoint value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

// Multiplication and sum of two NumericalSamples, contained in the sampe sample
NumericalPoint SobolIndicesAlgorithmImplementation::computeSumDotSamples(const NumericalSample & sample,
                                                                         const UnsignedInteger size,
                                                                         const UnsignedInteger indexX,
                                                                         const UnsignedInteger indexY) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = sample.getDimension();

  const NumericalScalar * xptr(&sample[indexX][0]);
  const NumericalScalar * yptr(&sample[indexY][0]);


  NumericalPoint value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

/* String converter */
String SobolIndicesAlgorithmImplementation::__str__(const String & offset) const
{
  return __repr__();
}

/** Internal method that compute Vi/VTi using a collection of samples */
NumericalSample SobolIndicesAlgorithmImplementation::computeIndices(const NumericalSample & design,
                                                                    NumericalSample & VTi) const
{
  // Method is defined in Jansan/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}

/** Method that draw (plot) the sensitivity graph */
Graph SobolIndicesAlgorithmImplementation::draw() const
{
  Graph graph(OSS() << " Aggregated sensitivity indices - " << getClassName(), "inputs", "Sensitivity indices ", true, "");
  // Define cloud for first order and total order indices
  const NumericalPoint aggregatedFO(getAggregatedFirstOrderIndices());
  const NumericalPoint aggregatedTO(getAggregatedTotalOrderIndices());
  NumericalSample data(aggregatedFO.getDimension(), 2);
  for (UnsignedInteger k = 0; k < aggregatedFO.getDimension(); ++k)
  {
    data[k][0] = k + 1;
    data[k][1] = aggregatedFO[k];
  }
  // Define cloud for FO
  Cloud firstOrderIndicesGraph(data, "red", "circle", "Aggregated FO");
  graph.add(firstOrderIndicesGraph);
  // Total order
  for (UnsignedInteger k = 0; k < aggregatedFO.getDimension(); ++k)
  {
    data[k][0] = (k + 1) + 0.1;
    data[k][1] = aggregatedTO[k];
  }
  // Define cloud for TO
  Cloud totalOrderIndicesGraph(data, "blue", "square", "Aggregated TO");
  graph.add(totalOrderIndicesGraph);
  // Set bounding box
  NumericalPoint boundingBox(4, -0.1);
  boundingBox[1] = aggregatedFO.getDimension() + 1;
  boundingBox[3] = 1.1 ;
  if (bootstrapSize_ > 0 && confidenceLevel_ > 0.0)
  {
    // Add plot of intervals
    const Interval foInterval(getFirstOrderIndicesInterval());
    const Interval toInterval(getTotalOrderIndicesInterval());
    // transform data
    data = NumericalSample(2, 2);
    for (UnsignedInteger k = 0; k < aggregatedFO.getDimension(); ++k)
    {
      // Relative to FirstOrder
      data[0][0] = (k + 1);
      data[0][1] = foInterval.getLowerBound()[k];
      data[1][0] = (k + 1);
      data[1][1] = foInterval.getUpperBound()[k];
      graph.add(Curve(data, "red", "solid", 2, ""));

      // Relative to TotalOrder
      data[0][0] = (k + 1) + 0.1;
      data[0][1] = toInterval.getLowerBound()[k];
      data[1][0] = (k + 1) + 0.1;
      data[1][1] = toInterval.getUpperBound()[k];
      graph.add(Curve(data, "blue", "solid", 2, ""));
    }
  }
  graph.setBoundingBox(boundingBox);
  graph.setLegendPosition("topright");
  return graph;
}

/** Method that draw the sensitivity graph of a fixed marginal */
Graph SobolIndicesAlgorithmImplementation::draw(UnsignedInteger marginalIndex) const
{
  Graph graph(OSS() << " Sensitivity indices - " << getClassName(), "inputs", "Sensitivity indices ", true, "");
  // Define cloud for first order and total order indices
  const NumericalPoint foIndices(getFirstOrderIndices(marginalIndex));
  const NumericalPoint toIndices(getTotalOrderIndices(marginalIndex));
  NumericalSample data(foIndices.getDimension(), 2);
  for (UnsignedInteger k = 0; k < foIndices.getDimension(); ++k)
  {
    data[k][0] = k + 1;
    data[k][1] = foIndices[k];
  }
  // Define cloud for FO
  Cloud firstOrderIndicesGraph(data, "red", "circle", "Aggregated FO");
  graph.add(firstOrderIndicesGraph);
  // Total order
  for (UnsignedInteger k = 0; k < foIndices.getDimension(); ++k)
  {
    data[k][0] = (k + 1) + 0.1;
    data[k][1] = toIndices[k];
  }
  // Define cloud for TO
  Cloud totalOrderIndicesGraph(data, "blue", "square", "Aggregated TO");
  graph.add(totalOrderIndicesGraph);
  // Set bounding box
  NumericalPoint boundingBox(4, -0.1);
  boundingBox[1] = foIndices.getDimension() + 1;
  boundingBox[3] = 1.1 ;
  graph.setBoundingBox(boundingBox);
  graph.setLegendPosition("topright");
  return graph;
}

/** Internal method that returns a bootstrap NSC */
NumericalSample SobolIndicesAlgorithmImplementation::getBootstrapDesign(const Indices & indices) const
{
  // Bootstrap with huge sample that contains several samples
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  NumericalSample bootstrapDesign(0, outputDimension);
  for (UnsignedInteger p = 0; p < 2 + inputDimension; ++p)
  {
    NumericalSample y(size_, outputDimension);
    NumericalScalar* yPermData = &y[0][0];

    const NumericalScalar* yData = &outputDesign_[p * size_][0];
    for (UnsignedInteger k = 0; k < size_; ++k, yPermData+=outputDimension)
      memcpy(yPermData, &yData[indices[k] * outputDimension], outputDimension * sizeof(NumericalScalar));
    // add samples to the collection
    bootstrapDesign.add(y);
  }
  return bootstrapDesign;
}

/** Function that computes merged indices using Vi/VTi + variance  */
NumericalPoint SobolIndicesAlgorithmImplementation::computeAggregatedIndices(const NumericalSample & Vi,
                                                                             const NumericalSample & VTi,
                                                                             const NumericalPoint & variance,
                                                                             NumericalPoint & mergedTotal) const
{
  // Generic implementation
  const UnsignedInteger inputDimension = Vi.getDimension();
  const UnsignedInteger outputDimension = Vi.getSize();
  if (inputDimension == 1)
  {
    mergedTotal = NumericalPoint(VTi[0]);
    return NumericalPoint(Vi[0]);
  }

  // Compute sum of Var(Y^k)
  NumericalScalar sumVariance = variance.norm1();

  // Compute merged indices
  mergedTotal = VTi.computeMean() * (outputDimension / sumVariance);
  return NumericalPoint(Vi.computeMean() * (outputDimension / sumVariance));
}

/* ImportanceFactors graph */
Graph SobolIndicesAlgorithmImplementation::DrawImportanceFactors(const NumericalPointWithDescription & importanceFactors,
                                                                 const String & title)
{
  return DrawImportanceFactors(importanceFactors, importanceFactors.getDescription(), title);
}

/* ImportanceFactors graph */
Graph SobolIndicesAlgorithmImplementation::DrawImportanceFactors(const NumericalPoint & values,
                                                                 const Description & names,
                                                                 const String & title)
{
  /* build data for the pie */
  const UnsignedInteger dimension = values.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on empty data.";
  if ((names.getSize() != 0) && (names.getSize() != dimension)) throw InvalidArgumentException(HERE) << "Error: the names size must match the value dimension.";
  NumericalScalar l1Norm = 0.0;
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


/* Method save() stores the object through the StorageManager */
void SobolIndicesAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputDesign_", inputDesign_ );
  adv.saveAttribute( "outputDesign_", outputDesign_ );
  adv.saveAttribute( "size_", size_ );
  adv.saveAttribute( "bootstrapSize_", bootstrapSize_ );
  adv.saveAttribute( "confidenceLevel_", confidenceLevel_ );
  adv.saveAttribute( "referenceVariance_",  referenceVariance_);
  adv.saveAttribute( "varianceI_", varianceI_);
  adv.saveAttribute( "varianceTI_", varianceTI_);
  adv.saveAttribute( "mergedFirstOrderIndices_", mergedFirstOrderIndices_);
  adv.saveAttribute( "mergedTotalOrderIndices_", mergedTotalOrderIndices_);
  adv.saveAttribute( "secondOrderIndices_", secondOrderIndices_);
  adv.saveAttribute( "firstOrderIndiceInterval_", firstOrderIndiceInterval_);
  adv.saveAttribute( "totalOrderIndiceInterval_", totalOrderIndiceInterval_);
}

/* Method load() reloads the object from the StorageManager */
void SobolIndicesAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputDesign_", inputDesign_ );
  adv.loadAttribute( "outputDesign_", outputDesign_ );
  adv.loadAttribute( "size_", size_ );
  adv.loadAttribute( "bootstrapSize_", bootstrapSize_ );
  adv.loadAttribute( "confidenceLevel_", confidenceLevel_ );
  adv.loadAttribute( "referenceVariance_",  referenceVariance_);
  adv.loadAttribute( "varianceI_", varianceI_);
  adv.loadAttribute( "varianceTI_", varianceTI_);
  adv.loadAttribute( "mergedFirstOrderIndices_", mergedFirstOrderIndices_);
  adv.loadAttribute( "mergedTotalOrderIndices_", mergedTotalOrderIndices_);
  adv.loadAttribute( "secondOrderIndices_", secondOrderIndices_);
  adv.loadAttribute( "firstOrderIndiceInterval_", firstOrderIndiceInterval_);
  adv.loadAttribute( "totalOrderIndiceInterval_", totalOrderIndiceInterval_);
}

END_NAMESPACE_OPENTURNS
