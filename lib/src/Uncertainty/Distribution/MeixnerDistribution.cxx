//                                               -*- C++ -*-
/**
 *  @brief The MeixnerDistribution distribution
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
#include "openturns/MeixnerDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TNC.hxx"
#include "openturns/Brent.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeixnerDistribution);

static const Factory<MeixnerDistribution> Factory_MeixnerDistribution;

/* Default constructor */
MeixnerDistribution::MeixnerDistribution()
  : ContinuousDistribution()
  , solver_(new TNC())
  , alpha_(0.0)
  , beta_(0.0)
  , delta_(0.0)
  , mu_(0.0)
  , logNormalizationFactor_(0.0)
{
  setName("MeixnerDistribution");
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeOptimizationSolverParameter();
  setAlphaBetaDelta(1.0, 0.0, 1.0);
  setDimension(1);
}

/* Parameters constructor */
MeixnerDistribution::MeixnerDistribution(const NumericalScalar alpha,
    const NumericalScalar beta,
    const NumericalScalar delta,
    const NumericalScalar mu)
  : ContinuousDistribution()
  , solver_(new TNC())
  , alpha_(0.0)
  , beta_(0.0)
  , delta_(0.0)
  , mu_(mu)
  , logNormalizationFactor_(0.0)
{
  setName("MeixnerDistribution");
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeOptimizationSolverParameter();
  setAlphaBetaDelta(alpha, beta, delta);
  setDimension(1);
}

/* Initialize optimization solver parameter using the ResourceMap */
void MeixnerDistribution::initializeOptimizationSolverParameter()
{
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("MeixnerDistribution-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("MeixnerDistribution-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("MeixnerDistribution-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("MeixnerDistribution-MaximumConstraintError"));
}

/* Comparison operator */
Bool MeixnerDistribution::operator ==(const MeixnerDistribution & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_) && (delta_ == other.delta_) && (mu_ == other.mu_);
}

/* String converter */
String MeixnerDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MeixnerDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " delta=" << delta_
      << " mu=" << mu_
      << " logNormalizationFactor=" << logNormalizationFactor_
      << " b=" << b_
      << " c=" << c_
      << " dc=" << dc_;
  return oss;
}

String MeixnerDistribution::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ", delta = " << delta_ << ", mu = " << mu_ << ")";
  return oss;
}

/* Alpha accessor */
void MeixnerDistribution::setAlpha(const NumericalScalar alpha)
{
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    update();
  }
}

NumericalScalar MeixnerDistribution::getAlpha() const
{
  return alpha_;
}


/* Beta accessor */
void MeixnerDistribution::setBeta(const NumericalScalar beta)
{
  if (std::abs(beta) >= M_PI) throw InvalidArgumentException(HERE) << "Beta MUST be in (-pi, pi)";
  if (beta != beta_)
  {
    beta_ = beta;
    update();
  }
}

NumericalScalar MeixnerDistribution::getBeta() const
{
  return beta_;
}

/* Delta accessor */
void MeixnerDistribution::setDelta(const NumericalScalar delta)
{
  if (delta <= 0.0) throw InvalidArgumentException(HERE) << "Delta MUST be positive";
  if (delta != delta_)
  {
    delta_ = delta;
    update();
  }
}

NumericalScalar MeixnerDistribution::getDelta() const
{
  return delta_;
}

void MeixnerDistribution::setAlphaBetaDelta(const NumericalScalar alpha,
    const NumericalScalar beta,
    const NumericalScalar delta)
{
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (std::abs(beta) >= M_PI) throw InvalidArgumentException(HERE) << "Beta MUST be in (-pi, pi)";
  if (delta <= 0.0) throw InvalidArgumentException(HERE) << "Delta MUST be positive";
  if ((alpha != alpha_) || (beta != beta_) || (delta != delta_))
  {
    alpha_ = alpha;
    beta_ = beta;
    delta_ = delta;
    update();
  }
}

/* Mu accessor */
void MeixnerDistribution::setMu(const NumericalScalar mu)
{
  if (mu_ != mu)
  {
    mu_ = mu;
    computeRange();
  }
}

NumericalScalar MeixnerDistribution::getMu() const
{
  return mu_;
}

