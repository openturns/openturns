//                                               -*- C++ -*-
/**
 *  @brief Implementation for sensitivity algorithms
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/TBB.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Pie.hxx"
#include "openturns/Text.hxx"
#include "openturns/SobolIndicesExperiment.hxx"
#include "openturns/Normal.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Dirac.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesAlgorithmImplementation)

static const Factory<SobolIndicesAlgorithmImplementation> Factory_SobolIndicesAlgorithmImplementation;

/* Default constructor */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation()
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(0)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , aggregatedFirstOrderIndices_()
  , aggregatedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceDistribution_()
  , totalOrderIndiceDistribution_()
  , alreadyComputedIndicesDistribution_(false)
  , useAsymptoticDistribution_(ResourceMap::GetAsBool("SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution"))
{
  // Nothing to do
}

/** Constructor with parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(0)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , aggregatedFirstOrderIndices_()
  , aggregatedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceDistribution_()
  , totalOrderIndiceDistribution_()
  , alreadyComputedIndicesDistribution_(false)
  , useAsymptoticDistribution_(ResourceMap::GetAsBool("SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution"))
{
  setDesign(inputDesign, outputDesign, size);
}

/* Virtual constructor */
SobolIndicesAlgorithmImplementation * SobolIndicesAlgorithmImplementation::clone() const
{
  return new SobolIndicesAlgorithmImplementation(*this);
}

