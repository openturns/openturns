//                                               -*- C++ -*-
/**
 *  @brief The NonCentralStudent distribution
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
#include "openturns/NonCentralStudent.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCentralStudent)

static const Factory<NonCentralStudent> Factory_NonCentralStudent;


/* Default constructor */
NonCentralStudent::NonCentralStudent(const Scalar nu,
                                     const Scalar delta,
                                     const Scalar gamma)
  : ContinuousDistribution()
  , nu_(0.0)
  , delta_(delta)
  , gamma_(gamma)
{
  setName("NonCentralStudent");
  // This call set also the range.
  setNu(nu);
  setDimension(1);
}

/* Comparison operator */
Bool NonCentralStudent::operator ==(const NonCentralStudent & other) const
{
  if (this == &other) return true;
  return (nu_ == other.nu_) && (delta_ == other.delta_) && (gamma_ == other.gamma_);
}

Bool NonCentralStudent::equals(const DistributionImplementation & other) const
{
  const NonCentralStudent* p_other = dynamic_cast<const NonCentralStudent*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NonCentralStudent::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonCentralStudent::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << nu_
      << " delta=" << delta_
      << " gamma=" << gamma_;
  return oss;
}

String NonCentralStudent::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(nu = " << nu_ << ", delta = " << delta_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
NonCentralStudent * NonCentralStudent::clone() const
{
  return new NonCentralStudent(*this);
}

/* Get one realization of the distribution */
Point NonCentralStudent::getRealization() const
{
  return Point(1, gamma_ + DistFunc::rNonCentralStudent(nu_, delta_));
}

/* Get the PDF of the distribution */
Scalar NonCentralStudent::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_);
}

/* Get the CDF of the distribution */
Scalar NonCentralStudent::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_, false);
}

/* Get the PDFGradient of the distribution */
Point NonCentralStudent::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar eps = std::pow(pdfEpsilon_, 1.0 / 3.0);
  Point pdfGradient(3);
  pdfGradient[0] = (DistFunc::dNonCentralStudent(nu_ + eps, delta_, point[0] - gamma_) - DistFunc::dNonCentralStudent(nu_ - eps, delta_, point[0] - gamma_)) / (2.0 * eps);
  pdfGradient[1] = (DistFunc::dNonCentralStudent(nu_, delta_ + eps, point[0] - gamma_) - DistFunc::dNonCentralStudent(nu_, delta_ - eps, point[0] - gamma_)) / (2.0 * eps);
  pdfGradient[2] = (DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_ - eps) - DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_ + eps)) / (2.0 * eps);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point NonCentralStudent::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar eps = std::pow(cdfEpsilon_, 1.0 / 3.0);
  Point cdfGradient(3);
  cdfGradient[0] = (DistFunc::pNonCentralStudent(nu_ + eps, delta_, point[0] - gamma_, false) - DistFunc::pNonCentralStudent(nu_ - eps, delta_, point[0] - gamma_, false)) / (2.0 * eps);
  cdfGradient[1] = (DistFunc::pNonCentralStudent(nu_, delta_ + eps, point[0] - gamma_, false) - DistFunc::pNonCentralStudent(nu_, delta_ - eps, point[0] - gamma_, false)) / (2.0 * eps);
  cdfGradient[2] = (DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_ - eps, false) - DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_ + eps, false)) / (2.0 * eps);
  return cdfGradient;
}