/* Virtual constructor */
MeixnerDistribution * MeixnerDistribution::clone() const
{
  return new MeixnerDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MeixnerDistribution::computeRange()
{
  const NumericalPoint mu(getMean());
  const NumericalPoint sigma(getStandardDeviation());
  const NumericalScalar logPDF(computeLogPDF(mu));
  const NumericalScalar logPDFEpsilon(std::log(getPDFEpsilon()));
  NumericalPoint lowerBound(mu);
  // Find the numerical lower bound based on the PDF value
  NumericalScalar logPDFLower(logPDF);
  while (logPDFLower > logPDFEpsilon)
  {
    lowerBound -= sigma;
    logPDFLower = computeLogPDF(lowerBound);
  }
  // Find the numerical upper bound based on the PDF value
  NumericalPoint upperBound(mu);
  NumericalPoint stepUpper(sigma);
  NumericalScalar logPDFUpper(logPDF);
  while (logPDFUpper > logPDFEpsilon)
  {
    upperBound += sigma;
    logPDFUpper = computeLogPDF(upperBound);
  }
  const Interval::BoolCollection finiteLowerBound(1, false);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

struct MeixnerBounds
{
  MeixnerBounds(const MeixnerDistribution & distribution)
    : distribution_(distribution)
  {
    // Nothing to do
  }

  NumericalPoint computeObjectiveB(const NumericalPoint & point) const
  {
    return NumericalPoint(1, distribution_.computePDF(point));
  }

  NumericalPoint computeObjectiveCD(const NumericalPoint & point) const
  {
    return NumericalPoint(1, point[0] * std::sqrt(distribution_.computePDF(point)));
  }

  const MeixnerDistribution & distribution_;
};

/* Update the derivative attributes */
void MeixnerDistribution::update()
{
  // First, the parameters of the distribution
  logNormalizationFactor_ = 2.0 * delta_ * std::log(2.0 * std::cos(0.5 * beta_)) - std::log(2.0 * M_PI * alpha_) - SpecFunc::LogGamma(2.0 * delta_);
  computeRange();
  // Second, the moments
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  // Third, the CDF and complementary CDF
  Collection<PiecewiseHermiteEvaluationImplementation> CDFCCDF(interpolateCDF(ResourceMap::GetAsUnsignedInteger("MeixnerDistribution-CDFDiscretization")));
  cdfApproximation_ = CDFCCDF[0];
  ccdfApproximation_ = CDFCCDF[1];
  // Fourth, the random generator
  // Update the bounds for the ratio of uniform sampling algorithm
  const MeixnerBounds boundsFunctions(*this);

  // define objectives functions 
  NumericalMathFunction fB(bindMethod<MeixnerBounds, NumericalPoint, NumericalPoint>(boundsFunctions, &MeixnerBounds::computeObjectiveB, 1, 1));
  const NumericalPoint epsilon(1.0e-5 * getStandardDeviation());
  const CenteredFiniteDifferenceGradient gradientB(epsilon, fB.getEvaluation());
  fB.setGradient(gradientB.clone());

  NumericalMathFunction fCD(bindMethod<MeixnerBounds, NumericalPoint, NumericalPoint>(boundsFunctions, &MeixnerBounds::computeObjectiveCD, 1, 1));
  const CenteredFiniteDifferenceGradient gradientCD(epsilon, fCD.getEvaluation());
  fCD.setGradient(gradientCD.clone());

  // Initilalyse Optimization problems 
  OptimizationProblem problem;
  problem.setBounds(getRange());
  solver_.setStartingPoint(getMean());

  // Define Optimization problem1 : maximization fB
  problem.setMinimization(false);	
  problem.setObjective(fB);
  solver_.setProblem(problem);
  solver_.run();
  b_ = std::sqrt(solver_.getResult().getOptimalValue()[0]);

  // Define Optimization problem2 : minimization fCD
  problem.setMinimization(true);
  problem.setObjective(fCD);	
  solver_.setProblem(problem);
  solver_.run();
  c_ = solver_.getResult().getOptimalValue()[0];

// Define Optimization problem3 : maximization fCD
  problem.setMinimization(false);	
  solver_.setProblem(problem);
  solver_.run();
  dc_ = solver_.getResult().getOptimalValue()[0] - c_;

}

/* Get one realization of the distribution
   We use the ratio-of-uniform method:
   if b=\sup_x \sqrt{p(x)}, c=\inf_x x\sqrt{p(x)}, d=\sup_x x\sqrt{p(x)}
   then \{(u,v)\,|\,0\leq u\leq b,c\leq v\leq d\} is an enclosing region for
   \{(u,v)\,|\,0\leq u\leq\sqrt{p(u/v)}\}
*/
NumericalPoint MeixnerDistribution::getRealization() const
{
  while (true)
  {
    const NumericalScalar u(b_ * RandomGenerator::Generate());
    if (u == 0.0) continue;
    const NumericalScalar v(c_ + dc_ * RandomGenerator::Generate());
    const NumericalScalar rho(v / u);
    if (2.0 * std::log(u) <= computeLogPDF(rho)) return NumericalPoint(1, rho);
  }
}

/* Get the PDF of the distribution */
NumericalScalar MeixnerDistribution::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return std::exp(computeLogPDF(point));
}

NumericalScalar MeixnerDistribution::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar z((point[0] - mu_) / alpha_);
  return logNormalizationFactor_ + beta_ * z + 2.0 * SpecFunc::LogGamma(NumericalComplex(delta_, z)).real();
}

/* Get the CDF of the distribution */
NumericalScalar MeixnerDistribution::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if (point[0] <= getMean()[0]) return cdfApproximation_(point)[0];
  else return 1.0 - ccdfApproximation_(point)[0];
}

