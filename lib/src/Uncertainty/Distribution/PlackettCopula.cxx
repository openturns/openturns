//                                               -*- C++ -*-
/**
 *  @brief The PlackettCopula distribution
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
#include "openturns/PlackettCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PlackettCopula)

static const Factory<PlackettCopula> Factory_PlackettCopula;

/* Default constructor */
PlackettCopula::PlackettCopula()
  : DistributionImplementation()
  , theta_(-1.0) // To force the update in the accessor
{
  isCopula_ = true;
  setName( "PlackettCopula" );
  setTheta(2.0);
  // We set the dimension of the PlackettCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
PlackettCopula::PlackettCopula(const Scalar theta)
  : DistributionImplementation()
  , theta_(-1.0) // To force the update in the accessor
{
  isCopula_ = true;
  setName( "PlackettCopula" );
  setTheta(theta);
  // We set the dimension of the PlackettCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Comparison operator */
Bool PlackettCopula::operator ==(const PlackettCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool PlackettCopula::equals(const DistributionImplementation & other) const
{
  const PlackettCopula* p_other = dynamic_cast<const PlackettCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PlackettCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << PlackettCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String PlackettCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
PlackettCopula * PlackettCopula::clone() const
{
  return new PlackettCopula(*this);
}

/* Get one realization of the distribution */
/* Algorithm from R. Nelsen, "An introduction to copulas, 2nd Edition", ex. 3.38 p99 */
Point PlackettCopula::getRealization() const
{
  Point realization(2);
  // We use the general algorithm based on conditional CDF inversion
  const Scalar u = RandomGenerator::Generate();
  realization[0] = u;
  // If this is the Min copula
  if (theta_ == 0.0)
  {
    realization[1] = u;
    return realization;
  }
  const Scalar t = RandomGenerator::Generate();
  // If this is the independent copula
  if (thetaMinus1_ == 0.0)
  {
    realization[1] = t;
    return realization;
  }
  // General case
  const Scalar a = t * (1.0 - t);
  const Scalar thetaMinus1Squared = thetaMinus1_ * thetaMinus1_;
  const Scalar b = theta_ + a * thetaMinus1Squared;
  const Scalar c = 2.0 * a * (u * theta_ * theta_ + 1.0 - u) + theta_ * (1.0 - 2.0 * a);
  const Scalar d = std::sqrt(theta_ * (theta_ + 4.0 * a * u * (1.0 - u) * thetaMinus1Squared));
  realization[1] = (c - (1.0 - 2.0 * t) * d) / (2.0 * b);
  return realization;
}

/* Get the PDF of the distribution */
Scalar PlackettCopula::computePDF(const Point & point) const
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
  // Independent or nearly independent case
  if (std::abs(thetaMinus1_) < 1e-7)
    return 1.0 + thetaMinus1_ * (1.0 - 2.0 * u) * (1.0 - 2.0 * v);
  // General case
  const Scalar uv = thetaMinus1_ * (2.0 * u * v - u - v);
  return -theta_ * (uv - 1.0) / std::pow(std::pow(thetaMinus1_ * (u - v), 2.0) - 2.0 * uv + 1.0, 1.5);
}

/* Get the CDF of the distribution */
Scalar PlackettCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0))
  {
    return 0.0;
  }
  // If we are outside of the support, in the upper part
  if ((u >= 1.0) && (v >= 1.0))
  {
    return 1.0;
  }
  // If we are outside of the support for u, in the upper part
  if (u >= 1.0)
  {
    return v;
  }
  // If we are outside of the support for v, in the upper part
  if (v >= 1.0)
  {
    return u;
  }
  // If we are in the support
  // Independent or nearly independent case
  if (std::abs(thetaMinus1_) < 1e-7)
    return u * v * (1.0 + (1.0 - u) * (1.0 - v));
  // General case
  return (1.0 + thetaMinus1_ * (u + v) - std::sqrt(std::pow(1.0 + thetaMinus1_ * (u + v), 2.0) - 4.0 * u * v * theta_ * thetaMinus1_)) / (2.0 * thetaMinus1_);
}

/* Get the PDFGradient of the distribution */
Point PlackettCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In PlackettCopula::computePDFGradient(const Point & point) const";
}

/* Get the CDFGradient of the distribution */
Point PlackettCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In PlackettCopula::computeCDFGradient(const Point & point) const";
}

