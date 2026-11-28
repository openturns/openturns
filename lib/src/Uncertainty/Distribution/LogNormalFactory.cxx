//                                               -*- C++ -*-
/**
 *  @brief Factory for LogNormal distribution
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LogNormalFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Brent.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/LinearLeastSquares.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/LeastSquaresDistributionFactory.hxx"
#include "openturns/MemoizeFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogNormalFactory)

static const Factory<LogNormalFactory> Factory_LogNormalFactory;

/* Default constructor */
LogNormalFactory::LogNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
LogNormalFactory * LogNormalFactory::clone() const
{
  return new LogNormalFactory(*this);
}


/* Here is the interface that all derived class must implement */

/* Algorithm associated with the method of moments */
LogNormal LogNormalFactory::buildMethodOfMoments(const Sample & sample) const
{
  if (sample.getSize() < 3) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution using the method of moments with a sample of size less than 3.";
  // ME
  const Scalar std = sample.computeStandardDeviation()[0];
  if (!std::isfinite(std)) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution if data contains NaN or Inf";
  if (std == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogNormal distribution based on a constant sample using the method of moments.";
  const Scalar skew = sample.computeSkewness()[0];
  const Scalar a3 = skew;
  // V parameter, see "LogNormal Distributions, Theory and Applications", ISBN 0-8247-7803-0, p116
  const Scalar v = cbrt(0.5 * (a3 + std::sqrt(4.0 + a3 * a3))) + cbrt(0.5 * (a3 - std::sqrt(4.0 + a3 * a3)));
  const Scalar omega = 1.0 + v * v;
  const Scalar sigma = std::sqrt(std::log(omega));
  const Scalar beta = std / std::sqrt(omega * (omega - 1.0));
  const Scalar mu = std::log(beta);
  const Scalar mean = sample.computeMean()[0];
  const Scalar gamma = mean - beta * std::sqrt(omega);
  LogNormal result(mu, sigma, gamma);
  result.setDescription(sample.getDescription());
  return result;
}

/* Algorithm associated with the method of local likelihood maximization */
LogNormal LogNormalFactory::buildMethodOfLocalLikelihoodMaximization(const Sample & sample) const
{
  const Scalar std = sample.computeStandardDeviation()[0];
  if (!std::isfinite(std)) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution if data contains NaN or Inf";
  if (std == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogNormal distribution based on a constant sample using the method of local maximum likelihood.";
  // The local maximum likelihood estimator of the shift parameter gamma is a
  // stationary point of the profile log-likelihood, i.e. a zero of the
  // LogNormalFactoryLMLEParameterConstraint function, see
  // A. Clifford Cohen & Betty Jones Whitten (1980), "Estimation in the
  // Three-Parameter Lognormal Distribution", Journal of the American
  // Statistical Association, 75:370, 399-404.
  // The bracketing strategy follows scipy.stats.lognorm.fit: the right end of
  // the bracket is anchored just below the sample minimum, then it is moved
  // to the left until the constraint function takes a negative value, and the
  // left end is moved to the left until the signs differ. The profile
  // log-likelihood at the stationary point is finally compared with the one
  // at the anchored right end: the stationary point is kept only if it
  // improves the log-likelihood, otherwise the local maximum likelihood
  // estimator is declared undefined and the caller falls back to the modified
  // moments estimator. This comparison discards the spurious stationary
  // points that the constraint function admits far on the left, where it
  // asymptotically vanishes and changes sign because of roundoff errors.
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  const LogNormalFactoryLMLEParameterConstraint constraint(sample);
  MemoizeFunction f(constraint);
  f.enableCache();
  // Right end of the bracket: the closest representable value below the
  // sample minimum, backed off by quantileEpsilon when it leads to
  // non-finite quantities (e.g. xMin == 0)
  Scalar gamma0 = std::nextafter(xMin, -SpecFunc::MaxScalar);
  if (!(gamma0 < xMin)) throw InvalidArgumentException(HERE) << "Error: unable to find a shift parameter below the sample minimum";
  Scalar f0 = f(Point(1, gamma0))[0];
  Point sums0(constraint.computeMaximumLikelihoodSums(gamma0));
  Bool finite0 = std::isfinite(f0);
  for (UnsignedInteger i = 0; i < 4; ++i) finite0 = finite0 && std::isfinite(sums0[i]);
  if (!finite0)
  {
    gamma0 = xMin - quantileEpsilon;
    if (!(gamma0 < xMin)) throw InvalidArgumentException(HERE) << "Error: unable to find a shift parameter below the sample minimum";
    f0 = f(Point(1, gamma0))[0];
    sums0 = constraint.computeMaximumLikelihoodSums(gamma0);
    finite0 = std::isfinite(f0);
    for (UnsignedInteger i = 0; i < 4; ++i) finite0 = finite0 && std::isfinite(sums0[i]);
    if (!finite0) throw InvalidArgumentException(HERE) << "Error: unable to evaluate the local maximum likelihood estimator";
  }
  const Scalar mu0 = sums0[1] / size;
  const Scalar sigma20 = sums0[2] / size - mu0 * mu0;
  if (!(sigma20 > 0.0)) throw InvalidArgumentException(HERE) << "Error: the variance local maximum likelihood estimator should be positive, here sigma2=" << sigma20;
  // First, move the right end of the bracket to the left until the constraint
  // function takes a negative value. The right end stays anchored to the
  // sample minimum instead of sliding with the interval.
  Scalar delta = xMin - gamma0;
  Scalar rbrack = gamma0;
  Scalar fRbrack = f0;
  while (fRbrack >= 0.0)
  {
    delta *= 2.0;
    rbrack = xMin - delta;
    if (!std::isfinite(rbrack) || !(rbrack < xMin) || !(delta < std::sqrt(SpecFunc::MaxScalar)))
    {
      // The profile log-likelihood increases towards the sample minimum: no
      // interior stationary point exists, the local maximum likelihood
      // estimator is not defined
      throw InvalidArgumentException(HERE) << "Error: unable to bracket the gamma parameter. The local maximum likelihood estimator is not defined";
    }
    fRbrack = f(Point(1, rbrack))[0];
    if (!std::isfinite(fRbrack))
    {
      // Same situation as above
      throw InvalidArgumentException(HERE) << "Error: unable to bracket the gamma parameter. The local maximum likelihood estimator is not defined";
    }
  }
  // Second, move the left end of the bracket to the left until the signs differ
  Scalar lbrack = rbrack - 1.0;
  const Scalar nextDown = std::nextafter(rbrack, -SpecFunc::MaxScalar);
  if (nextDown < lbrack) lbrack = nextDown;
  Scalar fLbrack = f(Point(1, lbrack))[0];
  if (!std::isfinite(fLbrack)) throw InvalidArgumentException(HERE) << "Error: unable to bracket the gamma parameter. The local maximum likelihood estimator is not defined";
  Scalar delta2 = 2.0 * (rbrack - lbrack);
  while ((fLbrack < 0.0) == (fRbrack < 0.0))
  {
    lbrack = rbrack - delta2;
    delta2 *= 2.0;
    if (!std::isfinite(lbrack) || !(delta2 < std::sqrt(SpecFunc::MaxScalar))) throw InvalidArgumentException(HERE) << "Error: unable to bracket the gamma parameter. The local maximum likelihood estimator is not defined";
    fLbrack = f(Point(1, lbrack))[0];
    if (!std::isfinite(fLbrack)) throw InvalidArgumentException(HERE) << "Error: unable to bracket the gamma parameter. The local maximum likelihood estimator is not defined";
  }
  // Third, solve the constraint equation
  const Brent solver(ResourceMap::GetAsScalar("LogNormalFactory-AbsolutePrecision"), ResourceMap::GetAsScalar("LogNormalFactory-RelativePrecision"), ResourceMap::GetAsScalar("LogNormalFactory-ResidualPrecision"), ResourceMap::GetAsUnsignedInteger("LogNormalFactory-MaximumIteration"));
  // Gamma estimate
  const Scalar gammaStar = solver.solve(f, 0.0, lbrack, rbrack, fLbrack, fRbrack);
  // Fourth, compare the profile log-likelihood at the stationary point with
  // the one at the boundary gamma0 and keep the stationary point only if it
  // improves the log-likelihood. Up to an additive constant independent of
  // gamma, ll(gamma) = -S1(gamma) - n/2 * log(sigma2(gamma)).
  const Point sumsStar(constraint.computeMaximumLikelihoodSums(gammaStar));
  const Scalar muStar = sumsStar[1] / size;
  const Scalar sigma2Star = sumsStar[2] / size - muStar * muStar;
  if (!(sigma2Star > 0.0)) throw InvalidArgumentException(HERE) << "Error: the variance local maximum likelihood estimator should be positive, here sigma2=" << sigma2Star;
  const Scalar deltaLL = -(sumsStar[1] - sums0[1]) - 0.5 * size * std::log(sigma2Star / sigma20);
  // A stationary point with a lower log-likelihood than the boundary is a
  // spurious root due to roundoff errors far on the left: the local maximum
  // likelihood estimator is not defined
  if (!(deltaLL > 0.0)) throw InvalidArgumentException(HERE) << "Error: unable to find a stationary point of the profile log-likelihood. The local maximum likelihood estimator is not defined";
  return LogNormal(muStar, std::sqrt(sigma2Star), gammaStar);
}

struct LogNormalFactoryMMEParameterConstraint
{
  /* Constructor from a sample and a derivative factor estimate */
  LogNormalFactoryMMEParameterConstraint(const UnsignedInteger n,
                                         const Scalar xMin,
                                         const Scalar mean,
                                         const Scalar std):
    eZ1_(DistFunc::eZ1(n)),
    alpha_(std::pow(std / (mean - xMin), 2))
  {
    // Nothing to do
  };

  Point computeConstraint(const Point & parameter) const
  {
    const Scalar omega = parameter[0];
    if (!(omega > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogNormal distribution based on the given sample using the method of modified moments, probably because the sample is constant.";
    return Point(1, alpha_ * std::pow(std::sqrt(omega) - std::exp(eZ1_ * std::sqrt(std::log(omega))), 2) - omega * (omega - 1.0));
  }

  Scalar getEZ1() const
  {
    return eZ1_;
  }

  // Some intermediate coefficients
  Scalar eZ1_;
  Scalar alpha_;
};

/* Algorithm associated with the method of modified moments */
LogNormal LogNormalFactory::buildMethodOfModifiedMoments(const Sample & sample) const
{
  const Scalar std = sample.computeStandardDeviation()[0];
  if (!std::isfinite(std)) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution if data contains NaN or Inf";
  if (std == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogNormal distribution based on a constant sample using the method of modified moments.";
  const Scalar mean = sample.computeMean()[0];
  const Scalar xMin = sample.getMin()[0];
  const LogNormalFactoryMMEParameterConstraint constraint(sample.getSize(), xMin, mean, std);
  const Function f(bindMethod<LogNormalFactoryMMEParameterConstraint, Point, Point>(constraint, &LogNormalFactoryMMEParameterConstraint::computeConstraint, 1, 1));
  // First, the bracketing interval
  Scalar ea = 1.0;
  Scalar eb = 2.0;
  Scalar a = 1.0 + ea;
  Scalar b = 1.0 + eb;
  Scalar fA = f(Point(1, a))[0];
  Scalar fB = f(Point(1, b))[0];
  // While f has the same sign at the two bounds, update the interval
  const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  while ((fA * fB > 0.0) && (ea > quantileEpsilon))
  {
    ea = 0.5 * ea;
    a = 1.0 + ea;
    fA = f(Point(1, a))[0];
    if (fA * fB <= 0.0) break;
    eb = 2.0 * eb;
    b = 1.0 + eb;
    fB = f(Point(1, b))[0];
  }
  const Scalar absolutePrecision = ResourceMap::GetAsScalar("LogNormalFactory-AbsolutePrecision");
  if (a <= quantileEpsilon) throw InvalidArgumentException(HERE) << "Error: the modified moment estimator is not defined";
  Scalar omega = 0.0;
  if (std::abs(fA) < absolutePrecision) omega = a;
  else if (std::abs(fB) < absolutePrecision) omega = b;
  else
  {
    const Brent solver(absolutePrecision, ResourceMap::GetAsScalar("LogNormalFactory-RelativePrecision"), ResourceMap::GetAsScalar("LogNormalFactory-ResidualPrecision"), ResourceMap::GetAsUnsignedInteger("LogNormalFactory-MaximumIteration"));
    // Omega estimate
    omega = solver.solve(f, 0.0, a, b, fA, fB);
  }
  const Scalar sigma = std::sqrt(std::log(omega));
  const Scalar eZ1 = constraint.getEZ1();
  const Scalar sqrtOmega = std::sqrt(omega);
  const Scalar beta = (mean - xMin) / (sqrtOmega - std::exp(eZ1 * sigma));
  const Scalar gamma = mean - beta * sqrtOmega;
  const Scalar mu = std::log(beta);
  return LogNormal(mu, sigma, gamma);
}

Distribution LogNormalFactory::build(const Sample & sample) const
{
  return build(sample, ResourceMap::GetAsUnsignedInteger("LogNormalFactory-EstimationMethod"));
}

Distribution LogNormalFactory::build(const Sample & sample,
                                     const UnsignedInteger method) const
{
  return buildAsLogNormal(sample, method).clone();
}

Distribution LogNormalFactory::build() const
{
  return buildAsLogNormal().clone();
}

Distribution LogNormalFactory::build(const Point & parameters) const
{
  return buildAsLogNormal(parameters).clone();
}

LogNormal LogNormalFactory::buildAsLogNormal(const Sample & sample) const
{
  return buildAsLogNormal(sample, ResourceMap::GetAsUnsignedInteger("LogNormalFactory-EstimationMethod"));
}

LogNormal LogNormalFactory::buildAsLogNormal(const Sample & sample,
    const UnsignedInteger method) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution from a sample of size < 3";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a LogNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  LogNormal result;
  switch (method)
  {
    case 0:
      try
      {
        result = buildMethodOfLocalLikelihoodMaximization(sample);
        break;
      }
      catch (const InvalidArgumentException & ex)
      {
        // We switch to the moment estimate
        LOGWARN(OSS() << ex.what());
        result = buildAsLogNormal(sample, 1);
        break;
      }
      break;
    case 1:
      try
      {
        result = buildMethodOfModifiedMoments(sample);
        break;
      }
      catch (const InvalidArgumentException & ex)
      {
        // We switch to the moment estimate
        LOGWARN(OSS() << ex.what());
        result = buildAsLogNormal(sample, 2);
        break;
      }
      break;
    case 2:
      result = buildMethodOfMoments(sample);
      break;
    case 3:
      result = buildMethodOfLeastSquares(sample);
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid value=" << method << " for the key 'LogNormalFactory-EstimationMethod' in ResourceMap";
  }
  adaptToKnownParameter(sample, &result);
  return result;
}

LogNormal LogNormalFactory::buildAsLogNormal(const Point & parameters) const
{
  try
  {
    LogNormal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution from the given parameters";
  }
}

LogNormal LogNormalFactory::buildAsLogNormal() const
{
  return LogNormal();
}

/* Algorithm associated with the method of least-squares */
LogNormal LogNormalFactory::buildMethodOfLeastSquares(const Sample & sample,
    const Scalar gamma) const
{
  const UnsignedInteger size = sample.getSize();
  if (!(gamma < sample.getMin()[0])) throw InvalidArgumentException(HERE) << "Error: cannot build a LogNormal distribution with a shift gamma greater or equal to the sample minimum";
  Sample dataIn(size, 1);
  Sample dataOut(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    dataIn(i, 0) = std::log(sample(i, 0) - gamma);
    dataOut(i, 0) = DistFunc::qNormal(sample.computeEmpiricalCDF(sample[i]) - 0.5 / size);
  }
  LinearLeastSquares leastSquares(dataIn, dataOut);
  leastSquares.run();
  // leastSquares gives a relation of the form z = a * (y - c) + b
  // and we need to rewrite it as z = a1 * y + a0
  // so a1 = a and a0 = b - a1 * c
  const Scalar c = leastSquares.getCenter()[0];
  const Scalar a1 = leastSquares.getLinear()(0, 0);
  const Scalar a0 = leastSquares.getConstant()[0];
  const Scalar sigmaLog = 1.0 / a1;
  const Scalar muLog = -(a0 - a1 * c) * sigmaLog;
  return LogNormal(muLog, sigmaLog, gamma);
}


class LogNormalFactoryResidualEvaluation : public EvaluationImplementation
{
public:
  LogNormalFactoryResidualEvaluation(const Sample & sample)
    : EvaluationImplementation()
    , sample_(sample)
    , xMin_(sample.getMin()[0])
  {
    const UnsignedInteger size = sample.getSize();
    dataOut_ = Sample(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      dataOut_(i, 0) = DistFunc::qNormal(sample.computeEmpiricalCDF(sample[i]) - 0.5 / size);
    }
  }

  LogNormalFactoryResidualEvaluation * clone() const override
  {
    return new LogNormalFactoryResidualEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return sample_.getSize();
  }

  Description getInputDescription() const override
  {
    return Description(1, "gamma");
  }

  Description getOutputDescription() const override
  {
    return Description(sample_.getSize(), "r");
  }

  Description getDescription() const override
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Point operator() (const Point & parameter) const override
  {
    const Scalar gamma = parameter[0];
    const UnsignedInteger size = sample_.getSize();
    // Early exit for incompatible values of gamma
    if (gamma >= xMin_)
      return Point(size, std::sqrt(SpecFunc::MaxScalar / (10.0 * size)));
    Sample dataIn(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      dataIn(i, 0) = std::log(sample_(i, 0) - gamma);
    }
    LinearLeastSquares leastSquares(dataIn, dataOut_);
    leastSquares.run();
    const Point result((dataOut_ - leastSquares.getResult().getMetaModel()(dataIn)).asPoint());
    return result;
  }

private:
  Sample sample_;
  Scalar xMin_;
  Sample dataOut_;
};


LogNormal LogNormalFactory::buildMethodOfLeastSquares(const Sample & sample) const
{
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can only build a LogNormal distribution from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  Scalar gamma = xMin - std::abs(xMin) / (2 + size);
  LogNormalFactoryResidualEvaluation residualEvaluation(sample);
  const Function residualFunction(residualEvaluation.clone());
  LeastSquaresProblem problem(residualFunction);
  OptimizationAlgorithm solver(OptimizationAlgorithm::Build(problem));
  solver.setStartingPoint(Point(1, gamma));
  solver.run();
  gamma = solver.getResult().getOptimalPoint()[0];
  return buildMethodOfLeastSquares(sample, gamma);
}


END_NAMESPACE_OPENTURNS
