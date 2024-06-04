//                                               -*- C++ -*-
/**
 *  @brief Implementation for Rank Sobol Sensitivity Algorithm
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/RankSobolSensitivityAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Text.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankSobolSensitivityAlgorithm)

static const Factory<RankSobolSensitivityAlgorithm> Factory_RankSobolSensitivityAlgorithm;

/* Default constructor */
RankSobolSensitivityAlgorithm::RankSobolSensitivityAlgorithm()
  : SobolIndicesAlgorithmImplementation()
{
  // Nothing to do
}

/** Constructor with parameters */
RankSobolSensitivityAlgorithm::RankSobolSensitivityAlgorithm(const Sample & inputDesign,
    const Sample & outputDesign): SobolIndicesAlgorithmImplementation()
{
  setDesign(inputDesign, outputDesign, inputDesign.getSize());
}

/* Design accessor */
void RankSobolSensitivityAlgorithm::setDesign(const Sample & inputDesign,
    const Sample & outputDesign,
    const UnsignedInteger size)
{

  inputDesign_  = inputDesign;
  outputDesign_ = outputDesign;

  size_ = size;
  inputDescription_ = inputDesign.getDescription();
  referenceVariance_ = outputDesign_.computeVariance();

  if (!(size_ > 1))
    throw InvalidArgumentException(HERE) << "Sobol design size must be > 1";

  if (size_ != outputDesign.getSize())
    throw InvalidArgumentException(HERE) << "Input and output samples have different size (" << size_
                                         << " vs " << outputDesign.getSize() << ")";
  if (!outputDesign.getDimension())
    throw InvalidArgumentException(HERE) << "Output sample dimension is null";

  for (UnsignedInteger j = 0; j < referenceVariance_.getDimension(); ++ j)
    if (!(referenceVariance_[j] > 0.0))
      throw InvalidArgumentException(HERE) << "Null output sample variance";
}


/* First order indices accessor */
Point RankSobolSensitivityAlgorithm::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke the method to compute first order indices
    varianceI_ = computeIndices();
  }

  const UnsignedInteger outputDimension = outputDesign_.getDimension();

  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "In RankSobolSensitivityAlgorithm::getFirstOrderIndices, marginalIndex should be in [0,"
                                         << outputDimension - 1;
  // return value
  const Point firstOrderSensitivity(varianceI_[marginalIndex] / referenceVariance_[marginalIndex]);
  return firstOrderSensitivity;
}

/** Internal method that compute Vi  **/
Sample RankSobolSensitivityAlgorithm::computeIndicesFromSample(const Sample & inputDesign, const Sample & outputDesign) const
{
  Sample marginalVar(outputDesign.getDimension(), inputDesign.getDimension());

  UnsignedInteger size = inputDesign.getSize();
  const Point meanOutputDesign = outputDesign.computeMean();

  for (UnsignedInteger j = 0; j < outputDesign.getDimension(); ++j)
  {
    for (UnsignedInteger i = 0; i < inputDesign.getDimension(); ++i)
    {
      // Sorting samples with respect to the input dimension
      const Indices idSort = inputDesign.getMarginal(i).argsort();
      Scalar sum = 0.;

      for (UnsignedInteger k = 0; k < size; ++k)
      {
        sum += outputDesign(idSort[k], j) * outputDesign(idSort[(k + 1) % size], j);
      }
      // Compute the marginal variance
      marginalVar(j, i) = 1. / size * sum - meanOutputDesign[j] * meanOutputDesign[j];
    }
  }

  return marginalVar;
}


Sample RankSobolSensitivityAlgorithm::computeIndices() const
{
  return computeIndicesFromSample(inputDesign_, outputDesign_);
}


// Definition of bootstrap policy without replacement to compute confidence intervals
// Parallelization is performed with TBB
struct RankSobolBootstrapPolicy
{
  const RankSobolSensitivityAlgorithm & sai_;
  const Sample & randomIndices_;
  Sample & bsFO_;

