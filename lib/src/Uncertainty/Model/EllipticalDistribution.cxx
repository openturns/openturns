//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for elliptical distributions
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/EllipticalDistribution.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/NatafEllipticalDistributionEvaluation.hxx"
#include "openturns/NatafEllipticalDistributionGradient.hxx"
#include "openturns/NatafEllipticalDistributionHessian.hxx"
#include "openturns/InverseNatafEllipticalDistributionEvaluation.hxx"
#include "openturns/InverseNatafEllipticalDistributionGradient.hxx"
#include "openturns/InverseNatafEllipticalDistributionHessian.hxx"
#include "openturns/Description.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EllipticalDistribution)

static const Factory<EllipticalDistribution> Factory_EllipticalDistribution;

/* Default constructor */
EllipticalDistribution::EllipticalDistribution()
  : DistributionImplementation()
  , sigma_(1, 1.0)
  , R_(1)
{
  // Nothing to do
}

/* Parameter constructor */
EllipticalDistribution::EllipticalDistribution(const Point & mu,
    const Point & sigma,
    const CorrelationMatrix & R,
    const Scalar covarianceScalingFactor)
  : DistributionImplementation()
  , sigma_(sigma)
  , R_(R)
  , covarianceScalingFactor_(covarianceScalingFactor)
{
  const UnsignedInteger dimension = R.getDimension();
  // We check if the inputs have the same dimension. If not, we throw an exception
  if ( (dimension != mu.getDimension()) ||
       (dimension != sigma.getDimension()) )
    throw InvalidArgumentException(HERE)
        << "Arguments have incompatible dimensions: R dimension=" << dimension
        << " sigma dimension=" << sigma.getDimension()
        << " mu dimension=" << mu.getDimension();
  // We check that the marginal standard deviations are >= 0
  for(UnsignedInteger i = 0; i < dimension; ++i)
    if (!(sigma[i] >= 0.0)) throw InvalidArgumentException(HERE) << "The marginal standard deviations must be >= 0 sigma=" << sigma[i];
  // Then we set the dimension of the Elliptical distribution
  setDimension(dimension);
  // The mean attribute is stored at an upper level
  mean_ = mu;
  // Compute the auxiliary attributes. It also set isAlreadyComputedMean to true
  update();
}


Bool EllipticalDistribution::equals(const DistributionImplementation & other) const
{
  // Check by properties
  // Do they have the same dimension?
  if (dimension_ != other.getDimension()) return false;
  // Can I cast other into an elliptical distribution?
  const EllipticalDistribution * p_other = dynamic_cast<const EllipticalDistribution*>(&other);
  if (p_other) return (mean_ == p_other->mean_) && (sigma_ == p_other->sigma_) && (R_ == p_other->R_);
  // Here, if dimension == 1 no more test can be done
  if (dimension_ == 1) return false;
  // Otherwise, check equality using properties
  // Are they both elliptical?
  if (!other.isElliptical()) return false;
  // The copula...
  if ( !( (hasIndependentCopula() && other.hasIndependentCopula()) || (getCopula() == other.getCopula()) ) ) return false;
  // Then the marginals
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
    if (!(getMarginal(i).operator == (other.getMarginal(i)))) return false;
  return true;
}

/* Centers and reduces a value u = Diag(sigma_)^(-1) * (x - mean_) */
Point EllipticalDistribution::normalize(const Point & x) const
{
  Point u(x - mean_);
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    if (sigma_[i] > 0.0) u[i] /= sigma_[i];
    else u[i] = 0.0;
  return u;
}

/* Decenters and scales a value x = mean_ + Diag(sigma_) * u */
Point EllipticalDistribution::denormalize(const Point & u) const
{
  Point x(mean_);
  for (UnsignedInteger i = 0; i < getDimension(); ++i) x[i] += sigma_[i] * u[i];
  return x;
}

/* String converter */
String EllipticalDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << EllipticalDistribution::GetClassName()
      << "parameters collection=" << getParametersCollection();
  return oss;
}

/* Tell if the distribution is elliptical */
Bool EllipticalDistribution::isElliptical() const
{
  return true;
}

/* Tell if the distribution has elliptical copula */
Bool EllipticalDistribution::hasEllipticalCopula() const
{
  return true;
}


/* Compute the density generator of the elliptical distribution, i.e.
 *  the function phi such that the density of the distribution can
 *  be written as p(x) = phi((x-mean_).C^{-1} * (x-mean_))                      */
