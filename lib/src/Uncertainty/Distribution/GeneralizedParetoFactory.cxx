//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedPareto distribution
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
#include "openturns/GeneralizedParetoFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedParetoFactory)

static const Factory<GeneralizedParetoFactory> Factory_GeneralizedParetoFactory;

/* Default constructor */
GeneralizedParetoFactory::GeneralizedParetoFactory()
  : DistributionFactoryImplementation()
{
  // Create the optimization solver parameters using the parameters in the ResourceMap
  solver_ = OptimizationAlgorithm::Build(ResourceMap::GetAsString("GeneralizedParetoFactory-DefaultOptimizationAlgorithm"));
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumConstraintError"));
  Cobyla* cobyla = dynamic_cast<Cobyla *>(solver_.getImplementation().get());
  if (cobyla)
    cobyla->setCheckStatus(false);
}

/* Virtual constructor */
GeneralizedParetoFactory * GeneralizedParetoFactory::clone() const
{
  return new GeneralizedParetoFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution GeneralizedParetoFactory::build(const Sample & sample) const
{
  return buildAsGeneralizedPareto(sample).clone();
}

Distribution GeneralizedParetoFactory::build(const Point & parameters) const
{
  return buildAsGeneralizedPareto(parameters).clone();
}

Distribution GeneralizedParetoFactory::build() const
{
  return buildAsGeneralizedPareto().clone();
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const Sample & sample) const
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from a sample of size < 2";
  Scalar xMin = sample.getMin()[0];
  if (!(xMin > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution based on a sample with nonpositive values.";
  Scalar smallSize = ResourceMap::GetAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize" );
  // The strategy is to use the probability weighted moment method for small size and to switch to the method of exponential regression if the estimator is not defined. For large size, we use the method of exponential regression in priority and switch to the probability weighted moment method if the estimator is not defined. No other fallback is proposed if both method failed.
  if (size <= smallSize)
  {
    try
    {
      return buildMethodOfProbabilityWeightedMoments(sample);
    }
    catch (const InvalidArgumentException &)
    {
      // Nothing to do, fallback on the exponential regression
      LOGINFO("Method of probability weighted moment failed, using method of exponential regression");
      return buildMethodOfExponentialRegression(sample);
    }
  } // small size
  try
  {
    return buildMethodOfExponentialRegression(sample);
  }
  catch (const InvalidArgumentException &)
  {
    // Nothing to do, fallback on the exponential regression
    LOGINFO("Method of exponential regression failed, using method of probability weighted moment");
    return buildMethodOfProbabilityWeightedMoments(sample);
  }
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const Point & parameters) const
{
  try
  {
    GeneralizedPareto distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from the given parameters: " << parameters;
  }
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto() const
{
  return GeneralizedPareto();
}

/* Algorithm associated with the method of moments */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfMoments(const Sample & sample) const
{
  LOGINFO("Using method of moment");
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  const Scalar u = xMin - std::abs(xMin) / (2.0 + size);
  const Scalar mean = sample.computeMean()[0] - u;
  if (!SpecFunc::IsNormal(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an GeneralizedPareto distribution if data contains NaN or Inf";
  const Scalar stddev = sample.computeStandardDeviation()[0];
  if (stddev == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution from a constant sample.";
  const Scalar xi = -0.5 * (std::pow(mean / stddev, 2.0) - 1.0);
  // The moment estimator is valid only if the estimated xi parameter is greater than -1/4
  if (xi >= 0.25) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution with the method of moments when the estimated xi parameter=" << xi << " is greater than 0.25";
  const Scalar sigma = 0.5 * mean * (std::pow(mean / stddev, 2) + 1.0);
  GeneralizedPareto result(sigma, xi, u);
  result.setDescription(sample.getDescription());
  return result;
}

struct GeneralizedParetoFactoryParameterConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  GeneralizedParetoFactoryParameterConstraint(const Sample & sortedSample)
    : sampleY_(0, 1)
    , size_(sortedSample.getSize())
  {
    sampleY_ = Sample(size_ - 2, 1);
    const Scalar xMin = sortedSample(0, 0);
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const Scalar xNm1mJ = sortedSample(size_ - 1 - j, 0);
      const Scalar xNm2mJ = sortedSample(size_ - 2 - j, 0);
      if ((xNm1mJ == xMin) || (xNm2mJ == xMin))
      {
        const String message("Cannot use exponential regression to estimate a GeneralizedPareto distribution when the mininmum value of the sample is repeated");
        LOGINFO(message);
        throw InvalidArgumentException(HERE) << message;
      }
      sampleY_(j, 0) = (j + 1.0) * std::log((xNm1mJ - xMin) / (xNm2mJ - xMin));
    }
  };

  Point computeConstraint(const Point & parameter) const
  {
    const Scalar gamma = parameter[0];
    // Separate the small gamma case for stability purpose
    if (std::abs(gamma) < 1.0e-4)
    {
      Scalar exponentialRegressionLogLikelihood = 0.0;
      for (UnsignedInteger j = 0; j < size_ - 2; ++j)
      {
        const Scalar logAlphaJ = std::log((j + 1.0) / size_);
        const Scalar gammaLogAlphaJ = gamma * logAlphaJ;
        const Scalar yLogAlphaJ = sampleY_(j, 0) * logAlphaJ;
        exponentialRegressionLogLikelihood += std::log(-logAlphaJ) + yLogAlphaJ + 0.5 * gammaLogAlphaJ * (1.0 + yLogAlphaJ + gammaLogAlphaJ * (1.0 / 12.0 + yLogAlphaJ / 3.0 + gammaLogAlphaJ * yLogAlphaJ / 12.0));
      }
      return Point(1, -exponentialRegressionLogLikelihood);
    }
    // Large gamma case
    Scalar exponentialRegressionLogLikelihood = 0.0;
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const Scalar alphaJ = (1.0 - std::pow((j + 1.0) / size_, gamma)) / gamma;
      exponentialRegressionLogLikelihood += std::log(alphaJ) - alphaJ * sampleY_(j, 0);
    }
    return Point(1, -exponentialRegressionLogLikelihood);
  }

  Sample sampleY_;
  UnsignedInteger size_;
};

/* Algorithm associated with the method of exponential regression */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfExponentialRegression(const Sample & sample) const
{
  LOGINFO("Using method of exponential regression");
  const Scalar mu = sample.computeMean()[0];
  if (!SpecFunc::IsNormal(mu)) throw InvalidArgumentException(HERE) << "Error: cannot build an GeneralizedPareto distribution if data contains NaN or Inf";
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  const Scalar u = xMin - std::abs(xMin) / (2.0 + size);
  const Sample sortedSample(sample.sort(0) - Point(1, u));
  GeneralizedParetoFactoryParameterConstraint constraint(sortedSample);
  Function f(bindMethod<GeneralizedParetoFactoryParameterConstraint, Point, Point>(constraint, &GeneralizedParetoFactoryParameterConstraint::computeConstraint, 1, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, f.getEvaluation());
  f.setGradient(gradient);

  // Define Optimization problem
  OptimizationProblem problem(f);

  const UnsignedInteger dimension = problem.getObjective().getInputDimension();
  Point parametersLowerBound(dimension, -1.0);
  Point parametersUpperBound(dimension,  1.0);
  problem.setBounds(Interval(parametersLowerBound, parametersUpperBound, Interval::BoolCollection(dimension, 0), Interval::BoolCollection(dimension, 0)));

  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(Point(dimension, 0.0));
  solver.run();

  // optimal point
  const Scalar xi = solver.getResult().getOptimalPoint()[0];
  const Scalar mean = sample.computeMean()[0] - u;
  // Compute the first probability weighted moment
  Scalar m = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample(i, 0);
  m /= size * size;
  const Scalar sigma = 2.0 * mean * m / (mean - 2.0 * m);
  GeneralizedPareto result(sigma, xi, u);
  result.setDescription(sample.getDescription());
  return result;
}

/* Algorithm associated with the method of modified moments */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfProbabilityWeightedMoments(const Sample & sample) const
{
  LOGINFO("Using method of probability weighted moment");
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  const Scalar u = xMin - std::abs(xMin) / (2.0 + size);
  const Scalar mean = sample.computeMean()[0] - u;
  if (!SpecFunc::IsNormal(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an GeneralizedPareto distribution if data contains NaN or Inf";
  const Sample sortedSample(sample.sort(0) - Point(1, u));
  // Compute the first probability weighted moment
  Scalar m = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample(i, 0);
  m /= size * size;
  // r=m/mu
  // rho=1/(1-2r)
  // xi=2-1/(1-2r)=(1-4r)/(1-2r)
  const Scalar rho = m / mean;
  const Scalar xi = (1.0 - 4.0 * rho) / (1.0 - 2.0 * rho);
  if (xi <= -0.5)
  {
    const String message(OSS() << "Error: cannot estimate a GeneralizedPareto distribution with the method of probability weighted moments when the estimated xi parameter=" << xi << " is less than -0.5");
    throw InvalidArgumentException(HERE) << message;
  }
  // sigma=2*m*/(1-2r)
  const Scalar sigma = 2.0 * m / (1.0 - 2.0 * rho);
  GeneralizedPareto result(sigma, xi, u);
  result.setDescription(sample.getDescription());
  return result;
}

/* Optimization solver accessor */
void GeneralizedParetoFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm GeneralizedParetoFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

Graph GeneralizedParetoFactory::drawMeanResidualLife(const Sample & sample) const
{
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Can only draw mean residual life from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Scalar uMin = sample.getMin()[0];
  const Scalar uMax = sample.getMax()[0] - 1.0;
  const UnsignedInteger pointsNumber = ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-MeanResidualLifePointNumber");
  Sample u(pointsNumber, 1);
  Sample mrl(pointsNumber, 1);
  Sample ciLow(pointsNumber, 1);
  Sample ciUp(pointsNumber, 1);
  const Scalar level = ResourceMap::GetAsScalar("GeneralizedParetoFactory-MeanResidualLifeConfidenceLevel");
  const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
  const Sample sortedSample(sample.sort(0));
  for (UnsignedInteger i = 0; i < pointsNumber; ++ i)
  {
    u(i, 0) = uMin + i * (uMax - uMin) / (pointsNumber - 1);

    // rebuild the sample Xi|Xi>u (no sorting)
    Sample xu(0, 1);
    for (UnsignedInteger j = 0; j < sample.getSize(); ++ j)
      if (sample(j, 0) > u(i, 0))
        xu.add(sample[j]);

    const UnsignedInteger n = xu.getSize();
    mrl(i, 0) = xu.computeMean()[0] - u(i, 0);
    const Scalar variance = xu.computeCovariance()(0, 0);
    const Scalar ciLength2 = xq * std::sqrt(variance / n);
    ciLow(i, 0) = mrl(i, 0) - ciLength2;
    ciUp(i, 0) = mrl(i, 0) + ciLength2;
  }
  Curve curveMrl(u, mrl, "mrl");
  curveMrl.setColor("red");
  Curve curveCILow(u, ciLow, "CI low");
  curveCILow.setLineStyle("dashed");
  Curve curveCIUp(u, ciUp, "CI up");
  curveCIUp.setLineStyle("dashed");
  Graph result("Mean residual life plot", "Threshold", "Mean excess", true, "topleft");
  result.add(curveMrl);
  result.add(curveCILow);
  result.add(curveCIUp);
  return result;
}

class GeneralizedParetoLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedParetoLikelihoodEvaluation(const Sample & sample, const Scalar u)
    : EvaluationImplementation()
    , sample_(sample)
    , m_(sample.getSize())
    , u_(u)
  {
    // Nothing to do
  }

  GeneralizedParetoLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedParetoLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & parameter) const override
  {
    const Scalar sigma = parameter[0];
    const Scalar xi = parameter[1];
    Point value(1);
    if (sigma <= 0.0)
    {
      value[0] = -std::log(SpecFunc::MaxScalar);
      return value;
    }
    Scalar ll = 0.0;
    UnsignedInteger n = 0;// count the number of x>u
    for (UnsignedInteger i = 0; i < m_; ++ i)
    {
      const Scalar zi = sample_(i, 0) - u_;
      if (zi > 0.0)
      {
        if (std::abs(xi) < SpecFunc::Precision)
        {
          ll += - 1.0 * zi / sigma;
        }
        else
        {
          const Scalar c1 = xi * zi / sigma;
          if (c1 <= SpecFunc::Precision - 1.0) // can be slightly off
          {
            ll += -std::log(SpecFunc::MaxScalar);
            continue;
          }
          ll += (-1.0 / xi - 1.0) * std::log1p(c1);
        }
        ++ n;
      }
    }
    ll += - 1.0 * n * std::log(sigma);
    value[0] = ll;
    return value;
  }

private:
  Sample sample_;
  UnsignedInteger m_ = 0;
  Scalar u_ = 0.0;
};



DistributionFactoryLikelihoodResult GeneralizedParetoFactory::buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample,
    const Scalar u) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of size>=2, here size=" << sample.getSize();

  const Function objective(new GeneralizedParetoLikelihoodEvaluation(sample, u));
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // sigma > 0
  const Point lowerBound({SpecFunc::Precision, -SpecFunc::MaxScalar});
  const Point upperBound(2, SpecFunc::MaxScalar);
  const Interval::BoolCollection finiteLowerBound({true, false});
  const Interval::BoolCollection finiteUpperBound(2, false);
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

  // 1+xi(zi-mu)/sigma > 0 for all order statistics taken into account
  const Scalar zMin = sample.getMin()[0];
  const Scalar zMax = sample.getMax()[0];

  Description formulas(2);
  formulas[0] = OSS() << "sigma + xi * (" << zMax << " - " << u <<")";
  formulas[1] = OSS() << "sigma + xi * (" << zMin << " - " << u <<")";
  const SymbolicFunction constraint(Description({"sigma", "xi"}), formulas);
  problem.setInequalityConstraint(constraint);

  Sample xu(0, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (sample(i, 0) > u)
      xu.add(sample[i]);
  if (xu.getSize() < 10)
    throw InvalidArgumentException(HERE) << "Not enough points, lower the threshold u";

  // starting point: initialize sigma as Gumbel parametrization, xi is arbitrary (see ismev package)
  const Scalar sigma0 = std::sqrt(6.0 * xu.computeCovariance()(0, 0)) / M_PI;
  const Scalar xi0 = 0.1;
  const Point x0({sigma0, xi0});

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(x0);
  solver.run();
  Point optimalParameter(solver.getResult().getOptimalPoint()); // sigma, xi
  optimalParameter.add(u);

  const Distribution distribution(buildAsGeneralizedPareto(optimalParameter));
  const Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, xu));
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  DistributionFactoryLikelihoodResult result(distribution, parameterDistribution, logLikelihood);
  return result;
}

GeneralizedPareto GeneralizedParetoFactory::buildMethodOfLikelihoodMaximization(const Sample & sample, const Scalar u) const
{
  const Distribution distribution(buildMethodOfLikelihoodMaximizationEstimator(sample, u).getDistribution());
  return buildAsGeneralizedPareto(distribution.getParameter());
}


class GeneralizedParetoProfileLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedParetoProfileLikelihoodEvaluation(const Sample & sample,
      const Scalar zMin,
      const Scalar zMax,
      const OptimizationAlgorithm & solver,
      const Scalar u)
    : EvaluationImplementation()
    , sample_(sample)
    , solver_(solver)
    , u_(u)
  {
    zMin_ = zMin;
    zMax_ = zMax;
  }

  GeneralizedParetoProfileLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedParetoProfileLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Description getInputDescription() const override
  {
    return {"xi"};
  }

  Point operator() (const Point & parameter) const override
  {
    const Scalar xi0 = parameter[0];

    const Function likelihood(GeneralizedParetoLikelihoodEvaluation(sample_, u_).clone());
    // only sigma remains to be optimized out of (sigma, xi, u)
    const ParametricFunction objective(likelihood, {1}, {xi0});
    OptimizationProblem problem(objective);
    problem.setMinimization(false);

    // sigma > 0
    const Point lowerBound({SpecFunc::Precision});
    const Point upperBound({SpecFunc::MaxScalar});
    const Interval::BoolCollection finiteLowerBound({true});
    const Interval::BoolCollection finiteUpperBound({false});
    problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

    // 1+xi*zi/sigma > 0
    Description formulas(2);
    formulas[0] = OSS() << "sigma + "<<xi0<<" * (" << zMax_ << " - " << u_ << ")";
    formulas[1] = OSS() << "sigma + "<<xi0<<" * (" << zMin_ << " - " << u_ << ")";
    const SymbolicFunction constraint(Description({"sigma"}), formulas);
    problem.setInequalityConstraint(constraint);

    Sample z(0, 1);
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
      if (sample_(i, 0) > u_)
        z.add(Point(1, sample_(i, 0) - u_));

    if (z.getSize() < 2)
      return Point(1, -std::log(SpecFunc::MaxScalar));

    const Scalar sigma0 = std::sqrt(6.0 * z.computeCovariance()(0, 0)) / M_PI;

    // solve optimization problem
    OptimizationAlgorithm solver(solver_);
    solver.setProblem(problem);
    solver.setStartingPoint({sigma0});
    try
    {
      solver.run();
      optimalPoint_ = solver.getResult().getOptimalPoint();
      const Point optimalValue(solver.getResult().getOptimalValue());
      return optimalValue;
    }
    catch (const Exception & ex)
    {
      return Point(1, -std::log(SpecFunc::MaxScalar));
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  Scalar zMin_ = 0.0;
  Scalar zMax_ = 0.0;
  mutable Point optimalPoint_;
  OptimizationAlgorithm solver_;
  Scalar u_ = 0.0;
};


ProfileLikelihoodResult GeneralizedParetoFactory::buildMethodOfXiProfileLikelihoodEstimator(const Sample & sample, const Scalar u) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Scalar zMin = sample.getMin()[0];
  const Scalar zMax = sample.getMax()[0];
  const Scalar xi0 = 0.1;

  const GeneralizedParetoProfileLikelihoodEvaluation profileLikelihoodEvaluation(sample, zMin, zMax, solver_, u);
  const Function objective(profileLikelihoodEvaluation.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint({xi0});
  solver.run();

  // rerun once to get optimal sigma at optimal xi
  const Scalar xi = solver.getResult().getOptimalPoint()[0];
  profileLikelihoodEvaluation(solver.getResult().getOptimalPoint());
  Point optimalParameter(profileLikelihoodEvaluation.getOptimalPoint()); // sigma
  optimalParameter.add(xi);
  optimalParameter.add(u);

  const Distribution distribution(buildAsGeneralizedPareto(optimalParameter));
//   Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));
//   parameterDistribution.setDescription({"sigma", "xi", "u"});
  Distribution parameterDistribution(Normal(3));
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  // Compute the extreme possible values for xi given the sample and (mu, sigma)
  /*
  const Scalar mu = optimalParameter[0];
  const Scalar sigma = optimalParameter[1];
  Scalar xiMin;
  if (zMax > mu)
    xiMin = -sigma / (zMax - mu);
  else
    xiMin = -SpecFunc::MaxScalar;
  Scalar xiMax;
  if (zMin < mu)
    xiMax = sigma / (mu - zMin);
  else
    xiMax = SpecFunc::MaxScalar;
  */
//   const Scalar uMin = 30-zMin;
//   const Scalar uMax = 2*zMax+30;
  const Scalar xiMin = -SpecFunc::MaxScalar;
  const Scalar xiMax = SpecFunc::MaxScalar;
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, xi, xiMin, xiMax);
  return result;
}

GeneralizedPareto GeneralizedParetoFactory::buildMethodOfXiProfileLikelihood(const Sample & sample, const Scalar u) const
{
  const Distribution distribution(buildMethodOfXiProfileLikelihoodEstimator(sample, u).getDistribution());
  return buildAsGeneralizedPareto(distribution.getParameter());
}


GridLayout GeneralizedParetoFactory::drawParameterThresholdStability(const Sample & sample, const Interval & thresholdRange) const
{
  const UnsignedInteger size = sample.getSize();
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Can only parameter threshold stability from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (thresholdRange.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Threshold range should be of dimension 1, here dimension=" << thresholdRange.getDimension();
  if (size < 20)
    throw InvalidArgumentException(HERE) << "Size must be at least 20, here size=" << size;

  const Scalar uMin = thresholdRange.getLowerBound()[0];
  const Scalar uMax = thresholdRange.getUpperBound()[0];
  const UnsignedInteger pointsNumber = ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-ThresholdStabilityPointNumber");
  const Scalar level = ResourceMap::GetAsScalar("GeneralizedParetoFactory-ThresholdStabilityConfidenceLevel");
  const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);

  Sample uS(pointsNumber, 1);
  Sample xiS(pointsNumber, 1);
  Sample xiCILow(pointsNumber, 1);
  Sample xiCIUp(pointsNumber, 1);

  Sample scaleS(pointsNumber, 1);
  Sample scaleCILow(pointsNumber, 1);
  Sample scaleCIUp(pointsNumber, 1);

  for (UnsignedInteger i = 0; i < pointsNumber; ++ i)
  {
    const Scalar u = uMin + i * (uMax - uMin) / (pointsNumber - 1);
    uS(i, 0) = u;
    const DistributionFactoryLikelihoodResult resultI(buildMethodOfLikelihoodMaximizationEstimator(sample, u));
    const Point parameter(resultI.getDistribution().getParameter());
    const Scalar sigma = parameter[0];
    const Scalar xi = parameter[1];

    // modified scale parameter: scale=sigma-xi*u
    const Scalar scale = sigma - xi * u;
    scaleS(i, 0) = scale;
    const CovarianceMatrix paramCov(resultI.getParameterDistribution().getCovariance());
    const Scalar varSigma = paramCov(0, 0);
    const Scalar stddevXi = std::sqrt(paramCov(1, 1));
    const Scalar covSigmaXi = paramCov(0, 1);
    const Scalar scaleStddev = std::sqrt(varSigma - 2.0 * u * covSigmaXi + std::pow(u * stddevXi, 2.0));
    scaleCILow(i, 0) = scale - scaleStddev * xq;
    scaleCIUp(i, 0) = scale + scaleStddev * xq;

    // shape parameter xi
    xiS(i, 0) = xi;
    const Interval xiCI(resultI.getParameterDistribution().getMarginal(1).computeBilateralConfidenceInterval(level));
    xiCILow(i, 0) = xiCI.getLowerBound()[0];
    xiCIUp(i, 0) = xiCI.getUpperBound()[0];
  }

  // scale
  Curve curveScale(uS, scaleS, "scale");
  curveScale.setColor("red");
  Curve curveScaleCILow(uS, scaleCILow, "CI low");
  curveScaleCILow.setLineStyle("dashed");
  Curve curveScaleCIUp(uS, scaleCIUp, "CI up");
  curveScaleCIUp.setLineStyle("dashed");
  Graph scaleGraph("Modified scale threshold stability", "", "Modified scale parameter", true, "topleft");
  scaleGraph.add(curveScale);
  scaleGraph.add(curveScaleCILow);
  scaleGraph.add(curveScaleCIUp);

  // shape
  Curve curveXi(uS, xiS, "xi");
  curveXi.setColor("red");
  Curve curveXiCILow(uS, xiCILow, "CI low");
  curveXiCILow.setLineStyle("dashed");
  Curve curveXiCIUp(uS, xiCIUp, "CI up");
  curveXiCIUp.setLineStyle("dashed");
  Graph shapeGraph("Shape threshold stability", "Threshold", "Shape parameter", true, "topleft");
  shapeGraph.add(curveXi);
  shapeGraph.add(curveXiCILow);
  shapeGraph.add(curveXiCIUp);

  GridLayout grid(2, 1);
  grid.setGraph(0, 0, scaleGraph);
  grid.setGraph(1, 0, shapeGraph);
  return grid;
}

END_NAMESPACE_OPENTURNS
