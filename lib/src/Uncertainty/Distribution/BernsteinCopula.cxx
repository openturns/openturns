//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BernsteinCopulas
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

#include "openturns/BernsteinCopula.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BernsteinCopula)

static const Factory<BernsteinCopula> Factory_BernsteinCopula;

/* Default constructor */
BernsteinCopula::BernsteinCopula()
  : CopulaImplementation()
  , copulaSample_(0, 1)
  , binNumber_(0)
  , logBetaFactors_(0)
  , logFactors_(0)
{
  setName("BernsteinCopula");
  setCopulaSample(Sample(1, 1));
  computeRange();
}

/* Default constructor */
BernsteinCopula::BernsteinCopula(const Sample & sample,
				 const UnsignedInteger binNumber,
				 const Bool isEmpiricalCopulaSample)
  : CopulaImplementation()
  , copulaSample_(0, 1)
  , binNumber_(binNumber)
  , logBetaFactors_(0)
  , logFactors_(0)
{
  setName("BernsteinCopula");
  setCopulaSample(copulaSample, isEmpiricalCopulaSample);
  computeRange();
}

/* Default constructor */
BernsteinCopula::BernsteinCopula(const Sample & copulaSample,
				 const UnsignedInteger binNumber,
				 const Point & logBetaFactors,
				 const Point & logFactors)
  : CopulaImplementation()
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaFactors_(logBetaFactors)
  , logFactors_(logFactors)
{
  setName("BernsteinCopula");
  computeRange();
}

/* Virtual constructor */
BernsteinCopula * BernsteinCopula::clone() const
{
  return new BernsteinCopula(*this);
}

/* Comparison operator */
Bool BernsteinCopula::operator ==(const BernsteinCopula & other) const
{
  if (this == &other) return true;
  return (copulaSample_ == other.copulaSample_) && (binNumber_ == other.binNumber_);
}

Bool BernsteinCopula::equals(const DistributionImplementation & other) const
{
  const BernsteinCopula* p_other = dynamic_cast<const BernsteinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String BernsteinCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BernsteinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " copulaSample=" << copulaSample_
      << " binNumber=" << binNumber_;
  return oss;
}

String BernsteinCopula::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "("
      << " copulaSample=" << copulaSample_
      << " binNumber=" << binNumber_
      << ")";
  return oss;
}

/* Copula sample accessor */
void BernsteinCopula::setCopulaSample(const Sample & copulaSample,
				      const Bool isEmpiricalCopulaSample)
{
  // Check the sample
  const UnsignedInteger size = copulaSample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of size>0.";
  const UnsignedInteger dimension = copulaSample.getDimension();  
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension>0.";
  const UnsignedInteger remainder = size % binNumber_;
  // If the given sample is an empirical copula sample of a compatible size
  if (isEmpiricalCopulaSample && remainder == 0)
    copulaSample_ = copulaSample;
  else
    {
      Sample localSample(copulaSample);
      if (remainder != 0)
	{
	  LOGINFO(OSS() << "Must drop the last " << remainder << " to build a BernsteinCopula as the given sample has a size=" << size << " which is not a multiple of the bin number=" << binNumber_);
	  (void) localSample.split(size - remainder);
	}
      copulaSample_ = localSample.toEmpiricalCopula();
    } // remainder > 0 or !isEmpiricalCopulaSample
  setDimension(dimension);
  // Now the sample is correct, compute the by-products
  update();
  computeRange();
}


/* Copula sample accessor */
Sample BernsteinCopula::getCopulaSample() const
{
  return copulaSample_;
}

/* Bin number accessor */
void BernsteinCopula::setBinNumber(const UnsignedInteger binNumber)
{
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a binNumber>0.";
  binNumber_ = binNumber;
  update();
}

UnsignedInteger BernsteinCopula::getBinNumber() const
{
  return binNumber_;
}

/* Get one realization of the BernsteinCopula */
Point BernsteinCopula::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point realization(dimension);
  const UnsignedInteger size = copulaSample_.getSize();
  // Select the atom
  const UnsignedInteger atomIndex(RandomGenerator::IntegerGenerate(size));
  UnsignedInteger linearIndex = atomIndex * dimension;
  for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      realization[j] = DistFunc::rBeta(logFactors_[linearIndex], binNumber_ - logFactors_[linearIndex] + 1.0);
      ++linearIndex;
    }
  return realization;
}

/* Get a sample of the distribution */
Sample BernsteinCopula::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  SampleImplementation sample(size, dimension);
  const UnsignedInteger mixtureSize = copulaSample_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      // Select the atom
      const UnsignedInteger atomIndex(RandomGenerator::IntegerGenerate(mixtureSize));
      UnsignedInteger linearIndex = atomIndex * dimension;
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  sample(i, j) = DistFunc::rBeta(logFactors_[linearIndex], binNumber_ - logFactors_[linearIndex] + 1.0);
	  ++linearIndex;
	} // j
    } // i
  sample.setDescription(getDescription());
  return sample;
}

/* Get the PDF of the BernsteinCopula */
Scalar BernsteinCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  Scalar pdfValue = 0.0;
  Point logX(dimension);
  Point log1pX(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      logX[i] = std::log(point[i]);
      log1pX[i] = log1p(-point[i]);
    }
  const UnsignedInteger size = copulaSample_.getSize();
  UnsignedInteger linearIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar logPDFAtom = logBetaFactors_[i];
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  logPDFAtom += (logFactors_[linearIndex] - 1.0) * logX[j] + (binNumber_ - logFactors_[linearIndex]) * log1pX[j] - logBetaFactors_[linearIndex];
	  ++linearIndex;
	} // j
      pdfValue += std::exp(logPDFAtom);
    } // i
  return pdfValue / size;
}