Scalar EllipticalDistribution::computeDensityGenerator(const Scalar ) const
{
  throw NotYetImplementedException(HERE) << "In EllipticalDistribution::computeDensityGenerator(const Scalar betaSquare) const";
}

Scalar EllipticalDistribution::computeLogDensityGenerator(const Scalar betaSquare) const
{
  const Scalar densityGenerator = computeDensityGenerator(betaSquare);
  if (densityGenerator == 0.0) return SpecFunc::LowestScalar;
  return std::log(densityGenerator);
}

/* Compute the derivative of the density generator */
Scalar EllipticalDistribution::computeDensityGeneratorDerivative(const Scalar betaSquare) const
{
  // Use centered finite difference
  const Scalar epsilon = std::cbrt(ResourceMap::GetAsScalar("Distribution-DefaultPDFEpsilon"));
  return (computeDensityGenerator(betaSquare + epsilon) - computeDensityGenerator(betaSquare - epsilon)) / (2.0 * epsilon);
}

/* Compute the seconde derivative of the density generator */
Scalar EllipticalDistribution::computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const
{
  // Use centered finite difference
  const Scalar epsilon = std::pow(ResourceMap::GetAsScalar("Distribution-DefaultPDFEpsilon"), 0.25);
  return (computeDensityGenerator(betaSquare + epsilon) - 2.0 * computeDensityGenerator(betaSquare) + computeDensityGenerator(betaSquare - epsilon)) / (epsilon * epsilon);
}

/* Get the DDF of the distribution */
Point EllipticalDistribution::computeDDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Check support for degenerate dimensions
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (sigma_[i] == 0.0)
    {
      if (point[i] != mean_[i]) return Point(dimension, 0.0);
      // Any degenerate dimension makes the DDF singular w.r.t. full Lebesgue measure
      if (dimension > 1) return Point(dimension, 0.0);
    }
  }

  switch(dimension)
  {
    case 1:
    {
      const Scalar iLx = (point[0] - mean_[0]) / sigma_[0];
      return Point(1, 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(iLx * iLx) * inverseCholesky_(0, 0) * iLx);
    }
    break;
    case 2:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      Scalar iLx, iLy;
      Point result(2);
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
        const Scalar betaSquare = iLx * iLx + iLy * iLy;
        const Scalar factor = 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(betaSquare);
        result[0] = factor * (inverseCholesky_(0, 0) * iLx + inverseCholesky_(1, 0) * iLy);
        result[1] = factor * (inverseCholesky_(1, 1) * iLy);
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY;
        iLy = inverseCholesky_(1, 1) * deltaY;
        const Scalar betaSquare = iLx * iLx + iLy * iLy;
        const Scalar factor = 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(betaSquare);
        result[0] = factor * (inverseCholesky_(0, 0) * iLx);
        result[1] = factor * (inverseCholesky_(0, 1) * iLx + inverseCholesky_(1, 1) * iLy);
      }
      return result;
    }
    break;
    case 3:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      const Scalar deltaZ = point[2] - mean_[2];
      Scalar iLx, iLy, iLz;
      Point result(3);
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
        iLz = inverseCholesky_(2, 0) * deltaX + inverseCholesky_(2, 1) * deltaY + inverseCholesky_(2, 2) * deltaZ;
        const Scalar betaSquare = iLx * iLx + iLy * iLy + iLz * iLz;
        const Scalar factor = 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(betaSquare);
        result[0] = factor * (inverseCholesky_(0, 0) * iLx + inverseCholesky_(1, 0) * iLy + inverseCholesky_(2, 0) * iLz);
        result[1] = factor * (inverseCholesky_(1, 1) * iLy + inverseCholesky_(2, 1) * iLz);
        result[2] = factor * (inverseCholesky_(2, 2) * iLz);
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY + inverseCholesky_(0, 2) * deltaZ;
        iLy = inverseCholesky_(1, 1) * deltaY + inverseCholesky_(1, 2) * deltaZ;
        iLz = inverseCholesky_(2, 2) * deltaZ;
        const Scalar betaSquare = iLx * iLx + iLy * iLy + iLz * iLz;
        const Scalar factor = 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(betaSquare);
        result[0] = factor * (inverseCholesky_(0, 0) * iLx);
        result[1] = factor * (inverseCholesky_(0, 1) * iLx + inverseCholesky_(1, 1) * iLy);
        result[2] = factor * (inverseCholesky_(0, 2) * iLx + inverseCholesky_(1, 2) * iLy + inverseCholesky_(2, 2) * iLz);
      }
      return result;
    }
    break;
    default:
      const Point iLx(inverseCholesky_ * (point - mean_));
      const Scalar betaSquare = iLx.normSquare();
      return 2.0 * normalizationFactor_ * computeDensityGeneratorDerivative(betaSquare) * inverseCholesky_.transpose() * iLx;
  }
}

