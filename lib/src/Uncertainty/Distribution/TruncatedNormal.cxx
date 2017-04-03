//                                               -*- C++ -*-
/**
 *  @brief The TruncatedNormal distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/TruncatedNormal.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedNormal);

static const Factory<TruncatedNormal> Factory_TruncatedNormal;

/* Default onstructor */
TruncatedNormal::TruncatedNormal()
  : ContinuousDistribution()
  , mu_(0.0)
  , sigma_(1.0)
  , a_(-1.0)
  , b_(1.0)
  , aNorm_(-1.0)
  , bNorm_(1.0)
  // 0.24197072451914333757 = exp(-1/2)/sqrt{2\pi}
  , phiANorm_(0.24197072451914333757)
  , phiBNorm_(0.24197072451914333757)
  // 0.15865525393145704647 = Phi(-1)
  , PhiANorm_(0.15865525393145704647)
  // 0.84134474606854292578 = Phi(1)
  , PhiBNorm_(0.84134474606854292578)
  // 1.46479477349154407761 = 1 / (Phi(1) - Phi(-1))
  , normalizationFactor_(1.46479477349154407761)
{
  setName("TruncatedNormal");
  setDimension(1);
  computeRange();
}

/* Default onstructor */
TruncatedNormal::TruncatedNormal(const NumericalScalar mu,
                                 const NumericalScalar sigma,
                                 const NumericalScalar a,
                                 const NumericalScalar b)
  : ContinuousDistribution()
  , mu_(mu)
  , sigma_(0.0)
  , a_(a)
  , b_(b)
  , aNorm_(0.0)
  , bNorm_(0.0)
  , phiANorm_(0.0)
  , phiBNorm_(0.0)
  , PhiANorm_(0.0)
  , PhiBNorm_(0.0)
  , normalizationFactor_(0.0)
{
  setName("TruncatedNormal");
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution with sigma <=0. Here, sigma=" << sigma;
  if (a >= b) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution with a >= b. Here, a=" << a << " and b=" << b;
  setSigma(sigma);
  setDimension(1);
  const NumericalScalar iSigma = 1.0 / sigma_;
  aNorm_ = (a_ - mu_) * iSigma;
  bNorm_ = (b_ - mu_) * iSigma;
  PhiANorm_ = DistFunc::pNormal(aNorm_);
  PhiBNorm_ = DistFunc::pNormal(bNorm_);
  NumericalScalar denominator = PhiBNorm_ - PhiANorm_;
  // If left tail truncature, use tail CDF to compute the normalization factor
  if (aNorm_ > 0.0) denominator = DistFunc::pNormal(aNorm_, true) - DistFunc::pNormal(bNorm_, true);
  if (!(denominator > 0.0)) throw InvalidArgumentException(HERE) << "Error: the truncation interval has a too small measure. Here, measure=" << denominator;
  normalizationFactor_ = 1.0 / denominator;
  phiANorm_ = SpecFunc::ISQRT2PI * std::exp(-0.5 * aNorm_ * aNorm_);
  phiBNorm_ = SpecFunc::ISQRT2PI * std::exp(-0.5 * bNorm_ * bNorm_);
  computeRange();
}



/* Comparison operator */
Bool TruncatedNormal::operator ==(const TruncatedNormal & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (sigma_ == other.sigma_) &&
         (a_ == other.a_) && (b_ == other.b_);
}

Bool TruncatedNormal::equals(const DistributionImplementation & other) const
{
  const TruncatedNormal* p_other = dynamic_cast<const TruncatedNormal*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String TruncatedNormal::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TruncatedNormal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String TruncatedNormal::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

/* Virtual constructor */
TruncatedNormal * TruncatedNormal::clone() const
{
  return new TruncatedNormal(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void TruncatedNormal::computeRange()
{
  setRange(Interval(a_, b_));
}


/* Get one realization of the distribution */
Point TruncatedNormal::getRealization() const
{
  /* Find a better method, e.g.
     Algorithm from John Geweke, "Efficient Simulation from the Multivariate Normal and Student-t Distributions Subject to Linear Constraints and the Evaluation of Constraint Probabilities", communication at the meeting "Computer Science and Statistics: the Twenty-Third Symposium on the Interface", April 22-24, 1991. */
  // If the truncation is strong, use CDF inversion, else use rejection. The cut-off must balance the cost of the two methods
  if (PhiBNorm_ - PhiANorm_ < 0.25) return computeQuantile(RandomGenerator::Generate());
  NumericalScalar value = -1.0;
  do
  {
    value = DistFunc::rNormal();
  }
  while ((value < aNorm_) || (value >= bNorm_));
  return Point(1, mu_ + sigma_ * value);
}


/* Get the DDF of the distribution */
Point TruncatedNormal::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= a_) || (x > b_)) return Point(1, 0.0);
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  return Point(1, -normalizationFactor_ * xNorm * SpecFunc::ISQRT2PI * std::exp(-0.5 * xNorm * xNorm) * iSigma * iSigma);
}


