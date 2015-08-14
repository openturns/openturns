//                                               -*- C++ -*-
/**
 *  @brief The Gumbel distribution
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
#include "Gumbel.hxx"
#include "RandomGenerator.hxx"
#include "SpecFunc.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Gumbel);

static Factory<Gumbel> RegisteredFactory("Gumbel");

/* Default constructor */
Gumbel::Gumbel()
  : ContinuousDistribution()
  , alpha_(1.0)
  , beta_(0.0)
{
  setName( "Gumbel" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Gumbel::Gumbel(const NumericalScalar arg1,
               const NumericalScalar arg2,
               const ParameterSet set)
  : ContinuousDistribution()
  , alpha_(0.0)
  , beta_(arg2)
{
  setName( "Gumbel" );
  switch (set)
  {
    case ALPHABETA:
      // This call set also the range.
      setAlpha(arg1);
      break;

    case MUSIGMA:
      // This call set also the range.
      setMuSigma(arg1, arg2);
      break;

    case AB:
      setAB(arg1, arg2);
      break;

    default:
      throw InvalidArgumentException(HERE) << "Invalid parameter set argument";
  }
  setDimension( 1 );
}

/* Comparison operator */
Bool Gumbel::operator ==(const Gumbel & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_);
}

/* String converter */
String Gumbel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Gumbel::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_;
  return oss;
}

String Gumbel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ")";
  return oss;
}

/* Virtual constructor */
Gumbel * Gumbel::clone() const
{
  return new Gumbel(*this);
}

/* Get one realization of the distribution */
NumericalPoint Gumbel::getRealization() const
{
  return NumericalPoint(1, beta_ - std::log(-std::log(RandomGenerator::Generate())) / alpha_);
}



/* Get the DDF of the distribution */
NumericalPoint Gumbel::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar expX(std::exp(-alpha_ * (point[0] - beta_)));
  return NumericalPoint(1, alpha_ * alpha_ * (expX - 1.0) * expX * std::exp(-expX));
}


/* Get the PDF of the distribution */
NumericalScalar Gumbel::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar expX(std::exp(-alpha_ * (point[0] - beta_)));
  return alpha_ * expX * std::exp(-expX);
}

NumericalScalar Gumbel::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar X(-alpha_ * (point[0] - beta_));
  return std::log(alpha_) + X - std::exp(X);
}


/* Get the CDF of the distribution */
NumericalScalar Gumbel::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(-alpha_ * (point[0] - beta_));
  const NumericalScalar expX(std::exp(x));
  return std::exp(-expX);
}

NumericalScalar Gumbel::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(-alpha_ * (point[0] - beta_));
  const NumericalScalar expX(std::exp(x));
  // -2.419227917539996841 = numerical bound for which the approximation has a relative error less than 1e-16
  if (x < -2.419227917539996841)
  {
    NumericalScalar value(expX);
    NumericalScalar coeff(expX);
    for (UnsignedInteger i = 2; i < 10; ++i)
    {
      coeff *= -expX / static_cast<NumericalScalar>(i);
      value += coeff;
    }
    return value;
  }
  return 1.0 - std::exp(-expX);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Gumbel::computeCharacteristicFunction(const NumericalScalar x) const
{
  return SpecFunc::Gamma(NumericalComplex(1.0, -x / alpha_)) * std::exp(NumericalComplex(0.0, beta_ * x));
}

NumericalComplex Gumbel::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return std::log(SpecFunc::Gamma(NumericalComplex(1.0, -x / alpha_))) + NumericalComplex(0.0, beta_ * x);
}

/* Get the PDFGradient of the distribution */
NumericalPoint Gumbel::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - beta_);
  const NumericalScalar expX(std::exp(-alpha_ * x));
  const NumericalScalar pdf(alpha_ * expX * std::exp(-expX));
  NumericalPoint pdfGradient(2);
  pdfGradient[0] = (1.0 / alpha_ - x * (1.0 - expX)) * pdf;
  pdfGradient[1] = alpha_ * (1.0 - expX) * pdf;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint Gumbel::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - beta_);
  const NumericalScalar expX(std::exp(-alpha_ * x));
  const NumericalScalar cdf(std::exp(-expX));
  NumericalPoint cdfGradient(2);
  cdfGradient[0] = x * expX * cdf;
  cdfGradient[1] = -alpha_ * expX * cdf;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar Gumbel::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return beta_ - std::log(-log1p(-prob)) / alpha_;
  return beta_ - std::log(-std::log(prob)) / alpha_;
}

/* Compute the mean of the distribution */
void Gumbel::computeMean() const
{
  mean_ = NumericalPoint(1, beta_ + SpecFunc::EulerConstant / alpha_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Gumbel::getStandardDeviation() const
{
  return NumericalPoint(1, getSigma());
}

/* Get the skewness of the distribution */
NumericalPoint Gumbel::getSkewness() const
{
  // 1.139547099404648657492793 = 12 * sqrt(6) * Zeta(3) / Pi^3
  return NumericalPoint(1, 1.139547099404648657492793);
}

/* Get the kurtosis of the distribution */
NumericalPoint Gumbel::getKurtosis() const
{
  // 5.4 = 27/5
  return NumericalPoint(1, 5.4);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Gumbel::Implementation Gumbel::getStandardRepresentative() const
{
  return Gumbel(1.0, 0.0).clone();
}

/* Compute the covariance of the distribution */
void Gumbel::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = SpecFunc::PI2_6 / (alpha_ * alpha_);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Gumbel::NumericalPointWithDescriptionCollection Gumbel::getParametersCollection() const
{
  NumericalPointWithDescriptionCollection parameters(1);
  NumericalPointWithDescription point(2);
  Description description(point.getDimension());
  point[0] = alpha_;
  point[1] = beta_;
  description[0] = "alpha";
  description[1] = "beta";
  point.setDescription(description);
  point.setName(getDescription()[0]);
  parameters[0] = point;
  return parameters;
}

void Gumbel::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  const NumericalScalar w(getWeight());
  *this = Gumbel(parametersCollection[0][0], parametersCollection[0][1]);
  setWeight(w);
}


/* Alpha accessor */
void Gumbel::setAlpha(const NumericalScalar alpha)
{
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar Gumbel::getAlpha() const
{
  return alpha_;
}


/* M accessor */
void Gumbel::setBeta(const NumericalScalar beta)
{
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on beta
    computeRange();
  }
}

NumericalScalar Gumbel::getBeta() const
{
  return beta_;
}

/* Mu accessor */
void Gumbel::setMuSigma(const NumericalScalar mu,
                        const NumericalScalar sigma)
{
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Sigma must be > 0, here sigma=" << sigma;
  alpha_ = SpecFunc::PI_SQRT6 / sigma;
  beta_ = mu - SpecFunc::EULERSQRT6_PI * sigma;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

NumericalScalar Gumbel::getMu() const
{
  return beta_ + SpecFunc::EulerConstant / alpha_;
}

NumericalScalar Gumbel::getSigma() const
{
  return SpecFunc::PI_SQRT6 / alpha_;
}

void Gumbel::setAB(const NumericalScalar a,
                   const NumericalScalar b)
{
  if ( b <= 0. ) throw InvalidArgumentException(HERE) << "b must be positive";
  beta_ = a;
  setAlpha( 1. / b );
}

NumericalScalar Gumbel::getA() const
{
  return beta_;
}

NumericalScalar Gumbel::getB() const
{
  return 1 / alpha_;
}

/* Method save() stores the object through the StorageManager */
void Gumbel::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void Gumbel::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  computeRange();
}



END_NAMESPACE_OPENTURNS