/* Get the PDF of the distribution */
Scalar EllipticalDistribution::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Check support for degenerate dimensions
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (sigma_[i] == 0.0)
    {
      if (point[i] != mean_[i]) return 0.0;
      // Any degenerate dimension makes the PDF singular w.r.t. full Lebesgue measure
      if (dimension > 1) return 0.0;
    }
  }

  switch(dimension)
  {
    case 1:
    {
      if (sigma_[0] == 0.0) return 0.0;
      const Scalar iLx = (point[0] - mean_[0]) / sigma_[0];
      return normalizationFactor_ * computeDensityGenerator(iLx * iLx);
    }
    break;
    case 2:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      Scalar iLx, iLy;
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY;
        iLy = inverseCholesky_(1, 1) * deltaY;
      }
      const Scalar betaSquare = iLx * iLx + iLy * iLy;
      return normalizationFactor_ * computeDensityGenerator(betaSquare);
    }
    break;
    case 3:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      const Scalar deltaZ = point[2] - mean_[2];
      Scalar iLx, iLy, iLz;
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
        iLz = inverseCholesky_(2, 0) * deltaX + inverseCholesky_(2, 1) * deltaY + inverseCholesky_(2, 2) * deltaZ;
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY + inverseCholesky_(0, 2) * deltaZ;
        iLy = inverseCholesky_(1, 1) * deltaY + inverseCholesky_(1, 2) * deltaZ;
        iLz = inverseCholesky_(2, 2) * deltaZ;
      }
      const Scalar betaSquare = iLx * iLx + iLy * iLy + iLz * iLz;
      return normalizationFactor_ * computeDensityGenerator(betaSquare);
    }
    break;
    default:
      const Point iLx(inverseCholesky_ * (point - mean_));
      const Scalar betaSquare = iLx.normSquare();
      return normalizationFactor_ * computeDensityGenerator(betaSquare);
  }
}

/* Get the log-PDF of the distribution */
Scalar EllipticalDistribution::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Check support for degenerate dimensions
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (sigma_[i] == 0.0)
    {
      if (point[i] != mean_[i]) return SpecFunc::LowestScalar;
      // Any degenerate dimension makes the PDF singular w.r.t. full Lebesgue measure
      if (dimension > 1) return SpecFunc::LowestScalar;
    }
  }

  switch(dimension)
  {
    case 1:
    {
      if (sigma_[0] == 0.0) return SpecFunc::LowestScalar;
      const Scalar iLx = (point[0] - mean_[0]) / sigma_[0];
      const Scalar betaSquare = iLx * iLx;
      const Scalar logDensityGenerator = computeLogDensityGenerator(betaSquare);
      if (!std::isfinite(logDensityGenerator)) return SpecFunc::LowestScalar;
      return std::log(normalizationFactor_) + logDensityGenerator;
    }
    break;
    case 2:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      Scalar iLx, iLy;
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY;
        iLy = inverseCholesky_(1, 1) * deltaY;
      }
      const Scalar betaSquare = iLx * iLx + iLy * iLy;
      const Scalar logDensityGenerator = computeLogDensityGenerator(betaSquare);
      if (!std::isfinite(logDensityGenerator)) return SpecFunc::LowestScalar;
      return std::log(normalizationFactor_) + logDensityGenerator;
    }
    break;
    case 3:
    {
      const Scalar deltaX = point[0] - mean_[0];
      const Scalar deltaY = point[1] - mean_[1];
      const Scalar deltaZ = point[2] - mean_[2];
      Scalar iLx, iLy, iLz;
      if (inverseCholesky_.isLowerTriangular())
      {
        iLx = inverseCholesky_(0, 0) * deltaX;
        iLy = inverseCholesky_(1, 0) * deltaX + inverseCholesky_(1, 1) * deltaY;
        iLz = inverseCholesky_(2, 0) * deltaX + inverseCholesky_(2, 1) * deltaY + inverseCholesky_(2, 2) * deltaZ;
      }
      else
      {
        iLx = inverseCholesky_(0, 0) * deltaX + inverseCholesky_(0, 1) * deltaY + inverseCholesky_(0, 2) * deltaZ;
        iLy = inverseCholesky_(1, 1) * deltaY + inverseCholesky_(1, 2) * deltaZ;
        iLz = inverseCholesky_(2, 2) * deltaZ;
      }
      const Scalar betaSquare = iLx * iLx + iLy * iLy + iLz * iLz;
      const Scalar logDensityGenerator = computeLogDensityGenerator(betaSquare);
      if (!std::isfinite(logDensityGenerator)) return SpecFunc::LowestScalar;
      return std::log(normalizationFactor_) + logDensityGenerator;
    }
    break;
    default:
      const Point iLx(inverseCholesky_ * (point - mean_));
      const Scalar betaSquare = iLx.normSquare();
      const Scalar logDensityGenerator = computeLogDensityGenerator(betaSquare);
      if (!std::isfinite(logDensityGenerator)) return SpecFunc::LowestScalar;
      return std::log(normalizationFactor_) + logDensityGenerator;
  }
}

