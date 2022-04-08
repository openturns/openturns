//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all EmpiricalBernsteinCopulas
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/EmpiricalBernsteinCopula.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EmpiricalBernsteinCopula)

static const Factory<EmpiricalBernsteinCopula> Factory_EmpiricalBernsteinCopula;

/* Default constructor */
EmpiricalBernsteinCopula::EmpiricalBernsteinCopula()
  : ContinuousDistribution()
  , copulaSample_(0, 1)
  , binNumber_(1)
  , logBetaFactors_(0)
  , logBetaMarginalFactors_(0, 0)
  , logFactors_(0, 0)
{
  setName("EmpiricalBernsteinCopula");
  setCopulaSample(Sample(2, 1));
  computeRange();
}

/* Default constructor */
EmpiricalBernsteinCopula::EmpiricalBernsteinCopula(const Sample & copulaSample,
    const UnsignedInteger binNumber,
    const Bool isEmpiricalCopulaSample)
  : ContinuousDistribution()
  , copulaSample_(0, 1)
  , binNumber_(binNumber)
  , logBetaFactors_(0)
  , logBetaMarginalFactors_(0, 0)
  , logFactors_(0, 0)
{
  setName("EmpiricalBernsteinCopula");
  setCopulaSample(copulaSample, isEmpiricalCopulaSample);
  computeRange();
}

/* Default constructor */
EmpiricalBernsteinCopula::EmpiricalBernsteinCopula(const Sample & copulaSample,
    const UnsignedInteger binNumber,
    const SampleImplementation & logBetaMarginalFactors,
    const SampleImplementation & logFactors)
  : ContinuousDistribution()
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaMarginalFactors_(logBetaMarginalFactors)
  , logFactors_(logFactors)
{
  const UnsignedInteger size = logFactors_.getSize();
  const UnsignedInteger dimension = logFactors_.getDimension();
  logBetaFactors_ = Point(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar sumLogBeta = 0.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
      sumLogBeta += logBetaMarginalFactors_(i, j);
    logBetaFactors_[i] = sumLogBeta;
  } // i
  setName("EmpiricalBernsteinCopula");
  setDimension(copulaSample.getDimension());
  computeRange();
  // If the given sample is an empirical copula sample of a compatible size
  const UnsignedInteger remainder = size % binNumber;
  isCopula_ = (remainder == 0);
}

/* Virtual constructor */
EmpiricalBernsteinCopula * EmpiricalBernsteinCopula::clone() const
{
  return new EmpiricalBernsteinCopula(*this);
}

/* Comparison operator */
Bool EmpiricalBernsteinCopula::operator ==(const EmpiricalBernsteinCopula & other) const
{
  if (this == &other) return true;
  return (copulaSample_ == other.copulaSample_) && (binNumber_ == other.binNumber_);
}

Bool EmpiricalBernsteinCopula::equals(const DistributionImplementation & other) const
{
  const EmpiricalBernsteinCopula* p_other = dynamic_cast<const EmpiricalBernsteinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String EmpiricalBernsteinCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << EmpiricalBernsteinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " copulaSample=" << copulaSample_
      << " binNumber=" << binNumber_;
  return oss;
}

String EmpiricalBernsteinCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "("
      << " copulaSample=" << copulaSample_
      << " binNumber=" << binNumber_
      << ")";
  return oss;
}