/* Get the PDF of the distribution */
NumericalScalar TruncatedNormal::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= a_) || (x > b_)) return 0.0;
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  return normalizationFactor_ * std::exp(-0.5 * xNorm * xNorm) * SpecFunc::ISQRT2PI * iSigma;
}


/* Get the logarithm of the PDF of the distribution */
NumericalScalar TruncatedNormal::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if ((x <= a_) || (x > b_)) return SpecFunc::LogMinNumericalScalar;
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  return std::log(SpecFunc::ISQRT2PI * iSigma * normalizationFactor_) - 0.5 * xNorm * xNorm;
}


/* Get the CDF of the distribution */
NumericalScalar TruncatedNormal::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  return normalizationFactor_ * (DistFunc::pNormal((x - mu_) / sigma_) - PhiANorm_);
}

NumericalScalar TruncatedNormal::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  if (x <= a_) return 1.0;
  if (x > b_) return 0.0;
  // Don't call pNormal with tail in the next line
  return normalizationFactor_ * (PhiBNorm_ - DistFunc::pNormal((x - mu_) / sigma_));
}

/* Get the product minimum volume interval containing a given probability of the distribution */
Interval TruncatedNormal::computeMinimumVolumeIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const
{
  // Unimodal decreasing with mode at a_
  if (mu_ <= a_)
    return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, false, marginalProb);
  // Unimodal increasing with mode at b_
  if (mu_ >= b_)
    return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, true, marginalProb);
  // Unimodal with mode in (a, b)
  // Different cases here:
  // 1) PDF(a) >= PDF(b): let \alpha\in(a,b) be such that PDF(\alpha)==PDF(a)
  // 1a) P([a,\alpha])>=prob: the minimum volume interval (MVI) [c,d] is such that
  //     PDF(c)=PDF(d) and a <= c < d <= \alpha -> this is a root-finding MVI
  // 1b) P([a,\alpha])<prob: the MVI [c, d] is such that
  //     c == a, \alpha < d <= b -> this is an unilateral MVI
  // 2) PDF(a) < PDF(b): let \beta\in(a,b) be such that PDF(\beta)==PDF(b)
  // 2a) PDF([\beta,b])>=prob: the MVI [c,d] is such that
  //     PDF(c)=PDF(d) and beta <= c < d <= b -> this is a root-finding MVI
  // 2b) P([\beta,b])<prob: the minimum volume interval [c, d] is such that
  //     d == b, a <= c < \beta -> this is a tail unilateral MVI

  // 1)
  if (phiANorm_ >= phiBNorm_)
    {
      // Find \alpha
      PDFWrapper pdfWrapper(this);
      Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
      const NumericalScalar alpha = solver.solve(pdfWrapper, normalizationFactor_ * phiANorm_ / sigma_ , mu_, b_);
      const NumericalScalar probability = computeProbability(Interval(a_, alpha));
      // 1a)
      if (probability >= prob)
	return computeUnivariateMinimumVolumeIntervalByRootFinding(prob, marginalProb);
      // 1b)
      return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, false, marginalProb);
    }
  // 2)
  // Find \beta
  PDFWrapper pdfWrapper(this);
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  const NumericalScalar beta = solver.solve(pdfWrapper, normalizationFactor_ * phiBNorm_ / sigma_, a_, mu_);
  const NumericalScalar probability = computeProbability(Interval(beta, b_));
  // 2a)
  if (probability >= prob)
    return computeUnivariateMinimumVolumeIntervalByRootFinding(prob, marginalProb);
  // 2b)
  return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, true, marginalProb);
}

