//                                               -*- C++ -*-
/**
 *  @brief The ClaytonCopula distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ClaytonCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ClaytonCopula)

static const Factory<ClaytonCopula> Factory_ClaytonCopula;

/* Default constructor */
ClaytonCopula::ClaytonCopula()
  : ArchimedeanCopula()
  , theta_(2.0)
{
  setName( "ClaytonCopula" );
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
ClaytonCopula::ClaytonCopula(const Scalar theta)
  : ArchimedeanCopula()
  , theta_(0)
{
  setName( "ClaytonCopula" );
  // We set the dimension of the ClaytonCopula distribution
  setDimension( 2 );
  setTheta(theta);
  computeRange();
}

/* Comparison operator */
Bool ClaytonCopula::operator ==(const ClaytonCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool ClaytonCopula::equals(const DistributionImplementation & other) const
{
  const ClaytonCopula* p_other = dynamic_cast<const ClaytonCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String ClaytonCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ClaytonCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String ClaytonCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
ClaytonCopula * ClaytonCopula::clone() const
{
  return new ClaytonCopula(*this);
}

/* Get one realization of the distribution */
Point ClaytonCopula::getRealization() const
{
  Point realization(2);
  const Scalar u1 = RandomGenerator::Generate();
  // We use the general algorithm based on conditional CDF inversion
  // W case
  if (theta_ == -1.0)
  {
    realization[0] = u1;
    realization[1] = 1.0 - u1;
    return realization;
  }
  // The inverse conditional CDF U2|U1
  const Scalar u2 = RandomGenerator::Generate();
  // Independent case
  if (theta_ == 0.0)
  {
    realization[0] = u1;
    realization[1] = u2;
    return realization;
  }
  // For a strict Clayton copula (theta>0), two possible algorithms:
  // + based on Laplace transform
  // + based on conditional quantile
  // Benchmarks show that the second one is more efficient (10^6 rng)
  // Conditional quantile: speed= 3739718.74788 rng/s, t= 0.267399787903 s
  // Laplace transform:    speed= 2719399.57105 rng/s, t= 0.367728233337 s
#ifdef CLAYTON_USE_LAPLACE
  if (theta_ > 0.0)
  {
    const Scalar x = -std::log(u1);
    const Scalar y = -std::log(u2);
    const Scalar z = DistFunc::rGamma(1.0 / theta_);
    realization[0] = std::exp(-theta_ * log1p(x / z));
    realization[1] = std::exp(-theta_ * log1p(y / z));
    return realization;
  }
#endif
  realization[0] = u1;
  if (theta_ < 1.0e-8)
  {
    const Scalar logU1 = std::log(u1);
    const Scalar logU2 = std::log(u2);
    realization[1] = u2 * (1.0 - logU2 * theta_ * (1.0 + logU1 - 0.5 * theta_ * ((1.0 + logU2) * logU1 * logU1 + (2.0 + logU2) * (1.0 + logU1))));
  }
  if (theta_ < 1.0e8) realization[1] = u1 * std::pow(std::pow(u2, -theta_ / (1.0 + theta_)) - 1.0 + std::pow(u1, theta_), -1.0 / theta_);
  else realization[1] = u1 * (1.0 - (log1p(-u2) + std::log(u2)) / theta_);
  return realization;
}

/* Get the DDF of the distribution */
Point ClaytonCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar u = point[0];
  Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return Point(2, 0.0);
  // We can impose u <= v as the copula is symmetric in (u, v)
  Bool exchanged = false;
  if (u > v)
  {
    std::swap(u, v);
    exchanged = true;
  }
  // W case (Frechet lower bound)
  if (theta_ == -1.0)
  {
    if (u == 1.0 - v) return Point(2, -SpecFunc::MaxScalar);
    return Point(2, 0.0);
  }
  Point ddf(2);
  // Independent case
  if (theta_ == 0.0) return ddf;
  // Nearly independent cases. The formula defining the copula suffers from
  // cancellation for |theta|<<1 so we use a truncated series of order 2
  if (std::abs(theta_) < 1.0e-8)
  {
    const Scalar logU = std::log(u);
    const Scalar logV = std::log(v);
    if ((theta_ < 0.0) && (theta_ * ((logU + logV) - 0.5 * theta_ * (logU * logU + logV * logV)) >= 1.0)) return ddf;
    const Scalar ddfU = 0.5 * theta_ * (2.0 * (logV + 1.0) + theta_ * (2.0 + 2.0 * logU + 8.0 * logV + 3.0 * logV * logV + 6.0 * logU * logV + 2.0 * logU * logV * logV)) / u;
    const Scalar ddfV = 0.5 * theta_ * (2.0 * (logU + 1.0) + theta_ * (2.0 + 2.0 * logV + 8.0 * logU + 3.0 * logU * logU + 6.0 * logV * logU + 2.0 * logV * logU * logU)) / v;
    if (exchanged)
    {
      ddf[0] = ddfV;
      ddf[1] = ddfU;
    }
    else
    {
      ddf[0] = ddfU;
      ddf[1] = ddfV;
    }
    return ddf;
  }
  // General case, we factor out u^(-theta) in order to prevent under/owerflow
  // for theta>>1
  // dc(u,v)/du = (u^(-theta)+v^(-theta)-1)^(-1/theta)*u^(-theta)*v^(-theta)*(theta+1)*(u^(-theta)*theta-theta*v^(-theta)-v^(-theta)+theta+1)/(u^2*v*(u^(-theta)+v^(-theta)-1)^3
  // = u*(1+(u/v)^theta-u^theta)^(-1/theta)*u^(-theta)*v^(-theta)*(theta+1)*theta*u^(-theta)*(1-(1+1/theta)*((u/v)^theta-u^theta))/(u^2*v*u^(-3*theta)*(1+(u/v)^theta-u^theta)^3
  // = theta*(theta+1)*(1+K)^(-1/theta-3)*u^(theta-1)*v^(-theta-1)*(1-(1+1/theta)*K)
  //
  // dc(u,v)/dv=-(u^(-theta)+v^(-theta)-1)^(-1/theta)*u^(-theta)*v^(-theta)*(theta+1)*(u^(-theta)*theta-theta*v^(-theta)-u^(-theta)-theta-1)/(v^2*u*(u^(-theta)+v^(-theta)-1)^3
  // = -(u^(-theta)+v^(-theta)-1)^(-1/theta)*u^(-theta)*v^(-theta)*(theta+1)*(u^(-theta)*theta-theta*v^(-theta)-u^(-theta)-theta-1)/(v^2*u*(u^(-theta)+v^(-theta)-1)^3
  // = -u*(1+(u/v)^theta-u^theta)^(-1/theta)*u^(-theta)*v^(-theta)*(theta+1)*theta*u^(-theta)*(1-(u/v)^theta-1/theta-(1+1/theta)*u^theta)/(v^2*u*u^(-3*theta)*(1+(u/v)^theta-u^theta)^3
  // = -(1+K)^(-1/theta)*u^theta*v^(-theta-2)*(theta+1)*theta*(1-(u/v)^theta-1/theta-(1+1/theta)*u^theta)/(1+K)^3

  // For moderate theta, this form is cancellation-free
  Scalar factor = -1.0;
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar logUOverV = std::log(u / v);
  if (theta_ < 100.0) factor = std::exp(theta_ * logU) * expm1(-theta_ * logV);
  // Here we have to insure that theta is multiplied by a negative value to prevent overflow (but get possible underflow...)
  else factor = expm1(theta_ * logUOverV) - expm1(theta_ * logU);
  if (factor <= -1.0) return ddf;
  const Scalar ddfU = theta_ * (1.0 + theta_) * (1.0 - (1.0 + 1.0 / theta_) * factor) * std::exp(-(3.0 + 1.0 / theta_) * log1p(factor) + (theta_ - 1.0) * logUOverV - 2.0 * logV);
  const Scalar t = -std::exp(theta_ * logUOverV) + 1.0 / theta_ - (1.0 + 1.0 / theta_) * std::exp(theta_ * logU);
  const Scalar ddfV = -theta_ * (1.0 + theta_) * (1.0 + t) * std::exp(-(3.0 + 1.0 / theta_) * log1p(factor) + theta_ * logUOverV - 2.0 * logV);
  if (exchanged)
  {
    ddf[0] = ddfV;
    ddf[1] = ddfU;
  }
  else
  {
    ddf[0] = ddfU;
    ddf[1] = ddfV;
  }
  return ddf;
}

/* Get the PDF of the distribution */
Scalar ClaytonCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar u = point[0];
  Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return 0.0;
  // We can impose u <= v as the copula is symmetric in (u, v)
  if (u > v) std::swap(u, v);
  // W case (Frechet lower bound)
  if (theta_ == -1.0)
  {
    if (u == 1.0 - v) return SpecFunc::MaxScalar;
    return 0.0;
  }
  // Independent case
  if (theta_ == 0.0) return 1.0;
  // Nearly independent cases. The formula defining the copula suffers from
  // cancellation for |theta|<<1 so we use a truncated series of order 2
  if (std::abs(theta_) < 1.0e-8)
  {
    const Scalar logU = std::log(u);
    const Scalar logV = std::log(v);
    if ((theta_ < 0.0) && (theta_ * ((logU + logV) - 0.5 * theta_ * (logU * logU + logV * logV)) >= 1.0)) return 0.0;
    return 1.0 + theta_ * (1.0 + logU + logV + logU * logV + theta_ * (logU + logV + 0.5 * (logU * logU + logV * logV + logU * logV * (8.0 + 3.0 * (logU + logV) + logU * logV))));
  }
  // General case, we factor out u^(-theta) in order to prevent underflow/overflow
  // for theta>>1
  // c(u,v)=(theta+1)(u^(-theta)+v^(-theta)-1)^(-(1+2theta)/theta)(uv)^(-theta-1)
  // = (theta+1)exp(theta*log(u)-log(v)-(1/theta+2)log1p(expm1(theta*log(u/v))-expm1(theta*log(u))))
  // For moderate theta, this form is cancellation-free
  Scalar factor = -1.0;
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar logUOverV = std::log(u / v);
  if (theta_ < 100.0) factor = std::exp(theta_ * logU) * expm1(-theta_ * logV);
  // Here we have to insure that theta is multiplied by a negative value to prevent overflow (but get possible underflow...)
  else factor = expm1(theta_ * logUOverV) - expm1(theta_ * logU);
  if (factor <= -1.0) return 0.0;
  return (1.0 + theta_) * std::exp(theta_ * logUOverV - logV - (1.0 / theta_ + 2.0) * log1p(factor));
}

/* Get the CDF of the distribution */
Scalar ClaytonCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar u = point[0];
  Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0)) return 0.0;
  // We can impose u <= v as the copula is symmetric in (u, v)
  if (u > v) std::swap(u, v);
  // If we are outside of the support, in the upper part
  if (u >= 1.0) return 1.0;
  // If we are outside of the support for u, in the upper part
  if (v >= 1.0) return u;
  // If we are in the support
  // W case (Frechet lower bound)
  if (theta_ == -1.0) return std::max(u + v - 1.0, 0.0);
  // Independent case
  if (theta_ == 0.0) return u * v;
  // Nearly independent cases. The formula defining the copula suffers from
  // cancellation for |theta|<<1 so we use a truncated series
  if (std::abs(theta_) < 1.0e-8)
  {
    const Scalar logU = std::log(u);
    const Scalar logV = std::log(v);
    if ((theta_ < 0.0) && (theta_ * ((logU + logV) - 0.5 * theta_ * (logU * logU + logV * logV)) >= 1.0)) return 0.0;
    return u * v * (1.0 + theta_ * logU * logV * (1.0 + 0.5 * theta_ * (logU * logV + logU + logV)));
  }
  // General case, we factor out u^(-theta) in order to prevent under/owerflow
  // for theta>>1
  // C(u,v)=u(1+(u/v)^theta-u^theta)^(-1/theta)
  // For moderate theta, this form is cancellation-free
  Scalar factor = -1.0;
  if (theta_ < 100.0) factor = std::exp(theta_ * std::log(u)) * expm1(-theta_ * std::log(v));
  // Here we have to insure that theta is multiplied by a negative value to prevent overflow (but get possible underflow...)
  else factor = expm1(theta_ * std::log(u / v)) - expm1(theta_ * std::log(u));
  if (factor <= -1.0) return 0.0;
  return u * std::exp(-(log1p(factor)) / theta_);
}