/** Constructor with distribution / model parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const Distribution & distribution,
    const UnsignedInteger size,
    const Function & model,
    const Bool computeSecondOrder)
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(0)
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , aggregatedFirstOrderIndices_()
  , aggregatedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceDistribution_()
  , totalOrderIndiceDistribution_()
  , alreadyComputedIndicesDistribution_(false)
  , useAsymptoticDistribution_(ResourceMap::GetAsBool("SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution"))
{
  const UnsignedInteger inputDimension = model.getInputDimension();
  if (inputDimension != distribution.getDimension())
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, incompatible dimension between model and distribution. distribution dimension=" << distribution.getDimension()
                                         << ", model input dimension = " << inputDimension;
  const SobolIndicesExperiment sobolExperiment(distribution, size, computeSecondOrder);
  // Here we check that we can use the asymptotic distributions
  const String experimentKind(sobolExperiment.getWeightedExperiment().getImplementation()->getClassName());
  if (useAsymptoticDistribution_ && (experimentKind != "MonteCarloExperiment"))
  {
    LOGWARN(OSS() << "Cannot use the asymptotic distribution of Sobol indices with non IID outer sampling, here sampling is " << sobolExperiment.getWeightedExperiment().getClassName());
    useAsymptoticDistribution_ = false;
  }
  const Sample inputDesign(sobolExperiment.generate());
  Sample outputDesign(model(inputDesign));

  setDesign(inputDesign, outputDesign, size);
}

/** Constructor with distribution / model parameters */
SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation(const WeightedExperiment & experiment,
    const Function & model,
    const Bool computeSecondOrder)
  : PersistentObject()
  , inputDesign_()
  , outputDesign_()
  , size_(experiment.getSize())
  , bootstrapSize_(ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize"))
  , confidenceLevel_(ResourceMap::GetAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel"))
  , referenceVariance_()
  , varianceI_()
  , varianceTI_()
  , aggregatedFirstOrderIndices_()
  , aggregatedTotalOrderIndices_()
  , secondOrderIndices_()
  , firstOrderIndiceDistribution_()
  , totalOrderIndiceDistribution_()
  , alreadyComputedIndicesDistribution_(false)
  , useAsymptoticDistribution_(ResourceMap::GetAsBool("SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution"))
{
  const UnsignedInteger inputDimension = model.getInputDimension();
  if (inputDimension != experiment.getDistribution().getDimension())
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::SobolIndicesAlgorithmImplementation, incompatible dimension between model and distribution. Experiment dimension=" << experiment.getDistribution().getDimension()
                                         << ", model input dimension = " << inputDimension;
  // Here we check that we can use the asymptotic distributions
  const String experimentKind(experiment.getImplementation()->getClassName());
  if (useAsymptoticDistribution_ && (experimentKind != "MonteCarloExperiment"))
  {
    LOGWARN(OSS() << "Cannot use the asymptotic distribution of Sobol indices with non IID sampling, here sampling is " << experiment.getClassName());
    useAsymptoticDistribution_ = false;
  }
  const SobolIndicesExperiment sobolExperiment(experiment, computeSecondOrder);
  const Sample inputDesign(sobolExperiment.generate());
  const Sample outputDesign(model(inputDesign));

  setDesign(inputDesign, outputDesign, experiment.getSize());
}


/* First order indices accessor */
Point SobolIndicesAlgorithmImplementation::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    // This method is defined in specific children classes
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getTotalOrderIndices, marginalIndex should be in [0," << outputDimension - 1;
  // return value
  const Point firstOrderSensitivity(varianceI_[marginalIndex] / referenceVariance_[marginalIndex]);
  for (UnsignedInteger p = 0; p < inputDesign_.getDimension(); ++p)
  {
    if ((firstOrderSensitivity[p] > 1.0) || firstOrderSensitivity[p] < 0.0)
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is not in the range [0, 1]. You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderSensitivity << ", ST_" << p << "=" << varianceTI_(marginalIndex, p) / referenceVariance_[marginalIndex]);
    // Another case : Si > STi
    if (varianceI_(marginalIndex, p) > varianceTI_(marginalIndex, p))
      LOGWARN(OSS() << "The estimated first order Sobol index (" << p << ") is greater than its total order index . You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  firstOrderSensitivity << ", ST_" << p << "=" << varianceTI_(marginalIndex, p) / referenceVariance_[marginalIndex]);
  }
  return firstOrderSensitivity;
}

struct BootstrapPolicy
{
  const SobolIndicesAlgorithmImplementation & sai_;
  const Indices & indices_;
  const UnsignedInteger size_;
  Sample & bsFO_;
  Sample & bsTO_;

  BootstrapPolicy( const SobolIndicesAlgorithmImplementation & sai,
                   const Indices & indices,
                   const UnsignedInteger size,
                   Sample & bsFO,
                   Sample & bsTO)
    : sai_(sai)
    , indices_(indices)
    , size_(size)
    , bsFO_(bsFO)
    , bsTO_(bsTO)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    Indices slice(size_);
    Sample VTi;
    Point aggregatedTotal;

    for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
    {
      // Extract indices
      memcpy(&slice[0], &indices_[k * size_], size_ * sizeof(UnsignedInteger));

      // Generate huge random sample using Bootstrap algorithm
      const Sample randomCollection(sai_.getBootstrapDesign(slice));
      // Pseudo-Reference variance
      Sample outReference(size_, sai_.outputDesign_.getDimension());
      for (UnsignedInteger i = 0; i < size_; ++i) outReference[i] = randomCollection[i];
      const Point variance(outReference.computeVariance());
      // Compute indices using this collection
      const Sample Vi(sai_.computeIndices(randomCollection, VTi));
      // Compute aggregated indices
      bsFO_[k] = sai_.computeAggregatedIndices(Vi, VTi, variance, aggregatedTotal);
      // Add to sample
      bsTO_[k] = aggregatedTotal;
    }
  }
}; /* end struct BootstrapPolicy */

/** void method that computes confidence interval */
void SobolIndicesAlgorithmImplementation::computeBootstrapDistribution() const
{
  // Build interval using sample variance
  // Mean reference is the Sensitivity values
  const Point aggregatedFirstOrder(getAggregatedFirstOrderIndices());
  const Point aggregatedTotalOrder(getAggregatedTotalOrderIndices());
  if (bootstrapSize_ > 0)
  {
    // Temporary samples that stores the first/total indices
    const UnsignedInteger inputDimension = inputDesign_.getDimension();
    Sample bsFO(0, inputDimension);
    Sample bsTO(0, inputDimension);
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

      Sample bsFOpartial(effectiveBlockSize, inputDimension);
      Sample bsTOpartial(effectiveBlockSize, inputDimension);
      const RandomGenerator::UnsignedIntegerCollection randomIndices(RandomGenerator::IntegerGenerate(size * effectiveBlockSize, size));
      const Indices indices(randomIndices.begin(), randomIndices.end());
      const BootstrapPolicy policy( *this, indices, size, bsFOpartial, bsTOpartial );
      TBB::ParallelFor( 0, effectiveBlockSize, policy );

      bsFO.add(bsFOpartial);
      bsTO.add(bsTOpartial);
    }

    KernelSmoothing factory;
    firstOrderIndiceDistribution_ = factory.build(bsFO);
    totalOrderIndiceDistribution_ = factory.build(bsTO);
  }
}