/* Get the minimum volume level set containing a given probability of the distribution */
LevelSet TruncatedNormal::computeMinimumVolumeLevelSetWithThreshold(const NumericalScalar prob, NumericalScalar & threshold) const
{
  const Interval interval(computeMinimumVolumeInterval(prob));
  const NumericalScalar lower = interval.getLowerBound()[0];
  const NumericalScalar upper = interval.getUpperBound()[0];
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  NumericalScalar minusLogPDFThreshold = -1.0;
  if (lower == a_)
    minusLogPDFThreshold = -computeLogPDF(upper);
  else
    minusLogPDFThreshold = -computeLogPDF(lower);

  threshold = std::exp(-minusLogPDFThreshold);
  return LevelSet(minimumVolumeLevelSetFunction, minusLogPDFThreshold);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex TruncatedNormal::computeCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalScalar iSigma2 = 1.0 / (sigma_ * std::sqrt(2.0));
  const NumericalScalar alpha = (a_ - mu_) * iSigma2;
  const NumericalScalar beta = (b_ - mu_) * iSigma2;
  const NumericalScalar erf1 = SpecFunc::Erf(alpha);
  const NumericalScalar erf2 = SpecFunc::Erf(beta);
  const NumericalScalar t = x * sigma_ / std::sqrt(2.0);
  const NumericalComplex w1(SpecFunc::Faddeeva(NumericalComplex(-t, -alpha)));
  const NumericalComplex w2(SpecFunc::Faddeeva(NumericalComplex(-t, -beta)));
  return std::exp(NumericalComplex(0.0, x * mu_)) * (w2 * std::exp(NumericalComplex(-beta * beta, 2.0 * beta * t)) - w1 * std::exp(NumericalComplex(-alpha * alpha, 2.0 * alpha * t))) / (erf2 - erf1);
}

NumericalComplex TruncatedNormal::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalScalar iSigma2 = 1.0 / (sigma_ * std::sqrt(2.0));
  const NumericalScalar alpha = (a_ - mu_) * iSigma2;
  const NumericalScalar beta = (b_ - mu_) * iSigma2;
  const NumericalScalar erf1 = SpecFunc::Erf(alpha);
  const NumericalScalar erf2 = SpecFunc::Erf(beta);
  const NumericalScalar t = x * sigma_ / std::sqrt(2.0);
  const NumericalComplex w1(SpecFunc::Faddeeva(NumericalComplex(-t, -alpha)));
  const NumericalComplex w2(SpecFunc::Faddeeva(NumericalComplex(-t, -beta)));
  return NumericalComplex(0.0, x * mu_) + std::log(w2 * std::exp(NumericalComplex(-beta * beta, 2.0 * beta * t)) - w1 * std::exp(NumericalComplex(-alpha * alpha, 2.0 * alpha * t))) - std::log(erf2 - erf1);
}

/* Get the PDFGradient of the distribution */
Point TruncatedNormal::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  Point pdfGradient(4, 0.0);
  if ((x <= a_) || (x > b_)) return pdfGradient;
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  const NumericalScalar iDenom = normalizationFactor_ * iSigma;
  const NumericalScalar iDenom2 = iDenom * iDenom;
  const NumericalScalar factPhiXNorm = std::exp(-0.5 * xNorm * xNorm) * SpecFunc::ISQRT2PI * iDenom2;
  pdfGradient[0] = factPhiXNorm * (xNorm * (PhiBNorm_ - PhiANorm_) + phiBNorm_ - phiANorm_);
  pdfGradient[1] = factPhiXNorm * ((xNorm * xNorm - 1.0) * (PhiBNorm_ - PhiANorm_) + bNorm_ * phiBNorm_ - aNorm_ * phiANorm_);
  pdfGradient[2] = factPhiXNorm * phiANorm_;
  pdfGradient[3] = -factPhiXNorm * phiBNorm_;
  return pdfGradient;
}

