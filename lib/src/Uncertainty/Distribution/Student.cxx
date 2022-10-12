//                                               -*- C++ -*-
/**
 *  @brief The Student distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>
#include <cmath>

#include "openturns/Student.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Student)

static const Factory<Student> Factory_Student;

/* Default constructor */
Student::Student(const Scalar nu,
                 const UnsignedInteger dimension)
  : EllipticalDistribution(Point(dimension, 0.0),
                           Point(dimension, 1.0),
                           CorrelationMatrix(dimension), -1.0)
  , nu_(0.0)
  , studentNormalizationFactor_(0.0)
{
  setName("Student");
  setDimension( dimension );
  // This call set also the range
  setNu(nu);
}

/* Constructor for 1D Student distribution */
Student::Student(const Scalar nu,
                 const Scalar mu,
                 const Scalar sigma)
  : EllipticalDistribution(Point(1, mu), Point(1, sigma), CorrelationMatrix(1), -1.0)
  , nu_(0.0)
  , studentNormalizationFactor_(0.0)
{
  setName("Student");
  setDimension(1);
  // Set nu with checks. This call set also the range.
  setNu(nu);
}

/* Constructor for multiD Student distribution */
Student::Student(const Scalar nu,
                 const Point & mu,
                 const Point & sigma,
                 const CorrelationMatrix & R)
  : EllipticalDistribution(mu, sigma, R, -1.0)
  , nu_(0.0)
  , studentNormalizationFactor_(0.0)
{
  setName("Student");
  setDimension(mu.getDimension());
  // Set nu with checks. This call set also the range.
  setNu(nu);
}

Student::Student(const Scalar nu,
                 const Point & mu,
                 const Point & sigma)
  : Student(nu, mu, sigma, CorrelationMatrix(mu.getDimension()))
{
  // Nothing to do
}

/* Comparison operator */
Bool Student::operator ==(const Student & other) const
{
  if (this == &other) return true;
  return (nu_ == other.nu_) && EllipticalDistribution::equals(other);
}

Bool Student::equals(const DistributionImplementation & other) const
{
  const Student* p_other = dynamic_cast<const Student*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Student::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Student::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " nu=" << nu_
      << " mean=" << mean_
      << " sigma=" << sigma_
      << " correlationMatrix=" << R_;
  return oss;
}

String Student::__str__(const String & offset) const
{
  OSS oss(false);
  oss << getClassName();
  if (getDimension() == 1) oss << "(nu = " << nu_ << ", mu = " << getMu() << ", sigma = " << getSigma()[0] << ")";
  else oss << "(nu = " << nu_ << ", mu = " << mean_.__str__() << ", sigma = " << getSigma().__str__() << ", R = " << getCorrelation().__str__(offset) << ")";
  return oss;
}

/* Compute the density generator of the elliptical generator, i.e.
 *  the function phi such that the density of the distribution can
 *  be written as p(x) = phi(t(x-mu)S^(-1)(x-mu))                      */
Scalar Student::computeDensityGenerator(const Scalar betaSquare) const
{
  return std::exp(studentNormalizationFactor_ - 0.5 * (nu_ + getDimension()) * log1p(betaSquare / nu_));
}

Scalar Student::computeLogDensityGenerator(const Scalar betaSquare) const
{
  return studentNormalizationFactor_ - 0.5 * (nu_ + getDimension()) * log1p(betaSquare / nu_);
}

/* Compute the derivative of the density generator */
Scalar Student::computeDensityGeneratorDerivative(const Scalar betaSquare) const
{
  const Scalar iNu = 1.0 / nu_;
  const UnsignedInteger dimension = getDimension();
  return -0.5 * std::exp(studentNormalizationFactor_ - (0.5 * (nu_ + dimension) + 1.0) * log1p(betaSquare * iNu)) * (1.0 + dimension * iNu);
}

/* Compute the second derivative of the density generator */
Scalar Student::computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const
{
  const Scalar iNu = 1.0 / nu_;
  const UnsignedInteger dimension = getDimension();
  return 0.25 * std::exp(studentNormalizationFactor_ - (0.5 * (nu_ + dimension) + 2.0) * log1p(betaSquare * iNu)) * (1.0 + dimension * iNu) * (1.0 + (dimension + 2.0) * iNu);
}


/* Virtual constructor */
Student * Student::clone() const
{
  return new Student(*this);
}