/* Get the PDF gradient of the distribution */
Point EllipticalDistribution::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Point u(normalize(point));
  Point iRu(u);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    iRu[i] *= std::max(SpecFunc::Precision, sigma_[i]);
  iRu = inverseCholesky_.transpose() * inverseCholesky_ * iRu;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    iRu[i] *= std::max(SpecFunc::Precision, sigma_[i]);
  const Scalar betaSquare = u.dot(iRu);
  const Scalar phi = computeDensityGenerator(betaSquare);
  const Scalar phiDerivative = computeDensityGeneratorDerivative(betaSquare);
  Point pdfGradient(2 * dimension + (dimension > 1 ? ((dimension - 1) * dimension) / 2 : 0));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (sigma_[i] > 0.0)
    {
      const Scalar iSigma = 1.0 / sigma_[i];
      // dPDF / dmu_i
      pdfGradient[2 * i] = -2.0 * normalizationFactor_ * phiDerivative * iRu[i] * iSigma;
      // dPDF / dsigma_i
      pdfGradient[2 * i + 1] = pdfGradient[2 * i] * u[i] - normalizationFactor_ * phi * iSigma;
    }
    else
    {
      pdfGradient[2 * i] = 0.0;
      pdfGradient[2 * i + 1] = 0.0;
    }
  }
  if (getDimension() > 1)
  {
    // use non-centered finite difference for correlation parameters
    const Scalar eps = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 0.5);
    const Scalar centerPDF = computePDF(point);
    Implementation cloneDistribution(clone());
    for (UnsignedInteger i = 2 * dimension; i < pdfGradient.getSize(); ++i)
    {
      Point newParameters(getParameter());
      newParameters[i] += eps;
      cloneDistribution->setParameter(newParameters);
      const Scalar rightPDF = cloneDistribution->computePDF(point);
      pdfGradient[i] = (rightPDF - centerPDF) / eps;
    }
  }
  return pdfGradient;
}

/* Get the survival function of the distribution */
Scalar EllipticalDistribution::computeSurvivalFunction(const Point & point) const
{
  return computeCDF(2.0 * mean_ - point);
}

/* Get the minimum volume level set containing at least a given probability of the distribution.
   The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
   where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
   In the case of an elliptical distribution, it is the opposite of the value of the log-PDF of the standard representative at any point at distance q from the origin, where q is the prob-quantile of the radial distribution.
*/
LevelSet EllipticalDistribution::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob,
    Scalar & threshold) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeLevelSet()";
  if (getDimension() == 1) return DistributionImplementation::computeMinimumVolumeLevelSetWithThreshold(prob, threshold);
  RadialCDFWrapper radialWrapper(this);
  const Distribution standard(getStandardDistribution());
  // First compute the log normalization factor between the distribution and its standard representative. It is made of two normalization factors:
  // 1) The elliptical one, taking into account the covariance matrix. This coefficient is equal to one for standard representatives.
  // 2) The specific one, taking into account the specific parameters (eg Normal vs Student)
  // This last coefficient is embedded into computeLogDensityGenerator.
  Scalar logThreshold = std::log(normalizationFactor_) + computeLogDensityGenerator(0) - standard.computeLogPDF(Point(dimension_));
  // Then compute the log-pdf iso-value of the level-set
  const Scalar xMax = standard.getRange().getUpperBound().norm();
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  Point point(dimension_);
  point[0] = solver.solve(radialWrapper, prob, 0.0, xMax, 0.0, 1.0);
  logThreshold += standard.computeLogPDF(point);
  // Compute the pdf threshold
  threshold = std::exp(logThreshold);
  // Finally, build the level set
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  return LevelSet(minimumVolumeLevelSetFunction, LessOrEqual(), -logThreshold);
}


