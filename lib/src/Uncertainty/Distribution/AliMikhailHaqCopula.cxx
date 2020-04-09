//                                               -*- C++ -*-
/**
 *  @brief The AliMikhailHaqCopula distribution
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
#include "openturns/AliMikhailHaqCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AliMikhailHaqCopula)

static const Factory<AliMikhailHaqCopula> Factory_AliMikhailHaqCopula;

/* Default constructor */
AliMikhailHaqCopula::AliMikhailHaqCopula()
  : ArchimedeanCopula()
  , theta_(0.5)
{
  setName( "AliMikhailHaqCopula" );
  setDimension( 2 );
  computeRange();
}

/* Parameters constructor */
AliMikhailHaqCopula::AliMikhailHaqCopula(const Scalar theta)
  : ArchimedeanCopula()
  , theta_(0.0)
{
  setName( "AliMikhailHaqCopula" );
  // We set the dimension of the AliMikhailHaqCopula distribution
  setDimension( 2 );
  // Check the given theta
  setTheta(theta);
  computeRange();
}

/* Comparison operator */
Bool AliMikhailHaqCopula::operator ==(const AliMikhailHaqCopula & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool AliMikhailHaqCopula::equals(const DistributionImplementation & other) const
{
  const AliMikhailHaqCopula* p_other = dynamic_cast<const AliMikhailHaqCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String AliMikhailHaqCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << AliMikhailHaqCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String AliMikhailHaqCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
AliMikhailHaqCopula * AliMikhailHaqCopula::clone() const
{
  return new AliMikhailHaqCopula(*this);
}

/* Get one realization of the distribution */
Point AliMikhailHaqCopula::getRealization() const
{
  Point realization(2);
  // We use the general algorithm based on conditional CDF inversion
  const Scalar u = RandomGenerator::Generate();
  realization[0] = u;
  const Scalar v = RandomGenerator::Generate();
  const Scalar theta2 = theta_ * theta_;
  const Scalar a = 1.0 - u;
  const Scalar b = -theta_ * (2.0 * a * v + 1.0) + 2.0 * theta2 * a * a * v + 1.0;
  const Scalar delta = 4.0 * v * (a - 1.0);
  const Scalar c = theta2 * (a * delta + 1.0) - theta_ * (delta + 2.0) + 1.0;
  realization[1] = 2.0 * v * std::pow(a * theta_ - 1.0, 2) / (b + std::sqrt(c));
  return realization;
}

/* Get the DDF of the distribution */
Point AliMikhailHaqCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar u = point[0];
  const Scalar v = point[1];
  // A copula has a null DDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return Point(2, 0.0);
  }
  Point result(2);
  const Scalar t1 = theta_ * theta_;
  const Scalar t2 = t1 * v;
  const Scalar t3 = theta_ * v;
  const Scalar t5 = theta_ * u;
  const Scalar t6 = t5 * v;
  const Scalar t7 = -1 + theta_ - t3 - t5 + t6;
  const Scalar t8 = t7 * t7;
  const Scalar t12 = t1 * u;
  const Scalar t16 = t8 * t8;
  result[0] = -(theta_ - t1 + t2 + t3) / t8 / t7 + 3 * (1 + t3 + t5 - t2 - t12 + t12 * v + t6 + t1 - 2 * theta_) / t16 * (-theta_ + t3);
  result[1] = -(theta_ - t1 + t12 + t5) / t8 / t7 + 3 * (1 + t3 + t5 - t12 - t1 * v + t12 * v + t6 + t1 - 2 * theta_) / t16 * (-theta_ + t5);
  return result;
}

/* Get the PDF of the distribution */
Scalar AliMikhailHaqCopula::computePDF(const Point & point) const
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
  const Scalar prod = theta_ * (1.0 - u) * (1.0 - v);
  return (1.0 + theta_ * ((1.0 + u) * (1.0 + v) - 3.0 + prod)) / std::pow(1.0 - prod, 3);
}

/* Get the CDF of the distribution */
Scalar AliMikhailHaqCopula::computeCDF(const Point & point) const
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
  return u * v / (1.0 - theta_ * (1.0 - u) * (1.0 - v));
}