/* Get one realization of the distribution */
Point Student::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return Point(1, mean_[0] + sigma_[0] * DistFunc::rStudent(nu_));
  Point value(dimension);
  // First, a realization of independent standard normal coordinates
  for (UnsignedInteger i = 0; i < dimension; ++i) value[i] = DistFunc::rNormal();
  return std::sqrt(0.5 * nu_ / DistFunc::rGamma(0.5 * nu_)) * (cholesky_ * value) + mean_;
}


Sample Student::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  Sample normalSample(size, dimension);
  Point gammaDeviates(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
      normalSample(i, j) = DistFunc::rNormal();
    gammaDeviates[i] = DistFunc::rGamma(0.5 * nu_);
  }
  Sample result;
  if (dimension == 1)
    result = normalSample * sigma_[0];
  else
    result = (cholesky_.getImplementation()->genSampleProd(normalSample, true, false, 'R'));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar alpha = std::sqrt(0.5 * nu_ / gammaDeviates[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      result(i, j) = result(i, j) * alpha + mean_[j];
  }
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}


/* Get the CDF of the distribution */
Scalar Student::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has a dimension incompatible with the distribution.";
  // Special case for dimension 1
  if (dimension == 1) return DistFunc::pStudent(nu_, (point[0] - mean_[0]) / sigma_[0]);
  // For moderate dimension, use a Gauss-Legendre integration
  if (dimension <= ResourceMap::GetAsUnsignedInteger("Student-SmallDimension"))
  {
    // Reduce the default integration point number for CDF computation in the range 3 < dimension <= Student-SmallDimension
    const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ), 1.0 / getDimension())));
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber" );
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of PDF evaluations. It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"Student-MaximumNumberOfPoints\"");
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    return ContinuousDistribution::computeCDF(point);
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Student::computeCDF(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10 * ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for small values of the CDF...");
  RandomGeneratorState initialState(RandomGenerator::GetState());
  RandomGenerator::SetSeed(ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" ));
  Scalar value = 0.0;
  Scalar variance = 0.0;
  Scalar a99 = DistFunc::qNormal(0.995);
  const UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" );
  UnsignedInteger outerMax = 10 * ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ) / blockSize;
  Scalar precision = 0.0;
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    const Sample sample(getSample(blockSize));
    LOGDEBUG(OSS(false) << "indexOuter=" << indexOuter << ", point=" << point << ", sample=" << sample);
    const Scalar valueBlock = sample.computeEmpiricalCDF(point);
    const Scalar varianceBlock = valueBlock * (1.0 - valueBlock) / blockSize;
    LOGDEBUG(OSS(false) << "valueBlock=" << valueBlock << ", varianceBlock=" << varianceBlock);
    const Scalar norm = 1.0 / (indexOuter + 1.0);
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    LOGDEBUG(OSS(false) << "value=" << value << ", variance=" << variance);
    // Quick return for value = 1
    if ((value >= 1.0 - ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsScalar( "Student-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  RandomGenerator::SetState(initialState);
  return value;
} // computeCDF

Sample Student::computeCDF(const Sample & sample) const
{
  if (dimension_ <= ResourceMap::GetAsUnsignedInteger("Student-SmallDimension"))
    return DistributionImplementation::computeCDFParallel(sample);
  return DistributionImplementation::computeCDFSequential(sample);
}

/* Compute the probability content of an interval */
Scalar Student::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  if (interval.isEmpty()) return 0.0;
  // The generic implementation provided by the DistributionImplementation upper class is more accurate than the generic implementation provided by the ContinuousDistribution upper class for dimension = 1
  if (dimension == 1) return DistributionImplementation::computeProbability(interval);
  // Decompose and normalize the interval
  Point lower(normalize(interval.getLowerBound()));
  Point upper(normalize(interval.getUpperBound()));
  const Interval::BoolCollection finiteLower(interval.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpper(interval.getFiniteUpperBound());
  /* General case */
  // For moderate dimension, use a Gauss-Legendre integration
  if (dimension <= ResourceMap::GetAsUnsignedInteger("Student-SmallDimension"))
  {
    // Reduce the default integration point number for CDF computation in the range 3 < dimension <= Student-SmallDimension
    const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ), 1.0 / getDimension())));
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber" );
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of PDF evaluations. It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"Student-MaximumNumberOfPoints\"");
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    return ContinuousDistribution::computeProbability(interval);
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Student::computeProbability(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10.0 * ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for low values of the CDF...");
  RandomGeneratorState initialState(RandomGenerator::GetState());
  RandomGenerator::SetSeed(ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" ));
  Scalar value = 0.0;
  Scalar variance = 0.0;
  const Scalar a99 = DistFunc::qNormal(0.995);
  UnsignedInteger outerMax = 10 * ResourceMap::GetAsUnsignedInteger( "Student-MaximumNumberOfPoints" ) / ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" );
  Scalar precision = 0.0;
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    Scalar valueBlock = 0.0;
    Scalar varianceBlock = 0.0;
    for (UnsignedInteger indexSample = 0; indexSample < ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" ); ++indexSample)
    {
      // ind value is 1.0 if the realization is inside of the integration domain, 0.0 else.
      Scalar ind = interval.numericallyContains(getRealization());
      Scalar norm = 1.0 / (indexSample + 1.0);
      varianceBlock = (varianceBlock * indexSample + (1.0 - norm) * (valueBlock - ind) * (valueBlock - ind)) * norm;
      valueBlock = (valueBlock * indexSample + ind) * norm;
    }
    Scalar norm = 1.0 / (indexOuter + 1.0);
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    // Quick return for value = 1
    const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
    if ((value >= 1.0 - quantileEpsilon) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Student-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsScalar( "Student-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  RandomGenerator::SetState(initialState);
  return value;
}

/* Compute the entropy of the distribution */
Scalar Student::computeEntropy() const
{
  const UnsignedInteger dimension = getDimension();
  // normalizationFactor_ == 1/\sqrt{|Det(\Sigma)|}
  // studentNormalizationFactor_ = SpecFunc::LnGamma(0.5 * (nu + dimension)) - SpecFunc::LnGamma(0.5 * nu) - 0.5 * dimension * std::log(nu * M_PI);
  return 0.5 * (nu_ + dimension) * (SpecFunc::Psi(0.5 * (nu_ + dimension)) - SpecFunc::Psi(0.5 * nu_)) - std::log(normalizationFactor_) - studentNormalizationFactor_;
}

/* Get the PDFGradient of the distribution */
Point Student::computePDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();

  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point pdfGradient(2 * dimension + 1, 0.0);
  if (dimension == 1)
  {
    const Point ellipticalPDFGradient(EllipticalDistribution::computePDFGradient(point));
    const Scalar epsNu = 1e-3;
    pdfGradient[0] = (Student(nu_ + epsNu, mean_, sigma_, R_).computePDF(point) - Student(nu_ - epsNu, mean_, sigma_, R_).computePDF(point)) / (2.0 * epsNu);
    for (UnsignedInteger i = 0; i < 2 * dimension; ++i) pdfGradient[i + 1] = ellipticalPDFGradient[i];
    return pdfGradient;
  }
  else throw NotYetImplementedException(HERE) << "In Student::computePDFGradient(const Point & point) const";
}

/* Get the CDFGradient of the distribution */
Point Student::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();

  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1)
  {
    Point cdfGradient(3, 0.0);
    const Scalar x = point[0] - mean_[0];
    const Scalar eps = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
    const Scalar i2Eps = 0.5 / eps;
    cdfGradient[0] = (DistFunc::pStudent(nu_ + eps, x / sigma_[0]) - DistFunc::pStudent(nu_ - eps, x / sigma_[0])) * i2Eps;
    // Opposite sign for eps because x - eps = point[0] - (mu + eps)
    cdfGradient[1] = (DistFunc::pStudent(nu_, (x - eps) / sigma_[0]) - DistFunc::pStudent(nu_, (x + eps) / sigma_[0])) * i2Eps;
    cdfGradient[2] = (DistFunc::pStudent(nu_, x / (sigma_[0] + eps)) - DistFunc::pStudent(nu_, x / (sigma_[0] - eps))) * i2Eps;
    return cdfGradient;
  }
  else throw NotYetImplementedException(HERE) << "In Student::computeCDFGradient(const Point & point) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
   For Student distribution, the conditional distribution is also Student, with mean and covariance
   such as:
   mean_cond = mean(x) + cov(x, y).cov(y, y)^(-1)(y - mean(y))
   cov_cond = cov(x, x) - cov(x, y).cov(y, y)^(-1)cov(x, y)
   This expression simplifies if we use the inverse of the Cholesky factor of the covariance matrix.
   See [Lebrun, Dutfoy, "Rosenblatt and Nataf transformation"]
   The number of degrees of freedom is modified according to nu_cond = nu + dim(cond)
*/
Scalar Student::computeConditionalPDF(const Scalar x,
                                      const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (conditioningDimension == 0)
  {
    const Scalar z = (x - mean_[conditioningDimension]) / sigma_[conditioningDimension];
    return std::exp(-0.5 * (nu_ + 1.0) * log1p(z * z / nu_) - SpecFunc::LogBeta(0.5, 0.5 * nu_)) / sigma_[conditioningDimension] / std::sqrt(nu_);
  }
  // General case
  // Extract the Cholesky factor of the covariance of Y
  MatrixImplementation cholY(conditioningDimension, conditioningDimension);
  UnsignedInteger start = 0;
  UnsignedInteger stop = conditioningDimension;
  UnsignedInteger shift = 0;
  Point yCentered(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    yCentered[i] = y[i] - mean_[i];
    std::copy(cholesky_.getImplementation()->begin() + start, cholesky_.getImplementation()->begin() + stop, cholY.begin() + shift);
    start += dimension_ + 1;
    stop += dimension_;
    shift += conditioningDimension + 1;
  }
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  const Scalar nuCond = nu_ + conditioningDimension;
  Scalar sigmaCond = std::sqrt((nu_ + Point(cholY.solveLinearSystemTri(yCentered)).normSquare()) / nuCond) * sigmaRos;
  Scalar meanRos = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
    meanRos += inverseCholesky_(conditioningDimension, i) * yCentered[i] / std::sqrt(sigma_[i]);
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  const Scalar z = (x - meanRos) / sigmaCond;
  Scalar value = std::exp(-0.5 * (nuCond + 1.0) * log1p(z * z / nuCond) - SpecFunc::LogBeta(0.5, 0.5 * nuCond)) / sigmaCond / std::sqrt(nuCond);
  return value;
}

Point Student::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  // Waiting for a better implementation
  Point y(0);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar xI = x[i];
    result[i] = computeConditionalPDF(xI, y);
    y.add(xI);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Student::computeConditionalCDF(const Scalar x,
                                      const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (conditioningDimension == 0)
    return DistFunc::pStudent(nu_, (x - mean_[conditioningDimension]) / sigma_[conditioningDimension]);
  // General case
  // Extract the Cholesky factor of the covariance of Y
  MatrixImplementation cholY(conditioningDimension, conditioningDimension);
  UnsignedInteger start = 0;
  UnsignedInteger stop = conditioningDimension;
  UnsignedInteger shift = 0;
  Point yCentered(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    yCentered[i] = y[i] - mean_[i];
    std::copy(cholesky_.getImplementation()->begin() + start, cholesky_.getImplementation()->begin() + stop, cholY.begin() + shift);
    start += dimension_ + 1;
    stop += dimension_;
    shift += conditioningDimension + 1;
  }
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  const Scalar nuCond = nu_ + conditioningDimension;
  Scalar sigmaCond = std::sqrt((nu_ + Point(cholY.solveLinearSystemTri(yCentered)).normSquare()) / nuCond) * sigmaRos;
  Scalar meanRos = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
    meanRos += inverseCholesky_(conditioningDimension, i) * yCentered[i] / std::sqrt(sigma_[i]);
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return DistFunc::pStudent(nuCond, (x - meanRos) / sigmaCond);
}

Point Student::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  // Waiting for a better implementation
  Point y(0);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar xI = x[i];
    result[i] = computeConditionalCDF(xI, y);
    y.add(xI);
  }
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Student::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case when no contitioning or independent copula
  if (conditioningDimension == 0) return mean_[0] + sigma_[0] * DistFunc::qStudent(nu_, q);
  // General case
  // Extract the Cholesky factor of the covariance of Y
  MatrixImplementation cholY(conditioningDimension, conditioningDimension);
  UnsignedInteger start = 0;
  UnsignedInteger stop = conditioningDimension;
  UnsignedInteger shift = 0;
  Point yCentered(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    yCentered[i] = y[i] - mean_[i];
    std::copy(cholesky_.getImplementation()->begin() + start, cholesky_.getImplementation()->begin() + stop, cholY.begin() + shift);
    start += dimension_ + 1;
    stop += dimension_;
    shift += conditioningDimension + 1;
  }
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  const Scalar nuCond = nu_ + conditioningDimension;
  Scalar sigmaCond = std::sqrt((nu_ + Point(cholY.solveLinearSystemTri(yCentered)).normSquare()) / nuCond) * sigmaRos;
  Scalar meanRos = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
    meanRos += inverseCholesky_(conditioningDimension, i) * yCentered[i] / std::sqrt(sigma_[i]);
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return meanRos + sigmaCond * DistFunc::qStudent(nuCond, q);
}

