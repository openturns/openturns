//                                               -*- C++ -*-
/**
 *  @brief The GumbelCopula distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/GumbelCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GumbelCopula)

static const Factory<GumbelCopula> Factory_GumbelCopula;

/* Default constructor */
GumbelCopula::GumbelCopula()
  : ArchimedeanCopula()
  , theta_(2.0)
{
  setName( "GumbelCopula" );
  // We set the dimension of the GumbelCopula distribution
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
GumbelCopula::GumbelCopula(const Scalar theta)
  : ArchimedeanCopula()
  , theta_(0.0)
{
  setName( "GumbelCopula" );
  // We set the dimension of the GumbelCopula distribution
  setDimension( 2 );
  // Check the value of theta
  setTheta(theta);
  computeRange();
}

/* Comparison operator */
Bool GumbelCopula::operator ==(const GumbelCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool GumbelCopula::equals(const DistributionImplementation & other) const
{
  const GumbelCopula* p_other = dynamic_cast<const GumbelCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String GumbelCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GumbelCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String GumbelCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
GumbelCopula * GumbelCopula::clone() const
{
  return new GumbelCopula(*this);
}

/* Get one realization of the distribution
   Algorithm using stable distribution, see:
   Kjersti Aas, "Modelling the dependence structure of financial assets: a survey of four copulas",
   Norwegian Computing Center report nr. SAMBA/22/04, December 2004.
*/
Point GumbelCopula::getRealization() const
{
  Point realization(2);
  const Scalar u = (RandomGenerator::Generate() - 0.5) * M_PI;
  const Scalar u2 = u + M_PI_2;
  const Scalar e = -std::log(RandomGenerator::Generate());
  const Scalar inverseTheta = 1.0 / theta_;
  const Scalar t = std::cos(u - u2 * inverseTheta) / e;
  const Scalar gamma = std::pow(std::sin(u2 * inverseTheta) / t, inverseTheta) * t / std::cos(u);
  realization[0] = std::exp(-std::pow(-std::log(RandomGenerator::Generate()), inverseTheta) / gamma);
  realization[1] = std::exp(-std::pow(-std::log(RandomGenerator::Generate()), inverseTheta) / gamma);
  return realization;
}

/* Get the DDF of the distribution */
Point GumbelCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return Point(2, 0.0);
  Point result(2);
  const Scalar t1 = std::log(u);
  const Scalar t2 = std::pow(-t1, theta_);
  const Scalar t3 = std::log(v);
  const Scalar t4 = std::pow(-t3, theta_);
  const Scalar t5 = t2 + t4;
  const Scalar t7 = std::pow(t5, 1.0 / theta_);
  const Scalar t8 = t2 * t2;
  const Scalar t10 = std::pow(u, 2.0);
  const Scalar t11 = 1.0 / t10;
  const Scalar t12 = t1 * t1;
  const Scalar t13 = 1.0 / t12;
  const Scalar t14 = t11 * t13;
  const Scalar t15 = t5 * t5;
  const Scalar t17 = 1.0 / t15 / t5;
  const Scalar t20 = std::exp(-t7);
  const Scalar t22 = -1.0 + theta_ + t7;
  const Scalar t23 = 1.0 / v;
  const Scalar t25 = 1.0 / t3;
  const Scalar t26 = t22 * t23 * t25;
  const Scalar t29 = t7 * t4;
  const Scalar t35 = t23 * t25;
  const Scalar t36 = 1.0 / t15;
  const Scalar t40 = t7 * t7;
  const Scalar t41 = t40 * t4;
  const Scalar t48 = t8 * t20;
  const Scalar t51 = t23 * t17 * t25;
  const Scalar t62 = t29 * t2 * t20 * t22;
  const Scalar t63 = t36 * t11;
  result[0] = t7 * t8 * t14 * t17 * t4 * t20 * t26 + t29 * t2 * theta_ * t11 * t13 * t20 * t22 * t35 * t36 - t41 * t8 * t11 * t13 * t17 * t20 * t26 + t41 * t48 * t14 * t51 - 2.0 * t29 * t48 * t22 * t51 * t14 * theta_ - t62 * t35 * t63 / t1 - t62 * t35 * t63 * t13;

  const Scalar t8b = t4 * t4;
  const Scalar t9b = t7 * t8b;
  const Scalar t10b = std::pow(v, 2.0);
  const Scalar t11b = 1.0 / t10b;
  const Scalar t12b = t3 * t3;
  const Scalar t13b = 1.0 / t12b;
  const Scalar t14b = t11b * t13b;
  const Scalar t18 = t14b * t17;
  const Scalar t21 = t2 * t20;
  const Scalar t23b = 1.0 / u;
  const Scalar t26b = t22 * t23b * t25;
  const Scalar t33 = t21 * t22;
  const Scalar t36b = 1.0 / t15 * t23b * t25;
  const Scalar t40b = t40 * t8b;
  const Scalar t52 = t29 * t33;
  result[1] = t9b * t18 * t21 * t26b + t29 * theta_ * t11b * t13b * t33 * t36b - t40b * t11b * t2 * t13b * t17 * t20 * t26b + t40b * t21 * t14b * t23b * t17 * t25 - t52 * t11b / t3 * t36b - t52 * t14b * t36b - 2.0 * t9b * t33 * t18 * t23b * t25 * theta_;
  return result;
}

/* Get the PDF of the distribution */
Scalar GumbelCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return 0.0;
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar minusLogUPowerTheta = std::pow(-logU, theta_);
  const Scalar minusLogVPowerTheta = std::pow(-logV, theta_);
  const Scalar sum1 = minusLogUPowerTheta + minusLogVPowerTheta;
  const Scalar pow1 = std::pow(sum1, 1.0 / theta_);
  return pow1 * minusLogUPowerTheta * minusLogVPowerTheta * std::exp(-pow1) * (pow1 + theta_ - 1.0) / (u * v * logU * logV * sum1 * sum1);
}

/* Get the CDF of the distribution */
Scalar GumbelCopula::computeCDF(const Point & point) const
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
  return std::exp(-std::pow(std::pow(-std::log(u), theta_) + std::pow(-std::log(v), theta_), 1.0 / theta_));
}