CovarianceMatrix EllipticalDistribution::getShape() const
{
  CovarianceMatrix shape(R_);
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      shape(i, j) *= sigma_[i] * sigma_[j];
  return shape;
}

/* Update the derivative attributes */
void EllipticalDistribution::update()
{
  const UnsignedInteger dimension = getDimension();
  if (dimension > 1)
  {
    // Build the shape matrix
    CovarianceMatrix shape(R_);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      for (UnsignedInteger j = 0; j <= i; ++ j)
        shape(i, j) *= sigma_[i] * sigma_[j];

    // Identify non-degenerate dimensions
    Indices nonDegenerate;
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      if (sigma_[i] > 0.0) nonDegenerate.add(i);
    const UnsignedInteger effectiveDimension = nonDegenerate.getSize();

    if (effectiveDimension == dimension)
    {
      // Standard case: all dimensions are non-degenerate
      TriangularMatrix cholesky(shape.computeRegularizedCholesky());
      inverseCholesky_ = cholesky.inverse().getImplementation();
      normalizationFactor_ = 1.0;
      for (UnsignedInteger i = 0; i < dimension; ++ i) normalizationFactor_ /= cholesky(i, i);
    }
    else if (effectiveDimension == 0)
    {
      // All dimensions degenerate
      inverseCholesky_ = TriangularMatrix(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        for (UnsignedInteger j = 0; j <= i; ++ j)
          inverseCholesky_(i, j) = 0.0;
      normalizationFactor_ = 1.0;
    }
    else
    {
      // Mixed case: extract submatrix for non-degenerate dimensions
      CovarianceMatrix subShape(effectiveDimension);
      for (UnsignedInteger ii = 0; ii < effectiveDimension; ++ ii)
      {
        const UnsignedInteger i = nonDegenerate[ii];
        for (UnsignedInteger jj = 0; jj <= ii; ++ jj)
        {
          const UnsignedInteger j = nonDegenerate[jj];
          subShape(ii, jj) = sigma_[i] * R_(i, j) * sigma_[j];
        }
      }

      TriangularMatrix subCholesky(subShape.computeRegularizedCholesky());

      // Build the full Cholesky factor (lower triangular) with zeros for degenerate dims
      TriangularMatrix cholesky(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        if (sigma_[i] > 0.0)
        {
          const UnsignedInteger ii = nonDegenerate.find(i);
          for (UnsignedInteger j = 0; j <= i; ++ j)
          {
            if (sigma_[j] > 0.0)
            {
              const UnsignedInteger jj = nonDegenerate.find(j);
              cholesky(i, j) = subCholesky(ii, jj);
            }
            else
              cholesky(i, j) = 0.0;
          }
        }
        else
        {
          for (UnsignedInteger j = 0; j <= i; ++ j)
            cholesky(i, j) = 0.0;
        }
      }

      // Invert only the non-degenerate subblock, then build full inverse with zeros
      TriangularMatrix subInverse(subCholesky.inverse().getImplementation());
      inverseCholesky_ = TriangularMatrix(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        for (UnsignedInteger j = 0; j <= i; ++ j)
        {
          if ((sigma_[i] > 0.0) && (sigma_[j] > 0.0))
          {
            const UnsignedInteger ii = nonDegenerate.find(i);
            const UnsignedInteger jj = nonDegenerate.find(j);
            inverseCholesky_(i, j) = subInverse(ii, jj);
          }
          else
            inverseCholesky_(i, j) = 0.0;
        }
      }
      normalizationFactor_ = 1.0;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        if (sigma_[i] > 0.0) normalizationFactor_ /= cholesky(i, i);
    }
  } // dimension > 1
  else  // dimension 1
  {
    if (!inverseCholesky_.getDimension())  // First time we enter here, set matrix sizes
    {
      inverseCholesky_ = TriangularMatrix(1);
    }
    if (sigma_[0] > 0.0)
    {
      inverseCholesky_(0, 0) = 1.0 / sigma_[0];
      normalizationFactor_ = 1.0 / sigma_[0];
    }
    else
    {
      inverseCholesky_(0, 0) = 0.0;
      normalizationFactor_ = 1.0;
    }
  } // dimension == 1
  isAlreadyComputedMean_ = true;
}

/* Mu accessor */
void EllipticalDistribution::setMu(const Point & mu)
{
  if (mu.getDimension() != getDimension())
    throw InvalidArgumentException(HERE)
        << "Mean point dimension (" << mu.getDimension()
        << ") differ from distribution dimension(" << getDimension()
        << "). Unable to construct EllipticalDistribution distribution object.";
  mean_ = mu;
  isAlreadyComputedMean_ = true;
  computeRange();
}

Point EllipticalDistribution::getMu() const
{
  return mean_;
}

/* Mean computation */
void EllipticalDistribution::computeMean() const
{
  // Just set the flag as it may have been changed e.g if the integration nodes number has changed
  isAlreadyComputedMean_ = true;
}

/* Covariance matrix computation */
void EllipticalDistribution::computeCovariance() const
{
  // We have to extract the implementation because we know that the result
  // is a valid covariance matrix, but it cannot be inferred by the C++
  // from the operands
  covariance_ = (covarianceScalingFactor_ * getShape()).getImplementation();
  isAlreadyComputedCovariance_ = true;
}

/* Sigma accessor */
void EllipticalDistribution::setSigma(const Point & sigma)
{
  if (sigma.getDimension() != getDimension())
    throw InvalidArgumentException(HERE)
        << "Sigma dimension (" << sigma.getDimension()
        << ") differ from distribution dimension(" << getDimension()
        << "). Unable to construct elliptical distribution object.";

  // We check that the marginal standard deviations are >= 0
  for(UnsignedInteger i = 0; i < sigma.getDimension(); ++i)
    if (!(sigma[i] >= 0.0)) throw InvalidArgumentException(HERE) << "The marginal standard deviations must be >= 0 sigma=" << sigma[i];
  sigma_ = sigma;
  update();
  computeRange();
  isAlreadyComputedCovariance_ = false;
}

/* Sigma accessor */
Point EllipticalDistribution::getSigma() const
{
  return sigma_;
}

/* Get the standard deviation of the distribution.
   Warning! This method MUST be overloaded for elliptical distributions without finite second moment:
   it is possible to have a well-defined sigma vector but no standard deviation, think about Student
   distribution with nu < 2 */
Point EllipticalDistribution::getStandardDeviation() const
{
  return std::sqrt(covarianceScalingFactor_) * sigma_;
}

/* Correlation matrix accessor */
void EllipticalDistribution::setR(const CorrelationMatrix & R)
{
  if (R.getDimension() != getDimension())
    throw InvalidArgumentException(HERE)
        << "Correlation Matrix dimension (" << R.getDimension()
        << ") differ from distribution dimension(" << getDimension()
        << "). Unable to construct elliptical distribution object.";

  R_ = R;
  update();
  isAlreadyComputedCovariance_ = false;
}

/* Correlation matrix accessor */
CorrelationMatrix EllipticalDistribution::getR() const
{
  return R_;
}

/* Cholesky factor of the correlation matrix accessor */
TriangularMatrix EllipticalDistribution::getCholesky() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1)
  {
    TriangularMatrix cholesky(1);
    cholesky(0, 0) = sigma_[0];
    return cholesky;
  }
  // Check for degenerate dimensions
  Indices nonDegenerate;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    if (sigma_[i] > 0.0) nonDegenerate.add(i);
  const UnsignedInteger effectiveDimension = nonDegenerate.getSize();

  if (effectiveDimension == dimension)
  {
    // Standard case: all dimensions are non-degenerate
    return getShape().computeRegularizedCholesky();
  }

  CovarianceMatrix shape(getShape());

  if (effectiveDimension == 0)
  {
    // All degenerate
    TriangularMatrix cholesky(dimension);
    return cholesky;
  }

  // Mixed case: extract submatrix for non-degenerate dimensions
  CovarianceMatrix subShape(effectiveDimension);
  for (UnsignedInteger ii = 0; ii < effectiveDimension; ++ ii)
  {
    const UnsignedInteger i = nonDegenerate[ii];
    for (UnsignedInteger jj = 0; jj <= ii; ++ jj)
    {
      const UnsignedInteger j = nonDegenerate[jj];
      subShape(ii, jj) = shape(i, j);
    }
  }

  TriangularMatrix subCholesky(subShape.computeRegularizedCholesky());

  // Build full Cholesky with zeros for degenerate dimensions
  TriangularMatrix cholesky(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    if (sigma_[i] > 0.0)
    {
      const UnsignedInteger ii = nonDegenerate.find(i);
      for (UnsignedInteger j = 0; j <= i; ++ j)
      {
        if (sigma_[j] > 0.0)
        {
          const UnsignedInteger jj = nonDegenerate.find(j);
          cholesky(i, j) = subCholesky(ii, jj);
        }
      }
    }
  }
  return cholesky;
}