/* Compute the covariance of the distribution */
void ClaytonCopula::computeCovariance() const
{
  CopulaImplementation::computeCovariance();
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix ClaytonCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(0, 1) = theta_ / (theta_ + 2.0);
  return tau;
}

/* Get the PDFGradient of the distribution */
Point ClaytonCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return Point(1, 0.0);
  const Scalar t1 = std::pow(u, -theta_);
  const Scalar t2 = std::pow(v, -theta_);
  const Scalar t3 = t1 + t2 - 1.0;
  if (t3 <= 0.0) return Point(1, 0.0);
  const Scalar t5 = std::pow(t3, -1.0 / theta_);
  const Scalar t7 = std::log(v);
  const Scalar t8 = theta_ * theta_;
  const Scalar t9 = t7 * t8;
  const Scalar t10 = std::log(u);
  const Scalar t11 = t10 * t8;
  const Scalar t16 = std::log(t3);
  const Scalar t17 = t16 * t1;
  const Scalar t19 = t16 * t2;
  const Scalar t21 = t8 * t1;
  const Scalar t24 = t8 * t2;
  const Scalar t27 = t8 * theta_;
  const Scalar t28 = t7 * t27;
  const Scalar t32 = t9 + t11 + theta_ * t2 * t7 + theta_ * t1 * t10 + t17 * theta_ + t19 * theta_ + 2 * t21 * t10 + 2 * t24 * t7 - t28 * t1 + t28 * t2 - t9 * t1;
  const Scalar t33 = t10 * t27;
  const Scalar t38 = -t33 * t2 - t11 * t2 - t16 - t8 + t17 + t19 - t16 * theta_ + t28 + t33 + t21 + t24 + t33 * t1;
  const Scalar t43 = t3 * t3;
  const Scalar t51 = t5 * t2 * t1 * (t32 + t38) / (t8 * t43 * t3 * v * u);
  return Point(1, t51);
}

