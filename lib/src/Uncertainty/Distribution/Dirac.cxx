//                                               -*- C++ -*-
/**
 *  @brief The Dirac distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Dirac.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Dirac)

static const Factory<Dirac> Factory_Dirac;

/* Default constructor */
Dirac::Dirac()
  : DiscreteDistribution()
  , point_(1, 0.0)
{
  setName("Dirac");
  // We set the dimension of the Dirac distribution
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Dirac::Dirac(const Scalar p)
  : DiscreteDistribution()
{
  setName("Dirac");
  // We set the dimension of the Dirac distribution
  setDimension(1);
  setPoint(Point(1, p));
  computeRange();
}

/* Parameters constructor */
Dirac::Dirac(const Point & point)
  : DiscreteDistribution()
{
  setName( "Dirac" );
  // We set the dimension of the Dirac distribution
  setDimension(point.getDimension());
  setPoint(point);
  computeRange();
}

/* Comparison operator */
Bool Dirac::operator ==(const Dirac & other) const
{
  if (this == &other) return true;
  return point_ == other.point_;
}

Bool Dirac::equals(const DistributionImplementation & other) const
{
  const Dirac* p_other = dynamic_cast<const Dirac*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Dirac::__repr__() const
{
  OSS oss;
  oss << "class=" << Dirac::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " point=" << point_;
  return oss;
}

String Dirac::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(point = " << point_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
Dirac * Dirac::clone() const
{
  return new Dirac(*this);
}

/* Get one realization of the distribution */
Point Dirac::getRealization() const
{
  return point_;
}

/* Get a sample of the distribution */
Sample Dirac::getSample(const UnsignedInteger size) const
{
  return Sample(size, point_);
}

/* Get the PDF of the distribution */
Scalar Dirac::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if ((point - point_).norm() <= supportEpsilon_) return 1.0;
  return 0.0;
}


/* Get the CDF of the distribution */
Scalar Dirac::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // If at least one component is too small
  for (UnsignedInteger i = 0; i < dimension; ++i) if (point_[i] > point[i]) return 0.0;
  return 1.0;
}

/* Compute the multivariate quantile of the distribution */
Point Dirac::computeQuantile(const Scalar,
                             const Bool ) const
{
  return point_;
}

/* Get the PDF gradient of the distribution */
Point Dirac::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In Dirac::computePDFGradient(const Point & point) const";
}


/* Get the CDF gradient of the distribution */
Point Dirac::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In Dirac::computeCDFGradient(const Point & point) const";
}

/* Get the quantile of the distribution */
Scalar Dirac::computeScalarQuantile(const Scalar,
                                    const Bool ) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  return point_[0];
}

/* Compute the entropy of the distribution */
Scalar Dirac::computeEntropy() const
{
  return 0.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Dirac::computeCharacteristicFunction(const Scalar u) const
{
  return std::exp(computeLogCharacteristicFunction(u));
}

Complex Dirac::computeLogCharacteristicFunction(const Scalar u) const
{
  if (getDimension() != 1) throw NotYetImplementedException(HERE) << "In Dirac::computeLogCharacteristicFunction(const Scalar u) const: the computeCharacteristicFunction() method is implemented for 1D distributions only.";
  const Complex value(0.0, u * point_[0]);
  return value;
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Dirac::computeGeneratingFunction(const Complex & z) const
{
  return std::exp(computeLogGeneratingFunction(z));
}

Complex Dirac::computeLogGeneratingFunction(const Complex & z) const
{
  if (getDimension() != 1) throw NotYetImplementedException(HERE) << "In Dirac::computeLogGeneratingFunction(const Complex & z) const: the computeGeneratingFunction() method is implemented for 1D distributions only.";
  return point_[0] * std::log(z);
}

/* Compute the mean of the distribution */
void Dirac::computeMean() const
{
  mean_ = point_;
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Dirac::getStandardDeviation() const
{
  return Point(getDimension(), 0.0);
}

/* Get the skewness of the distribution */
Point Dirac::getSkewness() const
{
  return Point(getDimension(), 0.0);
}

/* Get the kurtosis of the distribution */
Point Dirac::getKurtosis() const
{
  return Point(getDimension(), 0.0);
}

/* Get the moments of the standardized distribution */
Point Dirac::getStandardMoment(const UnsignedInteger n) const
{
  const UnsignedInteger dimension = getDimension();
  Point result(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = std::pow(point_[i], static_cast<int>(n));
  return result;
}

/* Compute the covariance of the distribution */
void Dirac::computeCovariance() const
{
  covariance_ = SquareMatrix(getDimension()).getImplementation();
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Dirac::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  if (interval.contains(point_)) return Sample(1, point_);
  return Sample(0, getDimension());
}

/* Parameters value and description accessor */
Dirac::PointWithDescriptionCollection Dirac::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(1);
    Description description(point.getDimension());
    point[0] = point_[i];
    description[0] = String(OSS() << "point_" << i);
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  return parameters;
}

void Dirac::setParametersCollection(const PointCollection & parametersCollection)
{
  const Scalar w = getWeight();
  const UnsignedInteger dimension = parametersCollection.getSize();
  Point point(dimension);
  // The ith component of the point is supposed to be the first component of the point
  // at position i in the parameters collection
  for (UnsignedInteger i = 0; i < dimension; ++i)
    point[i] = parametersCollection[i][0];
  *this = Dirac(point);
  setWeight(w);
}

Point Dirac::getParameter() const
{
  return point_;
}

void Dirac::setParameter(const Point & parameter)
{
  const Scalar w = getWeight();
  *this = Dirac(parameter);
  setWeight(w);
}

Description Dirac::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    description[i] = (OSS() << "point_" << i);
  }
  return description;
}

/* Point accessor */
void Dirac::setPoint(const Point & point)
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Expected a point of dimension " << dimension;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    if (!SpecFunc::IsNormal(point[i])) throw InvalidArgumentException(HERE) << "Cannot build a Dirac from nan/inf values";
  point_ = point;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Point accessor */
Point Dirac::getPoint() const
{
  return point_;
}


/* Compute the numerical range of the distribution given the parameters values */
void Dirac::computeRange()
{
  setRange(Interval(point_, point_));
}

/* Get the i-th marginal distribution */
Distribution Dirac::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  Dirac::Implementation marginal(new Dirac(point_[i]));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Dirac::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  Dirac::Implementation marginal(new Dirac(point_.select(indices)));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
}

/* Check if the distribution is elliptical */
Bool Dirac::isElliptical() const
{
  return true;
}

/* Tell if the distribution has an elliptical copula */
Bool Dirac::hasEllipticalCopula() const
{
  return true;
}

/* Tell if the distribution has independent copula */
Bool Dirac::hasIndependentCopula() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void Dirac::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "point_", point_ );
}

/* Method load() reloads the object from the StorageManager */
void Dirac::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "point_", point_ );
}

END_NAMESPACE_OPENTURNS