Point Student::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  // Waiting for a better implementation
  Point y(0);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar qI = q[i];
    result[i] = computeConditionalQuantile(qI, y);
    y.add(result[i]);
  }
  return result;
}

/* Get the i-th marginal distribution */
Distribution Student::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (getDimension() == 1) return clone();
  // General case
  const CorrelationMatrix R(1);
  const Point sigma(1, sigma_[i]);
  const Point mean(1, mean_[i]);
  Student::Implementation marginal(new Student(nu_, mean, sigma, R));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Student::getMarginal(const Indices & indices) const
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
    for (UnsignedInteger j = 0; j <= i; ++ j)
      R(i, j) = R_(index_i, indices[j]);
  }
  Student::Implementation marginal(new Student(nu_, mean_.select(indices), sigma_.select(indices), R));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
}

/* Compute the radial distribution CDF */
Scalar Student::computeRadialDistributionCDF(const Scalar radius,
    const Bool tail) const
{
  const Scalar r2 = radius * radius;
  return DistFunc::pBeta(0.5 * getDimension(), 0.5 * nu_, r2 / (nu_ + r2), tail);
}

/* Mu accessor */
void Student::setMu(const Scalar mu)
{
  if (getDimension() == 1) mean_ = Point(1, mu);
  computeRange();
}