void SobolIndicesAlgorithmImplementation::computeAsymptoticDistribution() const
{
  throw NotYetImplementedException(HERE) << "SobolIndicesAlgorithmImplementation::computeAsymptoticInterval";
}


Scalar SobolIndicesAlgorithmImplementation::computeVariance(const Sample & u, const Function & psi) const
{
  Point gradient(*psi.gradient(u.computeMean()).getImplementation());
  return dot(gradient, u.computeCovariance() * gradient) / size_;
}

void SobolIndicesAlgorithmImplementation::setConfidenceInterval(const Point & varianceFO,
                                                                const Point & varianceTO) const
{
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  Point standardDeviationFO(inputDimension);
  Point standardDeviationTO(inputDimension);
  ComposedDistribution::DistributionCollection marginalsFO(inputDimension);
  ComposedDistribution::DistributionCollection marginalsTO(inputDimension);
  const Point aggregatedFO(getAggregatedFirstOrderIndices());
  const Point aggregatedTO(getAggregatedTotalOrderIndices());
  Bool allNormalFO = true;
  Bool allNormalTO = true;
  for (UnsignedInteger p = 0; p < inputDimension; ++ p)
  {
    standardDeviationFO[p] = sqrt(varianceFO[p]);
    if (standardDeviationFO[p] > 0.0)
      marginalsFO[p] = Normal(aggregatedFO[p], standardDeviationFO[p]);
    else
    {
      allNormalFO = false;
      marginalsFO[p] = Dirac(aggregatedFO[p]);
    }

    standardDeviationTO[p] = sqrt(varianceTO[p]);
    if (standardDeviationTO[p] > 0.0)
      marginalsTO[p] = Normal(aggregatedTO[p], standardDeviationTO[p]);
    else
    {
      allNormalTO = false;
      marginalsTO[p] = Dirac(aggregatedTO[p]);
    }
  }

  if (allNormalFO)
    firstOrderIndiceDistribution_ = Normal(aggregatedFO, standardDeviationFO, CorrelationMatrix(inputDimension));
  else
    firstOrderIndiceDistribution_ = ComposedDistribution(marginalsFO);

  if (allNormalTO)
    totalOrderIndiceDistribution_ = Normal(aggregatedTO, standardDeviationTO, CorrelationMatrix(inputDimension));
  else
    totalOrderIndiceDistribution_ = ComposedDistribution(marginalsTO);
}