/* Get the LogPDFGradient of the distribution */
Point TruncatedNormal::computeLogPDFGradient(const Point & point) const
{
  if (point.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "In TruncatedNormal::computeLogPDFGradient, the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  Point logPdfGradient(getParameterDimension());
  if (!(x > a_) || !(x < b_)) return logPdfGradient;
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  const NumericalScalar aNorm = (a_ - mu_) * iSigma;
  const NumericalScalar bNorm = (b_ - mu_) * iSigma;
  const NumericalScalar iDenom = normalizationFactor_ * iSigma;
  logPdfGradient[0] = xNorm * iSigma +  iDenom * (phiBNorm_ - phiANorm_);
  logPdfGradient[1] = iSigma * ( -1.0 + xNorm * xNorm ) + iDenom * (phiBNorm_ * bNorm - phiANorm_ * aNorm);
  logPdfGradient[2] = phiANorm_ * iDenom;
  logPdfGradient[3] = - phiBNorm_ * iDenom;
  return logPdfGradient;
}

/* Get the CDFGradient of the distribution */
Point TruncatedNormal::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  Point cdfGradient(4, 0.0);
  if ((x <= a_) || (x > b_)) return cdfGradient;
  const NumericalScalar iSigma = 1.0 / sigma_;
  const NumericalScalar xNorm = (x - mu_) * iSigma;
  const NumericalScalar iDenom = normalizationFactor_ * normalizationFactor_ * iSigma;
  const NumericalScalar phiXNorm = std::exp(-0.5 * xNorm * xNorm) * SpecFunc::ISQRT2PI;
  const NumericalScalar PhiXNorm = DistFunc::pNormal(xNorm);
  cdfGradient[0] = (phiANorm_ * PhiBNorm_ - PhiANorm_ * phiBNorm_ + phiXNorm * PhiANorm_ - PhiXNorm * phiANorm_ + phiBNorm_ * PhiXNorm - PhiBNorm_ * phiXNorm) * iDenom;
  cdfGradient[1] = (phiANorm_ * aNorm_ * PhiBNorm_ - PhiANorm_ * phiBNorm_ * bNorm_ + phiXNorm * xNorm * PhiANorm_ - PhiXNorm * phiANorm_ * aNorm_ + phiBNorm_ * bNorm_ * PhiXNorm - PhiBNorm_ * phiXNorm * xNorm) * iDenom;
  cdfGradient[2] = phiANorm_ * (PhiXNorm - PhiBNorm_) * iDenom;
  cdfGradient[3] = phiBNorm_ * (PhiANorm_ - PhiXNorm) * iDenom;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar TruncatedNormal::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return mu_ + sigma_ * DistFunc::qNormal(PhiBNorm_ - prob / normalizationFactor_);
  return mu_ + sigma_ * DistFunc::qNormal(PhiANorm_ + prob / normalizationFactor_);
}