/* Compute the entropy of the distribution */
Scalar AliMikhailHaqCopula::computeEntropy() const
{
  if (theta_ == 0.0) return 0.0;
  if (std::abs(theta_) <= 0.25) return theta_ * theta_ * (-1.0 / 18.0 + (-1.0 / 36.0 + (-1.0 / 50.0 + (-1.0 / 75.0 + (-1.0 / 98.0 + (-3.0 / 392.0 + (-1.0 / 162.0 + (-2.0 / 405.0 + (-1.0 / 242.0 + (-5.0 / 1452.0 + (-1.0 / 338.0 + (-3.0 / 1183.0 + (-1.0 / 450.0 + (-7.0 / 3600.0 + (-1.0 / 578.0 + (-4.0 / 2601.0 + (-1.0 / 722.0 + (-9.0 / 7220.0 - theta_ / 882.0) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_);

  const Scalar t1 = theta_ - 1.0;
  const Scalar t3 = 1.0 / t1;
  const Scalar t5 = SpecFunc::DiLog(t3 * (theta_ + 1.0));
  const Scalar t9 = SpecFunc::DiLog(2.0 * t3 * theta_);
  const Scalar t11 = log1p(-theta_);
  const Scalar t12 = t11 * t11;
  const Scalar t15 = log1p(theta_);
  const Scalar t21 = 24.0 * theta_;
  const Scalar t24 = M_PI * M_PI;
  const Scalar t30 = -(12.0 * t1 * t5 + 12.0 * t9 * t1 + 12.0 * t12 * t1 - 12.0 * t11 * t1 * (M_LN2 + t15 + 2.0) + t15 * (12.0 * M_LN2 * t1 + t21 + 24.0) + theta_ * t24 - t24 - t21) / (48.0 * theta_);
  return t30;
}

/* Compute the covariance of the distribution */
void AliMikhailHaqCopula::computeCovariance() const
{
  covariance_ = CovarianceMatrix(2);
  covariance_(0, 0) = 1.0 / 12.0;
  covariance_(0, 1) = (std::abs(theta_) < 0.005633787383 ? (1.0 / 3.0 + (1.0 / 12.0 + (3.0 / 100.0 + (1.0 / 75.0 + (1.0 / 147.0 + (3.0 / 784.0 + theta_ / 432.0) / theta_) / theta_) / theta_) / theta_) / theta_) / (12.0 * theta_) : ((((1.0 + theta_) * SpecFunc::DiLog(theta_) - 2.0 * (1.0 - theta_) * log1p(-theta_)) / theta_) - 3.0) / theta_ - 0.25);
  covariance_(1, 1) = 1.0 / 12.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix AliMikhailHaqCopula::getKendallTau() const
{
  CorrelationMatrix tau(2);
  tau(1, 0) = (std::abs(theta_) < 0.005149755205 ? (2.0 / 9.0 + (1.0 / 18.0 + (1.0 / 45.0 + (1.0 / 90.0 + (2.0 / 315.0 + (1.0 / 252.0 + theta_ / 378.0) * theta_) * theta_) * theta_) * theta_) * theta_) * theta_ : 1.0 - 2.0 / (3.0 * theta_) + log1p(-theta_) * (-2.0 / 3.0 + 4.0 / (3.0 * theta_) - 2.0 / (3.0 * theta_ * theta_)));
  return tau;
}

/* Get the Spearman correlation of the distribution
We use the formulas developed in M. Machler, "Spearman's Rho for the AMH Copula:
a Beautiful Formula", https://cran.r-project.org/web/packages/copula/vignettes/rhoAMH-dilog.pdf
*/
CorrelationMatrix AliMikhailHaqCopula::getSpearmanCorrelation() const
{
  CorrelationMatrix rho(2);
  const Scalar t = std::abs(theta_);
  if (t < 7.0e-16)
  {
    rho(1, 0) = theta_ / 3.0;
    return rho;
  }
  if (t < 1.0e-04)
  {
    rho(1, 0) = theta_ / 3.0 * (1.0 + theta_ / 4.0);
    return rho;
  }
  if (t < 0.002)
  {
    rho(1, 0) = theta_ * (1.0 / 3.0 + theta_ * (1.0 / 12.0 + theta_ * 3.0 / 100.0));
    return rho;
  }
  if (t < 0.007)
  {
    rho(1, 0) = theta_ * (1.0 / 3.0 + theta_ * (1.0 / 12.0 + theta_ * (3.0 / 100.0 + theta_ / 75.0)));
    return rho;
  }
  if (t < 0.016)
  {
    rho(1, 0) = theta_ * (1.0 / 3.0 + theta_ * (1.0 / 12.0 + theta_ * (3.0 / 100.0 + theta_ * (1.0 / 75.0 + theta_ / 147.0))));
    return rho;
  }
  rho(1, 0) = 3.0 / theta_ * (4.0 * (1.0 + 1.0 / theta_) * SpecFunc::DiLog(theta_) - (theta_ < 1.0 ? 8.0 * (1.0 / theta_ - 1.0) * log1p(-theta_) : 0.0) - (theta_ + 12.0));
  return rho;
}

/* Get the PDFGradient of the distribution */
Point AliMikhailHaqCopula::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In AliMikhailHaqCopula::computePDFGradient(const Point & point) const";
}

/* Get the CDFGradient of the distribution */
Point AliMikhailHaqCopula::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In AliMikhailHaqCopula::computeCDFGradient(const Point & point) const";
}

/* Get the quantile of the distribution */
Point AliMikhailHaqCopula::computeQuantile(const Scalar prob,
    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const Scalar q = tail ? 1.0 - prob : prob;
  return Point(2, (q * theta_ + std::sqrt(q * (1.0 - theta_ + q * theta_))) / (1.0 + q * theta_));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar AliMikhailHaqCopula::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const Scalar u = y[0];
  const Scalar v = x;
  // If we are in the support
  return v * (1.0 - theta_ * (1.0 - v)) / std::pow(1.0 - theta_ * (1.0 - u) * (1.0 - v), 2);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar AliMikhailHaqCopula::computeConditionalQuantile(const Scalar q, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  if (q == 0.0) return 0.0;
  if (q == 1.0) return 1.0;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  // Optimized code given by Maple 13
  const Scalar u = y[0];
  const Scalar qTheta = q * theta_;
  const Scalar theta2 = theta_ * theta_;
  const Scalar qTheta2 = q * theta2;
  const Scalar u2 = u * u;
  const Scalar qTheta2U2 = qTheta2 * u2;
  const Scalar qThetaU = qTheta * u;
  const Scalar tmp1 = 2.0 * qThetaU;
  const Scalar tmp2 = 4.0 * qTheta2 * u;
  const Scalar tmp3 = std::sqrt(1.0 + theta2 + 4.0 * qThetaU - tmp2 + 4.0 * qTheta2U2 - 2.0 * theta_);
  return -0.5 * (theta_ + 2.0 * qTheta - 2.0 * qTheta2 - 2.0 * qTheta2U2 - tmp1 - 1.0 + tmp2 + tmp3) / (theta_ * (-1.0 + qTheta - tmp1 + qTheta * u2));
}

/* Compute the archimedean generator of the archimedean copula, i.e.
 * the function phi such that the CDF of the copula can
 * be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
 */
Scalar AliMikhailHaqCopula::computeArchimedeanGenerator(const Scalar t) const
{
  return log1p(-theta_ * (1.0 - t)) / t;
}

/* Compute the inverse of the archimedean generator */
Scalar AliMikhailHaqCopula::computeInverseArchimedeanGenerator(const Scalar t) const
{
  const Scalar y = t * (theta_ - 1.0) / theta_;
  return (y - SpecFunc::LambertW(-t * std::exp(y) / theta_)) / t;
}

/* Compute the derivative of the density generator */
Scalar AliMikhailHaqCopula::computeArchimedeanGeneratorDerivative(const Scalar t) const
{
  const Scalar u = 1.0 - theta_ * (1.0 - t);
  return (theta_ / u - std::log(u) / t) / t;
}

/* Compute the seconde derivative of the density generator */
Scalar AliMikhailHaqCopula::computeArchimedeanGeneratorSecondDerivative(const Scalar t) const
{
  const Scalar u = 1.0 - theta_ * (1.0 - t);
  return (-theta_ * theta_ / (u * u) - 2.0 * theta_ / (t * u) + 2.0 * std::log(u) / (t * t)) / t;
}

/* Parameters value accessor */
Point AliMikhailHaqCopula::getParameter() const
{
  return Point(1, theta_);
}

void AliMikhailHaqCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = AliMikhailHaqCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description AliMikhailHaqCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Tell if the distribution has independent copula */
Bool AliMikhailHaqCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Theta accessor */
void AliMikhailHaqCopula::setTheta(const Scalar theta)
{
  if ((theta <= -1.0) || (theta >= 1.0)) throw InvalidArgumentException(HERE) << "Theta MUST be in (-1, 1), here theta=" << theta;
  if (theta != theta_)
  {
    theta_ = theta;
    isAlreadyComputedCovariance_ = false;
  }
}

/* Theta accessor */
Scalar AliMikhailHaqCopula::getTheta() const
{
  return theta_;
}

/* Method save() stores the object through the StorageManager */
void AliMikhailHaqCopula::save(Advocate & adv) const
{
  ArchimedeanCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void AliMikhailHaqCopula::load(Advocate & adv)
{
  ArchimedeanCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