/* Copula sample accessor */
void EmpiricalBernsteinCopula::setCopulaSample(const Sample & copulaSample,
    const Bool isEmpiricalCopulaSample)
{
  // Check the sample
  const UnsignedInteger size = copulaSample.getSize();
  if (size <= 1) throw InvalidArgumentException(HERE) << "Error: expected a sample of size>1.";
  const UnsignedInteger dimension = copulaSample.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension>0.";
  const UnsignedInteger remainder = size % binNumber_;
  // If the given sample is an empirical copula sample of a compatible size
  if (isEmpiricalCopulaSample)
  {
    copulaSample_ = copulaSample;
    isCopula_ = (remainder == 0);
  }
  else
  {
    // Here we remove last point such as we build a copula
    // Thus isCopula_ is necessary true
    isCopula_ = true;
    if (remainder == 0)
      copulaSample_ = copulaSample.rank();
    else
    {
      LOGINFO(OSS() << "Must drop the last " << remainder << " to build a EmpiricalBernsteinCopula as the given sample has a size=" << size << " which is not a multiple of the bin number=" << binNumber_);
      copulaSample_ = Sample(copulaSample, 0, size - remainder).rank();
    }
    // Normalize
    copulaSample_ += 1.0;
    copulaSample_ /= 1.0 * (size - remainder);
  } // !(isEmpiricalCopulaSample && remainder == 0)
  setDimension(dimension);
  // Now the sample is correct, compute the by-products
  update();
  computeRange();
}


/* Copula sample accessor */
Sample EmpiricalBernsteinCopula::getCopulaSample() const
{
  return copulaSample_;
}

/* Bin number accessor */
void EmpiricalBernsteinCopula::setBinNumber(const UnsignedInteger binNumber)
{
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: expected a binNumber>0.";
  binNumber_ = binNumber;
  update();
}

UnsignedInteger EmpiricalBernsteinCopula::getBinNumber() const
{
  return binNumber_;
}

/* Get one realization of the EmpiricalBernsteinCopula */
Point EmpiricalBernsteinCopula::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point realization(dimension);
  const UnsignedInteger size = copulaSample_.getSize();
  // Select the atom
  const UnsignedInteger atomIndex(RandomGenerator::IntegerGenerate(size));
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    realization[j] = DistFunc::rBeta(logFactors_(atomIndex, j), binNumber_ - logFactors_(atomIndex, j) + 1.0);
  }
  return realization;
}

/* Get a sample of the distribution */
Sample EmpiricalBernsteinCopula::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  SampleImplementation sample(size, dimension);
  const UnsignedInteger mixtureSize = copulaSample_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Select the atom
    const UnsignedInteger atomIndex(RandomGenerator::IntegerGenerate(mixtureSize));
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      sample(i, j) = DistFunc::rBeta(logFactors_(atomIndex, j), binNumber_ - logFactors_(atomIndex, j) + 1.0);
    } // j
  } // i
  sample.setDescription(getDescription());
  return sample;
}

/* Get the PDF of the EmpiricalBernsteinCopula
   The empirical Bernstein copula is a mixture of products of Beta distributions:
   c_m(u_1,...,u_d)=1/n\sum_{i=1}^n\prod_{j=1}^d \beta_{r_j^i,s_j^i}(u_j)
   where r_j^i=\lceil mU_j^i\rceil, s_j^i=m+1-r_j^i
   and \beta_{r,s}(x)=x^{r-1}(1-x)^{s-1}/B(r,s)
   Here \log(r_j^i) is stored into logFactors_(i,j)=LF(i,j),
   B(r_j^i,s_j^i) is stored into logBetaMarginalFactors_(i,j)=LBMF(i,j) and
   \sum_{j=1}^d\log(Beta(r_j^i,s_j^i))=\sum_{j=1}^d LBMF(i,j) is stored into logBetaFactors_(i)=LBF(i)
   so c_m(u_1,...,u_d)=1/n\sum_{i=1}^n\exp(LBF(i)+\sum_{j=1}^d [(LF(i,j)-1)\log(u_j)+(m-LF(i,j))\log(1-u_j)])
*/
Scalar EmpiricalBernsteinCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (binNumber_ == 1) return 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  Scalar pdfValue = 0.0;
  Point logX(dimension);
  Point log1mX(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    logX[i] = std::log(point[i]);
    log1mX[i] = log1p(-point[i]);
  }
  const UnsignedInteger size = copulaSample_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar logPDFAtom = -logBetaFactors_[i];
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      logPDFAtom += (logFactors_(i, j) - 1.0) * logX[j] + (binNumber_ - logFactors_(i, j)) * log1mX[j];
    } // j
    pdfValue += std::exp(logPDFAtom);
  } // i
  return pdfValue / size;
}

