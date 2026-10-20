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
#include <limits>
#include "openturns/ChristoffelSubsampleExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/RandomGenerator.hxx"
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

/* Comparison operator: base members plus the Christoffel law, which embeds
   the value-based basis comparison */
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

/* Largest space dimension m = n-k for the target size n with an inexhaustible
   basis (k=0): the basis capacity trial needs the basis itself, see update() */
UnsignedInteger ChristoffelSubsampleExperiment::DeduceSpaceDimension(const UnsignedInteger size)
{
  return size;
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

/* Sample generation with weights: exact projection DPP for the first m
   points, then independent Christoffel draws for the remaining n-m points */
Sample ChristoffelSubsampleExperiment::generateWithWeights(Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  if ((spaceDimension_ == 0) || (christoffelDistribution_.getSize() == 0))
    throw InvalidArgumentException(HERE) << "Error: no orthogonal basis is configured, call setOrthogonalBasis() before generating.";
  Sample design(drawProjectionDPP(spaceDimension_));
  if (size_ > spaceDimension_)
  {
    const Sample fill(drawPool(size_ - spaceDimension_));
    for (UnsignedInteger i = 0; i < fill.getSize(); ++i) design.add(fill[i]);
  }
  design.setDescription(distribution_.getDescription());
  // The DPP block and the independent fill are not exchangeable: permute so
  // that the returned ordering does not expose which point comes from where.
  Indices order(IdentityIndices(size_));
  for (UnsignedInteger i = size_; i > 1; --i)
  {
    const UnsignedInteger j = RandomGenerator::IntegerGenerate(i);
    const UnsignedInteger swap = order[i - 1];
    order[i - 1] = order[j];
    order[j] = swap;
  }
  Sample permuted(size_, distribution_.getDimension());
  permuted.setDescription(distribution_.getDescription());
  for (UnsignedInteger i = 0; i < size_; ++i) permuted[i] = design[order[i]];
  weightsOut = computePoolWeights(permuted);
  checkFrame(permuted, weightsOut);
  return permuted;
}

/* Sample generation with weights from supplied points: exact volume sample
   when the supplied pool already contains the target size, conditional
   projection DPP otherwise */
Sample ChristoffelSubsampleExperiment::generateWithWeights(const Sample & poolSample, Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  if ((spaceDimension_ == 0) || (christoffelDistribution_.getSize() == 0))
    throw InvalidArgumentException(HERE) << "Error: no orthogonal basis is configured, call setOrthogonalBasis() before generating.";
  if (poolSample.getDimension() != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: expected supplied points of dimension=" << distribution_.getDimension() << ", got dimension=" << poolSample.getDimension();
  const UnsignedInteger suppliedSize = poolSample.getSize();
  if (suppliedSize == 0) return generateWithWeights(weightsOut);

  if (suppliedSize >= size_)
  {
    // Exact volume sample of the target size from the supplied candidates.
    const Matrix features(computeFeatures(poolSample));
    const Indices kept(ComputeVolumeSample(features, size_));
    Sample design(size_, distribution_.getDimension());
    design.setDescription(distribution_.getDescription());
    for (UnsignedInteger i = 0; i < size_; ++i) design[i] = poolSample[kept[i]];
    weightsOut = computePoolWeights(design);
    checkFrame(design, weightsOut);
    return design;
  }

  // A conditional projection DPP can only be anchored on at most m supplied
  // points: m independent feature vectors are needed to span the space.
  if (suppliedSize > spaceDimension_)
    throw InvalidArgumentException(HERE) << "Error: cannot condition a projection DPP of size=" << spaceDimension_ << " on " << suppliedSize << " supplied points.";
  Sample design(drawProjectionDPP(spaceDimension_, poolSample));
  if (size_ > spaceDimension_)
  {
    const Sample fill(drawPool(size_ - spaceDimension_));
    for (UnsignedInteger i = 0; i < fill.getSize(); ++i) design.add(fill[i]);
  }
  design.setDescription(distribution_.getDescription());
  weightsOut = computePoolWeights(design);
  checkFrame(design, weightsOut);
  return design;
}

/* Target pool size from the oversampling factor */
UnsignedInteger ChristoffelSubsampleExperiment::computePoolSize() const
{
  const Scalar poolFactor = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-PoolOversamplingFactor");
  if (!(poolFactor >= 1.0) || !std::isfinite(poolFactor)) throw InvalidArgumentException(HERE) << "Error: expected a finite pool oversampling factor not smaller than 1, here factor=" << poolFactor;
  const Scalar poolScale = poolFactor * size_;
  if (!std::isfinite(poolScale) || (poolScale >= static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max())))
    throw InvalidArgumentException(HERE) << "Error: the pool oversampling factor is too large, here factor=" << poolFactor;
  return std::max(size_, static_cast<UnsignedInteger>(std::ceil(poolScale)));
}

/* Draw fresh pool points from the Christoffel law */
Sample ChristoffelSubsampleExperiment::drawPool(const UnsignedInteger count) const
{
  if (count == 0) return Sample(0, distribution_.getDimension());
  return christoffelDistribution_.getSample(count);
}

/* Raw finite-basis features, one row per point */
Matrix ChristoffelSubsampleExperiment::computeFeatures(const Sample & sample,
    const Bool checkFinite) const
{
  const Basis finiteBasis(christoffelDistribution_.getBasis());
  const UnsignedInteger dimension = finiteBasis.getSize();
  const UnsignedInteger sampleSize = sample.getSize();
  Matrix features(sampleSize, dimension);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    const Point point(sample[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar value = finiteBasis[j](point)[0];
      if (checkFinite && !std::isfinite(value))
        throw InternalException(HERE) << "Error: non-finite basis value at point index " << i << " and function index " << j << ".";
      features(i, j) = value;
    }
  }
  return features;
}

/* Exact projection DPP sample of the given size, by sequential conditional
   sampling.  At step k the residual kernel is
   K_k(x,x) = ||phi(x)||^2 - c(x)^T (V^T V)^-1 c(x), with c(x) the
   scalar products of phi(x) with the k accepted feature vectors V.  The
   proposal is the Christoffel law, whose density is K_0/m, so accepting
   with probability K_k/K_0 is exact.  The mean number of proposals is
   m H_m. */
Sample ChristoffelSubsampleExperiment::drawProjectionDPP(const UnsignedInteger size,
    const Sample & fixed) const
{
  const UnsignedInteger dimension = spaceDimension_;
  const UnsignedInteger fixedSize = fixed.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive projection DPP size.";
  if (size > dimension) throw InvalidArgumentException(HERE) << "Error: expected a projection DPP size not larger than the space dimension, here size=" << size << " and dimension=" << dimension << ".";
  if (fixedSize > size) throw InvalidArgumentException(HERE) << "Error: expected at most size supplied points, here supplied=" << fixedSize << " and size=" << size << ".";
  if ((fixedSize > 0) && (fixed.getDimension() != distribution_.getDimension()))
    throw InvalidArgumentException(HERE) << "Error: expected supplied points of dimension=" << distribution_.getDimension() << ", got dimension=" << fixed.getDimension() << ".";
  Sample design(size, distribution_.getDimension());
  design.setDescription(distribution_.getDescription());
  for (UnsignedInteger i = 0; i < fixedSize; ++i) design[i] = fixed[i];
  Matrix acceptedFeatures(dimension, size);
  Matrix inverseGram(0, 0);
  // A valid conditional proposal has a positive acceptance probability.  This
  // guard only stops a pathological numerical failure from looping forever.
  const UnsignedInteger maxAttempts = 1000000;
  UnsignedInteger accepted = 0;
  UnsignedInteger attempts = 0;
  while (accepted < size)
  {
    const Bool fixedPoint = accepted < fixedSize;
    Point proposal;
    if (fixedPoint) proposal = fixed[accepted];
    else
    {
      ++attempts;
      if (attempts > maxAttempts)
        throw InternalException(HERE) << "Error: no admissible projection DPP proposal after " << maxAttempts << " attempts.";
      proposal = christoffelDistribution_.getRealization();
    }
    const Sample proposalSample(1, proposal);
    const Matrix rawFeatures(computeFeatures(proposalSample, false));
    Point feature(dimension);
    Scalar norm2 = 0.0;
    Bool finite = true;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      feature[j] = rawFeatures(0, j);
      if (!std::isfinite(feature[j])) finite = false;
      norm2 += feature[j] * feature[j];
    }
    if (!finite || !(norm2 > 0.0) || !std::isfinite(norm2))
    {
      if (fixedPoint) throw InvalidArgumentException(HERE) << "Error: the supplied point at index " << accepted << " has non-finite or null features.";
      continue;
    }
    Scalar residual = norm2;
    Point cross(accepted);
    Point solved(accepted);
    if (accepted > 0)
    {
      for (UnsignedInteger r = 0; r < accepted; ++r)
      {
        Scalar value = 0.0;
        for (UnsignedInteger j = 0; j < dimension; ++j) value += acceptedFeatures(j, r) * feature[j];
        cross[r] = value;
      }
      Scalar quadratic = 0.0;
      for (UnsignedInteger r = 0; r < accepted; ++r)
      {
        Scalar value = 0.0;
        for (UnsignedInteger b = 0; b < accepted; ++b) value += inverseGram(r, b) * cross[b];
        solved[r] = value;
        quadratic += cross[r] * value;
      }
      residual = norm2 - quadratic;
      if (!(residual > 0.0) || !std::isfinite(residual))
      {
        if (fixedPoint) throw InvalidArgumentException(HERE) << "Error: the supplied feature vectors are linearly dependent at index " << accepted << ".";
        continue;
      }
      if (!fixedPoint)
      {
        Scalar acceptance = residual / norm2;
        if (!std::isfinite(acceptance)) continue;
        acceptance = std::min(1.0, std::max(0.0, acceptance));
        if (RandomGenerator::Generate() >= acceptance) continue;
      }
    }
    for (UnsignedInteger j = 0; j < dimension; ++j) acceptedFeatures(j, accepted) = feature[j];
    if (accepted == 0)
    {
      inverseGram = Matrix(1, 1);
      inverseGram(0, 0) = 1.0 / norm2;
    }
    else
    {
      Matrix next(accepted + 1, accepted + 1);
      for (UnsignedInteger a = 0; a < accepted; ++a)
      {
        for (UnsignedInteger b = 0; b < accepted; ++b)
          next(a, b) = inverseGram(a, b) + solved[a] * solved[b] / residual;
        next(a, accepted) = -solved[a] / residual;
        next(accepted, a) = -solved[a] / residual;
      }
      next(accepted, accepted) = 1.0 / residual;
      inverseGram = next;
    }
    design[accepted] = proposal;
    ++accepted;
    attempts = 0;
  }
  return design;
}

/* Warn when the weighted design misses the frame bounds */
void ChristoffelSubsampleExperiment::checkFrame(const Sample & sample,
    const Point & weights) const
{
  const Scalar tolerance = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance");
  if (!(tolerance >= 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE) << "Error: expected a non-negative finite frame tolerance, here tolerance=" << tolerance;
  const UnsignedInteger sampleSize = sample.getSize();
  if (weights.getSize() != sampleSize)
    throw InvalidArgumentException(HERE) << "Error: expected as many weights as design points.";
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
    if (!(weights[i] > 0.0) || !std::isfinite(weights[i]))
      throw InternalException(HERE) << "Error: the design contains a zero or non-finite importance weight at index " << i << ".";
  const Point eigenValues(computeDesignEigenvalues(sample, weights));
  Scalar minEigenvalue = eigenValues[0];
  Scalar maxEigenvalue = eigenValues[0];
  for (UnsignedInteger i = 1; i < eigenValues.getSize(); ++i)
  {
    minEigenvalue = std::min(minEigenvalue, eigenValues[i]);
    maxEigenvalue = std::max(maxEigenvalue, eigenValues[i]);
  }
  if (minEigenvalue < 1.0 - tolerance || maxEigenvalue > 1.0 + tolerance)
    LOGWARN(OSS() << "Warning: the weighted design misses the frame bounds [" << 1.0 - tolerance << ", " << 1.0 + tolerance << "], got [" << minEigenvalue << ", " << maxEigenvalue << "]");
  if (minEigenvalue <= SpecFunc::Precision)
    LOGWARN(OSS() << "Warning: the weighted design is numerically rank deficient, got minimum eigenvalue=" << minEigenvalue);
}

/* Density ratios of pool points, reference over Christoffel law */
Point ChristoffelSubsampleExperiment::computePoolWeights(const Sample & pool) const
{
  const UnsignedInteger poolSize = pool.getSize();
  Point poolWeights(poolSize);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    // A point outside the numerical range has a null law density: the ratio
    // is undefined there, so keep such a point out of the weighted design.
    const Scalar lawPDF = christoffelDistribution_.computePDF(pool[i]);
    Scalar weight = (lawPDF > 0.0) ? distribution_.computePDF(pool[i]) / lawPDF : 0.0;
    // Recover weights whose direct ratio underflowed or overflowed by working
    // in the log domain. The law and reference log-densities are finite for
    // points in the numerical range; a null reference density stays null.
    if (!(weight > 0.0) || !std::isfinite(weight))
    {
      const Scalar logLawPDF = christoffelDistribution_.computeLogPDF(pool[i]);
      const Scalar logReferencePDF = distribution_.computeLogPDF(pool[i]);
      if ((logLawPDF > SpecFunc::LowestScalar) && std::isfinite(logLawPDF) &&
          (logReferencePDF > SpecFunc::LowestScalar) && std::isfinite(logReferencePDF))
      {
        const Scalar logWeight = logReferencePDF - logLawPDF;
        if (std::isfinite(logWeight) && (logWeight <= SpecFunc::LogMaxScalar))
          weight = std::exp(logWeight);
      }
    }
    poolWeights[i] = (weight > 0.0) && std::isfinite(weight) ? weight : 0.0;
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
  for (UnsignedInteger k = 0; k < sampleSize; ++k)
    if (!(weights[k] > 0.0) || !std::isfinite(weights[k]))
      throw InvalidArgumentException(HERE) << "Error: expected positive finite design weights, got weight=" << weights[k] << " at index " << k << ".";
  const Matrix features(computePoolFeatures(sample, weights));
  Indices all(sampleSize);
  for (UnsignedInteger k = 0; k < sampleSize; ++k) all[k] = k;
  // computePoolFeatures has applied the design weights, so scale by one here
  const Point unitWeights(sampleSize, 1.0);
  const SymmetricMatrix gramian(ComputeGramian(features, unitWeights, all));
  return gramian.computeEigenValues();
}

/* Thin a pool to size_, weigh and frame-check the design, growing a
   numerically unusable pool up to the cap */
Sample ChristoffelSubsampleExperiment::finalizeDesign(const Sample & pool,
    const Point & poolWeights,
    Point & weightsOut) const
{
  const Scalar maxFactor = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-PoolMaxOversamplingFactor");
  if (!(maxFactor > 0.0) || !std::isfinite(maxFactor)) throw InvalidArgumentException(HERE) << "Error: expected a positive finite pool maximum oversampling factor, here factor=" << maxFactor;
  const Scalar tolerance = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-FrameTolerance");
  if (!(tolerance >= 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE) << "Error: expected a non-negative finite frame tolerance, here tolerance=" << tolerance;
  const Scalar capScale = maxFactor * size_;
  if (!std::isfinite(capScale) || (capScale >= static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max())))
    throw InvalidArgumentException(HERE) << "Error: the pool maximum oversampling factor is too large, here factor=" << maxFactor;
  // Cap the adaptive growth, never below the incoming pool: both overloads
  // hand over at least poolSize() >= 1 points, so the growth below progresses
  const UnsignedInteger poolCap = std::max(pool.getSize(), static_cast<UnsignedInteger>(std::ceil(capScale)));
  Sample grownPool(pool);
  Point grownWeights(poolWeights);
  while (true)
  {
    // All-zero weights (for example a pool outside the numerical range) cannot
    // feed either thinning method: grow first, and report the cap explicitly.
    Bool hasPositiveWeight = false;
    for (UnsignedInteger i = 0; i < grownPool.getSize(); ++i)
      if (grownWeights[i] > 0.0)
      {
        hasPositiveWeight = true;
        break;
      }
    if (!hasPositiveWeight)
    {
      if (grownPool.getSize() >= poolCap)
        throw InternalException(HERE) << "Error: the pool contains no positive importance weight at the pool cap; increase the pool size or oversampling factor.";
      const UnsignedInteger grownSize = std::min(2 * grownPool.getSize(), poolCap);
      const Sample fresh(drawPool(grownSize - grownPool.getSize()));
      for (UnsignedInteger i = 0; i < fresh.getSize(); ++i) grownPool.add(fresh[i]);
      grownWeights = computePoolWeights(grownPool);
      continue;
    }
    Indices kept;
    Point selectionWeights(grownPool.getSize(), 1.0);
    // sqrt(w) * phi evaluations shared by the thinning step and the frame check below
    const Matrix poolFeatures(computePoolFeatures(grownPool, grownWeights));
    const String thinningMethod = ResourceMap::GetAsString("ChristoffelSubsampleExperiment-ThinningMethod");
    if (thinningMethod == "Barrier")
    {
      Point barrierWeights;
      barrierSelection(grownPool, poolFeatures, kept, barrierWeights);
      Scalar totalWeight = 0.0;
      for (UnsignedInteger k = 0; k < size_; ++k) totalWeight += barrierWeights[kept[k]] * grownWeights[kept[k]];
      if (!(totalWeight > 0.0)) throw InternalException(HERE) << "Error: null total barrier weight.";
      for (UnsignedInteger k = 0; k < size_; ++k) selectionWeights[kept[k]] = size_ * barrierWeights[kept[k]] / totalWeight;
    }
    else if (thinningMethod == "Removal")
    {
      kept = thinIndices(grownPool, poolFeatures);
    }
    else throw InvalidArgumentException(HERE) << "Error: unknown thinning method=" << thinningMethod << ", expected Barrier or Removal.";
    Sample sample(size_, distribution_.getDimension());
    sample.setDescription(distribution_.getDescription());
    weightsOut = Point(size_);
    for (UnsignedInteger k = 0; k < size_; ++k) weightsOut[k] = selectionWeights[kept[k]] * grownWeights[kept[k]];
    // Bulk row copy through the contiguous storage
    const UnsignedInteger inputDimension = distribution_.getDimension();
    auto poolBegin = grownPool.getImplementation()->data_begin();
    auto sampleBegin = sample.getImplementation()->data_begin();
    for (UnsignedInteger k = 0; k < size_; ++k)
      std::copy(poolBegin + kept[k] * inputDimension, poolBegin + (kept[k] + 1) * inputDimension, sampleBegin + k * inputDimension);
    Bool positiveWeights = true;
    for (UnsignedInteger k = 0; k < size_; ++k)
      if (!(weightsOut[k] > 0.0) || !std::isfinite(weightsOut[k])) positiveWeights = false;
    // Frame check on the weighted thinned design, reusing the pool evaluations
    // above: poolFeatures already carry sqrt(pool weight), selectionWeights add
    // the barrier normalization (unit for Removal)
    const Point eigenValues(ComputeGramian(poolFeatures, selectionWeights, kept).computeEigenValues());
    Scalar minEigenvalue = eigenValues[0];
    Scalar maxEigenvalue = eigenValues[0];
    for (UnsignedInteger k = 1; k < eigenValues.getSize(); ++k)
    {
      minEigenvalue = std::min(minEigenvalue, eigenValues[k]);
      maxEigenvalue = std::max(maxEigenvalue, eigenValues[k]);
    }
    const Bool usable = (minEigenvalue > SpecFunc::Precision);
    const Bool capped = (grownPool.getSize() >= poolCap);
    if (usable && positiveWeights)
    {
      if (minEigenvalue < 1.0 - tolerance || maxEigenvalue > 1.0 + tolerance)
        LOGWARN(OSS() << "Warning: the thinned design misses the frame bounds [" << 1.0 - tolerance << ", " << 1.0 + tolerance << "], got [" << minEigenvalue << ", " << maxEigenvalue << "]");
      return sample;
    }
    if (capped)
    {
      if (!positiveWeights)
        throw InternalException(HERE) << "Error: the thinned design still contains zero or non-finite weights at the pool cap; increase the pool size or oversampling factor.";
      if (minEigenvalue < 1.0 - tolerance || maxEigenvalue > 1.0 + tolerance)
        LOGWARN(OSS() << "Warning: the thinned design misses the frame bounds [" << 1.0 - tolerance << ", " << 1.0 + tolerance << "], got [" << minEigenvalue << ", " << maxEigenvalue << "]");
      return sample;
    }
    // Degenerate pool (eg duplicates): top up with fresh draws, keeping the
    // existing points so no evaluation is wasted, and thin again
    const UnsignedInteger grownSize = std::min(2 * grownPool.getSize(), poolCap);
    const Sample fresh(drawPool(grownSize - grownPool.getSize()));
    for (UnsignedInteger i = 0; i < fresh.getSize(); ++i) grownPool.add(fresh[i]);
    grownWeights = computePoolWeights(grownPool);
  }
}

/* Identity index set of given size */
Indices ChristoffelSubsampleExperiment::IdentityIndices(const UnsignedInteger size)
{
  Indices indices(size);
  for (UnsignedInteger i = 0; i < size; ++i) indices[i] = i;
  return indices;
}

/* Gramian of a subset of sqrt(w) * phi, via BLAS. The features are already
   scaled by sqrt(pool weight); designWeights scale the selected rows by the
   remaining sqrt(design weight), as in the thinned design itself. */
SymmetricMatrix ChristoffelSubsampleExperiment::ComputeGramian(const Matrix & features,
    const Point & designWeights,
    const Indices & kept)
{
  const UnsignedInteger dimension = features.getNbColumns();
  const UnsignedInteger keptSize = kept.getSize();
  if (keptSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty kept index set.";
  if (designWeights.getSize() != features.getNbRows()) throw InvalidArgumentException(HERE) << "Error: expected as many design weights as feature rows.";
  Matrix selected(keptSize, dimension);
  for (UnsignedInteger q = 0; q < keptSize; ++q)
  {
    const Scalar rootWeight = std::sqrt(designWeights[kept[q]]);
    for (UnsignedInteger j = 0; j < dimension; ++j) selected(q, j) = rootWeight * features(kept[q], j);
  }
  const CovarianceMatrix gram(selected.computeGram(true));
  return gram / (1.0 * keptSize);
}

/* Exact volume sample of the given size from the rows of a feature matrix.
   The law is proportional to det(A_S^T A_S) for every subset S of the
   requested size.  The implementation is the reverse-removal algorithm:
   when r > m rows remain, row i is removed with probability proportional
   to 1 - h_i, where h_i is its leverage score in the current Gramian and
   sum_i(1 - h_i) = r - m.  The solved Gram system and the leverage scores
   are updated by Sherman-Morrison after each removal, with a periodic exact
   refresh to control the accumulation of round-off errors. */
Indices ChristoffelSubsampleExperiment::ComputeVolumeSample(const Matrix & features,
    const UnsignedInteger size)
{
  const UnsignedInteger candidateSize = features.getNbRows();
  const UnsignedInteger dimension = features.getNbColumns();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive volume sample size.";
  if (candidateSize == 0) throw InvalidArgumentException(HERE) << "Error: expected a non-empty candidate feature matrix.";
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a candidate feature matrix with at least one column.";
  if (size > candidateSize) throw InvalidArgumentException(HERE) << "Error: expected a volume sample size not larger than the candidate count, here size=" << size << " and candidate count=" << candidateSize << ".";
  if (size < dimension) throw InvalidArgumentException(HERE) << "Error: expected a volume sample size not smaller than the feature dimension, here size=" << size << " and dimension=" << dimension << ".";
  for (UnsignedInteger i = 0; i < candidateSize; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (!std::isfinite(features(i, j)))
        throw InvalidArgumentException(HERE) << "Error: expected finite candidate features, got value=" << features(i, j) << " at row " << i << " and column " << j << ".";
  if (size == candidateSize) return IdentityIndices(size);

  Indices remaining(IdentityIndices(candidateSize));
  Matrix solved;
  Point leverage(candidateSize);

  // Rebuild the exact inverse Gramian, the solved Gram system and the
  // leverage scores from the rows that are still candidates.
  auto refresh = [&]()
  {
    const UnsignedInteger remainingSize = remaining.getSize();
    Matrix current(remainingSize, dimension);
    for (UnsignedInteger i = 0; i < remainingSize; ++i)
    {
      const UnsignedInteger candidate = remaining[i];
      for (UnsignedInteger j = 0; j < dimension; ++j) current(i, j) = features(candidate, j);
    }
    const SymmetricMatrix currentGram(current.computeGram(true));
    SymmetricMatrix inverseGram;
    try
    {
      inverseGram = currentGram.inverse();
    }
    catch (const Exception &)
    {
      throw InvalidArgumentException(HERE) << "Error: the candidate feature matrix is singular, volume sampling requires rank=" << dimension << ".";
    }
    solved = inverseGram * features.transpose();
    for (UnsignedInteger i = 0; i < remainingSize; ++i)
    {
      const UnsignedInteger candidate = remaining[i];
      Scalar value = 0.0;
      for (UnsignedInteger j = 0; j < dimension; ++j) value += features(candidate, j) * solved(j, candidate);
      leverage[candidate] = value;
    }
  };
  refresh();

  UnsignedInteger removedCount = 0;
  while (remaining.getSize() > size)
  {
    const UnsignedInteger remainingSize = remaining.getSize();
    Point probabilities(remainingSize);
    Scalar total = 0.0;
    for (UnsignedInteger i = 0; i < remainingSize; ++i)
    {
      const UnsignedInteger candidate = remaining[i];
      Scalar probability = 1.0 - leverage[candidate];
      if (!std::isfinite(probability)) probability = 0.0;
      probability = std::min(1.0, std::max(0.0, probability));
      probabilities[i] = probability;
      total += probability;
    }
    if (!(total > 0.0) || !std::isfinite(total))
      throw InternalException(HERE) << "Error: null removal probability in volume sampling, the candidate feature matrix is numerically rank deficient.";

    const Scalar target = RandomGenerator::Generate() * total;
    UnsignedInteger position = remainingSize - 1;
    Scalar cumulative = 0.0;
    for (UnsignedInteger i = 0; i < remainingSize; ++i)
    {
      cumulative += probabilities[i];
      if (target < cumulative)
      {
        position = i;
        break;
      }
    }
    const UnsignedInteger removed = remaining[position];
    const Scalar gap = 1.0 - leverage[removed];
    Bool needsRefresh = !(gap > SpecFunc::Precision) || !std::isfinite(gap);
    if (!needsRefresh)
    {
      Point direction(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j) direction[j] = solved(j, removed);
      for (UnsignedInteger i = 0; i < remainingSize; ++i)
      {
        const UnsignedInteger candidate = remaining[i];
        if (candidate == removed) continue;
        Scalar dot = 0.0;
        for (UnsignedInteger j = 0; j < dimension; ++j) dot += features(candidate, j) * direction[j];
        const Scalar ratio = dot / gap;
        for (UnsignedInteger j = 0; j < dimension; ++j) solved(j, candidate) += direction[j] * ratio;
        leverage[candidate] += dot * ratio;
        if (!std::isfinite(leverage[candidate]))
        {
          needsRefresh = true;
          break;
        }
      }
    }
    remaining.erase(remaining.begin() + position);
    ++removedCount;
    if (needsRefresh || ((removedCount % dimension) == 0)) refresh();
  }
  return remaining;
}

/* Min eigenvalue left after removing one candidate of a block, by downdate */
struct RemovalScoresPolicy
{
  const SymmetricMatrix & total_;
  const Matrix & features_;
  const Indices & kept_;
  const Scalar divisor_;
  const UnsignedInteger dimension_;
  Point & minima_;

  RemovalScoresPolicy(const SymmetricMatrix & total,
                      const Matrix & features,
                      const Indices & kept,
                      const Scalar divisor,
                      const UnsignedInteger dimension,
                      Point & minima)
    : total_(total)
    , features_(features)
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
          gramian(a, b) -= features_(idx, a) * features_(idx, b);
      gramian = gramian / divisor_;
      const Point eigenValues(gramian.computeEigenValues());
      Scalar minEigenvalue = eigenValues[0];
      for (UnsignedInteger k = 1; k < dimension_; ++k) minEigenvalue = std::min(minEigenvalue, eigenValues[k]);
      minima_[pos] = minEigenvalue;
    }
  }
}; /* end struct RemovalScoresPolicy */

/* Finite basis evaluations on a pool, scaled by sqrt(weight) */
Matrix ChristoffelSubsampleExperiment::computePoolFeatures(const Sample & pool,
    const Point & weights) const
{
  const Basis finiteBasis(christoffelDistribution_.getBasis());
  const UnsignedInteger dimension = finiteBasis.getSize();
  const UnsignedInteger poolSize = pool.getSize();
  if (weights.getSize() != poolSize) throw InvalidArgumentException(HERE) << "Error: expected as many weights as pool points.";
  Matrix features(poolSize, dimension);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
  {
    const Scalar rootWeight = std::sqrt(weights[i]);
    const Point point(pool[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar value = finiteBasis[j](point)[0];
      const Scalar scaledValue = (rootWeight == 0.0) ? 0.0 : rootWeight * value;
      if (!std::isfinite(scaledValue))
        throw InternalException(HERE) << "Error: non-finite basis value for a positive-weight pool point at index " << i << ".";
      features(i, j) = scaledValue;
    }
  }
  return features;
}

/* Greedy removal of pool points down to size_, maximizing the min eigenvalue */
Indices ChristoffelSubsampleExperiment::thinIndices(const Sample & pool,
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
          total(a, b) += features(idx, a) * features(idx, b);
    }
    Point minima(keptSize, SpecFunc::LowestScalar);
    const RemovalScoresPolicy policy(total, features, kept, 1.0 * (keptSize - 1), dimension, minima);
    const Bool parallel = christoffelDistribution_.isParallel();
    TBBImplementation::ParallelForIf(parallel && (keptSize > 256), 0, keptSize, policy, 256);
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
    const Matrix & features,
    Indices & keptOut,
    Point & barrierWeightsOut) const
{
  const UnsignedInteger poolSize = pool.getSize();
  const UnsignedInteger dimension = features.getNbColumns();
  const Scalar barrierStep = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-BarrierStep");
  if (!(barrierStep > 0.0) || !std::isfinite(barrierStep)) throw InvalidArgumentException(HERE) << "Error: expected a positive finite barrier step, here step=" << barrierStep;
  // Frame vectors v_i = sqrt(w_i / M) * phi(x_i); features already stores
  // sqrt(w_i) * phi(x_i), so only the 1/sqrt(M) factor remains.
  Matrix scaled(poolSize, dimension);
  const Scalar scale = 1.0 / std::sqrt(1.0 * poolSize);
  for (UnsignedInteger i = 0; i < poolSize; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j) scaled(i, j) = scale * features(i, j);
  SymmetricMatrix current(dimension);
  Scalar lower = -1.0 * dimension;
  Scalar upper = 1.0 * dimension;
  Indices remaining(IdentityIndices(poolSize));
  Indices selected;
  Point barrierWeights(poolSize, 0.0);
  // Trace budget: E||v||^2 = m/M, hence a mean step M/n gives a trace
  // increase of m/n per selection and tr(A) ~= m after n selections.
  const Scalar deltaCap = 1.0 * poolSize / size_;
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
    if (!(regularization > 0.0) || !std::isfinite(regularization)) throw InvalidArgumentException(HERE) << "Error: expected a positive finite barrier regularization, here value=" << regularization;
    // Re-validate the current levels after growth, then advance inside the spectrum
    lower = std::min(lower, minEig - regularization);
    upper = std::max(upper, maxEig + regularization);
    // Cap the step at half the remaining gap: a barrierStep larger than the
    // gap would stall nextLower on lower, freeze the lower potential and
    // skip every candidate. The midpoint stays strictly inside (lower, minEig).
    const Scalar nextLower = std::min(lower + barrierStep, lower + 0.5 * (minEig - lower));
    const Scalar nextUpper = std::max(upper + barrierStep, maxEig + regularization);
    SymmetricMatrix lowerBase(current);
    SymmetricMatrix upperBase(dimension);
    for (UnsignedInteger a = 0; a < dimension; ++a)
    {
      lowerBase(a, a) -= nextLower;
      for (UnsignedInteger b = 0; b < a; ++b) upperBase(a, b) = -current(a, b);
      upperBase(a, a) = nextUpper - current(a, a);
    }
    // Barrier potentials at the current and next levels, read off the spectrum:
    // tr((A - lI)^-1) = sum 1/(lambda_k - l) and tr((uI - A)^-1) = sum 1/(u - lambda_k).
    // Same values as solving with the unit vectors, in O(dimension) instead of
    // 4 * dimension dense O(dimension^3) solves per selection step.
    Scalar potentialLower = 0.0;
    Scalar potentialLowerNext = 0.0;
    Scalar potentialUpper = 0.0;
    Scalar potentialUpperNext = 0.0;
    for (UnsignedInteger k = 0; k < dimension; ++k)
    {
      potentialLower += 1.0 / (spectrum[k] - lower);
      potentialLowerNext += 1.0 / (spectrum[k] - nextLower);
      potentialUpper += 1.0 / (upper - spectrum[k]);
      potentialUpperNext += 1.0 / (nextUpper - spectrum[k]);
    }
    Scalar bestGap = SpecFunc::LowestScalar;
    UnsignedInteger bestPosition = 0;
    Scalar bestDelta = barrierStep;
    const UnsignedInteger remainingSize = remaining.getSize();
    Point gaps(remainingSize, SpecFunc::LowestScalar);
    Point stepDeltas(remainingSize, barrierStep);
    const BarrierScoresPolicy policy(lowerBase, upperBase, potentialLower, potentialLowerNext, potentialUpper, potentialUpperNext, scaled, remaining, gaps, stepDeltas, barrierStep, deltaCap, dimension);
    const Bool parallel = christoffelDistribution_.isParallel();
    TBBImplementation::ParallelForIf(parallel && (remainingSize > 256), 0, remainingSize, policy);
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
  // Largest approximation dimension m = n-k: try k=0,1,2,... until the basis
  // supplies m functions (finite bases throw beyond their capacity). Build each
  // function once here and hand the finite basis to the internal constructor, so
  // the Christoffel law does not build them a second time.
  UnsignedInteger dimension = DeduceSpaceDimension(size_);
  Basis::FunctionCollection coll(0);
  while (coll.getSize() < dimension)
  {
    try
    {
      coll.add(basis_.build(coll.getSize()));
    }
    catch (const InvalidArgumentException &)
    {
      dimension = coll.getSize();
      break;
    }
  }
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "Error: the orthogonal basis supplies no function.";
  const ChristoffelDistribution christoffel(basis_, Basis(coll), dimension);
  christoffelDistribution_ = christoffel;
  distribution_ = christoffel.getMeasure();
  spaceDimension_ = dimension;
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
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: loaded a ChristoffelSubsampleExperiment with a null size.";
  adv.loadAttribute("basis_", basis_);
  update();
}

END_NAMESPACE_OPENTURNS