/* Interval for the first order indices accessor */
Interval SobolIndicesAlgorithmImplementation::getFirstOrderIndicesInterval() const
{
  const Distribution distribution = getFirstOrderIndicesDistribution();
  const UnsignedInteger inputDimension = distribution.getDimension();
  Point lowerBound(inputDimension);
  Point upperBound(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    Distribution marginal(distribution.getMarginal(j));
    lowerBound[j] = marginal.computeQuantile(0.5 * (1.0 - confidenceLevel_))[0];
    upperBound[j] = marginal.computeQuantile(0.5 * (1.0 + confidenceLevel_))[0];
  }
  return Interval(lowerBound, upperBound);
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
    Point crossSquareMean(computeSumDotSamples(outputDesign_, size_, 0, size_) / size_);

    // Main loop
    for (UnsignedInteger k1 = 0; k1 < inputDimension; ++k1)
    {
      for (UnsignedInteger k2 = 0; k2 < k1; ++k2)
      {
        // Compute yEDotyC
        //const Point yEDotyC(computeSumDotSamples(yE, yC));
        const Point yEDotyC(computeSumDotSamples(outputDesign_, size_, (2 + k1) * size_, (2 + k2 + inputDimension) * size_));
        for (UnsignedInteger q = 0; q < outputDimension; ++q)
        {
          // Sij = (Vij - crossMean)/var - and S_{i}, S_{j}
          const Point firstOrderIndices(getFirstOrderIndices(q));
          secondOrderIndices_(k1, k2, q) = (yEDotyC[q] / (size_ - 1.0) - crossSquareMean[q]) / referenceVariance_[q] - firstOrderIndices[k1] - firstOrderIndices[k2] ;
          if ((secondOrderIndices_(k1, k2, q) < 0.0) || (secondOrderIndices_(k1, k2, q) > 1.0))
            LOGWARN(OSS() << "The estimated second order Sobol index (" << k1 << ", " << k2 << ") is not in the range [0, 1]. You may increase the sampling size.");
        }
      }
    }
  }
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getSecondOrderIndices, marginalIndex should be in [0," << outputDimension - 1 << "]";
  return secondOrderIndices_.getSheet(marginalIndex);
}

/* Total order indices accessor */
Point SobolIndicesAlgorithmImplementation::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
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
    throw InvalidArgumentException(HERE) << "In SobolIndicesAlgorithmImplementation::getTotalOrderIndices, marginalIndex should be in [0," << outputDimension - 1;
  for (UnsignedInteger p = 0; p < inputDimension; ++p)
  {
    // Another case : Si > STi
    if (varianceI_(marginalIndex, p) > varianceTI_(marginalIndex, p))
      LOGWARN(OSS() << "The estimated total order Sobol index (" << p << ") is lesser than first order index . You may increase the sampling size. HERE we have: S_"
              << p << "=" <<  varianceI_(marginalIndex, p) / referenceVariance_[marginalIndex] << ", ST_" << p << "=" << varianceTI_(marginalIndex, p) / referenceVariance_[marginalIndex]);
  }
  // return value
  return varianceTI_[marginalIndex] / referenceVariance_[marginalIndex] ;
}

/* Interval for the total order indices accessor */
Interval SobolIndicesAlgorithmImplementation::getTotalOrderIndicesInterval() const
{
  const Distribution distribution = getTotalOrderIndicesDistribution();
  const UnsignedInteger inputDimension = distribution.getDimension();
  Point lowerBound(inputDimension);
  Point upperBound(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    Distribution marginal(distribution.getMarginal(j));
    lowerBound[j] = marginal.computeQuantile(0.5 * (1.0 - confidenceLevel_))[0];
    upperBound[j] = marginal.computeQuantile(0.5 * (1.0 + confidenceLevel_))[0];
  }
  return Interval(lowerBound, upperBound);
}

void SobolIndicesAlgorithmImplementation::computeIndicesDistribution() const
{
  // if not already computed
  if (!alreadyComputedIndicesDistribution_)
  {
    if (useAsymptoticDistribution_)
    {
      computeAsymptoticDistribution();
    }
    else
    {
      computeBootstrapDistribution();
    }
    alreadyComputedIndicesDistribution_ = true;
  }
}


Distribution SobolIndicesAlgorithmImplementation::getFirstOrderIndicesDistribution() const
{
  computeIndicesDistribution();
  return firstOrderIndiceDistribution_;
}

Distribution SobolIndicesAlgorithmImplementation::getTotalOrderIndicesDistribution() const
{
  computeIndicesDistribution();
  return totalOrderIndiceDistribution_;
}

