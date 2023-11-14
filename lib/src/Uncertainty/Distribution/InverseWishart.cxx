//                                               -*- C++ -*-
/**
 *  @brief The InverseWishart distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/InverseWishart.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseWishart)

static const Factory<InverseWishart> Factory_InverseWishart;


/* Default constructor */
InverseWishart::InverseWishart()
  : ContinuousDistribution()
  , cholesky_()
  , nu_(1.0)
{
  setName("InverseWishart");
  setV(CovarianceMatrix(1));
  computeRange();
  update();
}

/* Parameters constructor */
InverseWishart::InverseWishart(const CovarianceMatrix & v,
                               const Scalar nu)
  : ContinuousDistribution()
  , cholesky_()
  , nu_(-1.0) // implies nu_ != nu: see setNu
{
  setName("InverseWishart");
  if (nu + 1 <= v.getDimension()) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << "is not greater than dimension-1=" << static_cast< SignedInteger > (v.getDimension()) - 1;
  setV(v);
  setNu(nu);
}

/* Comparison operator */
Bool InverseWishart::operator ==(const InverseWishart & other) const
{
  if (this == &other) return true;
  return (nu_ == other.nu_) && (cholesky_ == other.cholesky_);
}

Bool InverseWishart::equals(const DistributionImplementation & other) const
{
  const InverseWishart* p_other = dynamic_cast<const InverseWishart*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String InverseWishart::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseWishart::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " cholesky=" << cholesky_
      << " nu=" << nu_
      << " inverseCholeskyInverse=" << inverseCholeskyInverse_;
  return oss;
}

String InverseWishart::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(V = " << "\n" << offset << getV() << ", nu = " << nu_ << ")";
  return oss;
}

/* Virtual constructor */
InverseWishart * InverseWishart::clone() const
{
  return new InverseWishart(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void InverseWishart::computeRange()
{
  const UnsignedInteger p = cholesky_.getDimension();
  const Scalar bound = ChiSquare(1.0).getRange().getUpperBound()[0];
  UnsignedInteger index = 0;
  Point upper(getDimension());
  Point lower(getDimension());
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      upper[index] = std::sqrt(cholesky_(i, i) * cholesky_(j, j)) * bound;
      lower[index] = (i == j ? 0.0 : -upper[index]);
      ++index;
    }
  setRange(Interval(lower, upper, Interval::BoolCollection(index, true), Interval::BoolCollection(index, false)));
}

/* Get one realization of the distribution */
Point InverseWishart::getRealization() const
{
  const CovarianceMatrix X(getRealizationAsMatrix());
  const UnsignedInteger p = X.getDimension();
  Point realization(getDimension());
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      realization[index] = X(i, j);
      ++index;
    }
  return realization;
}

/* Get one realization of the distribution as a covariance matrix
We use the Barlett decomposition and the fact that if X is distributed according to the inverse Wishart distribution with covariance matrix V, then X^{-1} is distributed according to the Wishart distribution with parameter V^{-1}
X^{-1} = LAA'L' with LL'=V^{-1} gives X = L'^{-1}A'^{-1}A^{-1}L^{-1}
 */
CovarianceMatrix InverseWishart::getRealizationAsMatrix() const
{
  const UnsignedInteger p = cholesky_.getDimension();
  TriangularMatrix A(p);
  // The diagonal elements are chi-distributed
  for (UnsignedInteger i = 0; i < p; ++i)
  {
    A(i, i) = std::sqrt(2.0 * DistFunc::rGamma(0.5 * (nu_ - i)));
    // The off-diagonal elements are normally distributed
    for (UnsignedInteger j = 0; j < i; ++j) A(i, j) = DistFunc::rNormal();
  }
  return A.solveLinearSystem(inverseCholeskyInverse_).computeGram(true).getImplementation();
}

/* Get the PDF of the distribution */
Scalar InverseWishart::computePDF(const CovarianceMatrix & m) const
{
  if (m.getDimension() != cholesky_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given matrix must have dimension=" << cholesky_.getDimension() << ", here dimension=" << m.getDimension();
  const Scalar logPDF = computeLogPDF(m);
  const Scalar pdf = std::exp(logPDF);
  return pdf;
}

Scalar InverseWishart::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar logPDF = computeLogPDF(point);
  const Scalar pdf = std::exp(logPDF);
  return pdf;
}

