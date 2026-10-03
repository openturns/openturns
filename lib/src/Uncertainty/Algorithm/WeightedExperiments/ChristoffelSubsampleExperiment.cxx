//                                               -*- C++ -*-
/**
 *  @brief Christoffel subsample experiment with prescribed size
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
#include <cmath>
#include <algorithm>
#include "openturns/ChristoffelSubsampleExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ChristoffelSubsampleExperiment)

static const Factory<ChristoffelSubsampleExperiment> Factory_ChristoffelSubsampleExperiment;

/* Default constructor */
ChristoffelSubsampleExperiment::ChristoffelSubsampleExperiment()
  : WeightedExperimentImplementation()
  , basis_()
  , spaceDimension_(0)
{
  // Nothing to do: call setOrthogonalBasis() then setSize() before generating
}

/* Parameters constructor */
ChristoffelSubsampleExperiment::ChristoffelSubsampleExperiment(const OrthogonalBasis & basis,
    const UnsignedInteger size)
  : WeightedExperimentImplementation(size)
  , basis_(basis)
  , spaceDimension_(0)
{
  update();
}

/* Virtual constructor */
ChristoffelSubsampleExperiment * ChristoffelSubsampleExperiment::clone() const
{
  return new ChristoffelSubsampleExperiment(*this);
}

/* Comparison operator: base members plus the Christoffel law,
   as OrthogonalBasis has no value-based equality */
Bool ChristoffelSubsampleExperiment::operator ==(const ChristoffelSubsampleExperiment & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (christoffelDistribution_ == other.christoffelDistribution_);
}

Bool ChristoffelSubsampleExperiment::equals(const WeightedExperimentImplementation & other) const
{
  const ChristoffelSubsampleExperiment * p_other = dynamic_cast<const ChristoffelSubsampleExperiment *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ChristoffelSubsampleExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << ChristoffelSubsampleExperiment::GetClassName()
      << " name=" << getName()
      << " basis=" << basis_
      << " size=" << size_
      << " spaceDimension=" << spaceDimension_;
  return oss;
}

/* Orthogonal basis accessor */
void ChristoffelSubsampleExperiment::setOrthogonalBasis(const OrthogonalBasis & basis)
{
  const OrthogonalBasis previousBasis(basis_);
  basis_ = basis;
  try
  {
    update();
  }
  catch (...)
  {
    // restore the previous valid state: update() rebuilds derived members from basis_ and size_
    basis_ = previousBasis;
    update();
    throw;
  }
}

OrthogonalBasis ChristoffelSubsampleExperiment::getOrthogonalBasis() const
{
  return basis_;
}

/* Size accessor */
void ChristoffelSubsampleExperiment::setSize(const UnsignedInteger size)
{
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  const UnsignedInteger previousSize = size_;
  size_ = size;
  try
  {
    update();
  }
  catch (...)
  {
    // restore the previous valid state
    size_ = previousSize;
    update();
    throw;
  }
}

UnsignedInteger ChristoffelSubsampleExperiment::getSize() const
{
  return size_;
}

/* Space dimension accessor */
UnsignedInteger ChristoffelSubsampleExperiment::getSpaceDimension() const
{
  return spaceDimension_;
}

/* Reference distribution is embedded in the basis and cannot be set */
void ChristoffelSubsampleExperiment::setDistribution(const Distribution &)
{
  throw InvalidArgumentException(HERE) << "Error: the reference distribution of a ChristoffelSubsampleExperiment is embedded in its basis, use setOrthogonalBasis() instead.";
}

