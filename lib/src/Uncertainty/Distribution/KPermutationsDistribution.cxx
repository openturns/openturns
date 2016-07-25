//                                               -*- C++ -*-
/**
 *  @brief The KPermutationsDistribution distribution
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
#include <cmath>
#include "openturns/Collection.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/KPermutations.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/Poisson.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(KPermutationsDistribution);

static const Factory<KPermutationsDistribution> Factory_KPermutationsDistribution;

/* Default constructor */
KPermutationsDistribution::KPermutationsDistribution()
  : DiscreteDistribution()
  , k_(0)
  , n_(0)
{
  setName("KPermutationsDistribution");
  setK(1);
  setN(1);
}

/* Parameters constructor */
KPermutationsDistribution::KPermutationsDistribution(const UnsignedInteger k,
    const UnsignedInteger n)
  : DiscreteDistribution()
  , k_(0)
  , n_(0)
{
  setName("KPermutationsDistribution");
  // This method compute the range also
  setK(k);
  setN(n);
}

/* Comparison operator */
Bool KPermutationsDistribution::operator ==(const KPermutationsDistribution & other) const
{
  if (this == &other) return true;
  return (k_ == other.k_) && (n_ == other.n_);
}

Bool KPermutationsDistribution::equals(const DistributionImplementation & other) const
{
  const KPermutationsDistribution* p_other = dynamic_cast<const KPermutationsDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String KPermutationsDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << KPermutationsDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " k=" << k_
      << " n=" << n_;
  return oss;
}

String KPermutationsDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(k = " << k_ << ", n = " << n_ << ")";
  return oss;
}

/* Virtual constructor */
KPermutationsDistribution * KPermutationsDistribution::clone() const
{
  return new KPermutationsDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void KPermutationsDistribution::computeRange()
{
  const NumericalPoint lowerBound(k_, 0.0);
  const NumericalPoint upperBound(k_, n_ - 1.0);
  const Interval::BoolCollection finiteLowerBound(k_, true);
  const Interval::BoolCollection finiteUpperBound(k_, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
NumericalPoint KPermutationsDistribution::getRealization() const
{
  NumericalPoint realization(k_);
  Indices buffer(n_);
  buffer.fill();
  for (UnsignedInteger i = 0; i < k_; ++i)
  {
    UnsignedInteger index(i + RandomGenerator::IntegerGenerate(n_ - i));
    realization[i] = buffer[index];
    buffer[index] = buffer[i];
  }
  return realization;
}

/* Get the PDF of the distribution */
NumericalScalar KPermutationsDistribution::computeLogPDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Indices x(k_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar k(point[i]);
    if ((k < -supportEpsilon_) || (k > n_ + supportEpsilon_)) return -SpecFunc::MaxNumericalScalar;
    const UnsignedInteger ik(static_cast< UnsignedInteger > (round(k)));
    if (std::abs(k - ik) > supportEpsilon_) return -SpecFunc::MaxNumericalScalar;
    x[i] = ik;
  }
  if (!x.check(n_ - 1)) return 0.0;
  return logPDFValue_;
}

NumericalScalar KPermutationsDistribution::computePDF(const NumericalPoint & point) const
{
  const NumericalScalar logPDF(computeLogPDF(point));
  if (logPDF == -SpecFunc::MaxNumericalScalar) return 0.0;
  return std::exp(logPDF);
}

/* Get the CDF of the distribution */
NumericalScalar KPermutationsDistribution::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1) return static_cast < NumericalScalar >(k_) / n_;
  NumericalPoint sortedPoint(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const NumericalScalar x(point[i]);
    if (x < -supportEpsilon_) return 0.0;
    sortedPoint[i] = std::min(n_ - 1.0, floor(x + supportEpsilon_));
  }
  std::sort(sortedPoint.begin(), sortedPoint.end());
  NumericalScalar cdfValue(1.0);
  for (UnsignedInteger i = 0; i < dimension; ++i) cdfValue *= (sortedPoint[i] + 1.0 - i) / (n_ - i);
  return cdfValue;
}

/* Compute the scalar quantile of the 1D KPermutationsDistribution distribution */
NumericalScalar KPermutationsDistribution::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const UnsignedInteger i(static_cast< UnsignedInteger >(ceil(prob * (n_ - 1.0))));
  return (tail ? n_ - 1.0 - i : i);
} // computeScalarQuantile

/* Get the i-th marginal distribution */
KPermutationsDistribution::Implementation KPermutationsDistribution::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension(getDimension());
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  return new KPermutationsDistribution(1, n_);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
KPermutationsDistribution::Implementation KPermutationsDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension(indices.getSize());
  return new KPermutationsDistribution(outputDimension, n_);
} // getMarginal(Indices)

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample KPermutationsDistribution::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  return KPermutations(k_, n_).generate();
}

/* Compute the mean of the distribution */
void KPermutationsDistribution::computeMean() const
{
  mean_ = NumericalPoint(k_, 0.5 * (n_ - 1.0));
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void KPermutationsDistribution::computeCovariance() const
{
  const NumericalScalar var((n_ * n_ - 1.0) / 12.0);
  const NumericalScalar cov(-(n_ + 1.0) / 12.0);
  covariance_ = CovarianceMatrix(k_, NumericalPoint(k_ * k_, cov));
  for (UnsignedInteger i = 0; i < k_; ++i) covariance_(i, i) = var;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
KPermutationsDistribution::NumericalPointWithDescriptionCollection KPermutationsDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    NumericalPointWithDescription point(1);
    point[0] = n_;
    Description description(1);
    description[0] = "n";
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    NumericalPointWithDescription point(2);
    Description description(2);
    point[0] = k_;
    description[0] = "k";
    point[1] = n_;
    description[1] = "n";
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* K accessor */
void KPermutationsDistribution::setK(const UnsignedInteger k)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (k != k_)
  {
    k_ = k;
    logPDFValue_ = SpecFunc::LnGamma(k_ + 1) - SpecFunc::LnGamma(n_ + 1);
    setDimension(k);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
  }
}

/* K accessor */
UnsignedInteger KPermutationsDistribution::getK() const
{
  return k_;
}

/* N accessor */
void KPermutationsDistribution::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (n != n_)
  {
    n_ = n;
    logPDFValue_ = SpecFunc::LnGamma(k_ + 1) - SpecFunc::LnGamma(n_ + 1);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

UnsignedInteger KPermutationsDistribution::getN() const
{
  return n_;
}

/* Method save() stores the object through the StorageManager */
void KPermutationsDistribution::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "logPDFValue_", logPDFValue_ );
}

/* Method load() reloads the object from the StorageManager */
void KPermutationsDistribution::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "logPDFValue_", logPDFValue_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