/* Get the log-PDF of the EmpiricalBernsteinCopula */
Scalar EmpiricalBernsteinCopula::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (binNumber_ == 1) return 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return SpecFunc::LowestScalar;
  Scalar sumPDFValue = 0.0;
  Point logX(dimension);
  Scalar log1mX = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar log1mXi = log1p(-point[i]);
    logX[i] = std::log(point[i]) - log1mXi;
    log1mX += log1mXi;
  }
  const UnsignedInteger size = copulaSample_.getSize();
  Point matvec(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    matvec[i] = -logBetaFactors_[i];
    for (UnsignedInteger j = 0; j < dimension; ++j)
      matvec[i] += (logFactors_(i, j) - 1.0) * logX[j];
  }
  // To avoid overflows and improve accuracy, we replace
  //    log(sum(exp(matvec[i])))
  // by maxValue + log(sum(exp(matvec[i] - maxValue)))
  const Scalar maxValue = *std::max_element(matvec.begin(), matvec.end());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    sumPDFValue += std::exp(matvec[i] - maxValue);
  } // i
  return maxValue + std::log(sumPDFValue) + log1mX * (binNumber_ - 1) - std::log(1.0 * size);
}

/* Get the CDF of the EmpiricalBernsteinCopula */
Scalar EmpiricalBernsteinCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Scalar cdfValue = 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar cdfAtom = 1.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      cdfAtom *= SpecFunc::RegularizedIncompleteBeta(logFactors_(i, j), binNumber_ - logFactors_(i, j) + 1.0, point[j]);
    } // j
    cdfValue += cdfAtom;
  } // i
  return cdfValue / size;
}

/* Compute the probability content of an interval */
Scalar EmpiricalBernsteinCopula::computeProbability(const Interval & interval) const
{
  if (interval.isEmpty()) return 0.0;
  const UnsignedInteger dimension = getDimension();
  Scalar probabilityValue = 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  const Point lower(interval.getLowerBound());
  const Point upper(interval.getUpperBound());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar probabilityAtom = 1.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar r = logFactors_(i, j);
      const Scalar s = binNumber_ - logFactors_(i, j) + 1.0;
      probabilityAtom *= SpecFunc::RegularizedIncompleteBeta(r, s, upper[j]) - SpecFunc::RegularizedIncompleteBeta(r, s, lower[j]);
    } // j
    probabilityValue += probabilityAtom;
  } // i
  return probabilityValue / size;
}


/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
   The empirical Bernstein copula is a mixture of products of Beta distributions:
   c_m(u_1,...,u_d)=1/n\sum_{i=1}^n\prod_{j=1}^d \beta_{r_j^i,s_j^i}(u_j)
   where r_j^i=\lceil mU_j^i\rceil, s_j^i=m+1-r_j^i
   and \beta_{r,s}(x)=x^{r-1}(1-x)^{s-1}/B(r,s)
   Here \log(r_j^i) is stored into logFactors_(i,j)=LF(i,j),
   B(r_j^i,s_j^i) is stored into logBetaMarginalFactors_(i,j)=LBMF(i,j) and
   \sum_{j=1}^d\log(Beta(r_j^i,s_j^i))=\sum_{j=1}^d LBMF(i,j) is stored into logBetaFactors_(i)=LBF(i)
   so c_m(u_1,...,u_d)=1/n\sum_{i=1}^n\exp(LBF(i)+\sum_{j=1}^d [(LF(i,j)-1)\log(u_j)+(m-LF(i,j))\log(1-u_j)])

   c_m(u_1)=1/n\sum_{i=1}^n\exp(LBMF(i,0)+[(LF(i,j)-1)\log(u_j)+(m-LF(i,j))\log(1-u_j)])
   c_m(u_k|u_1,...,u_{k-1})=c_m(u_1,...,u_k)/c_m(u_1,...,u_{k-1})
                           =\sum_{i=1}^n\exp(LBF(i)+\sum_{j=1}^k [(LF(i,j)-1)\log(u_j)+(m-LF(i,j))\log(1-u_j)])/\sum_{i=1}^n\exp(LBF(i)+\sum_{j=1}^{k-1}[(LF(i,j)-1)\log(u_j)+(m-LF(i,j))\log(1-u_j)])