/* Aggregated first order indices accessor for multivariate samples */
Point SobolIndicesAlgorithmImplementation::getAggregatedFirstOrderIndices() const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  if (0 == aggregatedFirstOrderIndices_.getDimension())
  {
    // Compute aggregate indices
    aggregatedFirstOrderIndices_ = computeAggregatedIndices(varianceI_, varianceTI_, referenceVariance_, aggregatedTotalOrderIndices_);
  }
  // Indices computed
  return aggregatedFirstOrderIndices_;
}

/* Aggregated total order indices accessor for multivariate samples */
Point SobolIndicesAlgorithmImplementation::getAggregatedTotalOrderIndices() const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute first/ total order indices indices
    varianceI_ = computeIndices(outputDesign_, varianceTI_);
  }
  if (0 == aggregatedFirstOrderIndices_.getDimension())
  {
    // Compute aggregate indices
    aggregatedFirstOrderIndices_ = computeAggregatedIndices(varianceI_, varianceTI_, referenceVariance_, aggregatedTotalOrderIndices_);
  }
  // Indices computed
  return aggregatedTotalOrderIndices_;
}

// Getter for bootstrap size
UnsignedInteger SobolIndicesAlgorithmImplementation::getBootstrapSize() const
{
  return bootstrapSize_;
}

// Setter for bootstrap size
void SobolIndicesAlgorithmImplementation::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  if (bootstrapSize <= 0)
    throw InvalidArgumentException(HERE) << "Bootstrap sampling size should be positive. Here, bootstrapSize=" << bootstrapSize;
  bootstrapSize_ = bootstrapSize;
}

// Getter for bootstrap confidence level
Scalar SobolIndicesAlgorithmImplementation::getConfidenceLevel() const
{
  return confidenceLevel_;
}

// Setter for bootstrap confidence level
void SobolIndicesAlgorithmImplementation::setConfidenceLevel(const Scalar confidenceLevel)
{
  if ((confidenceLevel < 0.0) || (confidenceLevel >= 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level value should be in ]0,1[. Here, confidence level=" << confidenceLevel;
  confidenceLevel_ = confidenceLevel;
}

/* String converter */
String SobolIndicesAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

// Multiplication and sum of two Samples
// TODO Write method in Sample ?
Point SobolIndicesAlgorithmImplementation::computeSumDotSamples(const Sample & x,
    const Sample & y) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = x.getDimension();
  const UnsignedInteger size = x.getSize();

  const Scalar * xptr(&x(0, 0));
  const Scalar * yptr(&y(0, 0));


  Point value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

// Multiplication and sum of two Samples, contained in the sampe sample
Point SobolIndicesAlgorithmImplementation::computeSumDotSamples(const Sample & sample,
    const UnsignedInteger size,
    const UnsignedInteger indexX,
    const UnsignedInteger indexY) const
{
  // Internal method
  // Suppose that samples have the same size, same dimension
  const UnsignedInteger dimension = sample.getDimension();

  const Scalar * xptr(&sample(indexX, 0));
  const Scalar * yptr(&sample(indexY, 0));


  Point value(dimension, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j, ++xptr, ++yptr)
      value[j] += (*xptr) * (*yptr);
  return value;
}

/* Multiplication of two sub-samples */
Sample SobolIndicesAlgorithmImplementation::ComputeProdSample(const Sample & sample,
    const UnsignedInteger marginalIndex,
    const UnsignedInteger size,
    const UnsignedInteger indexX,
    const UnsignedInteger indexY)
{
  Sample prod(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
    prod(i, 0) = sample(i + indexX, marginalIndex) * sample(i + indexY, marginalIndex);
  return prod;
}

/* String converter */
String SobolIndicesAlgorithmImplementation::__str__(const String & ) const
{
  return __repr__();
}

/** Internal method that compute Vi/VTi using a collection of samples */
Sample SobolIndicesAlgorithmImplementation::computeIndices(const Sample &,
    Sample & ) const
{
  // Method is defined in Jansen/Saltelli/Martinez/Mauntz classes
  throw new NotYetImplementedException(HERE);
}


Graph SobolIndicesAlgorithmImplementation::DrawSobolIndices(const Description & inputDescription,
    const Point & firstOrderIndices,
    const Point & totalOrderIndices)
{
  Graph graph("Sobol' indices", "inputs", "index value", true, "");

  const UnsignedInteger dimension = firstOrderIndices.getDimension();

  // Define cloud for FO
  Sample data(dimension, 2);
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = k + 1.0;
    data(k, 1) = firstOrderIndices[k];
  }
  Cloud firstOrderIndicesGraph(data, "red", "circle", "First order");
  graph.add(firstOrderIndicesGraph);

  // Define cloud for TO
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = (k + 1.0) + dimension / 40.0;
    data(k, 1) = totalOrderIndices[k];
  }
  Cloud totalOrderIndicesGraph(data, "blue", "square", "Total order");
  graph.add(totalOrderIndicesGraph);

  // Description
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = (k + 1.0) + dimension / 20.0;
    data(k, 1) = 0.5 * (totalOrderIndices[k] + firstOrderIndices[k]);
  }
  Text text(data, inputDescription, "right");
  text.setColor("black");
  graph.add(text);

  // Set bounding box
  Point lowerBound(2, -0.1);
  lowerBound[0] = 1.0 - dimension / 10.0;
  Point upperBound(2, 1.1);
  const Scalar descriptionMargin = 1.6 * (dimension - 1.0) / (dimension + 2.0);
  upperBound[0] = dimension + descriptionMargin;
  graph.setBoundingBox(Interval(lowerBound, upperBound));

  graph.setLegendPosition("topright");
  return graph;
}

