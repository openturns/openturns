//                                               -*- C++ -*-
/**
 *  @brief The LogUniform distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SpecFunc.hxx"
#include "openturns/LogUniform.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogUniform);

static const Factory<LogUniform> Factory_LogUniform;

/* Default constructor */
LogUniform::LogUniform()
  : ContinuousDistribution()
  , aLog_(-1.0)
  , bLog_(1.0)
  , a_(std::exp(-1.0))
  , b_(std::exp(1.0))
{
  setName( "LogUniform" );
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
LogUniform::LogUniform(const NumericalScalar aLog,
                       const NumericalScalar bLog)
  : ContinuousDistribution()
  , aLog_(aLog)
  , bLog_(bLog)
  , a_(std::exp(aLog_))
  , b_(std::exp(bLog_))
{
  if (bLog <= aLog) throw InvalidArgumentException(HERE) << "Error the lower bound aLog of a LogUniform distribution must be lesser than its upper bound bLog, here aLog=" << aLog << " bLog=" << bLog;
  setName( "LogUniform" );
  setDimension( 1 );
  computeRange();
}

/* Comparison operator */
Bool LogUniform::operator ==(const LogUniform & other) const
{
  if (this == &other) return true;
  return (aLog_ == other.aLog_) && (bLog_ == other.bLog_);
}

Bool LogUniform::equals(const DistributionImplementation & other) const
{
  const LogUniform* p_other = dynamic_cast<const LogUniform*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String LogUniform::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LogUniform::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " aLog=" << aLog_
      << " bLog=" << bLog_
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String LogUniform::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(aLog = " << aLog_ << ", bLog = " << bLog_ << ")";
  return oss;
}

/* Virtual constructor */
LogUniform * LogUniform::clone() const
{
  return new LogUniform(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void LogUniform::computeRange()
{
  setRange(Interval(a_, b_));
}


/* Get one realization of the distribution */
NumericalPoint LogUniform::getRealization() const
{
  return NumericalPoint(1, std::exp(aLog_ + (bLog_ - aLog_) * RandomGenerator::Generate()));
}


/* Get the DDF of the distribution */
NumericalPoint LogUniform::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x < a_) || (x > b_)) return NumericalPoint(1, 0.0);
  return NumericalPoint(1, -1.0 / (x * x * (bLog_ - aLog_)));
}


/* Get the PDF of the distribution */
NumericalScalar LogUniform::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return 0.0;
  return 1.0 / (x * (bLog_ - aLog_));
}


/* Get the CDF of the distribution */
NumericalScalar LogUniform::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= a_) return 0.0;
  if (x >= b_)  return 1.0;
  return (std::log(x) - aLog_) / (bLog_ - aLog_);
}

NumericalScalar LogUniform::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  if (x <= a_) return 1.0;
  if (x > b_)  return 0.0;
  return (bLog_ - std::log(x)) / (bLog_ - aLog_);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex LogUniform::computeCharacteristicFunction(const NumericalScalar x) const
{
  NumericalComplex result;
  if (std::abs(x) <= 1.0e-8 * (b_ - a_)) result = NumericalComplex((bLog_ - aLog_) / (bLog_ + aLog_), (b_ - a_) * x / (bLog_ - aLog_));
  else
  {
    result = (SpecFunc::Ei(NumericalComplex(0.0, x * b_)) - SpecFunc::Ei(NumericalComplex(0.0, x * a_))) / (bLog_ - aLog_);
  }
  return result;
}

/* Get the PDFGradient of the distribution */
NumericalPoint LogUniform::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint pdfGradient(2, 0.0);
  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return pdfGradient;
  const NumericalScalar value(computePDF(point) / (bLog_ - aLog_));
  pdfGradient[0] = value;
  pdfGradient[1] = -value;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint LogUniform::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalPoint cdfGradient(2, 0.0);
  const NumericalScalar x(point[0]);
  if ((x <= a_) || (x > b_)) return cdfGradient;
  const NumericalScalar denominator(std::pow(bLog_ - aLog_, 2));
  const NumericalScalar logX(std::log(x));
  cdfGradient[0] = (logX - bLog_) / denominator;
  cdfGradient[1] = (aLog_ - logX) / denominator;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar LogUniform::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return std::exp(bLog_ - prob * (bLog_ - aLog_));
  return std::exp(aLog_ + prob * (bLog_ - aLog_));
}