Scalar Student::getMu() const
{
  if (getDimension() == 1) return mean_[0];
  throw InvalidArgumentException(HERE) << "Error: cannot call this method if dimension > 1.";
}

/* Get the mean of the distribution */
Point Student::getMean() const
{
  if (!(nu_ > 1.0)) throw NotDefinedException(HERE) << "Student mean is defined only for nu > 1, here nu=" << nu_;
  return EllipticalDistribution::getMean();
}

/* Get the standard deviation of the distribution */
Point Student::getStandardDeviation() const
{
  if (!(nu_ > 2.0)) throw NotDefinedException(HERE) << "Student standard deviation is defined only for nu > 2, here nu=" << nu_;
  return EllipticalDistribution::getStandardDeviation();
}

/* Get the skewness of the distribution */
Point Student::getSkewness() const
{
  if (!(nu_ > 3.0)) throw NotDefinedException(HERE) << "Student skewness is defined only for nu > 3, here nu=" << nu_;
  return Point(getDimension(), 0.0);
}

/* Get the kurtosis of the distribution */
Point Student::getKurtosis() const
{
  if (!(nu_ > 4.0)) throw NotDefinedException(HERE) << "Student kurtosis is defined only for nu > 4, here nu=" << nu_;
  return Point(getDimension(), 3.0 + 6.0 / (nu_ - 4.0));
}