/** Method that draw (plot) the sensitivity graph */
Graph SobolIndicesAlgorithmImplementation::draw() const
{
  Graph graph(DrawSobolIndices(inputDesign_.getDescription(), getAggregatedFirstOrderIndices(), getAggregatedTotalOrderIndices()));
  if (outputDesign_.getDimension() > 1)
    graph.setTitle(OSS() << "Aggregated Sobol' indices - " << getClassName());
  else
    graph.setTitle(OSS() << "Sobol' indices - " << getClassName());
  const UnsignedInteger dimension = aggregatedFirstOrderIndices_.getDimension();

  // Draw confidence intervals
  if (confidenceLevel_ > 0.0)
  {
    const Interval foInterval(getFirstOrderIndicesInterval());
    const Interval toInterval(getTotalOrderIndicesInterval());
    // transform data
    Sample data(2, 2);
    for (UnsignedInteger k = 0; k < dimension; ++k)
    {
      // Relative to FirstOrder
      data(0, 0) = (k + 1);
      data(0, 1) = foInterval.getLowerBound()[k];
      data(1, 0) = (k + 1);
      data(1, 1) = foInterval.getUpperBound()[k];
      graph.add(Curve(data, "red", "solid", 2, ""));

      // Relative to TotalOrder
      data(0, 0) = (k + 1) + dimension / 40.0;
      data(0, 1) = toInterval.getLowerBound()[k];
      data(1, 0) = (k + 1) + dimension / 40.0;
      data(1, 1) = toInterval.getUpperBound()[k];
      graph.add(Curve(data, "blue", "solid", 2, ""));
    }
  }
  return graph;
}

/** Method that draw the sensitivity graph of a fixed marginal */
Graph SobolIndicesAlgorithmImplementation::draw(UnsignedInteger marginalIndex) const
{
  Graph graph(DrawSobolIndices(inputDesign_.getDescription(), getFirstOrderIndices(marginalIndex), getTotalOrderIndices(marginalIndex)));
  if (outputDesign_.getDimension() > 1)
    graph.setTitle(OSS() << "Marginal #" << marginalIndex << " Sobol' indices - " << getClassName());
  else
    graph.setTitle(OSS() << "Sobol' indices - " << getClassName());
  return graph;
}