/* Get the CDFGradient of the distribution */
Point ClaytonCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // If we are outside of the support, in the lower parts
  if ((u <= 0.0) || (v <= 0.0)) return Point(1, 0.0);
  // If we are outside of the support, in the upper part
  if ((u >= 1.0) && (v >= 1.0)) return Point(1, 0.0);
  // If we are outside of the support for u, in the upper part
  if (u >= 1.0) return Point(1, 0.0);
  // If we are outside of the support for v, in the upper part
  if (v >= 1.0) return Point(1, 0.0);
  // If we are in the support
  const Scalar powUMinusTheta = std::pow(u, -theta_);
  const Scalar powVMinusTheta = std::pow(v, -theta_);
  const Scalar sum1 = powUMinusTheta + powVMinusTheta - 1.0;
  if (sum1 <= 0.0) return Point(1, 0.0);
  const Scalar factor1 = std::pow(sum1, -1.0 / theta_);
  return Point(1, factor1 * (std::log(sum1) * (powUMinusTheta + powVMinusTheta - 1) + theta_ * (powUMinusTheta * std::log(u) + powVMinusTheta * std::log(v))) / (theta_ * theta_ * sum1));
}

/* Get the quantile of the distribution */
Point ClaytonCopula::computeQuantile(const Scalar prob,
                                     const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  // Special case for boarding values
  if (q == 0.0) return getRange().getLowerBound();
  if (q == 1.0) return getRange().getUpperBound();
  // Independent case
  if (theta_ == 0.0) return Point(2, std::sqrt(q));
  // General case
  return Point(2, std::exp((M_LN2 - log1p(std::pow(q, -theta_))) / theta_));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar ClaytonCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  // If we are in the support
  const Scalar factor = std::pow(u, -theta_) + std::pow(v, -theta_) - 1.0;
  if (factor <= 0.0) return 0.0;
  return std::pow(factor, -1.0 - 1.0 / theta_) * std::pow(u, -1.0 - theta_);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar ClaytonCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case for no conditioning or independent copula
  if ((q == 0.0) || (q == 1.0)) return q;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar z = y[0];
  return z * std::pow(std::pow(q, -theta_ / (1.0 + theta_)) - 1.0 + std::pow(z, theta_), -1.0 / theta_);
}