Scalar InverseWishart::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const UnsignedInteger p = cholesky_.getDimension();
  // Build the covariance matrix associated to the given point
  CovarianceMatrix m(p);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      m(i, j) = point[index];
      ++index;
    }
  return computeLogPDF(m);
}

Scalar InverseWishart::computeLogPDF(const CovarianceMatrix & m) const
{
  if (m.getDimension() != cholesky_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given matrix must have dimension=" << cholesky_.getDimension() << ", here dimension=" << m.getDimension();
  const UnsignedInteger p = cholesky_.getDimension();
  try
  {
    // If the Cholesky factor is not defined, it means that M is not symmetric positive definite (an exception is thrown) and the PDF is zero
    TriangularMatrix X(CovarianceMatrix(m).computeCholesky());
    // Compute the determinant of the Cholesky factor, ie the square-root of the determinant of M
    Scalar logPDF = 0.0;
    // Here, the diagonal of X is positive
    for (UnsignedInteger i = 0; i < p; ++i) logPDF -= std::log(X(i, i));
    logPDF *= nu_ + p + 1.0;
    // Add the term which does not depend on M
    logPDF += logNormalizationFactor_;
    // Trace(V M^{-1}) = Trace(C C' X'^{-1} X^{-1}) = Trace(C'X'^{-1} X^{-1}C)
    //                 = Trace(A'A) with A = X^{-1}C
    const TriangularMatrix A(X.solveLinearSystemInPlace(cholesky_).getImplementation());
    logPDF -= 0.5 * A.computeGram(true).computeTrace();
    return logPDF;
  }
  catch (...)
  {
    return SpecFunc::LowestScalar;
  }
}

/* Get the CDF of the distribution */
Scalar InverseWishart::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  return ContinuousDistribution::computeCDF(point);
}

/* Compute the mean of the distribution */
void InverseWishart::computeMean() const
{
  const CovarianceMatrix V(getV());
  const UnsignedInteger p = cholesky_.getDimension();
  mean_ = Point(getDimension());
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      mean_[index] = V(i, j) / (nu_ - p - 1.0);
      ++index;
    }
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void InverseWishart::computeCovariance() const
{
  const UnsignedInteger p = cholesky_.getDimension();
  const Scalar den = (nu_ - p) * std::pow(nu_ - p - 1.0, 2) * (nu_ - p - 3.0);
  if (!(den > 0.0)) throw NotDefinedException(HERE) << "Error: the covariance of the inverse Wishart distribution is defined only if nu > p+3";
  const CovarianceMatrix V(getV());
  covariance_ = CovarianceMatrix(getDimension());
  UnsignedInteger indexRow = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      UnsignedInteger indexColumn = 0;
      for (UnsignedInteger m = 0; m < i; ++m)
        for (UnsignedInteger n = 0; n <= j; ++n)
        {
          covariance_(indexRow, indexColumn) = (2.0 * V(i, j) * V(m, n) + (nu_ - p - 1.0) * (V(i, m) * V(j, n) + V(i, n) * V(m, j))) / den;
          ++indexColumn;
        }
      ++indexRow;
    }
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard deviation of the distribution */
Point InverseWishart::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  Point sigma(getDimension());
  // If the covariance has already been computed, use it
  if (isAlreadyComputedCovariance_)
  {
    for (UnsignedInteger i = 0; i < getDimension(); ++i) sigma[i] = std::sqrt(covariance_(i, i));
    return sigma;
  }
  // else compute only the standard deviation as the covariance may be huge
  const UnsignedInteger p = cholesky_.getDimension();
  const Scalar den = (nu_ - p) * std::pow(nu_ - p - 1.0, 2) * (nu_ - p - 3.0);
  if (!(den > 0.0)) throw NotDefinedException(HERE) << "Error: the standard deviation of the inverse Wishart distribution is defined only if nu > p+3";
  const CovarianceMatrix V(getV());
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      sigma[index] = std::sqrt((2.0 * V(i, j) * V(i, j) + (nu_ - p - 1.0) * (V(i, i) * V(j, j) + V(i, j) * V(i, j))) / den);
      ++index;
    }
  return sigma;
}