*/
Scalar EmpiricalBernsteinCopula::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x <= 0.0 || x >= 1.0) return 0.0;
  for (UnsignedInteger i = 0; i < y.getDimension(); ++i)
    if (y[i] <= 0.0 || y[i] >= 1.0) return 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    if (isCopula()) return 1.0;
    const UnsignedInteger j = conditioningDimension;
    const Scalar logX = std::log(x);
    const Scalar log1mX = std::log1p(-x);
    Scalar conditionalPDF = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
      conditionalPDF += std::exp((logFactors_(i, j) - 1.0) * logX + (binNumber_ - logFactors_(i, j)) * log1mX - logBetaMarginalFactors_(i, j));
    return conditionalPDF / size;
  } // (conditioningDimension == 0) || (hasIndependentCopula())
  // Case with conditioning. The PDFs are computed up to an 1/n factor, which simplifies during the division.
  Point allConditioningAtomPDF(size);
  Scalar conditioningPDF = 0.0;
  // First the conditioning part
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar conditioningAtomLogPDF = 0.0;
    for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
      conditioningAtomLogPDF += (logFactors_(i, j) - 1.0) * std::log(y[j]) + (binNumber_ - logFactors_(i, j)) * std::log1p(-y[j]) - logBetaMarginalFactors_(i, j);
    const Scalar conditioningAtomPDF = std::exp(conditioningAtomLogPDF);
    allConditioningAtomPDF[i] = conditioningAtomPDF;
    conditioningPDF += conditioningAtomPDF;
  }
  // Should not occur except if underflow occurred
  if (conditioningPDF <= 0.0) return 0.0;
  // Second, the conditioned part
  Scalar conditionedPDF = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    conditionedPDF += std::exp((logFactors_(i, conditioningDimension) - 1.0) * std::log(x) + (binNumber_ - logFactors_(i, conditioningDimension)) * std::log1p(-x) - logBetaMarginalFactors_(i, conditioningDimension)) * allConditioningAtomPDF[i];
  return conditionedPDF / conditioningPDF;
}