/* Tell if the distribution has independent copula */
Bool ClaytonCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
Scalar ClaytonCopula::computeArchimedeanGenerator(const Scalar t) const
{
  if (theta_ == 0.0) return 0.0;
  return (std::pow(t, -theta_) -  1.0) / theta_;
}

/* Compute the inverse of the archimedean generator */
Scalar ClaytonCopula::computeInverseArchimedeanGenerator(const Scalar t) const
{
  if (theta_ == 0.0) return 0.0;
  return std::pow(1.0 + t * theta_, -1.0 / theta_);
}

/* Compute the derivative of the density generator */
Scalar ClaytonCopula::computeArchimedeanGeneratorDerivative(const Scalar t) const
{
  if (theta_ == 0.0) return 0.0;
  if (theta_ == 1.0) return 1.0;
  return -std::pow(t, -theta_ - 1.0);
}

/* Compute the seconde derivative of the density generator */
Scalar ClaytonCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar t) const
{
  if (theta_ == 0.0) return 0.0;
  if (theta_ == 1.0) return 0.0;
  return (theta_ + 1) * std::pow(t, -theta_ - 2.0);
}

/* Parameters value accessor */
Point ClaytonCopula::getParameter() const
{
  return Point(1, theta_);
}

void ClaytonCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 parameter, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = ClaytonCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description ClaytonCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Theta accessor */
void ClaytonCopula::setTheta(const Scalar theta)
{
  if (theta < -1.0) throw InvalidArgumentException(HERE) << "Theta MUST be greater or equal to -1";
  theta_ = theta;
}

/* Theta accessor */
Scalar ClaytonCopula::getTheta() const
{
  return theta_;
}

/* Method save() stores the object through the StorageManager */
void ClaytonCopula::save(Advocate & adv) const
{
  ArchimedeanCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void ClaytonCopula::load(Advocate & adv)
{
  ArchimedeanCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
