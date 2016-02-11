//                                               -*- C++ -*-
/**
 *  @brief The FarlieGumbelMorgensternCopula distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/FarlieGumbelMorgensternCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FarlieGumbelMorgensternCopula);

static const Factory<FarlieGumbelMorgensternCopula> RegisteredFactory;

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
FarlieGumbelMorgensternCopula::FarlieGumbelMorgensternCopula(const NumericalScalar theta)
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
NumericalPoint FarlieGumbelMorgensternCopula::getRealization() const
{
  NumericalPoint realization(2);
  // We use the algorithm described in Nelsen, "An Introduction to Copulas 2nd Edition", Exercise 3.23 to speed-up generation.
  const NumericalScalar u(RandomGenerator::Generate());
  const NumericalScalar t(RandomGenerator::Generate());
  const NumericalScalar a(1.0 + theta_ * (1.0 - 2.0 * u));
  const NumericalScalar b(std::sqrt(a * a - 4.0 * (a - 1.0) * t));
  realization[0] = u;
  realization[1] = 2.0 * t / (a + b);
  return realization;
}

/* Get the DDF of the distribution */
NumericalPoint FarlieGumbelMorgensternCopula::computeDDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u(point[0]);
  const NumericalScalar v(point[1]);
  NumericalPoint result(2, 0.0);
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
NumericalScalar FarlieGumbelMorgensternCopula::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u(point[0]);
  const NumericalScalar v(point[1]);
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return 0.0;
  }
  return 1.0 + theta_ * (2.0 * u - 1.0) * (2.0 * v - 1.0);
}

/* Get the CDF of the distribution */
NumericalScalar FarlieGumbelMorgensternCopula::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u(point[0]);
  const NumericalScalar v(point[1]);
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
NumericalPoint FarlieGumbelMorgensternCopula::computePDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u(point[0]);
  const NumericalScalar v(point[1]);
  // A copula has a null PDF outside of ]0, 1[^2
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return NumericalPoint(1, 0.0);
  }
  return NumericalPoint(1, (2.0 * u - 1.0) * (2.0 * v - 1.0));
}

/* Get the CDFGradient of the distribution */
NumericalPoint FarlieGumbelMorgensternCopula::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const NumericalScalar u(point[0]);
  const NumericalScalar v(point[1]);
  if ((u <= 0.0) || (u >= 1.0) || (v <= 0.0) || (v >= 1.0))
  {
    return NumericalPoint(1, 0.0);
  }
  return NumericalPoint(1, u * v * (1.0 - u) * (1.0 - v));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar FarlieGumbelMorgensternCopula::computeConditionalCDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return x;
  const NumericalScalar u(y[0]);
  const NumericalScalar v(x);
  // If we are in the support
  return v * (1.0 + theta_ * (v - 1.0) * (2.0 * u - 1.0));
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar FarlieGumbelMorgensternCopula::computeConditionalQuantile(const NumericalScalar q,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case for no conditioning or independent copula
  if ((q == 0.0) || (q == 1.0)) return q;
  // Initialize the conditional quantile with the quantile of the i-th marginal distribution
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  const NumericalScalar alpha(theta_ * (1.0 - 2.0 * y[0]));
  const NumericalScalar alpha1(1.0 + alpha);
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
NumericalPoint FarlieGumbelMorgensternCopula::getParameter() const
{
  return NumericalPoint(1, theta_);
}

void FarlieGumbelMorgensternCopula::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected 1 value, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = FarlieGumbelMorgensternCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description FarlieGumbelMorgensternCopula::getParameterDescription() const
{
  return Description(1, "theta");
}

/* Theta accessor */
void FarlieGumbelMorgensternCopula::setTheta(const NumericalScalar theta)
{
  if (std::abs(theta) > 1.0) throw InvalidArgumentException(HERE) << "Theta MUST be in [-1, 1]";
  theta_ = theta;
}

/* Theta accessor */
NumericalScalar FarlieGumbelMorgensternCopula::getTheta() const
{
  return theta_;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
FarlieGumbelMorgensternCopula::Implementation FarlieGumbelMorgensternCopula::getMarginal(const Indices & indices) const
{
  if (!indices.check(1)) throw InvalidArgumentException(HERE) << "The indices of an archimedean copula  must be in the range [0, 1] and  must be different";
  // General case
  const UnsignedInteger outputDimension(indices.getSize());
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
