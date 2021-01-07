//                                               -*- C++ -*-
/**
 *  @brief A class that implements a normal copula
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
#include "openturns/NormalCopula.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/NatafEllipticalCopulaEvaluation.hxx"
#include "openturns/NatafEllipticalCopulaGradient.hxx"
#include "openturns/NatafEllipticalCopulaHessian.hxx"
#include "openturns/InverseNatafEllipticalCopulaEvaluation.hxx"
#include "openturns/InverseNatafEllipticalCopulaGradient.hxx"
#include "openturns/InverseNatafEllipticalCopulaHessian.hxx"
#include "openturns/Indices.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalCopula)

static const Factory<NormalCopula> Factory_NormalCopula;

/* Default constructor */
NormalCopula::NormalCopula(const UnsignedInteger dim)
  : CopulaImplementation()
  , correlation_(dim)
  , normal_(dim)
{
  setName("NormalCopula");
  // The range is generic for all the copulas
  setDimension(dim);
  computeRange();
}

/* Default constructor */
NormalCopula::NormalCopula(const CorrelationMatrix & correlation)
  : CopulaImplementation()
  , correlation_(correlation)
  , normal_(Point(correlation.getNbRows(), 0.0), Point(correlation.getNbRows(), 1.0), correlation)
{
  setName("NormalCopula");
  // The range is generic for all the copulas
  setDimension(correlation.getNbRows());
  computeRange();
}

/* Comparison operator */
Bool NormalCopula::operator ==(const NormalCopula & other) const
{
  if (this == &other) return true;
  return correlation_ == other.correlation_;
}

Bool NormalCopula::equals(const DistributionImplementation & other) const
{
  const NormalCopula* p_other = dynamic_cast<const NormalCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NormalCopula::__repr__() const
{
  OSS oss;
  oss << "class=" << NormalCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " correlation=" << correlation_;
  return oss;
}

String NormalCopula::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(R = " << correlation_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
NormalCopula * NormalCopula::clone() const
{
  return new NormalCopula(*this);
}

/* Get one realization of the distribution */
Point NormalCopula::getRealization() const
{
  UnsignedInteger dimension = getDimension();
  if (hasIndependentCopula()) return RandomGenerator::Generate(dimension);
  else
  {
    Point realization(normal_.getRealization());
    for (UnsignedInteger i = 0; i < dimension; ++i) realization[i] = DistFunc::pNormal(realization[i]);
    return realization;
  }
}

struct NormalCopulaComputeSamplePolicy
{
  const Sample input_;
  Sample & output_;
  UnsignedInteger dimension_;

  NormalCopulaComputeSamplePolicy(const Sample & input,
                                  Sample & output)
    : input_(input)
    , output_(output)
    , dimension_(input.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        output_(i, j) = DistFunc::pNormal(input_(i, j));
  }

}; /* end struct NormalCopulaComputeSamplePolicy */

/* Get a sample of the distribution */
Sample NormalCopula::getSampleParallel(const UnsignedInteger size) const
{
  if (hasIndependentCopula())
  {
    const UnsignedInteger dimension = getDimension();
    Sample result(size, dimension);
    const Point rawData(RandomGenerator::Generate(dimension * size));
    result.getImplementation()->setData(rawData);
    result.setName(getName());
    result.setDescription(getDescription());
    return result;
  }
  else
  {
    const UnsignedInteger dimension = getDimension();
    const Sample normalSample(normal_.getSample(size));
    Sample result(size, dimension);
    const NormalCopulaComputeSamplePolicy policy( normalSample, result );
    TBB::ParallelFor( 0, size, policy );
    result.setName(getName());
    result.setDescription(getDescription());
    return result;
  } // Non independent copula
}

Sample NormalCopula::getSample(const UnsignedInteger size) const
{
  if (isParallel_) return getSampleParallel(size);
  return DistributionImplementation::getSample(size);
}

/* Get the DDF of the distribution */
Point NormalCopula::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Be careful to evaluate the copula only in the interior of its support
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support return 0.0
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return Point(dimension, 0.0);
  }
  Point x(dimension);
  Point marginalPDF(dimension);
  Scalar marginalPDFProduct = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar xi = DistFunc::qNormal(point[i]);
    x[i] = xi;
    const Scalar pdfI = DistFunc::dNormal(xi);
    marginalPDF[i] = pdfI;
    marginalPDFProduct *= pdfI;
  }
  const Point ddfNorm(normal_.computeDDF(x));
  const Scalar pdfNorm = normal_.computePDF(x);
  Point ddf(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) ddf[i] = (ddfNorm[i] + x[i] * pdfNorm) / (marginalPDFProduct * marginalPDF[i]);
  return ddf;
}