/* Inverse of the Cholesky factor of the correlation matrix accessor */
TriangularMatrix EllipticalDistribution::getInverseCholesky() const
{
  return inverseCholesky_;
}

/* Virtual copy constructor */
EllipticalDistribution * EllipticalDistribution::clone() const
{
  return new EllipticalDistribution(*this);
}

/* Get the isoprobabilist transformation */
EllipticalDistribution::IsoProbabilisticTransformation EllipticalDistribution::getIsoProbabilisticTransformation() const
{
  IsoProbabilisticTransformation transform;
  transform.setEvaluation(new NatafEllipticalDistributionEvaluation(mean_, inverseCholesky_));
  transform.setGradient(new NatafEllipticalDistributionGradient(inverseCholesky_));
  transform.setHessian(new NatafEllipticalDistributionHessian(getDimension()));
  // Set the parameters values and descriptions
  // The result of parameterGradient is given
  // in the following form:
  // (d/dmu, d/dsigma)
  // There is no gradient according to the dependence parameters yet (28/10/2006)
  const UnsignedInteger dimension = getDimension();
  Point parameters(2 * dimension);
  Description description(parameters.getDimension());
  PointWithDescriptionCollection parametersCollection(getParametersCollection());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Description parametersDescription(parametersCollection[i].getDescription());
    const String marginalName(parametersCollection[i].getName());
    parameters[i] = parametersCollection[i][0];
    parameters[dimension + i] = parametersCollection[i][1];
    description[i] = OSS() << marginalName << "_" << parametersDescription[0];
    description[dimension + i] = OSS() << marginalName << "_" << parametersDescription[1];
  }
  transform.setParameter(parameters);
  transform.setParameterDescription(description);
  return transform;
}