/** Internal method that returns a bootstrap NSC */
Sample SobolIndicesAlgorithmImplementation::getBootstrapDesign(const Indices & indices) const
{
  // Bootstrap with huge sample that contains several samples
  const UnsignedInteger inputDimension = inputDesign_.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();
  Sample bootstrapDesign(0, outputDimension);
  for (UnsignedInteger p = 0; p < 2 + inputDimension; ++p)
  {
    Sample y(size_, outputDimension);
    Scalar* yPermData = &y(0, 0);

    const Scalar* yData = &outputDesign_(p * size_, 0);
    for (UnsignedInteger k = 0; k < size_; ++k, yPermData += outputDimension)
      memcpy(yPermData, &yData[indices[k] * outputDimension], outputDimension * sizeof(Scalar));
    // add samples to the collection
    bootstrapDesign.add(y);
  }
  return bootstrapDesign;
}

/** Function that computes aggregated indices using Vi/VTi + variance  */
Point SobolIndicesAlgorithmImplementation::computeAggregatedIndices(const Sample & Vi,
    const Sample & VTi,
    const Point & variance,
    Point & aggregatedTotal) const
{
  // Generic implementation
  const UnsignedInteger inputDimension = Vi.getDimension();
  const UnsignedInteger outputDimension = Vi.getSize();
  if (inputDimension == 1)
  {
    aggregatedTotal = Point(VTi[0]);
    return Point(Vi[0]);
  }

  // Compute sum of Var(Y^k)
  Scalar sumVariance = variance.norm1();

  // Compute aggregated indices
  aggregatedTotal = VTi.computeMean() * (outputDimension / sumVariance);
  return Point(Vi.computeMean() * (outputDimension / sumVariance));
}

/* ImportanceFactors graph */
Graph SobolIndicesAlgorithmImplementation::DrawImportanceFactors(const PointWithDescription & importanceFactors,
    const String & title)
{
  return DrawImportanceFactors(importanceFactors, importanceFactors.getDescription(), title);
}

/* ImportanceFactors graph */
Graph SobolIndicesAlgorithmImplementation::DrawImportanceFactors(const Point & values,
    const Description & names,
    const String & title)
{
  /* build data for the pie */
  const UnsignedInteger dimension = values.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on empty data.";
  if ((names.getSize() != 0) && (names.getSize() != dimension)) throw InvalidArgumentException(HERE) << "Error: the names size must match the value dimension.";
  Scalar l1Norm = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) l1Norm += std::abs(values[i]);
  if (l1Norm == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on null data.";
  Point data(dimension);
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

Graph SobolIndicesAlgorithmImplementation::DrawCorrelationCoefficients(const PointWithDescription & correlationCoefficients,
                                                                       const String & title)
{
  return DrawCorrelationCoefficients(correlationCoefficients, correlationCoefficients.getDescription(), title);
}

Graph SobolIndicesAlgorithmImplementation::DrawCorrelationCoefficients(const Point & values,
                                                                       const Description & names,
                                                                       const String & title)
{
  /* build data for the pie */
  const UnsignedInteger dimension = values.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an importance factors pie based on empty data.";
  if ((names.getSize() != 0) && (names.getSize() != dimension)) throw InvalidArgumentException(HERE) << "Error: the names size must match the value dimension.";


  Graph graph(title, "inputs", "correlation coefficient", true, "");

  // Define cloud
  Sample data(dimension, 2);
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = k + 1.0;
    data(k, 1) = values[k];
  }
  Cloud cloud(data, "red", "circle", "");
  graph.add(cloud);

  // Min & max rhos
  const Scalar minRho = data.getMin()[1];
  const Scalar maxRho = data.getMax()[1];

  // Add text description
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = (k + 1.0) + dimension / 20.0;
    data(k, 1) = 0.5 * values[k];
  }

  Text text(data, names, "right");
  text.setColor("black");
  graph.add(text);

  // Set bounding box
  Point lowerBound(2);
  lowerBound[0] = 0.8;
  if (minRho < 0)
      lowerBound[1] = -1.1;
  else
      lowerBound[1] = -0.1;

  Point upperBound(2, 1.1);
  upperBound[0] = dimension + 1.6 * (dimension - 1.0) / (dimension + 2.0);

  if (maxRho > 0)
      upperBound[1] = 1.1;
  else
      upperBound[1] = 0.1;

  graph.setBoundingBox(Interval(lowerBound, upperBound));

  return graph;
}