/* Compute the mean of the distribution */
void NonCentralStudent::computeMean() const
{
  if (!(nu_ > 1.0)) throw NotDefinedException(HERE) << "Error: the mean is defined only for nu > 1 for a non central Student distribution";
  mean_ = Point(1, std::sqrt(0.5 * nu_) * std::exp(SpecFunc::LnGamma(0.5 * (nu_ - 1.0)) - SpecFunc::LnGamma(0.5 * nu_)) * delta_ + gamma_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point NonCentralStudent::getStandardDeviation() const
{
  if (!(nu_ > 2.0)) throw NotDefinedException(HERE) << "Error: the standard deviation is defined only for nu > 2 for a non central Student distribution";
  return Point(1, std::sqrt(getCovariance().operator()(0, 0)));
}

/* Get the skewness of the distribution */
Point NonCentralStudent::getSkewness() const
{
  Scalar mup1 = getMean()[0] - gamma_;
  Scalar mu2 = getCovariance().operator()(0, 0);
  return Point(1, mup1 * (nu_ * (2.0 * nu_ - 3.0 + delta_ * delta_) / ((nu_ - 2.0) * (nu_ - 3.0)) - 2.0 * mu2) * std::pow(mu2, -1.5));
}

/* Get the kurtosis of the distribution */
Point NonCentralStudent::getKurtosis() const
{
  Scalar mup1 = getMean()[0] - gamma_;
  Scalar mu2 = getCovariance().operator()(0, 0);
  Scalar delta2 = delta_ * delta_;
  return Point(1, (nu_ * nu_ * (3.0 + 6.0 * delta2 + delta2 * delta2) / ((nu_ - 2.0) * (nu_ - 4.0)) - mup1 * mup1 * (nu_ * ((nu_ + 1.0) * delta2 + 3.0 * (3.0 * nu_ - 5.0)) / ((nu_ - 2.0) * (nu_ - 3.0)) - 3.0 * mu2)) * std::pow(mu2, -2.0));
}

/* Get the moments of the standardized distribution */
Point NonCentralStudent::getStandardMoment(const UnsignedInteger n) const
{
  if (n >= nu_) throw NotDefinedException(HERE) << "Error: cannot compute a standard moment of order greater or equal to the number of degrees of freedom";
  UniVariatePolynomial p(Point(1, 1.0));
  for (UnsignedInteger k = 0; k < n; ++k) p = p.derivate() + p.incrementDegree(1);
  return Point(1, p(delta_) * std::exp(0.5 * n * std::log(0.5 * nu_) + SpecFunc::LogGamma(0.5 * (nu_ - n)) - SpecFunc::LogGamma(0.5 * nu_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution NonCentralStudent::getStandardRepresentative() const
{
  return new NonCentralStudent(nu_, delta_, 0.0);
}

/* Compute the covariance of the distribution */
void NonCentralStudent::computeCovariance() const
{
  if (!(nu_ > 2.0)) throw NotDefinedException(HERE) << "Error: the covariance is defined only for nu > 2 for a non central Student distribution";
  covariance_ = CovarianceMatrix(1);
  const Scalar mup1 = getMean()[0] - gamma_;
  covariance_(0, 0) =  nu_ / (nu_ - 2.0) * (1.0 + delta_ * delta_) - mup1 * mup1;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point NonCentralStudent::getParameter() const
{
  Point point(3);
  point[0] = nu_;
  point[1] = delta_;
  point[2] = gamma_;
  return point;
}

void NonCentralStudent::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = NonCentralStudent(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description NonCentralStudent::getParameterDescription() const
{
  Description description(3);
  description[0] = "nu";
  description[1] = "delta";
  description[2] = "gamma";
  return description;
}

/* Nu accessor */
void NonCentralStudent::setNu(const Scalar nu)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Nu MUST be strictly positive";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Nu accessor */
Scalar NonCentralStudent::getNu() const
{
  return nu_;
}


/* Delta accessor */
void NonCentralStudent::setDelta(const Scalar delta)
{
  if (delta != delta_)
  {
    delta_ = delta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Delta accessor */
Scalar NonCentralStudent::getDelta() const
{
  return delta_;
}

/* Gamma accessor */
void NonCentralStudent::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

/* Gamma accessor */
Scalar NonCentralStudent::getGamma() const
{
  return gamma_;
}

/* Method save() stores the object through the StorageManager */
void NonCentralStudent::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "delta_", delta_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void NonCentralStudent::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "delta_", delta_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