/* Compute the mean of the distribution */
void TruncatedNormal::computeMean() const
{
  mean_ = Point(1, mu_ - sigma_ * (phiBNorm_ - phiANorm_) * normalizationFactor_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point TruncatedNormal::getStandardDeviation() const
{
  const NumericalScalar ratio = (phiBNorm_ - phiANorm_) * normalizationFactor_;
  return Point(1, sigma_ * std::sqrt(1.0 - (bNorm_ * phiBNorm_ - aNorm_ * phiANorm_) * normalizationFactor_ - ratio * ratio));
}

/* Get the skewness of the distribution */
Point TruncatedNormal::getSkewness() const
{
  const NumericalScalar ratio = (phiBNorm_ - phiANorm_) * normalizationFactor_;
  const NumericalScalar ratio2 = ratio * ratio;
  const NumericalScalar crossTerm1 = (bNorm_ * phiBNorm_ - aNorm_ * phiANorm_) * normalizationFactor_;
  const NumericalScalar crossTerm2 = (bNorm_ * bNorm_ * phiBNorm_ - aNorm_ * aNorm_ * phiANorm_) * normalizationFactor_;
  return Point(1, (-2.0 * ratio * ratio2 - 3.0 * ratio * crossTerm1 + ratio - crossTerm2) / std::pow(1.0 - crossTerm1 - ratio2, 1.5));
}

/* Get the kurtosis of the distribution */
Point TruncatedNormal::getKurtosis() const
{
  const NumericalScalar ratio = (phiBNorm_ - phiANorm_) * normalizationFactor_;
  const NumericalScalar ratio2 = ratio * ratio;
  const NumericalScalar crossTerm1 = (bNorm_ * phiBNorm_ - aNorm_ * phiANorm_) * normalizationFactor_;
  const NumericalScalar crossTerm2 = (bNorm_ * bNorm_ * phiBNorm_ - aNorm_ * aNorm_ * phiANorm_) * normalizationFactor_;
  const NumericalScalar crossTerm3 = (bNorm_ * bNorm_ * bNorm_ * phiBNorm_ - aNorm_ * aNorm_ * aNorm_ * phiANorm_) * normalizationFactor_;
  return Point(1, (3.0 - 3.0 * ratio2 * ratio2 - 6.0 * ratio2 * crossTerm1 - 2.0 * ratio * (ratio + 2.0 * crossTerm2) - 3.0 * crossTerm1 - crossTerm3) / std::pow(1.0 - crossTerm1 - ratio2, 2.0));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
TruncatedNormal::Implementation TruncatedNormal::getStandardRepresentative() const
{
  return TruncatedNormal((2.0 * mu_ - (b_ + a_)) / (b_ - a_), 2.0 * sigma_ / (b_ - a_), -1.0, 1.0).clone();
}

/* Compute the covariance of the distribution */
void TruncatedNormal::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar ratio = (phiBNorm_ - phiANorm_) * normalizationFactor_;
  covariance_(0, 0) = sigma_ * sigma_ * (1.0 - (bNorm_ * phiBNorm_ - aNorm_ * phiANorm_) * normalizationFactor_ - ratio * ratio);
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point TruncatedNormal::getParameter() const
{
  Point point(4);
  point[0] = mu_;
  point[1] = sigma_;
  point[2] = a_;
  point[3] = b_;
  return point;
}

void TruncatedNormal::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = TruncatedNormal(parameter[0], parameter[1], parameter[2], parameter[3]);
  setWeight(w);
}

/* Parameters description accessor */
Description TruncatedNormal::getParameterDescription() const
{
  Description description(4);
  description[0] = "mu";
  description[1] = "sigma";
  description[2] = "a";
  description[3] = "b";
  return description;
}

/* Check if the distribution is elliptical */
Bool TruncatedNormal::isElliptical() const
{
  return std::abs(mu_ - 0.5 * (a_ + b_)) < ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon");
}

/* Mu accessor */
void TruncatedNormal::setMu(const NumericalScalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

NumericalScalar TruncatedNormal::getMu() const
{
  return mu_;
}


/* Sigma accessor */
void TruncatedNormal::setSigma(const NumericalScalar sigma)
{
  if (!(sigma > 0.0)) throw InvalidArgumentException(HERE) << "Sigma MUST be positive";
  if (sigma != sigma_)
  {
    sigma_ = sigma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

NumericalScalar TruncatedNormal::getSigma() const
{
  return sigma_;
}


/* A accessor */
void TruncatedNormal::setA(const NumericalScalar a)
{
  if (a != a_)
  {
    a_ = a;
    const NumericalScalar iSigma = 1.0 / sigma_;
    aNorm_ = (a_ - mu_) * iSigma;
    PhiANorm_ = DistFunc::pNormal(aNorm_);
    NumericalScalar denominator = PhiBNorm_ - PhiANorm_;
    // If left tail truncature, use tail CDF to compute the normalization factor
    if (aNorm_ > 0.0) denominator = DistFunc::pNormal(aNorm_, true) - DistFunc::pNormal(bNorm_, true);
    if (!(denominator > 0.0)) throw InvalidArgumentException(HERE) << "Error: the truncation interval has a too small measure. Here, measure=" << denominator;
    normalizationFactor_ = 1.0 / denominator;
    phiANorm_ = SpecFunc::ISQRT2PI * std::exp(-0.5 * aNorm_ * aNorm_);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar TruncatedNormal::getA() const
{
  return a_;
}


/* B accessor */
void TruncatedNormal::setB(const NumericalScalar b)
{
  if (b != b_)
  {
    b_ = b;
    const NumericalScalar iSigma = 1.0 / sigma_;
    bNorm_ = (b_ - mu_) * iSigma;
    PhiBNorm_ = DistFunc::pNormal(bNorm_);
    NumericalScalar denominator = PhiBNorm_ - PhiANorm_;
    if (!(denominator > 0.0)) throw InvalidArgumentException(HERE) << "Error: the truncation interval has a too small measure. Here, measure=" << denominator;
    normalizationFactor_ = 1.0 / denominator;
    phiBNorm_ = SpecFunc::ISQRT2PI * std::exp(-0.5 * bNorm_ * bNorm_);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar TruncatedNormal::getB() const
{
  return b_;
}

/* Method save() stores the object through the StorageManager */
void TruncatedNormal::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
  adv.saveAttribute( "aNorm_", aNorm_ );
  adv.saveAttribute( "bNorm_", bNorm_ );
  adv.saveAttribute( "phiANorm_", phiANorm_ );
  adv.saveAttribute( "phiBNorm_", phiBNorm_ );
  adv.saveAttribute( "PhiANorm_", PhiANorm_ );
  adv.saveAttribute( "PhiBNorm_", PhiBNorm_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void TruncatedNormal::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  adv.loadAttribute( "aNorm_", aNorm_ );
  adv.loadAttribute( "bNorm_", bNorm_ );
  adv.loadAttribute( "phiANorm_", phiANorm_ );
  adv.loadAttribute( "phiBNorm_", phiBNorm_ );
  adv.loadAttribute( "PhiANorm_", PhiANorm_ );
  adv.loadAttribute( "PhiBNorm_", PhiBNorm_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS

