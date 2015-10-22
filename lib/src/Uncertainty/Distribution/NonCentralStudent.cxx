//                                               -*- C++ -*-
/**
 *  @brief The NonCentralStudent distribution
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
#include "NonCentralStudent.hxx"
#include "RandomGenerator.hxx"
#include "SpecFunc.hxx"
#include "DistFunc.hxx"
#include "PersistentObjectFactory.hxx"
#include "Brent.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCentralStudent);

static const Factory<NonCentralStudent> RegisteredFactory;


/* Default constructor */
NonCentralStudent::NonCentralStudent(const NumericalScalar nu,
                                     const NumericalScalar delta,
                                     const NumericalScalar gamma)
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

String NonCentralStudent::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(nu = " << nu_ << ", delta = " << delta_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
NonCentralStudent * NonCentralStudent::clone() const
{
  return new NonCentralStudent(*this);
}

/* Get one realization of the distribution */
NumericalPoint NonCentralStudent::getRealization() const
{
  return NumericalPoint(1, gamma_ + DistFunc::rNonCentralStudent(nu_, delta_));
}

/* Get the PDF of the distribution */
NumericalScalar NonCentralStudent::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_);
}

/* Get the CDF of the distribution */
NumericalScalar NonCentralStudent::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_, false);
}

/* Get the PDFGradient of the distribution */
NumericalPoint NonCentralStudent::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar eps(std::pow(pdfEpsilon_, 1.0 / 3.0));
  NumericalPoint pdfGradient(3);
  pdfGradient[0] = (DistFunc::dNonCentralStudent(nu_ + eps, delta_, point[0] - gamma_) - DistFunc::dNonCentralStudent(nu_ - eps, delta_, point[0] - gamma_)) / (2.0 * eps);
  pdfGradient[1] = (DistFunc::dNonCentralStudent(nu_, delta_ + eps, point[0] - gamma_) - DistFunc::dNonCentralStudent(nu_, delta_ - eps, point[0] - gamma_)) / (2.0 * eps);
  pdfGradient[2] = (DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_ - eps) - DistFunc::dNonCentralStudent(nu_, delta_, point[0] - gamma_ + eps)) / (2.0 * eps);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint NonCentralStudent::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar eps(std::pow(cdfEpsilon_, 1.0 / 3.0));
  NumericalPoint cdfGradient(3);
  cdfGradient[0] = (DistFunc::pNonCentralStudent(nu_ + eps, delta_, point[0] - gamma_, false) - DistFunc::pNonCentralStudent(nu_ - eps, delta_, point[0] - gamma_, false)) / (2.0 * eps);
  cdfGradient[1] = (DistFunc::pNonCentralStudent(nu_, delta_ + eps, point[0] - gamma_, false) - DistFunc::pNonCentralStudent(nu_, delta_ - eps, point[0] - gamma_, false)) / (2.0 * eps);
  cdfGradient[2] = (DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_ - eps, false) - DistFunc::pNonCentralStudent(nu_, delta_, point[0] - gamma_ + eps, false)) / (2.0 * eps);
  return cdfGradient;
}

/* Compute the mean of the distribution */
void NonCentralStudent::computeMean() const
{
  if (nu_ <= 1.0) throw NotDefinedException(HERE) << "Error: the mean is defined only for nu > 1 for a non central Student distribution";
  mean_ = NumericalPoint(1, std::sqrt(0.5 * nu_) * std::exp(SpecFunc::LnGamma(0.5 * (nu_ - 1.0)) - SpecFunc::LnGamma(0.5 * nu_)) * delta_ + gamma_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint NonCentralStudent::getStandardDeviation() const
{
  if (nu_ <= 2.0) throw NotDefinedException(HERE) << "Error: the standard deviation is defined only for nu > 2 for a non central Student distribution";
  return NumericalPoint(1, std::sqrt(getCovariance().operator()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint NonCentralStudent::getSkewness() const
{
  NumericalScalar mup1(getMean()[0] - gamma_);
  NumericalScalar mu2(getCovariance().operator()(0, 0));
  return NumericalPoint(1, mup1 * (nu_ * (2.0 * nu_ - 3.0 + delta_ * delta_) / ((nu_ - 2.0) * (nu_ - 3.0)) - 2.0 * mu2) * std::pow(mu2, -1.5));
}

/* Get the kurtosis of the distribution */
NumericalPoint NonCentralStudent::getKurtosis() const
{
  NumericalScalar mup1(getMean()[0] - gamma_);
  NumericalScalar mu2(getCovariance().operator()(0, 0));
  NumericalScalar delta2(delta_ * delta_);
  return NumericalPoint(1, (nu_ * nu_ * (3.0 + 6.0 * delta2 + delta2 * delta2) / ((nu_ - 2.0) * (nu_ - 4.0)) - mup1 * mup1 * (nu_ * ((nu_ + 1.0) * delta2 + 3.0 * (3.0 * nu_ - 5.0)) / ((nu_ - 2.0) * (nu_ - 3.0)) - 3.0 * mu2)) * std::pow(mu2, -2.0));
}

/* Get the moments of the standardized distribution */
NumericalPoint NonCentralStudent::getStandardMoment(const UnsignedInteger n) const
{
  if (n >= nu_) throw NotDefinedException(HERE) << "Error: cannot compute a standard moment of order greater or equal to the number of degrees of freedom";
  UniVariatePolynomial p(NumericalPoint(1, 1.0));
  for (UnsignedInteger k = 0; k < n; ++k) p = p.derivate() + p.incrementDegree(1);
  return NumericalPoint(1, p(delta_) * std::exp(0.5 * n * std::log(0.5 * nu_) + SpecFunc::LogGamma(0.5 * (nu_ - n)) - SpecFunc::LogGamma(0.5 * nu_)));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
NonCentralStudent::Implementation NonCentralStudent::getStandardRepresentative() const
{
  return NonCentralStudent(nu_, delta_, 0.0).clone();
}

/* Compute the covariance of the distribution */
void NonCentralStudent::computeCovariance() const
{
  if (nu_ <= 2.0) throw NotDefinedException(HERE) << "Error: the covariance is defined only for nu > 2 for a non central Student distribution";
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar mup1(getMean()[0] - gamma_);
  covariance_(0, 0) =  nu_ / (nu_ - 2.0) * (1.0 + delta_ * delta_) - mup1 * mup1;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
NumericalPoint NonCentralStudent::getParameters() const
{
  NumericalPoint point(3);
  point[0] = nu_;
  point[1] = delta_;
  point[2] = gamma_;
  return point;
}

void NonCentralStudent::setParameters(const NumericalPoint & parameters)
{
  if (parameters.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 parameters, got " << parameters.getSize(); 
  const NumericalScalar w = getWeight();
  *this = NonCentralStudent(parameters[0], parameters[1], parameters[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description NonCentralStudent::getParametersDescription() const
{
  Description description(3);
  description[0] = "nu";
  description[1] = "delta";
  description[2] = "gamma";
  return description;
}

/* Nu accessor */
void NonCentralStudent::setNu(const NumericalScalar nu)
{
  if (nu <= 0.0) throw InvalidArgumentException(HERE) << "Nu MUST be strictly positive";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* Nu accessor */
NumericalScalar NonCentralStudent::getNu() const
{
  return nu_;
}


/* Delta accessor */
void NonCentralStudent::setDelta(const NumericalScalar delta)
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
NumericalScalar NonCentralStudent::getDelta() const
{
  return delta_;
}

/* Gamma accessor */
void NonCentralStudent::setGamma(const NumericalScalar gamma)
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
NumericalScalar NonCentralStudent::getGamma() const
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