/* Get the inverse isoprobabilist transformation */
EllipticalDistribution::InverseIsoProbabilisticTransformation EllipticalDistribution::getInverseIsoProbabilisticTransformation() const
{
  InverseIsoProbabilisticTransformation inverseTransform;
  const TriangularMatrix cholesky(getCholesky());
  inverseTransform.setEvaluation(new InverseNatafEllipticalDistributionEvaluation(mean_, cholesky));
  inverseTransform.setGradient(new InverseNatafEllipticalDistributionGradient(cholesky));
  inverseTransform.setHessian(new InverseNatafEllipticalDistributionHessian(getDimension()));
  // Set the parameters values and descriptions
  // The result of parameterGradient is given
  // in the following form:
  // (d/dmu, d/dsigma)
  // There is no gradient according to the dependence parameters yet (28/10/2006)
  const UnsignedInteger dimension = getDimension();
  Point parameters(2 * dimension);
  Description description(parameters.getDimension());
  PointWithDescriptionCollection parametersCollection(getParametersCollection());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Description parametersDescription(parametersCollection[i].getDescription());
    const String marginalName(parametersCollection[i].getName());
    parameters[i] = parametersCollection[i][0];
    parameters[dimension + i] = parametersCollection[i][1];
    description[i] = OSS() << marginalName << "_" << parametersDescription[0];
    description[dimension + i] = OSS() << marginalName << "_" << parametersDescription[1];
  }
  inverseTransform.setParameter(parameters);
  inverseTransform.setParameterDescription(description);
  return inverseTransform;
}

/* Get the standard distribution, i.e. a distribution of the same kind but with zero mean,
 * unit marginal standard distribution and identity correlation */
Distribution EllipticalDistribution::getStandardDistribution() const
{
  EllipticalDistribution * p_standardDistribution(clone());
  const UnsignedInteger dimension = getDimension();
  p_standardDistribution->setMu(Point(dimension, 0.0));
  p_standardDistribution->setSigma(Point(dimension, 1.0));
  p_standardDistribution->setR(CorrelationMatrix(dimension));
  return p_standardDistribution;
}