/* Get the covariance of the distribution */
CovarianceMatrix Student::getCovariance() const
{
  if (!(nu_ > 2.0)) throw NotDefinedException(HERE) << "Student covariance is defined only for nu > 2, here nu=" << nu_;
  return EllipticalDistribution::getCovariance();
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Student::getStandardRepresentative() const
{
  return new Student(nu_);
}

/* Parameters value and description accessor */
Student::PointWithDescriptionCollection Student::getParametersCollection() const
{
  // First, get the parameters of the underlying elliptical distribution, it means mu, sigma and R
  PointWithDescriptionCollection parameters(EllipticalDistribution::getParametersCollection());
  // We get a collection of PointWithDescription, we append the value of nu at the beginning of each PointWithDescription
  const UnsignedInteger size = parameters.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const PointWithDescription ellipticalParameterI(parameters[i]);
    const Description ellipticalDescriptionI(ellipticalParameterI.getDescription());
    const UnsignedInteger ellipticalParameterIDimension = ellipticalParameterI.getDimension();
    PointWithDescription parameterI(ellipticalParameterIDimension + 1);
    Description descriptionI(ellipticalParameterIDimension + 1);
    parameterI[0] = nu_;
    descriptionI[0] = "nu";
    for (UnsignedInteger j = 0; j < ellipticalParameterIDimension; ++j)
    {
      parameterI[j + 1] = ellipticalParameterI[j];
      descriptionI[j + 1] = ellipticalDescriptionI[j];
    }
    parameterI.setDescription(descriptionI);
    parameters[i] = parameterI;
  }
  return parameters;
}

