//                                               -*- C++ -*-
/**
 *  @brief The FarlieGumbelMorgensternCopula distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FarlieGumbelMorgensternCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FarlieGumbelMorgensternCopula)

static const Factory<FarlieGumbelMorgensternCopula> Factory_FarlieGumbelMorgensternCopula;

/* Default constructor */
FarlieGumbelMorgensternCopula::FarlieGumbelMorgensternCopula()
  : CopulaImplementation()
  , theta_(0.5)
{
  setName( "FarlieGumbelMorgensternCopula" );
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
FarlieGumbelMorgensternCopula::FarlieGumbelMorgensternCopula(const Scalar theta)
  : CopulaImplementation()
  , theta_(0.0)
{
  setName( "FarlieGumbelMorgensternCopula" );
  // Check the value of theta
  setTheta(theta);
  // We set the dimension of the FarlieGumbelMorgensternCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Comparison operator */
Bool FarlieGumbelMorgensternCopula::operator ==(const FarlieGumbelMorgensternCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool FarlieGumbelMorgensternCopula::equals(const DistributionImplementation & other) const
{
  const FarlieGumbelMorgensternCopula* p_other = dynamic_cast<const FarlieGumbelMorgensternCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String FarlieGumbelMorgensternCopula::__repr__() const
{
  OSS oss;
  oss << "class=" << FarlieGumbelMorgensternCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

/* Virtual constructor */
FarlieGumbelMorgensternCopula * FarlieGumbelMorgensternCopula::clone() const
{
  return new FarlieGumbelMorgensternCopula(*this);
}

/* Get one realization of the distribution */
Point FarlieGumbelMorgensternCopula::getRealization() const
{
  Point realization(2);
  // We use the algorithm described in Nelsen, "An Introduction to Copulas 2nd Edition", Exercise 3.23 to speed-up generation.
  const Scalar u = RandomGenerator::Generate();
  const Scalar t = RandomGenerator::Generate();
  const Scalar a = 1.0 + theta_ * (1.0 - 2.0 * u);
  const Scalar b = std::sqrt(a * a - 4.0 * (a - 1.0) * t);
  realization[0] = u;
  realization[1] = 2.0 * t / (a + b);
  return realization;
}

/* Get the DDF of the distribution */
Point FarlieGumbelMorgensternCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  Point result(2, 0.0);
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return result;
  }
  result[0] = 2.0 * theta_ * (2.0 * v - 1.0);
  result[1] = 2.0 * theta_ * (2.0 * u - 1.0);
  return result;
}

/* Get the PDF of the distribution */
Scalar FarlieGumbelMorgensternCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return 0.0;
  }
  return 1.0 + theta_ * (2.0 * u - 1.0) * (2.0 * v - 1.0);
}

/* Get the CDF of the distribution */
Scalar FarlieGumbelMorgensternCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0)) return 0.0;
  // If we are outside of the support, in the upper part
  if ((u >= 1.0) && (v >= 1.0)) return 1.0;
  // If we are outside of the support for u, in the upper part
  if (u >= 1.0) return v;
  // If we are outside of the support for v, in the upper part
  if (v >= 1.0) return u;
  // If we are in the support
  return u * v * (1.0 + theta_ * (1.0 - u) * (1.0 - v));
}

/* Compute the covariance of the distribution */
void FarlieGumbelMorgensternCopula::computeCovariance() const
{
  covariance_ = CovarianceMatrix(2);
  covariance_(0, 0) = 1.0 / 12.0;
  covariance_(0, 1) = theta_ / 36.0;
  covariance_(1, 1) = 1.0 / 12.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix FarlieGumbelMorgensternCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(0, 1) = 2.0 * theta_ / 9.0;
  return tau;
}

/* Get the PDFGradient of the distribution */
Point FarlieGumbelMorgensternCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return Point(1, 0.0);
  }
  return Point(1, (2.0 * u - 1.0) * (2.0 * v - 1.0));
}

/* Get the CDFGradient of the distribution */
Point FarlieGumbelMorgensternCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return Point(1, 0.0);
  }
  return Point(1, u * v * (1.0 - u) * (1.0 - v));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar FarlieGumbelMorgensternCopula::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  // If we are in the support
  return v * (1.0 + theta_ * (v - 1.0) * (2.0 * u - 1.0));
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar FarlieGumbelMorgensternCopula::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case for no conditioning or independent copula
  if ((q == 0.0) || (q == 1.0)) return q;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar alpha = theta_ * (1.0 - 2.0 * y[0]);
  const Scalar alpha1 = 1.0 + alpha;
  return 2.0 * q / (alpha1 + std::sqrt(alpha1 * alpha1 - 4.0 * q * alpha));
}

/* Tell if the distribution has an elliptical copula */
Bool FarlieGumbelMorgensternCopula::hasEllipticalCopula() const
{
  return (theta_ == 0.0);
}

/* Tell if the distribution has an independent copula */
Bool FarlieGumbelMorgensternCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Parameters value accessor */
Point FarlieGumbelMorgensternCopula::getParameter() const
{
  return Point(1, theta_);
}

void FarlieGumbelMorgensternCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = FarlieGumbelMorgensternCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description FarlieGumbelMorgensternCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Theta accessor */
void FarlieGumbelMorgensternCopula::setTheta(const Scalar theta)
{
  if (!(std::abs(theta) <= 1.0)) throw InvalidArgumentException(HERE) << "Theta MUST be in [-1, 1]";
  if (theta != theta_)
  {
    theta_ = theta;
    isAlreadyComputedCovariance_ = false;
  }
}

/* Theta accessor */
Scalar FarlieGumbelMorgensternCopula::getTheta() const
{
  return theta_;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution FarlieGumbelMorgensternCopula::getMarginal(const Indices & indices) const
{
  if (!indices.check(2)) throw InvalidArgumentException(HERE) << "The indices of an archimedean copula must be in the range [0, 1] and must be different";
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  // Only one indice is needed, call the specialized method
  if (outputDimension == 1) return getMarginal(indices[0]);
  // The indices correspond to all the components, with a possible transposition of the two components.
  // As an archimedean copula is exchangeable, the resulting distribution is the copula itself, we just
  // have to transpose the descriptions
  FarlieGumbelMorgensternCopula * result(clone());
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

/* Compute the entropy of the distribution */
Scalar FarlieGumbelMorgensternCopula::computeEntropy() const
{
  if (std::abs(theta_) <= 0.024604263883779147)
  {
    const Scalar theta2 = theta_ * theta_;
    return -theta2 * (1.0 / 18.0 + theta2 * (1.0 / 300.0 + theta2 * (1.0 / 1470.0 + theta2 / 4536.0)));
  }
  return -((3.0 + theta_ * (4.0 + theta_)) * log1p(theta_) + (-3.0 + theta_ * (4.0 - theta_)) * log1p(-theta_)) / (8.0 * theta_) + (5.0 - (SpecFunc::DiLog(theta_) - SpecFunc::DiLog(-theta_)) / theta_) / 4.0;
}

/* Method save() stores the object through the StorageManager */
void FarlieGumbelMorgensternCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void FarlieGumbelMorgensternCopula::load(Advocate & adv)
{
  CopulaImplementation::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