/* Parameters value and description accessor */
EllipticalDistribution::PointWithDescriptionCollection EllipticalDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension + (dimension > 1 ? 1 : 0));
  // First put the marginal parameters
  const Description description(getDescription());
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    PointWithDescription point(2);
    Description marginalDescription(point.getDimension());
    point[0] = mean_[marginalIndex];
    point[1] = sigma_[marginalIndex];
    marginalDescription[0] = OSS() << "mu_" << marginalIndex;
    marginalDescription[1] = OSS() << "sigma_" << marginalIndex;
    point.setDescription(marginalDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  if (dimension > 1)
  {
    // Second put the dependence parameters
    PointWithDescription point(dimension * (dimension - 1) / 2);
    Description dependenceDescription(point.getDimension());
    point.setName("dependence");
    UnsignedInteger dependenceIndex = 0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        point[dependenceIndex] = R_(i, j);
        dependenceDescription[dependenceIndex] = OSS() << "R_" << i << "_" << j;
        ++dependenceIndex;
      }
    }
    point.setDescription(dependenceDescription);
    parameters[dimension] = point;
  } // dimension > 1
  return parameters;
} // getParametersCollection

void EllipticalDistribution::setParametersCollection(const PointCollection & parametersCollection)
{
  const UnsignedInteger size = parametersCollection.getSize();
  const UnsignedInteger dimension = size > 1 ? size - 1 : size;
  setDimension(dimension);
  mean_ = Point(dimension);
  sigma_ = Point(dimension);
  CorrelationMatrix R(dimension);
  if (dimension == 1)
  {
    mean_[0] = parametersCollection[0][0];
    sigma_[0] = parametersCollection[0][1];
    if (!(sigma_[0] >= 0.0)) throw InvalidArgumentException(HERE) << "The marginal standard deviations must be >= 0 sigma=" << sigma_[0];
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      mean_[i] = parametersCollection[i][0];
      sigma_[i] = parametersCollection[i][1];
      if (!(sigma_[i] >= 0.0)) throw InvalidArgumentException(HERE) << "The marginal standard deviations must be >= 0 sigma=" << sigma_[i];
    }
    UnsignedInteger parameterIndex = 0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        R(i, j) = parametersCollection[size - 1][parameterIndex];
        ++parameterIndex;
      }
    }
    setR(R);
  }
  update();
  computeRange();
  isAlreadyComputedCovariance_ = false;
}

Point EllipticalDistribution::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  Point point(2 * dimension + (dimension > 1 ? ((dimension - 1)*dimension) / 2 : 0));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    point[2 * i] = mean_[i];
    point[2 * i + 1] = sigma_[i];
  }
  UnsignedInteger index = 2 * dimension;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      point[index] = R_(i, j);
      ++ index;
    }
  }
  return point;
}

void EllipticalDistribution::setParameter(const Point & parameters)
{
  // N = 2*d+((d-1)*d)/2
  const UnsignedInteger size = parameters.getSize();
  Scalar dimReal = 0.5 * std::sqrt(9.0 + 8.0 * size) - 1.5;
  if (dimReal != round(dimReal)) throw InvalidArgumentException(HERE) << "Error: invalid parameter number for EllipticalDistribution";
  const UnsignedInteger dimension = dimReal;
  Point mean(dimension);
  Point sigma(dimension);
  CorrelationMatrix R(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    mean[i] = parameters[2 * i];
    sigma[i] = parameters[2 * i + 1];
    if (!(sigma[i] >= 0.0))
      throw InvalidArgumentException(HERE) << "The marginal standard deviations must be >= 0 sigma=" << sigma[i];
  }
  if (dimension > 1)
  {
    UnsignedInteger index = 2 * dimension;
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      for (UnsignedInteger j = 0; j < i; ++ j)
      {
        R(i, j) = parameters[index];
        ++ index;
      }
    }
  }
  // commit all changes at once
  mean_ = mean;
  sigma_ = sigma;
  R_ = R;
  update();
  computeRange();
  isAlreadyComputedCovariance_ = false;
}

Description EllipticalDistribution::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description(2 * dimension + (dimension > 1 ? ((dimension - 1)*dimension) / 2 : 0));
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    description[2 * i] = OSS() << "mu_" << i;
    description[2 * i + 1] = OSS() << "sigma_" << i;
  }
  UnsignedInteger index = 2 * dimension;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      description[index] = OSS() << "R_" << i << "_" << j;
      ++ index;
    }
  }
  return description;
}


/* Method save() stores the object through the StorageManager */
void EllipticalDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "R_", R_ );
  adv.saveAttribute( "sigma_", sigma_ );
  adv.saveAttribute( "inverseCholesky_", inverseCholesky_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void EllipticalDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "R_", R_ );
  adv.loadAttribute( "sigma_", sigma_ );
  adv.loadAttribute( "inverseCholesky_", inverseCholesky_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
}

END_NAMESPACE_OPENTURNS