void Student::setParametersCollection(const PointCollection & parametersCollection)
{
  const Scalar w = getWeight();
  const UnsignedInteger size = parametersCollection.getSize();
  const UnsignedInteger dimension = size > 1 ? size - 1 : size;
  if (dimension == 1) *this = Student(parametersCollection[0][0], parametersCollection[0][1], parametersCollection[0][2]);
  else
  {
    const Scalar nu = parametersCollection[0][0];
    Point mean(dimension);
    Point sigma(dimension);
    CorrelationMatrix R(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      mean[i] = parametersCollection[i][1];
      sigma[i] = parametersCollection[i][2];
    }
    UnsignedInteger parameterIndex = 1;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        R(i, j) = parametersCollection[size - 1][parameterIndex];
        ++parameterIndex;
      }
    }
    *this = Student(nu, mean, sigma, R);
  }
  setWeight(w);
}


/* Parameters value accessor */
Point Student::getParameter() const
{
  Point point(1, nu_);
  point.add(EllipticalDistribution::getParameter());
  return point;
}

void Student::setParameter(const Point & parameter)
{
  // N = 2*d+((d-1)*d)/2+1
  const UnsignedInteger size = parameter.getSize();
  Scalar dimReal = 0.5 * std::sqrt(1.0 + 8.0 * size) - 1.5;
  if (dimReal != round(dimReal)) throw InvalidArgumentException(HERE) << "Error: invalid parameter number for Student";
  const UnsignedInteger dimension = dimReal;
  const Scalar nu = parameter[0];

  if (dimension == 1)
  {
    *this = Student(nu, parameter[1], parameter[2]);
  }
  else
  {
    Point mean(dimension);
    Point sigma(dimension);
    CorrelationMatrix R(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      mean[i] = parameter[2 * i + 1];
      sigma[i] = parameter[2 * i + 2];
    }
    UnsignedInteger parameterIndex = 2 * dimension + 1;
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      for (UnsignedInteger j = 0; j < i; ++ j)
      {
        R(i, j) = parameter[parameterIndex];
        ++ parameterIndex;
      }
    }
    *this = Student(nu, mean, sigma, R);
  }
}

/* Parameters description accessor */
Description Student::getParameterDescription() const
{
  Description description(1, "nu");
  description.add(EllipticalDistribution::getParameterDescription());
  return description;
}

/* Nu accessor */
void Student::setNu(const Scalar nu)
{
  if (nu <= 2.0) LOGWARN(OSS() << "Warning! As nu <= 2, the covariance of the distribution will not be defined");
  const UnsignedInteger dimension = getDimension();
  nu_ = nu;
  // Only set the covarianceScalingFactor if nu > 0, else its value is -1.0
  if (nu > 2.0) covarianceScalingFactor_ = nu_ / (nu_ - 2.0);
  studentNormalizationFactor_ = SpecFunc::LnGamma(0.5 * (nu + dimension)) - SpecFunc::LnGamma(0.5 * nu) - 0.5 * dimension * std::log(nu * M_PI);
  computeRange();
}

/* Tell if the distribution has independent copula */
Bool Student::hasIndependentCopula() const
{
  // A multivariate Student distribution never has an independent copula
  return getDimension() == 1;
}

/* Nu accessor */
Scalar Student::getNu() const
{
  return nu_;
}

/* Quantile computation for dimension=1 */
Scalar Student::computeScalarQuantile(const Scalar prob,
                                      const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  return mean_[0] + sigma_[0] * DistFunc::qStudent(nu_, prob, tail);
}

/* Method save() stores the object through the StorageManager */
void Student::save(Advocate & adv) const
{
  EllipticalDistribution::save(adv);
  adv.saveAttribute( "nu_", nu_ );
  adv.saveAttribute( "studentNormalizationFactor_", studentNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Student::load(Advocate & adv)
{
  EllipticalDistribution::load(adv);
  adv.loadAttribute( "nu_", nu_ );
  adv.loadAttribute( "studentNormalizationFactor_", studentNormalizationFactor_ );
  computeRange();
}




END_NAMESPACE_OPENTURNS