  RankSobolBootstrapPolicy( const RankSobolSensitivityAlgorithm & sai,
                            const Sample & randomIndices,
                            Sample & bsFO)
    : sai_(sai)
    , randomIndices_(randomIndices)
    , bsFO_(bsFO)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
    {
      Indices index(randomIndices_.getDimension());
      for (UnsignedInteger l = 0; l < randomIndices_.getDimension(); l++)
      {
        index[l] = static_cast <OT::UnsignedInteger>(randomIndices_(k, l));
      }

      const Sample selectedOutput = sai_.outputDesign_.select(index);

      const Point variance(selectedOutput.computeVariance());
      // Compute indices using this collection
      const Sample Vi(sai_.computeIndicesFromSample(sai_.inputDesign_.select(index), selectedOutput));
      // Compute aggregated indices
      bsFO_[k] = sai_.computeAggregatedIndices(Vi, variance);
    }

  }
}; /* end struct BootstrapPolicy */


// Draw of First indices only
Graph RankSobolSensitivityAlgorithm::DrawSobolFirstOrderIndices(const Description & inputDescription,
    const Point & firstOrderIndices,
    const Interval & firstOrderConfidenceInterval)
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

  const Cloud firstOrderIndicesGraph(data, "red", "circle", "First order");

  graph.add(firstOrderIndicesGraph);

  // Description
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    data(k, 0) = (k + 1.0) + dimension / 20.0;
    data(k, 1) = firstOrderIndices[k];
  }
  Text text(data, inputDescription, "right");
  text.setColor("black");
  graph.add(text);

  // Draw confidence intervals
  data = Sample(2, 2);
  if (firstOrderConfidenceInterval.getDimension())
  {
    // transform data
    for (UnsignedInteger k = 0; k < dimension; ++k)
    {
      // Relative to FirstOrder
      data(0, 0) = (k + 1);
      data(0, 1) = firstOrderConfidenceInterval.getLowerBound()[k];
      data(1, 0) = (k + 1);
      data(1, 1) = firstOrderConfidenceInterval.getUpperBound()[k];
      graph.add(Curve(data, "red", "solid", 2, ""));
    }
  }
  // Set bounding box
  Point lowerBound(2, -0.1);
  lowerBound[0] = 1.0 - dimension / 10.0;
  Point upperBound(2, 1.1);
  const Scalar descriptionMargin = 1.6 * (dimension - 1.0) / (dimension + 2.0);
  upperBound[0] = dimension + descriptionMargin;
  graph.setBoundingBox(Interval(lowerBound, upperBound));
  graph.setIntegerXTick(true);
  graph.setLegendPosition("topright");
  return graph;
}

/** Method that draw (plot) the sensitivity graph */
Graph RankSobolSensitivityAlgorithm::draw() const
{
  Graph graph(DrawSobolFirstOrderIndices(inputDescription_, getAggregatedFirstOrderIndices()));
  if (outputDesign_.getDimension() > 1)
    graph.setTitle(OSS() << "Aggregated Sobol' indices - " << getClassName());
  else
    graph.setTitle(OSS() << "Sobol' indices - " << getClassName());
  const UnsignedInteger dimension = aggregatedFirstOrderIndices_.getDimension();

  // Draw confidence intervals
  if (confidenceLevel_ > 0.0)
  {
    const Interval foInterval(getFirstOrderIndicesInterval());

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
    }
  }
  return graph;
}

// Function that computes aggregated indices  using Vi + variance
// Indices are aggregated with respect to output dimensions
Point RankSobolSensitivityAlgorithm::computeAggregatedIndices(const Sample & Vi,
    const Point & variance) const
{
  // Generic implementation
  const UnsignedInteger inputDimension = Vi.getDimension();
  const UnsignedInteger outputDimension = outputDesign_.getDimension();

  if (inputDimension == 1)
  {
    return Point(Vi[0]);
  }

  // Compute sum of Var(Y^k)
  const Scalar sumVariance = variance.norm1();

  // Compute aggregated indices
  return Point(Vi.computeMean() * (outputDimension / sumVariance));
}