/* Get the PDFGradient of the distribution */
Point GumbelCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null PDF gradient outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return Point(1, 0.0);
  const Scalar epsilon = ResourceMap::GetAsScalar("DistFunc-Precision");
  return Point(1, (GumbelCopula(theta_ + epsilon).computePDF(point) - GumbelCopula(theta_ - epsilon).computePDF(point)) / (2.0 * epsilon));
}

/* Get the CDFGradient of the distribution */
Point GumbelCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null CDF gradient outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0)) return Point(1, 0.0);
  // If we are in the support
  const Scalar logU = std::log(u);
  const Scalar logV = std::log(v);
  const Scalar minusLogUPowerTheta = std::pow(-logU, theta_);
  const Scalar minusLogVPowerTheta = std::pow(-logV, theta_);
  const Scalar sum1 = minusLogUPowerTheta + minusLogVPowerTheta;
  const Scalar inverseTheta = 1.0 / theta_;
  const Scalar pow1 = std::pow(sum1, inverseTheta);
  return Point(1, pow1 * std::exp(-pow1) * inverseTheta * (std::log(sum1) * inverseTheta - (minusLogUPowerTheta * std::log(-logU) + minusLogVPowerTheta * std::log(-logV)) / sum1));
}

/* Get the quantile of the distribution */
Point GumbelCopula::computeQuantile(const Scalar prob,
                                    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  if (q == 0.0) return getRange().getLowerBound();
  if (q == 1.0) return getRange().getUpperBound();
  return Point(2, std::exp(-std::exp(std::log(-std::log(q)) - M_LN2 / theta_)));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar GumbelCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  // If we are in the support
  const Scalar minusLogU = -std::log(u);
  const Scalar minusLogUPowTheta = std::pow(minusLogU, theta_);
  const Scalar minusLogVPowTheta = std::pow(-std::log(v), theta_);
  const Scalar sum = minusLogUPowTheta + minusLogVPowTheta;
  return std::pow(sum, -1.0 + 1.0 / theta_) * minusLogUPowTheta * std::exp(-std::pow(sum, 1.0 / theta_)) / (u * minusLogU);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar GumbelCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q <= 0.0) || (q >= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const Scalar u = y[0];
  const Scalar inverseThetaMinusOne = 1.0 / (theta_ - 1.0);
  const Scalar minusLogU = -std::log(u);
  const Scalar minusLogUPowTheta = std::pow(minusLogU, theta_);
  const Scalar factor = minusLogUPowTheta / (u * q * minusLogU);
  return std::exp(-std::pow(std::exp(theta_ * (std::log(factor) / (theta_ - 1.0) - SpecFunc::LambertW(std::pow(factor, inverseThetaMinusOne) * inverseThetaMinusOne))) - minusLogUPowTheta, 1.0 / theta_));
}

/* Compute the covariance of the distribution */
void GumbelCopula::computeCovariance() const
{
  CopulaImplementation::computeCovariance();
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix GumbelCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(0, 1) = 1.0 - 1.0 / theta_;
  return tau;
}

/* Parameters value accessor */
Point GumbelCopula::getParameter() const
{
  return Point(1, theta_);
}

void GumbelCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = GumbelCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description GumbelCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
Scalar GumbelCopula::computeArchimedeanGenerator(const Scalar t) const
{
  return std::pow(-std::log(t), theta_);
}

/* Compute the inverse of the archimedean generator */
Scalar GumbelCopula::computeInverseArchimedeanGenerator(const Scalar t) const
{
  return std::exp(-std::pow(t, 1.0 / theta_));
}

/* Compute the derivative of the density generator */
Scalar GumbelCopula::computeArchimedeanGeneratorDerivative(const Scalar t) const
{
  return -theta_ * std::pow(-std::log(t), theta_ - 1.0) / t;
}

/* Compute the seconde derivative of the density generator */
Scalar GumbelCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar t) const
{
  Scalar logT = std::log(t);
  return theta_ * (theta_ - logT - 1.0) * std::pow(-logT, theta_ - 2.0) / (t * t);
}

/* Tell if the distribution has independent copula */
Bool GumbelCopula::hasIndependentCopula() const
{
  return (theta_ == 1.0);
}

/* Theta accessor */
void GumbelCopula::setTheta(const Scalar theta)
{
  if (!(theta >= 1.0)) throw InvalidArgumentException(HERE) << "Theta MUST be greater or equal to 1";
  if (theta != theta_)
    {
      theta_ = theta;
      isAlreadyComputedCovariance_ = false;
    }
}

/* Theta accessor */
Scalar GumbelCopula::getTheta() const
{
  return theta_;
}

/* Method save() stores the object through the StorageManager */
void GumbelCopula::save(Advocate & adv) const
{
  ArchimedeanCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void GumbelCopula::load(Advocate & adv)
{
  ArchimedeanCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