/* Get the PDF of the distribution */
Scalar NormalCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Be careful to evaluate the copula only in the interior of its support
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support return 0.0
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  }
  // Compute the normal point such that a normal distribution with this copula
  // and standard 1D normal marginals has the same CDF at this normal point
  // than the copula at the given point.
  // Compute the multiplicative factor between the copula PDF
  // and the PDF of the associated generic normal using the specific form of
  // the standard normal PDF
  Point normalPoint(dimension);
  Scalar value = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar yi = DistFunc::qNormal(point[i]);
    normalPoint[i] = yi;
    // Prod_i phi(yi), phi being the univariate standard gaussian
    // density function
    value *= DistFunc::dNormal(yi);
  }
  return normal_.computePDF(normalPoint) / value;
}

/* Get the CDF of the distribution */
Scalar NormalCopula::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Compute the normal point such that a normal distribution with this copula
  // and standard 1D normal marginals has the same CDF at this normal point
  // than the copula at the given point.
  // Be careful to evaluate the copula only in the interior of its support
  Indices indices;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support, in the lower part, return 0.0
    if (point[i] <= 0.0) return 0.0;
    // If the current component is in the interior of the support, its index
    // is taken into account
    if (point[i] < 1.0) indices.add(i);
  }
  const UnsignedInteger activeDimension = indices.getSize();
  // Quick return if all the components are >= 1
  if (activeDimension == 0) return 1.0;
  Point normalPoint(activeDimension);
  for (UnsignedInteger i = 0; i < activeDimension; ++i) normalPoint[i] = DistFunc::qNormal(point[indices[i]]);
  // In the usual case when the given point is in the interior of the support
  // use the associated normal distribution
  if (dimension == activeDimension) return normal_.computeCDF(normalPoint);
  // In the other case, we must use the appropriate marginal distribution
  else return normal_.getMarginal(indices).computeCDF(normalPoint);
} // computeCDF

/* Get the survival function of the distribution */
Scalar NormalCopula::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  // Compute the normal point such that a normal distribution with this copula
  // and standard 1D normal marginals has the same CDF at this normal point
  // than the copula at the given point.
  // Be careful to evaluate the copula only in the interior of its support
  Indices indices;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If outside of the support, in the upper part, return 0.0
    if (point[i] >= 1.0) return 0.0;
    // If the current component is in the interior of the support, its index
    // is taken into account
    if (point[i] > 0.0) indices.add(i);
  }
  const UnsignedInteger activeDimension = indices.getSize();
  // Quick return if all the components are >= 1
  if (activeDimension == 0) return 1.0;
  Point normalPoint(activeDimension);
  for (UnsignedInteger i = 0; i < activeDimension; ++i) normalPoint[i] = DistFunc::qNormal(point[indices[i]]);
  // In the usual case when the given point is in the interior of the support
  // use the associated normal distribution
  if (dimension == activeDimension) return normal_.computeSurvivalFunction(normalPoint);
  // In the other case, we must use the appropriate marginal distribution
  else return Distribution(normal_.getMarginal(indices)).computeSurvivalFunction(normalPoint);
} // computeSurvivalFunction