/** void method that computes confidence interval */
void RankSobolSensitivityAlgorithm::computeBootstrapDistribution() const
{
  // Build interval using sample variance
  // Mean reference is the Sensitivity values
  if (bootstrapSize_ > 0)
  {
    // Temporary samples that stores the first/total indices
    const UnsignedInteger inputDimension = inputDescription_.getSize();
    Sample bsFO(0, inputDimension);

    UnsignedInteger bootstrapSampleSize = ResourceMap::GetAsScalar("RankSobolSensitivityAlgorithm-DefaultBootstrapSampleRatio") * size_;

    const UnsignedInteger blockSize = std::min(bootstrapSize_, ResourceMap::GetAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBlockSize"));
    const UnsignedInteger maximumOuterSampling = bootstrapSize_ / blockSize;
    const UnsignedInteger modulo = bootstrapSize_ % blockSize;
    const UnsignedInteger lastBlockSize = modulo == 0 ? blockSize : modulo;

    // Use of KPermutations to perform bootstrap without replication
    Distribution KPermutation = KPermutationsDistribution(bootstrapSampleSize, size_);

    for(UnsignedInteger outerSampling = 0; outerSampling < maximumOuterSampling; ++outerSampling)
    {
      // the last block can be smaller
      const UnsignedInteger effectiveBlockSize = (outerSampling + 1) < maximumOuterSampling ? blockSize : lastBlockSize;

      Sample bsFOpartial(effectiveBlockSize, inputDimension);

      const Sample randomIndices = KPermutation.getSample(effectiveBlockSize);

      const RankSobolBootstrapPolicy policy( *this, randomIndices, bsFOpartial );
      TBBImplementation::ParallelFor( 0, effectiveBlockSize, policy );
      bsFO.add(bsFOpartial);

    }

    const KernelSmoothing factory;
    firstOrderIndiceDistribution_ = factory.build(bsFO);

  }
}

Point RankSobolSensitivityAlgorithm::getAggregatedFirstOrderIndices() const
{
  if (0 == varianceI_.getSize())
  {
    // Invoke internal method to compute aggregated first order indices
    varianceI_ = computeIndices();
  }
  if (0 == aggregatedFirstOrderIndices_.getDimension())
  {
    // Compute aggregate indices
    aggregatedFirstOrderIndices_ = computeAggregatedIndices(varianceI_, referenceVariance_);
  }
  // Indices computed
  return aggregatedFirstOrderIndices_;
}



/** Methods of base class that are not yet implemented **/

Sample RankSobolSensitivityAlgorithm::computeIndices(const Sample &, Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::computeIndices(const Sample & sample, Sample & VTi) const";
}


SymmetricMatrix RankSobolSensitivityAlgorithm::getSecondOrderIndices(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getSecondOrderIndices(const UnsignedInteger marginalIndex = 0) const";
}

Point RankSobolSensitivityAlgorithm::getTotalOrderIndices(const UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getTotalOrderIndices(const UnsignedInteger marginalIndex = 0) const";
}

Interval RankSobolSensitivityAlgorithm::getTotalOrderIndicesInterval() const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getTotalOrderIndicesInterval() const";
}

/** Aggregated total order indices accessor for multivariate samples */
Point RankSobolSensitivityAlgorithm::getAggregatedTotalOrderIndices() const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getAggregatedTotalOrderIndices() const";
}

/** Whether to use bootstrap or asymptotic distribution */
Bool RankSobolSensitivityAlgorithm::getUseAsymptoticDistribution() const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getUseAsymptoticDistribution() const";
}

Distribution RankSobolSensitivityAlgorithm::getTotalOrderIndicesDistribution() const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::getTotalOrderIndicesDistribution() const";
}


/** Method that draw  the sensitivity graph of a fixed marginal */
Graph RankSobolSensitivityAlgorithm::draw(UnsignedInteger) const
{
  throw NotYetImplementedException(HERE) << "In RankSobolSensitivityAlgorithm::draw(UnsignedInteger marginalIndex) const ";
}

/* Virtual constructor */
RankSobolSensitivityAlgorithm * RankSobolSensitivityAlgorithm::clone() const
{
  return new RankSobolSensitivityAlgorithm(*this);
}

/* String converter */
String RankSobolSensitivityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " " << SobolIndicesAlgorithmImplementation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RankSobolSensitivityAlgorithm::save(Advocate & adv) const
{
  SobolIndicesAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void RankSobolSensitivityAlgorithm::load(Advocate & adv)
{
  SobolIndicesAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
