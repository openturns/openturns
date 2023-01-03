//                                               -*- C++ -*-
/**
 *  @brief The Wishart distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Wishart.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Wishart)

static const Factory<Wishart> Factory_Wishart;


/* Default constructor */
Wishart::Wishart()
  : ContinuousDistribution()
  , cholesky_()
  , nu_(1.0)
{
  setName("Wishart");
  setV(CovarianceMatrix(1));
  computeRange();
  update();
}

/* Parameters constructor */
Wishart::Wishart(const CovarianceMatrix & v,
                 const Scalar nu)
  : ContinuousDistribution()
  , cholesky_()
  , nu_(-1.0)
{
  setName("Wishart");
  if (nu + 1 <= v.getDimension()) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu=" << nu << "is not greater than dimension-1=" << static_cast< SignedInteger > (v.getDimension()) - 1;
  setV(v);
  setNu(nu);
}

/* Comparison operator */
Bool Wishart::operator ==(const Wishart & other) const
{
  if (this == &other) return true;
  return (nu_ == other.nu_) && (cholesky_ == other.cholesky_);
}

Bool Wishart::equals(const DistributionImplementation & other) const
{
  const Wishart* p_other = dynamic_cast<const Wishart*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Wishart::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Wishart::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " cholesky=" << cholesky_
      << " nu=" << nu_;
  return oss;
}

String Wishart::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName() << "(V = " << Os::GetEndOfLine() << offset << getV() << ", nu = " << nu_ << ")";
  return oss;
}

/* Virtual constructor */
Wishart * Wishart::clone() const
{
  return new Wishart(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Wishart::computeRange()
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
Point Wishart::getRealization() const
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
We use the Barlett decomposition X = LAA'L' where LL' = V is the covariance parameter of the distribution and A is a lower triangular matrix
 */
CovarianceMatrix Wishart::getRealizationAsMatrix() const
{
  const UnsignedInteger p = cholesky_.getDimension();
  TriangularMatrix A(p);
  // The diagonal elements are chi-distributed
  for (UnsignedInteger i = 0; i < p; ++i)
  {
    A(i, i) = std::sqrt(2.0 * DistFunc::rGamma(0.5 * (nu_ - i)));
    // The off-diagonal elements are normaly distributed
    for (UnsignedInteger j = 0; j < i; ++j) A(i, j) = DistFunc::rNormal();
  }
  return (cholesky_ * A).computeGram(false).getImplementation();
}


/* Get the PDF of the distribution */
Scalar Wishart::computePDF(const CovarianceMatrix & m) const
{
  if (m.getDimension() != cholesky_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given matrix must have dimension=" << cholesky_.getDimension() << ", here dimension=" << m.getDimension();
  const Scalar logPDF = computeLogPDF(m);
  return std::exp(logPDF);
}

Scalar Wishart::computePDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar Wishart::computeLogPDF(const Point & point) const
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

Scalar Wishart::computeLogPDF(const CovarianceMatrix & m) const
{
  if (m.getDimension() != cholesky_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the given matrix must have dimension=" << cholesky_.getDimension() << ", here dimension=" << m.getDimension();
  const UnsignedInteger p = cholesky_.getDimension();
  try
  {
    // If the Cholesky factor is not defined, it means that M is not symmetric positive definite (an exception is thrown) and the PDF is zero
    TriangularMatrix X(CovarianceMatrix(m).computeCholesky());
    // Compute the determinant of the Cholesky factor, ie the square-root of the determinant of M
    // Here, the diagonal of X is positive
    Scalar logPDF = 0.0;
    for (UnsignedInteger i = 0; i < p; ++i) logPDF += std::log(X(i, i));
    logPDF *= nu_ - p - 1.0;
    // Add the term which does not depend on M
    logPDF += logNormalizationFactor_;
    // Trace(V^{-1} M) = Trace(C'^{-1} C^{-1} X X') = Trace(C^{-1}XX'C'^{-1})
    //                 = Trace(AA') with A = C^{-1}X
    const TriangularMatrix A(cholesky_.solveLinearSystem(X).getImplementation());
    logPDF -= 0.5 * A.computeGram(false).computeTrace();
    return logPDF;
  }
  catch (...)
  {
    return SpecFunc::LowestScalar;
  }
}

/* Get the CDF of the distribution */
Scalar Wishart::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  return ContinuousDistribution::computeCDF(point);
}

/* Compute the mean of the distribution */
void Wishart::computeMean() const
{
  const CovarianceMatrix V(getV());
  const UnsignedInteger p = cholesky_.getDimension();
  mean_ = Point(getDimension());
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      mean_[index] = nu_ * V(i, j);
      ++index;
    }
  isAlreadyComputedMean_ = true;
}

/* Compute the entropy of the distribution */
Scalar Wishart::computeEntropy() const
{
  // E = p\nu/2 - logNormalizationFactor + (p + 1 - \nu)[\log|L|+{p\log(2)\sum_{i=0}^{p-1}\psi((\nu-i)/2)}/2]
  const Scalar p = cholesky_.getDimension();
  Scalar E = 0.5 * nu_ * p - logNormalizationFactor_;
  Scalar logDetL = 0.0;
  for (UnsignedInteger i = 0; i < p; ++i) logDetL += std::log(cholesky_(i, i));
  Scalar sumPsi = 0.0;
  for (UnsignedInteger i = 0; i < p; ++i) sumPsi += SpecFunc::Psi(0.5 * (nu_ - i));
  E += (p + 1.0 - nu_) * (0.5 * (p * M_LN2 + sumPsi) + logDetL);
  return E;
}

/* Get the standard deviation of the distribution */
Point Wishart::getStandardDeviation() const /*throw(NotDefinedException)*/
{
  const UnsignedInteger p = cholesky_.getDimension();
  Point sigma(getDimension());
  // If the covariance has already been computed, use it
  if (isAlreadyComputedCovariance_)
  {
    for (UnsignedInteger i = 0; i < getDimension(); ++i) sigma[i] = std::sqrt(covariance_(i, i));
    return sigma;
  }
  // else compute only the standard deviation as the covariance may be huge
  const CovarianceMatrix V(getV());
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      sigma[index] = std::sqrt(nu_ * (V(i, j) * V(j, i) + V(i, i) * V(j, j)));
      ++index;
    }
  return sigma;
}


/* Parameters value and description accessor */
Wishart::PointWithDescriptionCollection Wishart::getParametersCollection() const
{
  PointWithDescription point(getDimension() + 1);
  Description description(point.getDimension());
  const UnsignedInteger p = cholesky_.getDimension();
  UnsignedInteger index = 0;
  const CovarianceMatrix V(getV());
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      point[index] = V(i, j);
      description[index] = String(OSS() << "V_" << i << "_" << j);
      ++index;
    }
  point[index] = nu_;
  description[index] = "nu";
  point.setDescription(description);
  PointWithDescriptionCollection parameters(point.getDimension());
  return parameters;
}