/* Compute the mean of the distribution */
void LogUniform::computeMean() const
{
  mean_ = NumericalPoint(1, (b_ - a_) / (bLog_ - aLog_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint LogUniform::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Get the skewness of the distribution */
NumericalPoint LogUniform::getSkewness() const
{
  NumericalScalar t1 = std::sqrt(2.0);
  NumericalScalar t2 = a_ * a_;
  NumericalScalar t4 = b_ * b_;
  NumericalScalar t6 = bLog_ * aLog_;
  NumericalScalar t9 = bLog_ * bLog_;
  NumericalScalar t14 = aLog_ * aLog_;
  NumericalScalar t30 = a_ * b_;
  NumericalScalar t39 = 12.0 * t2 + 12.0 * t4 - 4.0 * t4 * t6 + 2.0 * t4 * t9 - 9.0 * t4 * bLog_ + 2.0 * t4 * t14 + 9.0 * t4 *
                        aLog_ - 9.0 * t2 * aLog_ + 2.0 * t2 * t9 + 2.0 * b_ * a_ * t9 + 9.0 * t2 * bLog_ - 4.0 * t2 * t6 - 4.0 * t30 * t6 + 2.0 * b_ * a_ *
                        t14 - 24.0 * t30 + 2.0 * t2 * t14;
  NumericalScalar t42 = std::sqrt(b_ - a_);
  NumericalScalar t45 = -bLog_ + aLog_;
  NumericalScalar t46 = t45 * t45;
  NumericalScalar t54 = -2.0 * a_ + aLog_ * b_ + 2.0 * b_ + aLog_ * a_ - bLog_ * b_ - bLog_ * a_;
  NumericalScalar t56 = std::sqrt(-t54 / t46);
  NumericalScalar t63 = 1 / t45 / t54 / t56 / t42 * t39 * t1 / 3.0;
  return NumericalPoint(1, t63);
}

/* Get the kurtosis of the distribution */
NumericalPoint LogUniform::getKurtosis() const
{
  NumericalScalar t5 = a_ * a_;
  NumericalScalar t7 = b_ * b_;
  NumericalScalar t9 = bLog_ * aLog_;
  NumericalScalar t12 = bLog_ * bLog_;
  NumericalScalar t17 = aLog_ * aLog_;
  NumericalScalar t33 = a_ * b_;
  NumericalScalar t42 = 12.0 * t5 + 12.0 * t7 - 4.0 * t7 * t9 + 2.0 * t7 * t12 - 9.0 * t7 * bLog_ + 2.0 * t7 * t17 + 9.0 * t7 *
                        aLog_ - 9.0 * t5 * aLog_ + 2.0 * t5 * t12 + 2.0 * b_ * a_ * t12 + 9.0 * t5 * bLog_ - 4.0 * t5 * t9 - 4.0 * t33 * t9 + 2.0 * b_ *
                        a_ * t17 - 24.0 * t33 + 2.0 * t5 * t17;
  NumericalScalar t50 = std::pow(-2.0 * a_ + aLog_ * b_ + 2.0 * b_ + aLog_ * a_ - bLog_ * b_ - bLog_ * a_, 2.0);
  NumericalScalar t54 = 2.0 / 3.0 / t50 * t42 * (-bLog_ + aLog_) / (-b_ + a_);
  return NumericalPoint(1, t54);
}

/* Compute the covariance of the distribution */
void LogUniform::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar aLogbLog(bLog_ - aLog_);
  covariance_(0, 0) = 0.5 * (b_ - a_) * (b_ * (aLogbLog - 2.0) + a_ * (aLogbLog + 2.0)) / std::pow(aLogbLog, 2);
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
NumericalPoint LogUniform::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(1, 1.0);
  return NumericalPoint(1, (std::pow(b_, static_cast<int>(n)) - std::pow(a_, static_cast<int>(n))) / (n * (bLog_ - aLog_)));
}

/* Parameters value accessor */
NumericalPoint LogUniform::getParameter() const
{
  NumericalPoint point(2);
  point[0] = aLog_;
  point[1] = bLog_;
  return point;
}

void LogUniform::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = LogUniform(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description LogUniform::getParameterDescription() const
{
  Description description(2);
  description[0] = "aLog";
  description[1] = "bLog";
  return description;
}

/* A accessor */
void LogUniform::setALog(const NumericalScalar aLog)
{
  if (aLog != aLog_)
  {
    aLog_ = aLog;
    a_ = std::exp(aLog_);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar LogUniform::getALog() const
{
  return aLog_;
}


/* B accessor */
void LogUniform::setBLog(const NumericalScalar bLog)
{
  if (bLog != bLog_)
  {
    bLog_ = bLog;
    b_ = std::exp(bLog);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar LogUniform::getBLog() const
{
  return bLog_;
}

/* Method save() stores the object through the StorageManager */
void LogUniform::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "aLog_", aLog_ );
  adv.saveAttribute( "bLog_", bLog_ );
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
}

/* Method load() reloads the object from the StorageManager */
void LogUniform::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "aLog_", aLog_ );
  adv.loadAttribute( "bLog_", bLog_ );
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
