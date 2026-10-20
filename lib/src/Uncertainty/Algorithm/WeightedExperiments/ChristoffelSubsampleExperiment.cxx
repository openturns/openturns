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
#include "openturns/RandomGenerator.hxx"

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
  return hasEqualBase(other) && (certified_ == other.certified_) && (gamma_ == other.gamma_) && (christoffelDistribution_ == other.christoffelDistribution_);
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
      << " spaceDimension=" << spaceDimension_
      << " certified=" << (certified_ ? "true" : "false")
      << " gamma=" << gamma_;
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

/* Largest m >= 1 such that gamma m log(m) <= size.  The search is bounded
   by size because m > n is never useful, and it is a binary search to
   avoid looping up to a large target size. */
UnsignedInteger ChristoffelSubsampleExperiment::ComputeSpaceDimension(const UnsignedInteger size,
    const Scalar gamma)
{
  if (!std::isfinite(gamma) || !(gamma > 0.0))
    throw InvalidArgumentException(HERE) << "Error: expected a finite positive ChristoffelSubsampleExperiment-Gamma, got gamma=" << gamma;
  if (size == 0) return 0;
  UnsignedInteger lower = 1;
  UnsignedInteger upper = size;
  while (lower < upper)
  {
    const UnsignedInteger middle = lower + (upper - lower + 1) / 2;
    const Scalar budget = gamma * middle * std::log(static_cast<Scalar>(middle));
    if (std::isfinite(budget) && (budget <= static_cast<Scalar>(size)))
      lower = middle;
    else
      upper = middle - 1;
  }
  return lower;
}

/* Deduce the space dimension from the target size: identity for the default
   square design, gamma m log(m) <= n when the Certified key is true.  The
   basis capacity is applied in update(). */
UnsignedInteger ChristoffelSubsampleExperiment::DeduceSpaceDimension(const UnsignedInteger size)
{
  if (!ResourceMap::GetAsBool("ChristoffelSubsampleExperiment-Certified")) return size;
  const Scalar gamma = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-Gamma");
  return ComputeSpaceDimension(size, gamma);
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

/* Sample generation with weights: in certified mode, n independent
   Christoffel draws with no thinning; otherwise an exact projection DPP
   for the first m points and independent Christoffel draws for the
   remaining n-m points */
Sample ChristoffelSubsampleExperiment::generateWithWeights(Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  if ((spaceDimension_ == 0) || (christoffelDistribution_.getSize() == 0))
    throw InvalidArgumentException(HERE) << "Error: no orthogonal basis is configured, call setOrthogonalBasis() before generating.";
  if (certified_)
  {
    Sample design(christoffelDistribution_.getSample(size_));
    design.setDescription(distribution_.getDescription());
    weightsOut = computeImportanceWeights(design);
    checkFrame(design, weightsOut);
    return design;
  }
  Sample design(drawProjectionDPP(spaceDimension_));
  if (size_ > spaceDimension_)
  {
    const Sample fill(drawChristoffelPoints(size_ - spaceDimension_));
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
  weightsOut = computeImportanceWeights(permuted);
  checkFrame(permuted, weightsOut);
  return permuted;
}

/* Sample generation with weights from supplied points: exact volume sample
   when the supplied sample already contains the target size, conditional
   projection DPP otherwise */
Sample ChristoffelSubsampleExperiment::generateWithWeights(const Sample & suppliedSample, Point & weightsOut) const
{
  if (size_ == 0) throw InvalidArgumentException(HERE) << "Error: expected a positive experiment size.";
  if ((spaceDimension_ == 0) || (christoffelDistribution_.getSize() == 0))
    throw InvalidArgumentException(HERE) << "Error: no orthogonal basis is configured, call setOrthogonalBasis() before generating.";
  if (suppliedSample.getDimension() != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: expected supplied points of dimension=" << distribution_.getDimension() << ", got dimension=" << suppliedSample.getDimension();
  const UnsignedInteger suppliedSize = suppliedSample.getSize();
  if (suppliedSize == 0) return generateWithWeights(weightsOut);
  if (certified_)
    throw InvalidArgumentException(HERE) << "Error: the certified mode only generates independent Christoffel points; supplied points are not supported.";

  if (suppliedSize >= size_)
  {
    // Exact volume sample of the target size from the supplied candidates.
    const Matrix features(computeFeatures(suppliedSample));
    const Indices kept(ComputeVolumeSample(features, size_));
    Sample design(size_, distribution_.getDimension());
    design.setDescription(distribution_.getDescription());
    for (UnsignedInteger i = 0; i < size_; ++i) design[i] = suppliedSample[kept[i]];
    weightsOut = computeImportanceWeights(design);
    checkFrame(design, weightsOut);
    return design;
  }

  // A conditional projection DPP can only be anchored on at most m supplied
  // points: m independent feature vectors are needed to span the space.
  if (suppliedSize > spaceDimension_)
    throw InvalidArgumentException(HERE) << "Error: cannot condition a projection DPP of size=" << spaceDimension_ << " on " << suppliedSize << " supplied points.";
  Sample design(drawProjectionDPP(spaceDimension_, suppliedSample));
  if (size_ > spaceDimension_)
  {
    const Sample fill(drawChristoffelPoints(size_ - spaceDimension_));
    for (UnsignedInteger i = 0; i < fill.getSize(); ++i) design.add(fill[i]);
  }
  design.setDescription(distribution_.getDescription());
  weightsOut = computeImportanceWeights(design);
  checkFrame(design, weightsOut);
  return design;
}

/* Draw fresh Christoffel points */
Sample ChristoffelSubsampleExperiment::drawChristoffelPoints(const UnsignedInteger count) const
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

/* Density ratios of design points, reference over Christoffel law */
Point ChristoffelSubsampleExperiment::computeImportanceWeights(const Sample & sample) const
{
  const UnsignedInteger sampleSize = sample.getSize();
  Point weights(sampleSize);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    // A point outside the numerical range has a null law density: the ratio
    // is undefined there, so keep such a point out of the weighted design.
    const Scalar lawPDF = christoffelDistribution_.computePDF(sample[i]);
    Scalar weight = (lawPDF > 0.0) ? distribution_.computePDF(sample[i]) / lawPDF : 0.0;
    // Recover weights whose direct ratio underflowed or overflowed by working
    // in the log domain. The law and reference log-densities are finite for
    // points in the numerical range; a null reference density stays null.
    if (!(weight > 0.0) || !std::isfinite(weight))
    {
      const Scalar logLawPDF = christoffelDistribution_.computeLogPDF(sample[i]);
      const Scalar logReferencePDF = distribution_.computeLogPDF(sample[i]);
      if ((logLawPDF > SpecFunc::LowestScalar) && std::isfinite(logLawPDF) &&
          (logReferencePDF > SpecFunc::LowestScalar) && std::isfinite(logReferencePDF))
      {
        const Scalar logWeight = logReferencePDF - logLawPDF;
        if (std::isfinite(logWeight) && (logWeight <= SpecFunc::LogMaxScalar))
          weight = std::exp(logWeight);
      }
    }
    weights[i] = (weight > 0.0) && std::isfinite(weight) ? weight : 0.0;
  }
  return weights;
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
  const Matrix features(computeFeatures(sample));
  Indices all(sampleSize);
  for (UnsignedInteger k = 0; k < sampleSize; ++k) all[k] = k;
  const SymmetricMatrix gramian(ComputeGramian(features, weights, all));
  return gramian.computeEigenValues();
}

/* Identity index set of given size */
Indices ChristoffelSubsampleExperiment::IdentityIndices(const UnsignedInteger size)
{
  Indices indices(size);
  for (UnsignedInteger i = 0; i < size; ++i) indices[i] = i;
  return indices;
}

/* Gramian of a subset of sqrt(w) * phi, via BLAS.  The features are the
   raw basis values; designWeights scale the selected rows by sqrt(w). */
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
  // the Christoffel law does not build them a second time.  The Certified key
  // switches to m = argmax gamma m log(m) <= n; otherwise m = n-k is tried.
  certified_ = ResourceMap::GetAsBool("ChristoffelSubsampleExperiment-Certified");
  gamma_ = ResourceMap::GetAsScalar("ChristoffelSubsampleExperiment-Gamma");
  UnsignedInteger dimension = certified_ ? ComputeSpaceDimension(size_, gamma_) : size_;
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