/* Compute the probability content of an interval */
Scalar NormalCopula::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // Reduce the given interval to the support of the distribution, which is the nD unit cube
  const Interval intersect(interval.intersect(Interval(dimension)));
  // If the intersection is empty
  if (intersect.isEmpty()) return 0.0;
  const Point lowerBoundIntersect(intersect.getLowerBound());
  const Point upperBoundIntersect(intersect.getUpperBound());
  Point lowerBound(dimension);
  Point upperBound(dimension);
  Interval::BoolCollection finiteLowerBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (lowerBoundIntersect[i] == 0.0)
    {
      finiteLowerBound[i] = false;
      lowerBound[i] = normal_.getRange().getLowerBound()[i];
    }
    else
    {
      finiteLowerBound[i] = true;
      lowerBound[i] = DistFunc::qNormal(lowerBoundIntersect[i]);
    }
    if (upperBoundIntersect[i] == 1.0)
    {
      finiteUpperBound[i] = false;
      upperBound[i] = normal_.getRange().getUpperBound()[i];
    }
    else
    {
      finiteUpperBound[i] = true;
      upperBound[i] = DistFunc::qNormal(upperBoundIntersect[i]);
    }
  }
  return normal_.computeProbability(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* compute the covariance of the distribution */
void NormalCopula::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // Off-diagonal terms
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = std::asin(0.5 * correlation_(i, j)) / (2.0 * M_PI);
    // Diagonal term
    covariance_(i, i) = 1.0 / 12.0;
  }
  isAlreadyComputedCovariance_ = true;
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix NormalCopula::getKendallTau() const
{
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix tau(dimension);
  for (UnsignedInteger i = 1; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < i; ++j) tau(i, j) = std::asin(correlation_(i, j)) * 2.0 / M_PI;
  return tau;
}

/* Compute the entropy of the distribution */
Scalar NormalCopula::computeEntropy() const
{
  return normal_.computeEntropy() - getDimension() * (SpecFunc::LOGSQRT2PI + 0.5);
}

/* Get the Shape matrix of the copula */
CorrelationMatrix NormalCopula::getShapeMatrix() const
{
  return correlation_;
}

/* Get the PDF gradient of the distribution */
Point NormalCopula::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In NormalCopula::computePDFGradient(const Point & point) const";
}

/* Get the CDF gradient of the distribution */
Point NormalCopula::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In NormalCopula::computeCDFGradient(const Point & point) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
   For Normal distribution, the conditional distribution is also Normal, with mean and covariance
   such as:
   mean_cond = mean(x) + cov(x, y).cov(y, y)^(-1)(y - mean(y))
   cov_cond = cov(x, x) - cov(x, y).cov(y, y)^(-1)cov(x, y)
   This expression simplifies if we use the inverse of the Cholesky factor of the covariance matrix.
   See [Lebrun, Dutfoy, "Rosenblatt and Nataf transformation"]
   The conditional normal copula is the copula of this conditional normal distribution
*/
Scalar NormalCopula::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return (x >= 0.0 && x < 1.0 ? 1.0 : 0.0);
  // General case
  const TriangularMatrix inverseCholesky(normal_.getInverseCholesky());
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky(conditioningDimension, i) * DistFunc::qNormal(y[i]);
  }
  meanRos *= -sigmaRos;
  const Scalar z = DistFunc::qNormal(x);
  const Scalar u = (z - meanRos) / sigmaRos;
  return std::exp(-0.5 * (u * u - z * z)) / sigmaRos;
}

Point NormalCopula::computeConditionalPDF(const Point & x,
    const Sample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning sample of dimension greater or equal to the distribution dimension.";
  const UnsignedInteger size = x.getSize();
  if (y.getSize() != x.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning sample of size different from the argument size.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    Point result(size, 1.0);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      if (x[i]  < 0.0) result[i] = 0.0;
      if (x[i] >= 1.0) result[i] = 0.0;
    }
    return result;
  }
  // General case
  const TriangularMatrix inverseCholesky(normal_.getInverseCholesky());
  const Scalar sigmaRos = 1.0 / inverseCholesky(conditioningDimension, conditioningDimension);
  MatrixImplementation row(1, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) row(0, i) = inverseCholesky(conditioningDimension, i);
  const MatrixImplementation qY(conditioningDimension, size, DistFunc::qNormal(y.getImplementation()->getData()));
  const MatrixImplementation meanRos(row.genProd(qY));
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar z = DistFunc::qNormal(x[i]);
    const Scalar u = z / sigmaRos + meanRos[i];
    result[i] = std::exp(-0.5 * (u * u - z * z)) / sigmaRos;
  }
  return result;
}