void SobolIndicesAlgorithmImplementation::setUseAsymptoticDistribution(Bool useAsymptoticDistribution)
{
  if (useAsymptoticDistribution_ != useAsymptoticDistribution)
  {
    useAsymptoticDistribution_ = useAsymptoticDistribution;
    alreadyComputedIndicesDistribution_ = false;
  }
}

Bool SobolIndicesAlgorithmImplementation::getUseAsymptoticDistribution() const
{
  return useAsymptoticDistribution_;
}

/* Design accessor */
void SobolIndicesAlgorithmImplementation::setDesign(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
{
  if (!(size > 1))
    throw InvalidArgumentException(HERE) << "Sobol design size must be > 1";

  // Check data is consistent
  const UnsignedInteger inputDimension = inputDesign.getDimension();
  if (outputDesign.getSize() < size * (inputDimension + 2))
    throw InvalidArgumentException(HERE) << "Sobol experiment is too small ("<< outputDesign.getSize()
                                         << " vs " << size * (inputDimension + 2)<<")";
  if (inputDesign.getSize() != outputDesign.getSize())
    throw InvalidArgumentException(HERE) << "Input and output samples have different size (" << inputDesign.getSize()
                                         << " vs " << outputDesign.getSize() << ")";

  inputDesign_ = inputDesign;
  size_ = size;

  Sample fullOutputDesign(outputDesign);
  if ((inputDimension == 2) && (outputDesign.getSize() == size * (inputDimension + 2)))
  {
    // Special case when dim=2, SO=true; the experiment is allowed to be smaller by symmetry
    // its size is N(d+2) instead of N(2d+2) as it does not contain the C=[E_2, E_1]
    Sample E1(outputDesign, size * 2, size * 3);
    Sample E2(outputDesign, size * 3, size * 4);
    fullOutputDesign.add(E2);
    fullOutputDesign.add(E1);
  }

  // center Y
  Point muY(fullOutputDesign.computeMean());
  outputDesign_ = fullOutputDesign - muY;

  // yA variance
  referenceVariance_ = Sample(fullOutputDesign, 0, size).computeVariance();
  for (UnsignedInteger j = 0; j < referenceVariance_.getDimension(); ++ j)
    if (!(referenceVariance_[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";

  alreadyComputedIndicesDistribution_ = false;
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
  adv.saveAttribute( "aggregatedFirstOrderIndices_", aggregatedFirstOrderIndices_);
  adv.saveAttribute( "aggregatedTotalOrderIndices_", aggregatedTotalOrderIndices_);
  adv.saveAttribute( "secondOrderIndices_", secondOrderIndices_);
  adv.saveAttribute( "firstOrderIndiceDistribution_", firstOrderIndiceDistribution_);
  adv.saveAttribute( "totalOrderIndiceDistribution_", totalOrderIndiceDistribution_);
  adv.saveAttribute( "alreadyComputedIndicesDistribution_", alreadyComputedIndicesDistribution_);
  adv.saveAttribute( "useAsymptoticDistribution_", useAsymptoticDistribution_);
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
  adv.loadAttribute( "aggregatedFirstOrderIndices_", aggregatedFirstOrderIndices_);
  adv.loadAttribute( "aggregatedTotalOrderIndices_", aggregatedTotalOrderIndices_);
  adv.loadAttribute( "secondOrderIndices_", secondOrderIndices_);
  adv.loadAttribute( "firstOrderIndiceDistribution_", firstOrderIndiceDistribution_);
  adv.loadAttribute( "totalOrderIndiceDistribution_", totalOrderIndiceDistribution_);
  adv.loadAttribute( "alreadyComputedIndicesDistribution_", alreadyComputedIndicesDistribution_);
  adv.loadAttribute( "useAsymptoticDistribution_", useAsymptoticDistribution_);
}

END_NAMESPACE_OPENTURNS