/* Get the complementary CDF of the distribution */
NumericalScalar MeixnerDistribution::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  if (point[0] <= getMean()[0]) return 1.0 - cdfApproximation_(point)[0];
  else return ccdfApproximation_(point)[0];
}

/* Compute the quantile function of the distribution */
NumericalScalar MeixnerDistribution::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const NumericalScalar a(getRange().getLowerBound()[0]);
  const NumericalScalar b(getRange().getUpperBound()[0]);
  if (prob <= 0.0) return (tail ? b : a);
  if (prob >= 1.0) return (tail ? a : b);
  const UnsignedInteger n(cdfApproximation_.getLocations().getSize());
  if (tail)
  {
    // Here we have to solve ComplementaryCDF(x) = prob which is mathematically
    // equivalent to CDF(x) = 1 - prob, but numerically different with an
    // accuracy that depends on prob.
    // The cut-off is around the mean value
    if (prob <= ccdfApproximation_.getValues()[0][0]) return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(ccdfApproximation_, prob, ccdfApproximation_.getLocations()[0], ccdfApproximation_.getLocations()[n - 1], ccdfApproximation_.getValues()[0][0], ccdfApproximation_.getValues()[n - 1][0]);
    return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(cdfApproximation_, 1.0 - prob, cdfApproximation_.getLocations()[0], cdfApproximation_.getLocations()[n - 1], cdfApproximation_.getValues()[0][0], cdfApproximation_.getValues()[n - 1][0]);
  }
  // Here we have to solve CDF(x) = prob which is mathematically
  // equivalent to ComplementaryCDF(x) = 1 - prob, but numerically
  // different with an accuracy that depends on prob.
  // The cut-off is around the mean value
  if (prob <= cdfApproximation_.getValues()[n - 1][0]) return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(cdfApproximation_, prob, cdfApproximation_.getLocations()[0], cdfApproximation_.getLocations()[n - 1], cdfApproximation_.getValues()[0][0], cdfApproximation_.getValues()[n - 1][0]);
  return Brent(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_).solve(ccdfApproximation_, 1.0 - prob, ccdfApproximation_.getLocations()[0], ccdfApproximation_.getLocations()[n - 1], ccdfApproximation_.getValues()[0][0], ccdfApproximation_.getValues()[n - 1][0]);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex MeixnerDistribution::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex MeixnerDistribution::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return NumericalComplex(2.0 * delta_ * std::log(std::cos(0.5 * beta_)), mu_ * x) - 2.0 * delta_ * std::log(std::cosh(NumericalComplex(0.5 * alpha_ * x, -0.5 * mu_)));
}

/* Compute the mean of the distribution */
void MeixnerDistribution::computeMean() const
{
  mean_ = NumericalPoint(1, alpha_ * delta_ * std::tan(0.5 * beta_) + mu_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint MeixnerDistribution::getStandardDeviation() const
{
  return NumericalPoint(1, alpha_ * std::sqrt(delta_ / (1.0 + std::cos(beta_))));
}

/* Get the skewness of the distribution */
NumericalPoint MeixnerDistribution::getSkewness() const
{
  return NumericalPoint(1, std::sin(0.5 * beta_) * std::sqrt(2.0 / delta_));
}

/* Get the kurtosis of the distribution */
NumericalPoint MeixnerDistribution::getKurtosis() const
{
  return NumericalPoint(1, 3.0 + (2.0 - std::cos(beta_)) / delta_);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
MeixnerDistribution::Implementation MeixnerDistribution::getStandardRepresentative() const
{
  return MeixnerDistribution(1.0, beta_, delta_, 0.0).clone();
}

/* Compute the covariance of the distribution */
void MeixnerDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = alpha_ * alpha_ * delta_ / (1.0 + std::cos(beta_));
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
NumericalPoint MeixnerDistribution::getParameter() const
{
  NumericalPoint point(4);
  point[0] = alpha_;
  point[1] = beta_;
  point[2] = delta_;
  point[3] = mu_;
  return point;
}

void MeixnerDistribution::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = MeixnerDistribution(parameter[0], parameter[1], parameter[2], parameter[3]);
  setWeight(w);
}

/* Parameters description accessor */
Description MeixnerDistribution::getParameterDescription() const
{
  Description description(4);
  description[0] = "alpha";
  description[1] = "beta";
  description[2] = "delta";
  description[2] = "mu";
  return description;
}

/* Check if the distribution is elliptical */
Bool MeixnerDistribution::isElliptical() const
{
  return beta_ == 0.0;
}

/* Method save() stores the object through the StorageManager */
void MeixnerDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "delta_", delta_ );
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void MeixnerDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "delta_", delta_ );
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  update();
}

/* Optimization solver accessor */
OptimizationSolver MeixnerDistribution::getOptimizationSolver() const
{
  return solver_;
}

void MeixnerDistribution::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}

END_NAMESPACE_OPENTURNS
