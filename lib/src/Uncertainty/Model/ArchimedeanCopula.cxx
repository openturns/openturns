//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for archimedean copulas
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

#include "openturns/ArchimedeanCopula.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ArchimedeanCopula)


/* Default constructor */
ArchimedeanCopula::ArchimedeanCopula()
  : DistributionImplementation()
{
  isCopula_ = true;
  setDimension(2);
}

/* Virtual constructor */
ArchimedeanCopula * ArchimedeanCopula::clone() const
{
  return new ArchimedeanCopula(*this);
}

/* Comparison operator */
Bool ArchimedeanCopula::operator ==(const ArchimedeanCopula & other) const
{
  return (this == &other);
}

/* String converter */
String ArchimedeanCopula::__repr__() const
{
  OSS oss;
  oss << "class=" << ArchimedeanCopula::GetClassName();
  return oss;
}

/* Get the PDF of the distribution */
Scalar ArchimedeanCopula::computePDF(const Point & point) const
{
  if ((point[0] <= 0.0) || (point[1] <= 0.0) || (point[0] >= 1.0) || point[1] >= 1.0) return 0.0;
  Scalar sum = computeArchimedeanGenerator(point[0]) + computeArchimedeanGenerator(point[1]);
  Scalar inv = computeInverseArchimedeanGenerator(sum);
  Scalar den = computeArchimedeanGeneratorDerivative(inv);
  return -computeArchimedeanGeneratorSecondDerivative(inv) * computeArchimedeanGeneratorDerivative(point[0]) * computeArchimedeanGeneratorDerivative(point[1]) / (den * den * den);
}

/* Get the CDF of the distribution */
Scalar ArchimedeanCopula::computeCDF(const Point & point) const
{
  if ((point[0] <= 0.0) || (point[1] <= 0.0)) return 0.0;
  if ((point[0] >= 1.0) && (point[1] >= 1.0)) return 1.0;
  if (point[0] >= 1.0) return point[1];
  if (point[1] >= 1.0) return point[0];
  return computeInverseArchimedeanGenerator(computeArchimedeanGenerator(point[0]) + computeArchimedeanGenerator(point[1]));
}

Scalar ArchimedeanCopula::computeComplementaryCDF(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::computeComplementaryCDF(const Point & point) const";
}

/* Compute the probability content of an interval */
Scalar ArchimedeanCopula::computeProbability(const Interval & interval) const
{
  Interval intersect(interval.intersect(Interval(2)));
  // Empty interval
  if (intersect.isEmpty()) return 0.0;
  Point lowerBound(intersect.getLowerBound());
  Point upperBound(intersect.getUpperBound());
  Scalar uuCDF = computeCDF(upperBound);
  Scalar llCDF = computeCDF(lowerBound);
  Point ul(2);
  ul[0] = upperBound[0];
  ul[1] = lowerBound[1];
  Scalar ulCDF = computeCDF(ul);
  Point lu(2);
  lu[0] = lowerBound[0];
  lu[1] = upperBound[1];
  Scalar luCDF = computeCDF(lu);
  return uuCDF - ulCDF - luCDF + llCDF;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar ArchimedeanCopula::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // If x is outside of the range of the marginal distribution
  if ((x <= 0.0) || (x >= 1.0)) return 0.0;
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return 1.0;
  const Scalar z = y[0];
  // If the conditioning variable is outside of the range of the marginal distribution
  if ((z <= 0.0) || (z >= 1.0)) return 0.0;
  Point point(2);
  point[0] = z;
  point[1] = x;
  return computePDF(point);
}


/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
Scalar ArchimedeanCopula::computeArchimedeanGenerator(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::computeArchimedeanGenerator(const Scalar t) const";
}

/* Compute the inverse of the archimedean generator */
Scalar ArchimedeanCopula::computeInverseArchimedeanGenerator(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::computeInverseArchimedeanGenerator(const Scalar t) const";
}

/* Compute the derivative of the density generator */
Scalar ArchimedeanCopula::computeArchimedeanGeneratorDerivative(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::computeArchimedeanGeneratorDerivative(const Scalar t) const";
}

/* Compute the seconde derivative of the density generator */
Scalar ArchimedeanCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar t) const";
}

/* Tell if the distribution has elliptical copula */
Bool ArchimedeanCopula::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

/* Tell if the distribution has independent copula */
Bool ArchimedeanCopula::hasIndependentCopula() const
{
  throw NotYetImplementedException(HERE) << "In ArchimedeanCopula::hasIndependentCopula() const";
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution ArchimedeanCopula::getMarginal(const Indices & indices) const
{
  if (!indices.check(2)) throw InvalidArgumentException(HERE) << "The indices of an archimedean copula must be in the range [0, 1] and must be different";
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  // Only one indice is needed, call the specialized method
  if (outputDimension == 1) return getMarginal(indices[0]);
  // The indices correspond to all the components, with a possible transposition of the two components.
  // As an archimedean copula is exchangeable, the resulting distribution is the copula itself, we just
  // have to transpose the descriptions
  ArchimedeanCopula * result(clone());
  if (indices[0] != 0)
  {
    Description description(getDescription());
    String tmp(description[0]);
    description[0] = description[1];
    description[1] = tmp;
    result->setDescription(description);
  }
  return result;
}

END_NAMESPACE_OPENTURNS