Point EmpiricalBernsteinCopula::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  Point result(dimension_);
  const UnsignedInteger size = copulaSample_.getSize();
  // Special case for no conditioning or independent copula
  if (hasIndependentCopula())
  {
    if (isCopula()) return Point(dimension_, 1.0);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      if ((x[j] > 0.0) && (x[j] < 1.0))
      {
        const Scalar logX = std::log(x[j]);
        const Scalar log1mX = std::log1p(-x[j]);
        Scalar conditionalPDF = 0.0;
        for (UnsignedInteger i = 0; i < size; ++i)
          conditionalPDF += std::exp((logFactors_(i, j) - 1.0) * logX + (binNumber_ - logFactors_(i, j)) * log1mX - logBetaMarginalFactors_(i, j));
        result[j] = conditionalPDF / size;
      } // 0 < x[j] < 1
    } // j
    return result;
  } // hasIndependentCopula()
  // Case with conditioning.
  Point allConditionedAtomPDF(size, 1.0);
  Scalar conditioningPDF = 1.0;
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    Scalar conditionedPDF = 0.0;
    if ((x[j] > 0.0) && (x[j] < 1.0) && conditioningPDF > 0.0)
    {
      const Scalar logX = std::log(x[j]);
      const Scalar log1mX = std::log1p(-x[j]);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        allConditionedAtomPDF[i] *= std::exp((logFactors_(i, j) - 1.0) * logX + (binNumber_ - logFactors_(i, j)) * log1mX - logBetaMarginalFactors_(i, j));
        conditionedPDF += allConditionedAtomPDF[i];
      }
    } // 0<x<1
    else return result;
    conditionedPDF /= size;
    result[j] = conditionedPDF / conditioningPDF;
    conditioningPDF = conditionedPDF;
  } // j
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar EmpiricalBernsteinCopula::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x <= 0.0) return 0.0;
  if (x >= 1.0) return 1.0;
  for (UnsignedInteger i = 0; i < y.getDimension(); ++i)
    if (y[i] <= 0.0 || y[i] >= 1.0) return 0.0;
  const UnsignedInteger size = copulaSample_.getSize();
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    if (isCopula()) return x;
    const UnsignedInteger j = conditioningDimension;
    Scalar conditionalCDF = 1.0;
    for (UnsignedInteger i = 0; i < size; ++i)
      conditionalCDF += SpecFunc::RegularizedIncompleteBeta(logFactors_(i, j), binNumber_ - logFactors_(i, j) + 1.0, x);
    return conditionalCDF / size;
  } // (conditioningDimension == 0) || (hasIndependentCopula())
  // Case with conditioning. The PDFs are computed up to an 1/n factor, which simplifies during the division.
  Point allConditioningAtomPDF(size);
  Scalar conditioningPDF = 0.0;
  // First the conditioning part
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar conditioningAtomLogPDF = 0.0;
    for (UnsignedInteger j = 0; j < conditioningDimension; ++j)
      conditioningAtomLogPDF += (logFactors_(i, j) - 1.0) * std::log(y[j]) + (binNumber_ - logFactors_(i, j)) * std::log1p(-y[j]) - logBetaMarginalFactors_(i, j);
    const Scalar conditioningAtomPDF = std::exp(conditioningAtomLogPDF);
    allConditioningAtomPDF[i] = conditioningAtomPDF;
    conditioningPDF += conditioningAtomPDF;
  }
  // Should not occur except if underflow occurred
  if (conditioningPDF <= 0.0) return 0.0;
  // Second, the conditioned part
  Scalar conditionedCDF = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    conditionedCDF += SpecFunc::RegularizedIncompleteBeta(logFactors_(i, conditioningDimension), binNumber_ - logFactors_(i, conditioningDimension) + 1.0, x) * allConditioningAtomPDF[i];
  return conditionedCDF / conditioningPDF;
}

Point EmpiricalBernsteinCopula::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  Point result(dimension_);
  const UnsignedInteger size = copulaSample_.getSize();
  // Special case for no conditioning or independent copula
  if (hasIndependentCopula())
  {
    if (isCopula()) return Point(dimension_, 1.0);
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      if (x[j] <= 0.0) result[j] = 0.0;
      else if (x[j] >= 1.0) result[j] = 1.0;
      else
      {
        Scalar conditionalPDF = 0.0;
        for (UnsignedInteger i = 0; i < size; ++i)
          conditionalPDF += SpecFunc::RegularizedIncompleteBeta(logFactors_(i, j), binNumber_ - logFactors_(i, j) + 1.0, x[j]);
        result[j] = conditionalPDF;
      } // 0 < x[j] < 1
    } // j
    return result;
  } // hasIndependentCopula()
  // Case with conditioning. The PDFs are computed up to a 1/size factor, which simplifies
  Point allConditionedAtomPDF(size, 1.0);
  Scalar conditioningPDF = size;
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    Scalar conditionedPDF = 0.0;
    Scalar conditionedCDF = 0.0;
    if ((x[j] > 0.0) && (x[j] < 1.0) && conditioningPDF > 0.0)
    {
      const Scalar logX = std::log(x[j]);
      const Scalar log1mX = std::log1p(-x[j]);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const Scalar currentPDF = std::exp((logFactors_(i, j) - 1.0) * logX + (binNumber_ - logFactors_(i, j)) * log1mX - logBetaMarginalFactors_(i, j));
        conditionedCDF += allConditionedAtomPDF[i] * SpecFunc::RegularizedIncompleteBeta(logFactors_(i, j), binNumber_ - logFactors_(i, j) + 1.0, x[j]);
        allConditionedAtomPDF[i] *= currentPDF;
        conditionedPDF += allConditionedAtomPDF[i];
      }
    } // 0<x<1
    else return result;
    result[j] = conditionedCDF / conditioningPDF;
    conditioningPDF = conditionedPDF;
  } // j
  return result;
}