void Wishart::setParametersCollection(const PointCollection & parametersCollection)
{
  const Scalar w = getWeight();
  const Scalar pReal = 0.5 * std::sqrt(8.0 * parametersCollection.getSize() - 7.0) - 0.5;
  const UnsignedInteger p = static_cast< UnsignedInteger >(pReal);
  if (pReal != p) throw InvalidArgumentException(HERE) << "Error: the given parameters cannot be converted into a covariance matrix and a number of degrees of freedom.";
  CovarianceMatrix V(p);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      V(i, j) = parametersCollection[0][index];
      ++index;
    }
  const Scalar nu = parametersCollection[0][index];
  *this = Wishart(V, nu);
  setWeight(w);
}

Point Wishart::getParameter() const
{
  const CovarianceMatrix V(getV());
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

void Wishart::setParameter(const Point & parameter)
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
  *this = Wishart(V, nu);
  setWeight(w);
}

Description Wishart::getParameterDescription() const
{
  const UnsignedInteger p = getDimension();
  Description description((p * (p + 1)) / 2 + 1);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < p; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      description[index] = (OSS() << "v_" << i << "_" << j);
    }
  }
  description[index] = "nu";
  return description;
}

/* V accessor */
void Wishart::setV(const CovarianceMatrix & v)
{
  try
  {
    // Copy of v because v is const and not computeCholesky()
    cholesky_ = CovarianceMatrix(v).computeCholesky();
  }
  catch(...)
  {
    throw InvalidArgumentException(HERE) << "Error: V must be positive definite";
  }
  const UnsignedInteger p = cholesky_.getDimension();
  setDimension((p * (p + 1)) / 2);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

CovarianceMatrix Wishart::getV() const
{
  return cholesky_.computeGram(false).getImplementation();
}


/* Nu accessor */
void Wishart::setNu(const Scalar nu)
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

Scalar Wishart::getNu() const
{
  return nu_;
}

/* Compute the normalization factor on log-scale */
void Wishart::update()
{
  const UnsignedInteger p = cholesky_.getDimension();
  // logNormalizationFactor = -log(2^{p\nu/2}|V|^{\nu/2}multigamma_p(\nu/2)
  logNormalizationFactor_ = -0.5 * p * (nu_ * M_LN2 + 0.5 * (p - 1) * std::log(M_PI));
  for (UnsignedInteger i = 0; i < p; ++i) logNormalizationFactor_ -= SpecFunc::LogGamma(0.5 * (nu_ - i)) + nu_ * std::log(cholesky_(i, i));
}

/* Method save() stores the object through the StorageManager */
void Wishart::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "cholesky_", cholesky_ );
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Wishart::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "cholesky_", cholesky_ );
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
