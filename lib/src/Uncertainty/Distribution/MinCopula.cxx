//                                               -*- C++ -*-
/**
 *  @brief A class that implements an independent copula
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MinCopula.hxx"
#include "openturns/Indices.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Description.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MinCopula)

static const Factory<MinCopula> Factory_MinCopula;

/* Default constructor */
MinCopula::MinCopula(const UnsignedInteger dim)
  : DistributionImplementation()
{
  isCopula_ = true;
  setName("MinCopula");
  // The range is generic for all the copulas
  setDimension(dim);
  computeRange();
}

/* Comparison operator */
Bool MinCopula::operator ==(const MinCopula & other) const
{
  if (this == &other) return true;
  return getDimension() == other.getDimension();
}

Bool MinCopula::equals(const DistributionImplementation & other) const
{
  const MinCopula* p_other = dynamic_cast<const MinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MinCopula::__repr__() const
{
  OSS oss;
  oss << "class=" << MinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  return oss;
}

String MinCopula::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(dim = " << getDimension() << ")";
  return oss;
}

/* Virtual constructor */
MinCopula * MinCopula::clone() const
{
  return new MinCopula(*this);
}

/* Get one realization of the distribution */
Point MinCopula::getRealization() const
{
  return Point(getDimension(), RandomGenerator::Generate());
}

/* Get the DDF of the distribution */
Point MinCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  return Point(dimension, 0.0);
}

/* Get the PDF of the distribution */
Scalar MinCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  if ((u <= 0.0) || (u > 1.0)) return 0.0;
  for (UnsignedInteger i = 1; i < dimension; ++i) if (u != point[i]) return 0.0;
  return u;
}

/* Get the CDF of the distribution */
Scalar MinCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  return std::max(0.0, std::min(1.0, *std::min_element(point.begin(), point.end())));
} // computeCDF

/* Compute the survival function */
Scalar MinCopula::computeSurvivalFunction(const Point & point) const
{
  return computeCDF(Point(getDimension(), 1.0) - point);
}

/* Get the quantile of the distribution */
Point MinCopula::computeQuantile(const Scalar prob,
                                 const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  // Special case for boarding values
  if (q == 0.0) return getRange().getLowerBound();
  if (q == 1.0) return getRange().getUpperBound();
  return Point(getDimension(), q);
}

/* Compute the entropy of the distribution */
Scalar MinCopula::computeEntropy() const
{
  return 1.0;
}

/* Compute the covariance of the distribution */
void MinCopula::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension, Collection<Scalar>(dimension * dimension, 1.0 / 12.0));
  isAlreadyComputedCovariance_ = true;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix MinCopula::getKendallTau() const
{
  const UnsignedInteger dimension = getDimension();
  return CorrelationMatrix(dimension, Collection<Scalar>(dimension * dimension, 1.0));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MinCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  return new MinCopula(indices.getSize());
}

/* Get the isoprobabilist transformation */
MinCopula::IsoProbabilisticTransformation MinCopula::getIsoProbabilisticTransformation() const
{
  throw NotDefinedException(HERE) << "Error: no iso-probabilistic transformation can be defined for the Min copula.";
}

/* Get the inverse isoprobabilist transformation */
MinCopula::InverseIsoProbabilisticTransformation MinCopula::getInverseIsoProbabilisticTransformation() const
{
  throw NotDefinedException(HERE) << "Error: no inverse iso-probabilistic transformation can be defined for the Min copula.";
}

/* Check if the distribution is elliptical */
Bool MinCopula::isElliptical() const
{
  return false;
}

/* Check if the distribution is continuos */
Bool MinCopula::isContinuous() const
{
  return false;
}

/* Tell if the distribution has elliptical copula */
Bool MinCopula::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

/* Tell if the distribution has independent copula */
Bool MinCopula::hasIndependentCopula() const
{
  return (getDimension() == 1);
}

/* Method save() stores the object through the StorageManager */
void MinCopula::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void MinCopula::load(Advocate & adv)
{
  // The range is generic for all the copulas
  DistributionImplementation::load(adv);
  computeRange();
}

END_NAMESPACE_OPENTURNS