/* Get the distribution of the marginal distribution corresponding to the index */
Distribution EmpiricalBernsteinCopula::getMarginal(const UnsignedInteger i) const
{
  if (i >= dimension_) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (isCopula()) return Uniform(0.0, 1.0);
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution EmpiricalBernsteinCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  const UnsignedInteger size = copulaSample_.getSize();
  const UnsignedInteger marginalDimension = indices.getSize();
  SampleImplementation marginalLogBetaFactors(size, marginalDimension);
  SampleImplementation marginalLogFactors(size, marginalDimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < marginalDimension; ++j)
    {
      const UnsignedInteger marginalIndex = indices[j];
      marginalLogBetaFactors(i, j) = logBetaMarginalFactors_(i, marginalIndex);
      marginalLogFactors(i, j) = logFactors_(i, marginalIndex);
    } // j
  } // i
  return new EmpiricalBernsteinCopula(copulaSample_.getMarginal(indices), binNumber_, marginalLogBetaFactors, marginalLogFactors);
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix EmpiricalBernsteinCopula::getSpearmanCorrelation() const
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
      for (UnsignedInteger k = 0; k < size; ++k)
      {
        value += logFactors_(k, i) * logFactors_(k, j);
      }
      rho(i, j) = value * normalizationFactor - 3.0;
    } // j
  } // i
  return rho;
}

/* Tell if the distribution has elliptical copula */
Bool EmpiricalBernsteinCopula::hasEllipticalCopula() const
{
  return (getDimension() == 1);
}

/* Tell if the distribution has independent copula */
Bool EmpiricalBernsteinCopula::hasIndependentCopula() const
{
  return (getDimension() == 1);
}

/* Compute the numerical range of the distribution given the parameters values */
void EmpiricalBernsteinCopula::computeRange()
{
  range_ = Interval(dimension_);
}

/* Compute the normalization factors */
void EmpiricalBernsteinCopula::update()
{
  const UnsignedInteger size = copulaSample_.getSize();
  const UnsignedInteger dimension = copulaSample_.getDimension();
  logBetaMarginalFactors_ = SampleImplementation(size, dimension);
  logBetaFactors_ = Point(size);
  logFactors_ = SampleImplementation(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar sumLogBeta = 0.0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar xIJ = copulaSample_(i, j);
      const Scalar r = ceil(binNumber_ * xIJ);
      const Scalar s = binNumber_ - r + 1.0;
      const Scalar logBeta = SpecFunc::LogBeta(r, s);
      sumLogBeta += logBeta;
      logBetaMarginalFactors_(i, j) = logBeta;
      logFactors_(i, j) = r;
    } // j
    logBetaFactors_[i] = sumLogBeta;
  } // i
}

Point EmpiricalBernsteinCopula::getParameter() const
{
  const UnsignedInteger size = copulaSample_.getSize();
  Point parameter;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    parameter.add(copulaSample_[i]);
  }
  parameter.add(binNumber_);
  return parameter;
}

void EmpiricalBernsteinCopula::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = copulaSample_.getSize();
  if (parameter.getDimension() != (dimension * size + 1)) throw InvalidArgumentException(HERE) << "Expected " << (dimension * size) << " parameters";
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      copulaSample_(i, j) = parameter[index];
      ++ index;
    }
  }
  binNumber_ = static_cast<UnsignedInteger>(parameter[index]);
}

Description EmpiricalBernsteinCopula::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = copulaSample_.getSize();
  Description parameter;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
      parameter.add(String(OSS() << "s_" << i << "_" << j));
  }
  parameter.add("binNumber");
  return parameter;
}


/* Method save() stores the object through the StorageManager */
void EmpiricalBernsteinCopula::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "copulaSample_", copulaSample_ );
  adv.saveAttribute( "binNumber_", binNumber_ );
}

/* Method load() reloads the object from the StorageManager */
void EmpiricalBernsteinCopula::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "copulaSample_", copulaSample_ );
  adv.loadAttribute( "binNumber_", binNumber_ );
  update();
  computeRange();
}

END_NAMESPACE_OPENTURNS