/* Deduce the space dimension m from the target size n by n = gamma * m * log(m) */
UnsignedInteger ChristoffelSubsampleExperiment::DeduceSpaceDimension(const UnsignedInteger size)
{
  const Scalar gamma = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-Gamma");
  if (!(gamma > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive gamma, here value=" << gamma;
  UnsignedInteger spaceDimension = 1;
  while (gamma * (spaceDimension + 1) * std::log(spaceDimension + 1) <= size) ++spaceDimension;
  return spaceDimension;
}

/* Uniform weights ? */
Bool ChristoffelSubsampleExperiment::hasUniformWeights() const
{
  return false;
}

/* Random experiment ? */
Bool ChristoffelSubsampleExperiment::isRandom() const
{
  return true;
}

/* Sample generation with weights: pool from the Christoffel law, thin to size_ */
Sample ChristoffelSubsampleExperiment::generateWithWeights(Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  const Sample pool(drawPool(computePoolSize()));
  return finalizeDesign(pool, computePoolWeights(pool), weightsOut);
}

/* Sample generation with weights: existing points topped up, then thinned */
Sample ChristoffelSubsampleExperiment::generateWithWeights(const Sample & poolSample, Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  if (poolSample.getDimension() != distribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a pool sample of dimension=" << distribution_.getDimension() << ", got dimension=" << poolSample.getDimension();
  Sample pool(poolSample);
  const UnsignedInteger poolSize = computePoolSize();
  if (pool.getSize() < poolSize)
  {
    const Sample fresh(drawPool(poolSize - pool.getSize()));
    for (UnsignedInteger i = 0; i < fresh.getSize(); ++i) pool.add(fresh[i]);
  }
  return finalizeDesign(pool, computePoolWeights(pool), weightsOut);
}

/* Target pool size from the oversampling factor */
UnsignedInteger ChristoffelSubsampleExperiment::computePoolSize() const
{
  const Scalar poolFactor = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor");
  if (!(poolFactor >= 1.0)) throw InvalidArgumentException(HERE) << "Error: expected a pool oversampling factor not smaller than 1, here factor=" << poolFactor;
  return std::max(size_, static_cast<UnsignedInteger>(std::ceil(poolFactor * size_)));
}

/* Draw fresh pool points from the Christoffel law */
Sample ChristoffelSubsampleExperiment::drawPool(const UnsignedInteger count) const
{
  if (count == 0) return Sample(0, distribution_.getDimension());
  return christoffelDistribution_.getSample(count);
}

/* Density ratios of pool points, reference over Christoffel law */
Point ChristoffelSubsampleExperiment::computePoolWeights(const Sample & pool) const
{
  const UnsignedInteger poolSize = pool.getSize();
  Point poolWeights(poolSize);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    // a point outside the numerical range has a null law density: the ratio
    // is undefined (0/0 there), keep such a point out of the weighted design
    const Scalar lawPDF = christoffelDistribution_.computePDF(pool[i]);
    poolWeights[i] = lawPDF > 0.0 ? distribution_.computePDF(pool[i]) / lawPDF : 0.0;
  }
  return poolWeights;
}

/* Eigenvalues of the weighted design Gramian */
Point ChristoffelSubsampleExperiment::computeDesignEigenvalues(const Sample & sample,
    const Point & weights) const
{
  const UnsignedInteger sampleSize = sample.getSize();
  if (sampleSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty design sample.";
  if (sample.getDimension() != distribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a design of dimension=" << distribution_.getDimension() << ", got dimension=" << sample.getDimension();
  if (weights.getSize() != sampleSize) throw InvalidArgumentException(HERE) << "Error: expected as many weights as design points.";
  const Basis finiteBasis(christoffelDistribution_.getBasis());
  const UnsignedInteger dimension = finiteBasis.getSize();
  Matrix features(sampleSize, dimension);
  for (UnsignedInteger k = 0; k < sampleSize; ++k)
  {
    const Point point(sample[k]);
    for (UnsignedInteger j = 0; j < dimension; ++j) features(k, j) = finiteBasis[j](point)[0];
  }
  Indices all(sampleSize);
  for (UnsignedInteger k = 0; k < sampleSize; ++k) all[k] = k;
  const SymmetricMatrix gramian(ComputeGramian(features, weights, all));
  return gramian.computeEigenValues();
}

/* Thin a pool to size_, weigh and frame-check the design */
Sample ChristoffelSubsampleExperiment::finalizeDesign(const Sample & pool,
    const Point & poolWeights,
    Point & weightsOut) const
{
  Indices kept;
  Point selectionWeights(pool.getSize(), 1.0);
  // finite basis evaluations shared by the thinning step and the frame check below
  const Matrix poolFeatures(computePoolFeatures(pool));
  const String thinningMethod = ResourceMap::GetAsString("ChristoffelSubsampleExperiment-ThinningMethod");
  if (thinningMethod == "Barrier")
  {
    Point barrierWeights;
    barrierSelection(pool, poolWeights, poolFeatures, kept, barrierWeights);
    Scalar totalWeight = 0.0;
    for (UnsignedInteger k = 0; k < size_; ++k) totalWeight += barrierWeights[kept[k]] * poolWeights[kept[k]];
    if (!(totalWeight > 0.0)) throw InternalException(HERE) << "Error: null total barrier weight.";
    for (UnsignedInteger k = 0; k < size_; ++k) selectionWeights[kept[k]] = size_ * barrierWeights[kept[k]] / totalWeight;
  }
  else if (thinningMethod == "Removal")
  {
    kept = thinIndices(pool, poolWeights, poolFeatures);
  }
  else throw InvalidArgumentException(HERE) << "Error: unknown thinning method=" << thinningMethod << ", expected Barrier or Removal.";
  Sample sample(size_, distribution_.getDimension());
  sample.setDescription(distribution_.getDescription());
  weightsOut = Point(size_);
  for (UnsignedInteger k = 0; k < size_; ++k) weightsOut[k] = selectionWeights[kept[k]] * poolWeights[kept[k]];
  // Bulk row copy through the contiguous storage
  const UnsignedInteger inputDimension = distribution_.getDimension();
  auto poolBegin = pool.getImplementation()->data_begin();
  auto sampleBegin = sample.getImplementation()->data_begin();
  for (UnsignedInteger k = 0; k < size_; ++k)
    std::copy(poolBegin + kept[k] * inputDimension, poolBegin + (kept[k] + 1) * inputDimension, sampleBegin + k * inputDimension);
  // Frame check on the thinned design, reusing the pool evaluations above
  Point designWeights(pool.getSize());
  for (UnsignedInteger i = 0; i < pool.getSize(); ++i) designWeights[i] = selectionWeights[i] * poolWeights[i];
  const Point eigenValues(ComputeGramian(poolFeatures, designWeights, kept).computeEigenValues());
  Scalar minEigenvalue = eigenValues[0];
  Scalar maxEigenvalue = eigenValues[0];
  for (UnsignedInteger k = 1; k < eigenValues.getSize(); ++k)
  {
    minEigenvalue = std::min(minEigenvalue, eigenValues[k]);
    maxEigenvalue = std::max(maxEigenvalue, eigenValues[k]);
  }
  const Scalar tolerance = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance");
  if (minEigenvalue < 1.0 - tolerance || maxEigenvalue > 1.0 + tolerance)
    LOGWARN(OSS() << "Warning: the thinned design misses the frame bounds [" << 1.0 - tolerance << ", " << 1.0 + tolerance << "], got [" << minEigenvalue << ", " << maxEigenvalue << "]");
  return sample;
}

/* Identity index set of given size */
Indices ChristoffelSubsampleExperiment::IdentityIndices(const UnsignedInteger size)
{
  Indices indices(size);
  for (UnsignedInteger i = 0; i < size; ++i) indices[i] = i;
  return indices;
}

/* Gramian of a subset: mean over kept of w * phi * phi^T, via BLAS */
SymmetricMatrix ChristoffelSubsampleExperiment::ComputeGramian(const Matrix & features,
    const Point & weights,
    const Indices & kept)
{
  const UnsignedInteger dimension = features.getNbColumns();
  const UnsignedInteger keptSize = kept.getSize();
  if (keptSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty kept index set.";
  Matrix scaled(keptSize, dimension);
  for (UnsignedInteger q = 0; q < keptSize; ++q)
  {
    const UnsignedInteger idx = kept[q];
    const Scalar rootWeight = std::sqrt(weights[idx]);
    for (UnsignedInteger j = 0; j < dimension; ++j) scaled(q, j) = rootWeight * features(idx, j);
  }
  const CovarianceMatrix gram(scaled.computeGram(true));
  return gram / (1.0 * keptSize);
}

/* Min eigenvalue left after removing one candidate of a block, by downdate */
struct RemovalScoresPolicy
{
  const SymmetricMatrix & total_;
  const Matrix & features_;
  const Point & poolWeights_;
  const Indices & kept_;
  const Scalar divisor_;
  const UnsignedInteger dimension_;
  Point & minima_;

  RemovalScoresPolicy(const SymmetricMatrix & total,
                      const Matrix & features,
                      const Point & poolWeights,
                      const Indices & kept,
                      const Scalar divisor,
                      const UnsignedInteger dimension,
                      Point & minima)
    : total_(total)
    , features_(features)
    , poolWeights_(poolWeights)
    , kept_(kept)
    , divisor_(divisor)
    , dimension_(dimension)
    , minima_(minima)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger pos = r.begin(); pos != r.end(); ++pos)
    {
      const UnsignedInteger idx = kept_[pos];
      SymmetricMatrix gramian(total_);
      for (UnsignedInteger a = 0; a < dimension_; ++a)
        for (UnsignedInteger b = 0; b <= a; ++b)
          gramian(a, b) -= poolWeights_[idx] * features_(idx, a) * features_(idx, b);
      gramian = gramian / divisor_;
      const Point eigenValues(gramian.computeEigenValues());
      Scalar minEigenvalue = eigenValues[0];
      for (UnsignedInteger k = 1; k < dimension_; ++k) minEigenvalue = std::min(minEigenvalue, eigenValues[k]);
      minima_[pos] = minEigenvalue;
    }
  }
}; /* end struct RemovalScoresPolicy */

/* Finite basis evaluations on a pool */
Matrix ChristoffelSubsampleExperiment::computePoolFeatures(const Sample & pool) const
{
  const Basis finiteBasis(christoffelDistribution_.getBasis());
  const UnsignedInteger dimension = finiteBasis.getSize();
  const UnsignedInteger poolSize = pool.getSize();
  Matrix features(poolSize, dimension);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    const Point point(pool[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j) features(i, j) = finiteBasis[j](point)[0];
  }
  return features;
}

/* Greedy removal of pool points down to size_, maximizing the min eigenvalue */
Indices ChristoffelSubsampleExperiment::thinIndices(const Sample & pool,
    const Point & poolWeights,
    const Matrix & features) const
{
  const UnsignedInteger poolSize = pool.getSize();
  const UnsignedInteger dimension = features.getNbColumns();
  Indices kept(IdentityIndices(poolSize));
  while (kept.getSize() > size_)
  {
    const UnsignedInteger keptSize = kept.getSize();
    // Unscaled sum once per iteration, downdated per candidate below
    SymmetricMatrix total(dimension);
    for (UnsignedInteger q = 0; q < keptSize; ++q)
    {
      const UnsignedInteger idx = kept[q];
      for (UnsignedInteger a = 0; a < dimension; ++a)
        for (UnsignedInteger b = 0; b <= a; ++b)
          total(a, b) += poolWeights[idx] * features(idx, a) * features(idx, b);
    }
    Point minima(keptSize, SpecFunc::LowestScalar);
    const RemovalScoresPolicy policy(total, features, poolWeights, kept, 1.0 * (keptSize - 1), dimension, minima);
    TBBImplementation::ParallelForIf(keptSize > 256, 0, keptSize, policy);
    Scalar bestMinEigenvalue = SpecFunc::LowestScalar;
    UnsignedInteger bestPosition = 0;
    for (UnsignedInteger pos = 0; pos < keptSize; ++pos)
      if (minima[pos] > bestMinEigenvalue)
      {
        bestMinEigenvalue = minima[pos];
        bestPosition = pos;
      }
    kept.erase(kept.begin() + bestPosition);
  }
  return kept;
}

/* Barrier gap and step scores of one block of candidates */
struct BarrierScoresPolicy
{
  const SymmetricMatrix & lowerBase_;
  const SymmetricMatrix & upperBase_;
  const Scalar potentialLower_;
  const Scalar potentialLowerNext_;
  const Scalar potentialUpper_;
  const Scalar potentialUpperNext_;
  const Matrix & scaled_;
  const Indices & remaining_;
  Point & gaps_;
  Point & deltas_;
  const Scalar barrierStep_;
  const Scalar deltaCap_;
  const UnsignedInteger dimension_;

  BarrierScoresPolicy(const SymmetricMatrix & lowerBase,
                      const SymmetricMatrix & upperBase,
                      const Scalar potentialLower,
                      const Scalar potentialLowerNext,
                      const Scalar potentialUpper,
                      const Scalar potentialUpperNext,
                      const Matrix & scaled,
                      const Indices & remaining,
                      Point & gaps,
                      Point & deltas,
                      const Scalar barrierStep,
                      const Scalar deltaCap,
                      const UnsignedInteger dimension)
    : lowerBase_(lowerBase)
    , upperBase_(upperBase)
    , potentialLower_(potentialLower)
    , potentialLowerNext_(potentialLowerNext)
    , potentialUpper_(potentialUpper)
    , potentialUpperNext_(potentialUpperNext)
    , scaled_(scaled)
    , remaining_(remaining)
    , gaps_(gaps)
    , deltas_(deltas)
    , barrierStep_(barrierStep)
    , deltaCap_(deltaCap)
    , dimension_(dimension)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    Point direction(dimension_);
    for (UnsignedInteger pos = r.begin(); pos != r.end(); ++pos)
    {
      const UnsignedInteger idx = remaining_[pos];
      for (UnsignedInteger k = 0; k < dimension_; ++k) direction[k] = scaled_(idx, k);
      const Point invLower(lowerBase_.solveLinearSystem(direction));
      const Point invUpper(upperBase_.solveLinearSystem(direction));
      Scalar lowerLinear = 0.0;
      Scalar lowerQuadratic = 0.0;
      Scalar upperLinear = 0.0;
      Scalar upperQuadratic = 0.0;
      for (UnsignedInteger k = 0; k < dimension_; ++k)
      {
        lowerLinear += direction[k] * invLower[k];
        lowerQuadratic += invLower[k] * invLower[k];
        upperLinear += direction[k] * invUpper[k];
        upperQuadratic += invUpper[k] * invUpper[k];
      }
      const Scalar lowerDenominator = potentialLowerNext_ - potentialLower_;
      const Scalar upperDenominator = potentialUpper_ - potentialUpperNext_;
      if (!(lowerDenominator > 0.0)) continue;
      if (!(upperDenominator > 0.0)) continue;
      const Scalar lowerScore = lowerQuadratic / lowerDenominator - lowerLinear;
      const Scalar upperScore = upperQuadratic / upperDenominator + upperLinear;
      gaps_[pos] = lowerScore - upperScore;
      if (lowerScore > upperScore && lowerScore + upperScore > 0.0)
        deltas_[pos] = std::min(2.0 / (lowerScore + upperScore), deltaCap_);
      else
        deltas_[pos] = std::min(barrierStep_, deltaCap_);
    }
  }
}; /* end struct BarrierScoresPolicy */

/* Forward barrier greedy selection of size_ pool points plus their weights */
void ChristoffelSubsampleExperiment::barrierSelection(const Sample & pool,
    const Point & poolWeights,
    const Matrix & features,
    Indices & keptOut,
    Point & barrierWeightsOut) const
{
  const UnsignedInteger poolSize = pool.getSize();
  const UnsignedInteger dimension = features.getNbColumns();
  const Scalar barrierStep = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-BarrierStep");
  if (!(barrierStep > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive barrier step, here step=" << barrierStep;
  // Frame vectors v_i = sqrt(w_i / M) * phi(x_i), with sum v_i v_i^T ~= identity
  Matrix scaled(poolSize, dimension);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    const Scalar scale = std::sqrt(poolWeights[i] / poolSize);
    for (UnsignedInteger j = 0; j < dimension; ++j) scaled(i, j) = scale * features(i, j);
  }
  SymmetricMatrix current(dimension);
  Scalar lower = -1.0 * dimension;
  Scalar upper = 1.0 * dimension;
  Indices remaining(IdentityIndices(poolSize));
  Indices selected;
  Point barrierWeights(poolSize, 0.0);
  // Trace budget: E||v||^2 = m/M hence mean step M/n for tr(A) ~= m
  const Scalar deltaCap = 1.0 * poolSize / size_ * dimension;
  for (UnsignedInteger step = 0; step < size_; ++step)
  {
    // Clamp the barriers strictly inside the current spectrum: fixed steps
    // outrun small spectra (eg rank-deficient early Gramians), singular solves
    const Point spectrum(current.computeEigenValues());
    Scalar minEig = spectrum[0];
    Scalar maxEig = spectrum[0];
    for (UnsignedInteger k = 1; k < dimension; ++k)
    {
      minEig = std::min(minEig, spectrum[k]);
      maxEig = std::max(maxEig, spectrum[k]);
    }
    const Scalar regularization = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-BarrierRegularization");
    if (!(regularization > 0.0)) throw InvalidArgumentException(HERE) << "Error: expected a positive barrier regularization, here value=" << regularization;
    // Re-validate the current levels after growth, then advance inside the spectrum
    lower = std::min(lower, minEig - regularization);
    upper = std::max(upper, maxEig + regularization);
    const Scalar nextLower = std::min(lower + barrierStep, minEig - regularization);
    const Scalar nextUpper = std::max(upper + barrierStep, maxEig + regularization);
    SymmetricMatrix lowerBase(current);
    SymmetricMatrix lowerCurrent(current);
    SymmetricMatrix upperBase(dimension);
    SymmetricMatrix shiftedUpper(dimension);
    for (UnsignedInteger a = 0; a < dimension; ++a)
    {
      lowerBase(a, a) -= nextLower;
      lowerCurrent(a, a) -= lower;
      for (UnsignedInteger b = 0; b < a; ++b)
      {
        upperBase(a, b) = -current(a, b);
        shiftedUpper(a, b) = -current(a, b);
      }
      upperBase(a, a) = nextUpper - current(a, a);
      shiftedUpper(a, a) = upper - current(a, a);
    }
    // Barrier potentials at current and next levels
    Scalar potentialLower = 0.0;
    Scalar potentialLowerNext = 0.0;
    Scalar potentialUpper = 0.0;
    Scalar potentialUpperNext = 0.0;
    for (UnsignedInteger k = 0; k < dimension; ++k)
    {
      Point unit(dimension, 0.0);
      unit[k] = 1.0;
      potentialLower += lowerCurrent.solveLinearSystem(unit)[k];
      potentialLowerNext += lowerBase.solveLinearSystem(unit)[k];
      potentialUpper += shiftedUpper.solveLinearSystem(unit)[k];
      potentialUpperNext += upperBase.solveLinearSystem(unit)[k];
    }
    Scalar bestGap = SpecFunc::LowestScalar;
    UnsignedInteger bestPosition = 0;
    Scalar bestDelta = barrierStep;
    const UnsignedInteger remainingSize = remaining.getSize();
    Point gaps(remainingSize, SpecFunc::LowestScalar);
    Point stepDeltas(remainingSize, barrierStep);
    const BarrierScoresPolicy policy(lowerBase, upperBase, potentialLower, potentialLowerNext, potentialUpper, potentialUpperNext, scaled, remaining, gaps, stepDeltas, barrierStep, deltaCap, dimension);
    TBBImplementation::ParallelForIf(remainingSize > 256, 0, remainingSize, policy);
    for (UnsignedInteger pos = 0; pos < remainingSize; ++pos)
      if (gaps[pos] > bestGap)
      {
        bestGap = gaps[pos];
        bestPosition = pos;
        bestDelta = stepDeltas[pos];
      }
    // every candidate skipped (null denominators): no admissible step exists
    if (!(bestGap > SpecFunc::LowestScalar)) throw InternalException(HERE) << "Error: no admissible barrier candidate at this step.";
    const UnsignedInteger chosen = remaining[bestPosition];
    for (UnsignedInteger a = 0; a < dimension; ++a)
      for (UnsignedInteger b = 0; b <= a; ++b)
        current(a, b) += bestDelta * scaled(chosen, a) * scaled(chosen, b);
    barrierWeights[chosen] = bestDelta;
    selected.add(chosen);
    remaining.erase(remaining.begin() + bestPosition);
    lower = nextLower;
    upper = nextUpper;
  }
  // Kept indices in pool order for a stable layout
  keptOut = Indices();
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    Bool found = false;
    for (UnsignedInteger q = 0; q < selected.getSize(); ++q)
      if (selected[q] == i)
      {
        found = true;
        break;
      }
    if (found) keptOut.add(i);
  }
  barrierWeightsOut = barrierWeights;
}

/* Rebuild the derived members */
void ChristoffelSubsampleExperiment::update()
{
  if (size_ == 0)
  {
    spaceDimension_ = 0;
    return;
  }
  spaceDimension_ = DeduceSpaceDimension(size_);
  const ChristoffelDistribution christoffel(basis_, spaceDimension_);
  christoffelDistribution_ = christoffel;
  distribution_ = christoffel.getMeasure();
}

/* Method save() stores the object through the StorageManager: only the basis
   is class-specific here, the base save keeps size_ and distribution_, and
   update() rebuilds spaceDimension_ and the Christoffel law on load */
void ChristoffelSubsampleExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("basis_", basis_);
}

/* Method load() reloads the object from the StorageManager */
void ChristoffelSubsampleExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("basis_", basis_);
  update();
}

END_NAMESPACE_OPENTURNS