Point NormalCopula::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = (x[i] >= 0.0 && x[i] < 1.0 ? 1.0 : 0.0);
  else
  {
    const TriangularMatrix inverseCholesky(normal_.getInverseCholesky());
    const Point z(DistFunc::qNormal(x));
    const Point u(inverseCholesky * z);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = std::exp(-0.5 * (u[i] * u[i] - z[i] * z[i])) * inverseCholesky(i, i);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar NormalCopula::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
    return (x <= 0.0 ? 0.0 : (x >= 1.0 ? 1.0 : x));
  // General case
  const TriangularMatrix inverseCholesky(normal_.getInverseCholesky());
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
    meanRos += inverseCholesky(conditioningDimension, i) * DistFunc::qNormal(y[i]);
  meanRos *= -sigmaRos;
  return DistFunc::pNormal((DistFunc::qNormal(x) - meanRos) / sigmaRos);
}

Point NormalCopula::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  if (hasIndependentCopula())
  {
    Point result(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = (x[i] <= 0.0 ? 0.0 : (x[i] >= 1.0 ? 1.0 : x[i]));
    return result;
  }
  return DistFunc::pNormal(normal_.getInverseCholesky() * DistFunc::qNormal(x));
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar NormalCopula::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return q;
  // General case
  const TriangularMatrix inverseCholesky(normal_.getInverseCholesky());
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky(conditioningDimension, i) * DistFunc::qNormal(y[i]);
  }
  meanRos *= - sigmaRos;
  return DistFunc::pNormal(meanRos + sigmaRos * DistFunc::qNormal(q));
}

Point NormalCopula::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  if (hasIndependentCopula()) return q;
  return DistFunc::pNormal(normal_.getCholesky() * DistFunc::qNormal(q));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution NormalCopula::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  CorrelationMatrix R(outputDimension);
  // Extract the correlation matrix, the marginal standard deviations and means
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    for (UnsignedInteger j = 0; j <= i; ++j) R(i, j) = correlation_(index_i, indices[j]);
  }
  return new NormalCopula(R);
}

/* Get the isoprobabilist transformation */
NormalCopula::IsoProbabilisticTransformation NormalCopula::getIsoProbabilisticTransformation() const
{
  IsoProbabilisticTransformation transformation;
  transformation.setEvaluation(new NatafEllipticalCopulaEvaluation(getStandardDistribution(), normal_.getInverseCholesky()));
  transformation.setGradient(new NatafEllipticalCopulaGradient(getStandardDistribution(), normal_.getInverseCholesky()));
  transformation.setHessian(new NatafEllipticalCopulaHessian(getStandardDistribution(), normal_.getInverseCholesky()));

  return transformation;
}

/* Get the inverse isoprobabilist transformation */
NormalCopula::InverseIsoProbabilisticTransformation NormalCopula::getInverseIsoProbabilisticTransformation() const
{
  InverseIsoProbabilisticTransformation transformation;
  transformation.setEvaluation(new InverseNatafEllipticalCopulaEvaluation(getStandardDistribution(), normal_.getCholesky()));
  transformation.setGradient(new InverseNatafEllipticalCopulaGradient(getStandardDistribution(), normal_.getCholesky()));
  transformation.setHessian(new InverseNatafEllipticalCopulaHessian(getStandardDistribution(), normal_.getCholesky()));

  return transformation;
}

/* Tell if the distribution has an elliptical copula */
Bool NormalCopula::hasEllipticalCopula() const
{
  return true;
}

/* Tell if the distribution has independent copula */
Bool NormalCopula::hasIndependentCopula() const
{
  return normal_.hasIndependentCopula();
}