/* Get the quantile of the distribution */
Point PlackettCopula::computeQuantile(const Scalar prob,
                                      const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  // Special case for boarding values
  if (q == 0.0) return getRange().getLowerBound();
  if (q == 1.0) return getRange().getUpperBound();
  // Independent or nearly independent case
  if (std::abs(thetaMinus1_) < 1e-7)
  {
    const Scalar sqrtQ = std::sqrt(q);
    return Point(2, sqrtQ * (1.0 + thetaMinus1_ * (-0.5 + sqrtQ - 0.5 * q)));
  }
  // Min copula case
  if (theta_ == 0.0)
    return Point(2, q);
  // General case
  return Point(2, (thetaMinus1_ * q + std::sqrt(q * (theta_ - q * thetaMinus1_))) / theta_);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar PlackettCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  return 0.5 * (1.0 + (2.0 * v + thetaMinus1_ * (v - u) - 1.0) / std::sqrt(1.0 + std::pow(thetaMinus1_ * (u - v), 2.0) + thetaMinus1_ * (2.0 * v + u * (2.0 - 4.0 * v))));
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar PlackettCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar u = y[0];
  const Scalar a = thetaMinus1_;
  const Scalar t1 = a * a;
  const Scalar t2 = q * q;
  const Scalar t3 = t1 * t2;
  const Scalar t4 = t1 * q;
  const Scalar t8 = t3 * u;
  const Scalar t10 = t4 * u;
  const Scalar t12 = a * t2;
  const Scalar t16 = q * a;
  const Scalar t22 = t1 * a;
  const Scalar t23 = t2 * t2;
  const Scalar t24 = t22 * t23;
  const Scalar t25 = u * u;
  const Scalar t30 = t2 * q;
  const Scalar t31 = t22 * t30;
  const Scalar t34 = t1 * t23;
  const Scalar t39 = t22 * t2;
  const Scalar t44 = t1 * t30;
  const Scalar t49 = t22 * q;
  const Scalar t56 = 16.0 * t24 * t25 - 16.0 * t24 * u + 20.0 * t3 * t25 - 32.0 * t31 * t25 + 16.0 * t34 * t25 + 20.0 * t39 * t25 - 32.0 * t44 * t25 - 4.0 * t49 * t25 + 32.0 * t31 * u - 16.0 * t34 * u - 20.0 * t39 * u + 32.0 * t44 * u;
  const Scalar t70 = -4.0 * t4 * t25 + 4.0 * t49 * u + 2.0 * a - 4.0 * q + t1 + 4.0 * t10 + 0.8e1 * t12 - 0.8e1 * t16 + 4.0 * t2 + 4.0 * t3 - 4.0 * t4 - 20.0 * t8 + 1.0;
  const Scalar A = 1.0 / (t3 - t4 - a - 1.0);
  const Scalar B = (4.0 * t12 * u - 4.0 * t16 * u - a - 2.0 * t10 - 2.0 * t12 + 2.0 * t16 + 2.0 * t8 - 1.0);
  const Scalar C = std::sqrt(t56 + t70);
  if (q <= 0.5)
    return A * (B + C);
  else
    return A * (B - C);
}

/* Parameters value accessor */
Point PlackettCopula::getParameter() const
{
  return Point(1, theta_);
}

void PlackettCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  if (parameter[0] != theta_)
  {
    const Scalar w = getWeight();
    *this = PlackettCopula(parameter[0]);
    setWeight(w);
  }
}

/* Parameters description accessor */
Description PlackettCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/*Compute the covariance of the distribution */
void PlackettCopula::computeCovariance() const
{
  covariance_ = CovarianceMatrix(2);
  covariance_(0, 0) = 1.0 / 12.0;
  covariance_(1, 1) = 1.0 / 12.0;
  if (theta_ == 0.0)
    covariance_(1, 0) = -1.0 / 12.0;
  else if (std::abs(thetaMinus1_) < 1.0e-7)
    covariance_(1, 0) = thetaMinus1_ * (1.0 - 0.5 * thetaMinus1_) / 3.0;
  else
    covariance_(1, 0) = ((theta_ + 1) - 2.0 * theta_ * std::log(theta_) / thetaMinus1_) / thetaMinus1_;
  isAlreadyComputedCovariance_ = true;
}

/* Tell if the distribution has independent copula */
Bool PlackettCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Theta accessor */
void PlackettCopula::setTheta(const Scalar theta)
{
  if (theta < 0.0) throw InvalidArgumentException(HERE) << "Error: expected theta>=0, got theta=" << theta;
  if (theta != theta_)
  {
    theta_ = theta;
    thetaMinus1_ = theta - 1.0;
    isAlreadyComputedCovariance_ = false;
  }
}

/* Theta accessor */
Scalar PlackettCopula::getTheta() const
{
  return theta_;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution PlackettCopula::getMarginal(const Indices & indices) const
{
  if (!indices.check(2)) throw InvalidArgumentException(HERE) << "The indices of a Plackett copula must be in the range [0, 1] and must be different";
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  // Only one indice is needed, call the specialized method
  if (outputDimension == 1) return getMarginal(indices[0]);
  // The indices correspond to all the components, with a possible transposition of the two components.
  // As a Plackett copula is exchangeable, the resulting distribution is the copula itself, we just
  // have to transpose the descriptions
  PlackettCopula * result(clone());
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

/* Method save() stores the object through the StorageManager */
void PlackettCopula::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void PlackettCopula::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  thetaMinus1_ = theta_ - 1.0;
  computeRange();
}

END_NAMESPACE_OPENTURNS