Point InverseWishart::getParameter() const
{
  const CovarianceMatrix V(getCovariance());
  const UnsignedInteger p = V.getDimension();
  Point point((p * (p + 1)) / 2 + 1);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++ i)
    for (UnsignedInteger j = 0; j <= i; ++ j)
    {
      point[index] = V(i, j);
      ++ index;
    }
  point[index] = nu_;
  return point;
}

void InverseWishart::setParameter(const Point & parameter)
{
  const UnsignedInteger size = parameter.getSize();
  const Scalar pReal = 0.5 * std::sqrt(8.0 * size - 7.0) - 0.5;
  const UnsignedInteger p = static_cast< UnsignedInteger >(pReal);
  if (pReal != p) throw InvalidArgumentException(HERE) << "Error: the given parameter cannot be converted into a covariance matrix and a number of degrees of freedom.";
  CovarianceMatrix V(p);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++ i)
    for (UnsignedInteger j = 0; j <= i; ++ j)
    {
      V(i, j) = parameter[index];
      ++ index;
    }
  const Scalar nu = parameter[size - 1];
  const Scalar w = getWeight();
  *this = InverseWishart(V, nu);
  setWeight(w);
}

Description InverseWishart::getParameterDescription() const
{
  const UnsignedInteger p = getDimension();
  Description description((p * (p + 1)) / 2 + 1);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      description[index] = (OSS() << "V_" << i << "_" << j);
    }
  }
  description[index] = "nu";
  return description;
}


/* V accessor */
void InverseWishart::setV(const CovarianceMatrix & v)
{
  const UnsignedInteger p = v.getDimension();
  try
  {
    // Copy of v because v is const and not computeCholesky()
    cholesky_ = CovarianceMatrix(v).computeCholesky();
  }
  catch(...)
  {
    throw InvalidArgumentException(HERE) << "Error: V must be positive definite";
  }
  const TriangularMatrix T(cholesky_.solveLinearSystem(IdentityMatrix(p)).getImplementation());
  // vInverse = T'.T, non const because we compute its Cholesky factor
  CovarianceMatrix vInverse(T.computeGram(true));
  // Flag false means that vInverse is not preserved, non const because we solve a linear system with this matrix
  TriangularMatrix vInverseCholesky(vInverse.computeCholesky(false));
  inverseCholeskyInverse_ = vInverseCholesky.solveLinearSystemInPlace(IdentityMatrix(p)).getImplementation();
  setDimension((p * (p + 1)) / 2);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

CovarianceMatrix InverseWishart::getV() const
{
  return cholesky_.computeGram(false).getImplementation();
}


/* Nu accessor */
void InverseWishart::setNu(const Scalar nu)
{
  if (nu + 1.0 <= cholesky_.getDimension()) throw InvalidArgumentException(HERE) << "Error: nu must be greater than V dimension - 1";
  if (nu != nu_)
  {
    nu_ = nu;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    update();
    computeRange();
  }
}

Scalar InverseWishart::getNu() const
{
  return nu_;
}

/* Compute the normalization factor on log-scale */
void InverseWishart::update()
{
  const UnsignedInteger p = cholesky_.getDimension();
  logNormalizationFactor_ = -0.5 * p * (nu_ * M_LN2 + 0.5 * (p - 1) * std::log(M_PI));
  for (UnsignedInteger i = 0; i < p; ++i) logNormalizationFactor_ -= SpecFunc::LogGamma(0.5 * (nu_ - i)) - nu_ * std::log(cholesky_(i, i));
}

/* Method save() stores the object through the StorageManager */
void InverseWishart::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "cholesky_", cholesky_ );
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "inverseCholeskyInverse_", inverseCholeskyInverse_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseWishart::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "cholesky_", cholesky_ );
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "inverseCholeskyInverse_", inverseCholeskyInverse_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