void NormalCopula::setParametersCollection(const PointCollection & parametersCollection)
{
  // Check if the given parameters are ok
  if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: the given collection has a size=" << parametersCollection.getSize() << " but should be of size=1";
  const Point parameters(parametersCollection[0]);
  const UnsignedInteger dimension = getDimension();
  if (parameters.getDimension() != dimension * (dimension - 1) / 2) throw InvalidArgumentException(HERE) << "Error: got " << parameters.getDimension() << " parameters instead of " << dimension * (dimension - 1) / 2;
  if (dimension == 1) return;
  UnsignedInteger dependenceIndex = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      correlation_(i, j) = parameters[dependenceIndex];
      ++dependenceIndex;
    }
  }
}

Point NormalCopula::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  Point point;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      point.add(correlation_(i, j));
    }
  }
  return point;
}

void NormalCopula::setParameter(const Point & parameter)
{
  // N = ((d-1)*d)/2
  const UnsignedInteger size = parameter.getSize();
  Scalar dimReal = 0.5 * std::sqrt(1.0 + 8.0 * size) + 0.5;
  if (dimReal != round(dimReal)) throw InvalidArgumentException(HERE) << "Error: invalid value number for NormalCopula";
  const UnsignedInteger dimension = dimReal;
  const Scalar w = getWeight();
  if (dimension > 1)
  {
    CorrelationMatrix R(dimension);
    UnsignedInteger dependenceIndex = 0;
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      for (UnsignedInteger j = 0; j < i; ++ j)
      {
        R(i, j) = parameter[dependenceIndex];
        ++ dependenceIndex;
      }
    }
    *this = NormalCopula(R);
  }
  else
  {
    *this = NormalCopula(dimension);
  }
  setWeight(w);
}

Description NormalCopula::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      description.add(OSS() << "R_" << i + 1 << "_" << j + 1);
    }
  }
  return description;
}

/* Compute the correlation matrix of a Normal Copula from its Spearman correlation matrix */
CorrelationMatrix NormalCopula::GetCorrelationFromSpearmanCorrelation(const CorrelationMatrix & matrix)
{
  const UnsignedInteger dimension = matrix.getNbRows();
  CorrelationMatrix result(dimension);
  for (UnsignedInteger i = 1; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < i; ++j) result(i, j) = 2.0 * std::sin(M_PI * matrix(i, j) / 6.0);
  if (!result.isPositiveDefinite()) throw NotSymmetricDefinitePositiveException(HERE) << "Error: the normal copula correlation matrix built from the given Spearman correlation matrix is not definite positive";
  return result;
}

/* Compute the correlation matrix of a Normal Copula from its Kendal correlation matrix */
CorrelationMatrix NormalCopula::GetCorrelationFromKendallCorrelation(const CorrelationMatrix & matrix)
{
  const UnsignedInteger dimension = matrix.getNbRows();
  CorrelationMatrix result(dimension);
  for (UnsignedInteger i = 1; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < i; ++j) result(i, j) = std::sin(M_PI_2 * matrix(i, j));
  if (!result.isPositiveDefinite()) throw NotSymmetricDefinitePositiveException(HERE) << "Error: the normal copula correlation matrix built from the given Kendall correlation matrix is not definite positive";
  return result;
}

/* Method save() stores the object through the StorageManager */
void NormalCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
  adv.saveAttribute( "correlation_", correlation_ );
  adv.saveAttribute( "covariance_duplicate", covariance_ );
  adv.saveAttribute( "normal_", normal_ );
  adv.saveAttribute( "integrationNodesNumber_duplicate", integrationNodesNumber_ );
  adv.saveAttribute( "isAlreadyComputedCovariance_duplicate", isAlreadyComputedCovariance_ );
}

/* Method load() reloads the object from the StorageManager */
void NormalCopula::load(Advocate & adv)
{
  // The range is generic for all the copulas
  CopulaImplementation::load(adv);
  adv.loadAttribute( "correlation_", correlation_ );
  adv.loadAttribute( "covariance_duplicate", covariance_ );
  adv.loadAttribute( "normal_", normal_ );
  adv.loadAttribute( "integrationNodesNumber_duplicate", integrationNodesNumber_ );
  adv.loadAttribute( "isAlreadyComputedCovariance_duplicate", isAlreadyComputedCovariance_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