/* Get the log-PDF of the BernsteinCopula */
Scalar BernsteinCopula::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return -SpecFunc::LogMaxScalar;
  Scalar sumPDFValue = 0.0;
  Point logX(dimension);
  Point log1pX(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      logX[i] = std::log(point[i]);
      log1pX[i] = log1p(-point[i]);
    }
  const UnsignedInteger size = copulaSample_.getSize();
  UnsignedInteger linearIndex = 0;
  Scalar maxLogPDF = 0.0;
  const Scalar logW = -std::log(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar logPDFAtom = logBetaFactors_[i];
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  logPDFAtom += (logFactors_[linearIndex] - 1.0) * logX[j] + (binNumber_ - logFactors_[linearIndex]) * log1pX[j] + logW - logBetaFactors_[linearIndex];
	  ++linearIndex;
	} // j
      if (logPDFAtom > maxLogPDF)
	{
	  sumPDFValue += std::exp(maxLogPDF);
	  maxLogPDF = logPDFAtom;
	}
      else
	sumPDFValue += std::exp(logPDFAtom);
    } // i
  return maxLogPDF + log1p(sumPDFValue * std::exp(-maxLogPDF));
}

/* Get the CDF of the BernsteinCopula */
Scalar BernsteinCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Scalar cdfValue = 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  UnsignedInteger linearIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar cdfAtom = 1.0;
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  cdfAtom *= SpecFunc::RegularizedIncompleteBeta(logFactors_[linearIndex], binNumber_ - logFactors_[linearIndex] + 1.0, point[j]);
	  ++linearIndex;
	} // j
      cdfValue += cdfAtom;
    } // i
  return cdfValue / size;
}

/* Compute the probability content of an interval */
Scalar BernsteinCopula::computeProbability(const Interval & interval) const
{
  if (interval.isNumericallyEmpty()) return 0.0;
  const UnsignedInteger dimension = getDimension();
  Scalar probabilityValue = 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  UnsignedInteger linearIndex = 0;
  const Point lower(interval.getLowerBound());
  const Point upper(interval.getUpperBound());
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar probabilityAtom = 1.0;
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  const Scalar r = logFactors_[linearIndex];
	  const Scalar s = binNumber_ - logFactors_[linearIndex] + 1.0;
	  probabilityAtom *= SpecFunc::RegularizedIncompleteBeta(r, s, upper[j]) - SpecFunc::RegularizedIncompleteBeta(r, s, lower[j]);
	  ++linearIndex;
	} // j
      probabilityValue += probabilityAtom;
    } // i
  return probabilityValue / size;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
BernsteinCopula::Implementation BernsteinCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  return new BernsteinCopula(copulaSample_.getMarginal(indices), binNumber_);
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix BernsteinCopula::getSpearmanCorrelation() const
{
  // return DistributionImplementation::getSpearmanCorrelation();
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = copulaSample_.getSize();
  CorrelationMatrix rho(dimension);
  Scalar normalizationFactor = 12.0 / (size * (binNumber_ + 1.0) * (binNumber_ + 1.0));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      Scalar value = 0.0;
      UnsignedInteger base = 0;
      for (UnsignedInteger k = 0; k < size; ++k)
	{
	  value += logFactors_[base + i] * logFactors_[base + j];
	  base += dimension;
	}
      rho(i, j) = value * normalizationFactor - 3.0;
    } // j
  } // i
  return rho;
}

/* Tell if the distribution has elliptical copula */
Bool BernsteinCopula::hasEllipticalCopula() const
{
  return (getDimension() == 1);
}

/* Tell if the distribution has independent copula */
Bool BernsteinCopula::hasIndependentCopula() const
{
  return (getDimension() == 1);
}

/* Compute the normalization factors */
void BernsteinCopula::update()
{
  const UnsignedInteger size = copulaSample_.getSize();
  const UnsignedInteger dimension = copulaSample_.getDimension();
  logBetaFactors_ = Point(size);
  logFactors_ = Point(size * dimension);
  UnsignedInteger linearIndex = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
	{
	  const Scalar xIJ = copulaSample_(i, j);
	  const Scalar r = ceil(binNumber_ * xIJ);
	  const Scalar s = binNumber_ - r + 1.0;
	  logBetaFactors_[linearIndex] = SpecFunc::LogBeta(r, s);
	  logFactors_[linearIndex] = r;
	  ++linearIndex;
	} // j
    } // i
}

/* Method save() stores the object through the StorageManager */
void BernsteinCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "copulaSample_", copulaSample_ );
  adv.saveAttribute( "binNumber_", binNumber_ );
  adv.saveAttribute( "logBetaFactors_", logBetaFactors_ );
  adv.saveAttribute( "logFactors_", logFactors_ );
}

/* Method load() reloads the object from the StorageManager */
void BernsteinCopula::load(Advocate & adv)
{
  CopulaImplementation::load(adv);
  adv.loadAttribute( "copulaSample_", copulaSample_ );
  adv.loadAttribute( "binNumber_", binNumber_ );
  adv.loadAttribute( "logBetaFactors_", logBetaFactors_ );
  adv.loadAttribute( "logFactors_", logFactors_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
