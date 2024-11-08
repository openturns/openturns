//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedPareto distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BlockIndependentDistribution.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/JointDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedParetoFactory)

static const Factory<GeneralizedParetoFactory> Factory_GeneralizedParetoFactory;

/* Default constructor */
GeneralizedParetoFactory::GeneralizedParetoFactory()
  : DistributionFactoryImplementation()
{
  // Create the optimization solver parameters using the parameters in the ResourceMap
  solver_ = OptimizationAlgorithm::GetByName(ResourceMap::GetAsString("GeneralizedParetoFactory-DefaultOptimizationAlgorithm"));
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-MaximumCallsNumber"));
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
        const String message("Cannot use exponential regression to estimate a GeneralizedPareto distribution when the minimum value of the sample is repeated");
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
  const Scalar uMax = sample.getMax()[0];
  const UnsignedInteger pointsNumber = ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-MeanResidualLifePointNumber");
  Sample u(pointsNumber, 1);
  Sample mrl(pointsNumber, 1);
  Sample ciLow(pointsNumber, 1);
  Sample ciUp(pointsNumber, 1);
  const Scalar level = ResourceMap::GetAsScalar("GeneralizedParetoFactory-MeanResidualLifeConfidenceLevel");
  const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
  for (UnsignedInteger i = 0; i < pointsNumber; ++ i)
  {
    u(i, 0) = uMin + i * (uMax - uMin) / (pointsNumber + 1);

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
  curveCILow.setColor("blue");
  curveCILow.setLineStyle("dashed");
  Curve curveCIUp(u, ciUp, "CI up");
  curveCIUp.setColor("blue");
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
    if (sigma <= 0.0)
      return {-SpecFunc::LogMaxScalar};
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
            ll += -SpecFunc::LogMaxScalar;
            continue;
          }
          ll += (-1.0 / xi - 1.0) * std::log1p(c1);
        }
        ++ n;
      }
    }
    ll += - 1.0 * n * std::log(sigma);
    return {ll};
  }

private:
  Sample sample_;
  UnsignedInteger m_ = 0;
  Scalar u_ = 0.0;
};


DistributionFactoryLikelihoodResult GeneralizedParetoFactory::buildMethodOfLikelihoodMaximizationEstimatorWithStartingPoint(const Sample & sample, const Point & startingPoint,
    const Scalar u) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of size>=2, here size=" << sample.getSize();

  const Function objective(new GeneralizedParetoLikelihoodEvaluation(sample, u));
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // sigma > 0
  const Point lowerBound({SpecFunc::Precision, -SpecFunc::Infinity});
  const Point upperBound(2, SpecFunc::Infinity);
  const Interval::BoolCollection finiteLowerBound({true, false});
  const Interval::BoolCollection finiteUpperBound(2, false);
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

  // 1+xi(zi-mu)/sigma > 0 for all order statistics taken into account
  const Scalar zMin = sample.getMin()[0];
  const Scalar zMax = sample.getMax()[0];

  Description formulas(2);
  formulas[0] = OSS() << "sigma + xi * (" << zMax << " - " << u << ")";
  formulas[1] = OSS() << "sigma + xi * (" << zMin << " - " << u << ")";
  const SymbolicFunction constraint(Description({"sigma", "xi"}), formulas);
  problem.setInequalityConstraint(constraint);

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(startingPoint);
  solver.run();
  Point optimalParameter(solver.getResult().getOptimalPoint()); // sigma, xi

  // distribution of (sigma,xi)
  const SymmetricMatrix hessian(objective.hessian(optimalParameter).getSheet(0) * -1.0);
  const CovarianceMatrix covariance(hessian.solveLinearSystem(IdentityMatrix(2)).getImplementation());
  Normal sigmaXiDistribution(optimalParameter, covariance);
  sigmaXiDistribution.setDescription({"sigma", "xi"});

  // distribution of (sigma,xi,u)
  optimalParameter.add(u);
  const Distribution distribution(buildAsGeneralizedPareto(optimalParameter));
  Dirac uDistribution(u);
  uDistribution.setDescription({"u"});
  const BlockIndependentDistribution parameterDistribution({sigmaXiDistribution, uDistribution});
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  DistributionFactoryLikelihoodResult result(distribution, parameterDistribution, logLikelihood);
  return result;
}

DistributionFactoryLikelihoodResult GeneralizedParetoFactory::buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample,
    const Scalar u) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of size>=2, here size=" << sample.getSize();

  Sample xu(0, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (sample(i, 0) > u)
      xu.add(sample[i]);
  const Scalar nxu = xu.getSize();
  if (nxu < 10)
    throw InvalidArgumentException(HERE) << "Not enough points, lower the threshold u";

  // starting point: initialize sigma as Gumbel parametrization, xi is arbitrary (see ismev package)
  const Scalar sigma0 = std::sqrt(6.0 * xu.computeCovariance()(0, 0)) / M_PI;
  const Scalar xi0 = 0.1;
  const Point x0({sigma0, xi0});

  return buildMethodOfLikelihoodMaximizationEstimatorWithStartingPoint(sample, x0, u);
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
    const Point upperBound({SpecFunc::Infinity});
    const Interval::BoolCollection finiteLowerBound({true});
    const Interval::BoolCollection finiteUpperBound({false});
    problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

    // 1+xi*zi/sigma > 0
    Description formulas(2);
    formulas[0] = OSS() << "sigma + " << xi0 << " * (" << zMax_ << " - " << u_ << ")";
    formulas[1] = OSS() << "sigma + " << xi0 << " * (" << zMin_ << " - " << u_ << ")";
    const SymbolicFunction constraint(Description({"sigma"}), formulas);
    problem.setInequalityConstraint(constraint);

    Sample z(0, 1);
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
      if (sample_(i, 0) > u_)
        z.add(Point(1, sample_(i, 0) - u_));

    if (z.getSize() < 2)
      return {-SpecFunc::LogMaxScalar};

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
    catch (const Exception &)
    {
      return {-SpecFunc::LogMaxScalar};
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
  Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));
  parameterDistribution.setDescription({"sigma", "xi", "u"});
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  // Compute the extreme possible values for xi given the sample and (mu, sigma)
  /*
  const Scalar mu = optimalParameter[0];
  const Scalar sigma = optimalParameter[1];
  Scalar xiMin;
  if (zMax > mu)
    xiMin = -sigma / (zMax - mu);
  else
    xiMin = -SpecFunc::Infinity;
  Scalar xiMax;
  if (zMin < mu)
    xiMax = sigma / (mu - zMin);
  else
    xiMax = SpecFunc::Infinity;
  */
//   const Scalar uMin = 30-zMin;
//   const Scalar uMax = 2*zMax+30;
  const Scalar xiMin = -SpecFunc::Infinity;
  const Scalar xiMax = SpecFunc::Infinity;
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

  // find the first parameters from scratch
  const DistributionFactoryLikelihoodResult result0(buildMethodOfLikelihoodMaximizationEstimator(sample, uMin));
  Point parameter(result0.getDistribution().getParameter());

  for (UnsignedInteger i = 0; i < pointsNumber; ++ i)
  {
    const Scalar u = uMin + i * (uMax - uMin) / (pointsNumber - 1);
    uS(i, 0) = u;
    const Point x0 = {parameter[0], parameter[1]}; // xi, sigma
    // reuse the parameters from the previous iteration
    const DistributionFactoryLikelihoodResult resultI(buildMethodOfLikelihoodMaximizationEstimatorWithStartingPoint(sample, x0, u));
    parameter = (resultI.getDistribution().getParameter());
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
  curveScaleCILow.setColor("blue");
  curveScaleCILow.setLineStyle("dashed");
  Curve curveScaleCIUp(uS, scaleCIUp, "CI up");
  curveScaleCIUp.setColor("blue");
  curveScaleCIUp.setLineStyle("dashed");
  Graph scaleGraph("Modified scale threshold stability", "", "Modified scale parameter", true, "topleft");
  scaleGraph.add(curveScale);
  scaleGraph.add(curveScaleCILow);
  scaleGraph.add(curveScaleCIUp);

  // shape
  Curve curveXi(uS, xiS, "xi");
  curveXi.setColor("red");
  Curve curveXiCILow(uS, xiCILow, "CI low");
  curveXiCILow.setColor("blue");
  curveXiCILow.setLineStyle("dashed");
  Curve curveXiCIUp(uS, xiCIUp, "CI up");
  curveXiCIUp.setColor("blue");
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


class GeneralizedParetoCovariatesLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedParetoCovariatesLikelihoodEvaluation(const Sample & sample,
      const Scalar u,
      const Matrix & sigmaCovariates,
      const Matrix & xiCovariates,
      const Function & sigmaLink,
      const Function & xiLink,
      const Scalar startingValue)
    : EvaluationImplementation()
    , sample_(sample)
    , u_(u)
    , sigmaCovariates_(sigmaCovariates)
    , xiCovariates_(xiCovariates)
    , sigmaLink_(sigmaLink.getEvaluation().getImplementation()->isActualImplementation() ? sigmaLink : IdentityFunction(1))
    , xiLink_(xiLink.getEvaluation().getImplementation()->isActualImplementation() ? xiLink : IdentityFunction(1))
    , sigmaDim_(sigmaCovariates.getNbColumns())
    , xiDim_(xiCovariates.getNbColumns())
    , startingValue_(startingValue)
  {
    // Nothing to do
  }

  GeneralizedParetoCovariatesLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedParetoCovariatesLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return sigmaDim_ + xiDim_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 3;
  }

  Point operator() (const Point & beta) const override
  {
    // Sigma
    Point betaSigma(sigmaDim_);
    std::copy(beta.begin(), beta.begin() + sigmaDim_, betaSigma.begin());
    const Sample sigmaT(Sample::BuildFromPoint(sigmaCovariates_ * betaSigma));
    UnsignedInteger shift = sigmaDim_;
    // Xi
    Point betaXi(xiDim_);
    std::copy(beta.begin() + shift, beta.begin() + shift + xiDim_, betaXi.begin());
    const Sample xiT(Sample::BuildFromPoint(xiCovariates_ * betaXi));
    shift += xiDim_;

    Scalar ll = startingValue_;
    Scalar minSigma = SpecFunc::MaxScalar;
    Scalar minC1 = SpecFunc::MaxScalar;
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
    {
      const Scalar sigma = sigmaLink_(sigmaT[i])[0];
      const Scalar xi = xiLink_(xiT[i])[0];
      minSigma = std::min(minSigma, sigma);
      const Scalar zi = sample_(i, 0) - u_;
      LOGDEBUG(OSS() << "i=" << i << ", u=" << u_ << ", sigma=" << sigma << ", xi=" << xi << ", zi=" << zi);
      if (zi > 0.0)
      {
        if (std::abs(xi) < SpecFunc::Precision)
        {
          ll += - 1.0 * zi / sigma;
        }
        else
        {
          const Scalar c1 = xi * zi / sigma;
          minC1 = std::min(minC1, 1.0 + c1);
          if (c1 <= SpecFunc::Precision - 1.0) // can be slightly off
          {
            ll += -SpecFunc::LogMaxScalar;
            continue;
          }
          ll += (-1.0 / xi - 1.0) * std::log1p(c1);
        }
        ll -= std::log(sigma);
      }
    }
    LOGTRACE(OSS(false) << "covariates log-likelihood beta=" << beta << ", log-likelihood=" << ll << ", min_t sigma(t)=" << minSigma << ", min_t c1(t)=" << minC1);
    return {ll, minSigma, minC1};
  }

  void setStartingValue(const Scalar startingValue)
  {
    startingValue_ = startingValue;
  }

private:
  Sample sample_;
  Scalar u_ = 0.0;
  Matrix sigmaCovariates_;
  Matrix xiCovariates_;
  Function sigmaLink_;
  Function xiLink_;
  UnsignedInteger sigmaDim_ = 0;
  UnsignedInteger xiDim_ = 0;
  Scalar startingValue_ = 0.0;
};



class GeneralizedParetoPDFEvaluation : public EvaluationImplementation
{
public:
  GeneralizedParetoPDFEvaluation()
    : EvaluationImplementation()
  {
    // Nothing to do
  }

  GeneralizedParetoPDFEvaluation * clone() const override
  {
    return new GeneralizedParetoPDFEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 3;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & theta) const override
  {
    const Scalar logPDF = factory_.buildAsGeneralizedPareto(theta).computeLogPDF(X_);
    return {logPDF};
  }

  static void SetX(const Point & x)
  {
    X_ = x;
  }

private:
  GeneralizedParetoFactory factory_;
  static Point X_;
};

Point GeneralizedParetoPDFEvaluation::X_;

/** Covariates */
CovariatesResult GeneralizedParetoFactory::buildCovariates(const Sample & sample,
    const Scalar u,
    const Sample & covariates0,
    const Indices & sigmaIndices0,
    const Indices & xiIndices0,
    const Function & sigmaLink,
    const Function & xiLink,
    const String & initializationMethod,
    const String & normalizationMethod) const
{
  const UnsignedInteger size = sample.getSize();

  if (size < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (covariates0.getSize() != size)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only if the sample of covariates has the same size as the sample of observations";

  UnsignedInteger covariatesDimension = covariates0.getDimension();
  if (!sigmaIndices0.check(covariatesDimension))
    throw InvalidArgumentException(HERE) << "Error: the indices for sigma are not compatible with the covariates dimension";
  if (!xiIndices0.check(covariatesDimension))
    throw InvalidArgumentException(HERE) << "Error: the indices for xi are not compatible with the covariates dimension";

  // h is optional
  if (sigmaLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (sigmaLink.getInputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from an inverse link function of input dimension 1, here dimension=" << sigmaLink.getInputDimension();
    if (sigmaLink.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from an inverse link function of input dimension 1, here dimension=" << sigmaLink.getInputDimension();
  }
  if (xiLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (xiLink.getInputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from an inverse link function of input dimension 1, here dimension=" << xiLink.getInputDimension();
    if (xiLink.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from an inverse link function of input dimension 1, here dimension=" << xiLink.getInputDimension();
  }

  // check for constant covariate
  const Point sigmaCov(covariates0.computeStandardDeviation());
  UnsignedInteger constantCovariateIndex = covariatesDimension;
  for (UnsignedInteger j = 0; j < covariatesDimension; ++ j)
  {
    if (!(sigmaCov[j] > 0.0))
    {
      if (constantCovariateIndex == covariatesDimension)
        constantCovariateIndex = j;
      else
        throw InvalidArgumentException(HERE) << "Cannot provide more than one constant covariate";
    }
  }

  // add constant covariate column if needed
  Sample covariates(covariates0);
  Indices sigmaIndices(sigmaIndices0);
  Indices xiIndices(xiIndices0);
  if (constantCovariateIndex == covariatesDimension)
  {
    covariates.stack(Sample(size, Point({1.0})));
    sigmaIndices.add(covariatesDimension);
    xiIndices.add(covariatesDimension);
    ++ covariatesDimension;
  }

  // the provided constant covariate must be non-null
  if (std::abs(covariates(0, constantCovariateIndex)) < SpecFunc::Precision)
    throw InvalidArgumentException(HERE) << "Null constant covariate at index " << constantCovariateIndex;

  // indices must at least reference the constant covariate
  if (!sigmaIndices.contains(constantCovariateIndex))
    sigmaIndices.add(constantCovariateIndex);
  if (!xiIndices.contains(constantCovariateIndex))
    xiIndices.add(constantCovariateIndex);

  // Get an initial guest for (sigma, xi, u) as if they were constant
  Point initialGuess;
  LOGINFO(OSS() << "Initialization method is \"" << initializationMethod << "\"");
  const UnsignedInteger sigmaDim = sigmaIndices.getSize();
  const UnsignedInteger xiDim = xiIndices.getSize();
  if (initializationMethod == "Generic")
  {
    Sample xu(0, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
      if (sample(i, 0) > u)
        xu.add(sample[i]);
    initialGuess = buildAsGeneralizedPareto(xu).getParameter();
  }
  else if (initializationMethod == "Static")
  {
    initialGuess = buildMethodOfLikelihoodMaximization(sample, u).getParameter();
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << initializationMethod << " is invalid for the \"GeneralizedParetoFactory-InitializationMethod\" key in ResourceMap. Valid values are \"Static\" and \"Generic\"";
  LOGINFO(OSS(false) << "In buildCovariates, initial guess=" << initialGuess);

  // normalize covariates
  Point center(covariatesDimension);
  const Point constant(covariatesDimension);
  SquareMatrix linear(covariatesDimension);
  if (normalizationMethod == "CenterReduce")
  {
    center = covariates.computeMean();
    const Point stdCovariates = covariates.computeStandardDeviation();
    for (UnsignedInteger i = 0; i < covariatesDimension; ++ i)
      linear(i, i) = (stdCovariates[i] > 0.0 ? 1.0 / stdCovariates[i] : 1.0);
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", center=" << center << ", linear=" << linear);
  }
  else if (normalizationMethod == "MinMax")
  {
    const Point minCovariates = covariates.getMin();
    const Point maxCovariates = covariates.getMax();
    for (UnsignedInteger i = 0; i < covariatesDimension; ++ i)
      linear(i, i) = (minCovariates[i] < maxCovariates[i] ? 1.0 / (maxCovariates[i] - minCovariates[i]) : 1.0);
    center = minCovariates;
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", center=" << center << ", linear=" << linear);
  }
  else if (normalizationMethod == "None")
  {
    linear = IdentityMatrix(covariatesDimension);
    LOGINFO("No normalization of the covariates");
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << normalizationMethod << " is invalid for the \"GeneralizedParetoFactory-NormalizationMethod\" key in ResourceMap. Valid values are \"MinMax\", \"CenterReduce\", \"None\"";

  // normalization should not nullify constant column
  if (constantCovariateIndex < covariatesDimension)
    center[constantCovariateIndex] = 0.0;

  const LinearFunction normalizationFunction(center, constant, linear);
  const Sample normalizedCovariates(normalizationFunction(covariates));

  // Extract the 3 matrices corresponding to the covariates for sigma, xi and u
  const Matrix sigmaCovariates(Matrix(sigmaIndices.getSize(), normalizedCovariates.getSize(), normalizedCovariates.getMarginal(sigmaIndices).getImplementation()->getData()).transpose());
  const Matrix xiCovariates(Matrix(xiIndices.getSize(), normalizedCovariates.getSize(), normalizedCovariates.getMarginal(xiIndices).getImplementation()->getData()).transpose());

  // Conpute the log-likelihood associated to the initial point with a zero reference value in order to find a feasible initial point
  GeneralizedParetoCovariatesLikelihoodEvaluation evaluation(sample, u, sigmaCovariates, xiCovariates, sigmaLink, xiLink, 0.0);

  // set initial guess on coefficients for constant covariate
  Point x0(sigmaDim + xiDim);
  if (sigmaIndices.contains(constantCovariateIndex))
    x0[sigmaIndices.find(constantCovariateIndex)] = initialGuess[0];
  UnsignedInteger shift = sigmaDim;
  if (xiIndices.contains(constantCovariateIndex))
    x0[shift + xiIndices.find(constantCovariateIndex)] = initialGuess[1];

  LOGINFO(OSS(false) << "Starting points for the coefficients=" << x0);

  // Now take into account the initial log-likelihood in order to work on the log-likelihood improvement during the optimization step
  // It gives a more robust stopping criterion
  const Scalar startingValue = -evaluation(x0)[0];
  evaluation.setStartingValue(startingValue);

  const Function objectiveAndConstraints(evaluation.clone());
  const Function objective(objectiveAndConstraints.getMarginal(0));
  const Function inequalities(objectiveAndConstraints.getMarginal(Indices({1, 2})));
  OptimizationProblem problem(objective);
  problem.setInequalityConstraint(inequalities);
  problem.setMinimization(false);

  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0] - startingValue;
  LOGINFO(OSS(false) << "Optimal coefficients=" << optimalParameter << ", optimal log-likelihood=" << logLikelihood);

  // reorder the normalization coefficients for the beta coefficients
  const UnsignedInteger nP = sigmaDim + xiDim;
  Point alpha(nP);
  Point gamma(nP);
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
  {
    gamma[i] = center[sigmaIndices[i]];
    alpha[i] = linear(sigmaIndices[i], sigmaIndices[i]);
  }
  shift = sigmaDim;
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
  {
    gamma[shift + i] = center[xiIndices[i]];
    alpha[shift + i] = linear(xiIndices[i], xiIndices[i]);
  }

  // compute the beta coefficients from the beta coefficients matching the normalized covariates
  Point optimalBeta(optimalParameter);
  Scalar offset = 0.0;
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
  {
    if (sigmaIndices[i] != constantCovariateIndex)
    {
      optimalBeta[i] *= alpha[i];
      offset += optimalParameter[i] * alpha[ i] * gamma[i];
    }
  }

  // report the centering coefficients on the constant term
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
    if (sigmaIndices[i] == constantCovariateIndex)
      optimalBeta[i] -= offset;

  shift = sigmaDim;
  offset = 0.0;
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
  {
    if (xiIndices[i] != constantCovariateIndex)
    {
      optimalBeta[shift + i] *= alpha[shift + i];
      offset += optimalParameter[shift + i] * alpha[shift + i] * gamma[shift + i];
    }
  }

  // report the centering coefficients on the constant term
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
    if (xiIndices[i] == constantCovariateIndex)
      optimalBeta[shift + i] -= offset;

  LOGINFO(OSS(false) << "Optimal unnormalized coefficients=" << optimalBeta);

  // Build the theta function which maps a dim(covariates) vector into a (sigma, xi, u) vector.
  const Description sigmaBetaDesc(Description::BuildDefault(sigmaDim, "sigmaBeta"));
  const Description xiBetaDesc(Description::BuildDefault(xiDim, "xiBeta"));
  const Description yDesc(Description::BuildDefault(covariatesDimension, "y"));
  Description sigmaVars(sigmaBetaDesc);
  Description xiVars(xiBetaDesc);
  sigmaVars.add(yDesc);
  xiVars.add(yDesc);
  String sigmaFormula;
  String xiFormula;
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
    sigmaFormula += OSS() << sigmaBetaDesc[i] << " * " << yDesc[sigmaIndices[i]] << (i < sigmaDim - 1 ? " + " : "");
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
    xiFormula += OSS() << xiBetaDesc[i] << " * " << yDesc[xiIndices[i]] << (i < xiDim - 1 ? " + " : "");

  // use beta variables as parameters
  Description thetaBetaVars(sigmaBetaDesc);
  thetaBetaVars.add(xiBetaDesc);
  thetaBetaVars.add(yDesc);
  Indices betaVarsIndices(sigmaDim + xiDim);
  betaVarsIndices.fill();
  const String uFormula = (OSS() << u);
  SymbolicFunction thetaBetaFunction(thetaBetaVars, {sigmaFormula, xiFormula, uFormula});
  Function thetaFunction = ParametricFunction(thetaBetaFunction, betaVarsIndices, optimalBeta);

  // The theta function is the composition between the inverse link function and the linear function
  if (sigmaLink.getEvaluation().getImplementation()->isActualImplementation()
      || xiLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    Function link1(sigmaLink.getEvaluation().getImplementation()->isActualImplementation() ? sigmaLink : IdentityFunction(1));
    link1 = ComposedFunction(link1, SymbolicFunction({"x1", "x2", "x3"}, {"x1"}));
    Function link2(xiLink.getEvaluation().getImplementation()->isActualImplementation() ? xiLink : IdentityFunction(1));
    link2 = ComposedFunction(link2, SymbolicFunction({"x1", "x2", "x3"}, {"x2"}));
    Function link3 = SymbolicFunction({"x1", "x2", "x3"}, {"x3"});
    AggregatedFunction thetaLink({link1, link2, link3});
    thetaFunction = ComposedFunction(thetaLink, thetaFunction);
  }

  // useful for the theta(y) graphs
  thetaFunction.setOutputDescription({"$\\sigma$", "$\\xi$", "u"});

  // compose the y->theta->pdf function
  GeneralizedParetoPDFEvaluation pdfFunction;
  ComposedFunction yToPDF(pdfFunction, thetaFunction);

  Distribution parameterDistribution;
  try
  {
    // estimate parameter distribution via the Fisher information matrix
    Matrix fisher(nP, nP);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      // set the location through a global variable
      GeneralizedParetoPDFEvaluation::SetX(sample[i]);

      // compute the jacobian wrt the beta coefficients
      const Matrix dpdfi(yToPDF.parameterGradient(covariates[i]));
      fisher = fisher + dpdfi.computeGram(false);
    }
    const CovarianceMatrix covariance(SymmetricMatrix(fisher.getImplementation()).solveLinearSystem(IdentityMatrix(nP) / size).getImplementation());
    parameterDistribution = Normal(optimalBeta, covariance);
  }
  catch (const Exception &)
  {
    parameterDistribution = JointDistribution({Dirac(optimalBeta[0]), Dirac(optimalBeta[1]), Dirac(optimalBeta[2])});
    LOGWARN("Could not compute GPD covariates parameter distribution covariance");
  }

  const CovariatesResult result(*this, thetaFunction, covariates, parameterDistribution, normalizationFunction, logLikelihood);
  return result;
}


TimeVaryingResult GeneralizedParetoFactory::buildTimeVarying(const Sample & sample,
    const Scalar u,
    const Sample & timeStamps,
    const Basis & basis,
    const Indices & sigmaIndices,
    const Indices & xiIndices,
    const Function & sigmaLink,
    const Function & xiLink,
    const String & initializationMethod,
    const String & normalizationMethod) const
{
  if (timeStamps.getSize() != sample.getSize())
    throw InvalidArgumentException(HERE) << "GeneralizedPareto timeStamps size (" << timeStamps.getSize() << ") must match sample size (" << sample.getSize() << ")";
  if (timeStamps.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << timeStamps.getDimension();
  if (!basis.getSize())
    throw InvalidArgumentException(HERE) << "Basis is empty";
  if (!sigmaIndices.check(basis.getSize()))
    throw InvalidArgumentException(HERE) << "Error: the indices for sigma are not compatible with the basis size";
  if (!xiIndices.check(basis.getSize()))
    throw InvalidArgumentException(HERE) << "Error: the indices for xi are not compatible with the basis size";

  // normalize timestamps
  LinearFunction normalizationFunction(Point(1), Point(1), IdentityMatrix(1));
  if (normalizationMethod == "CenterReduce")
  {
    const Scalar meanTimeStamps = timeStamps.computeMean()[0];
    const Scalar stdTimeStamps = timeStamps.computeStandardDeviation()[0];
    SymmetricMatrix linear(1);
    linear(0, 0) = (stdTimeStamps > 0.0 ? 1.0 / stdTimeStamps : 1.0);
    normalizationFunction = LinearFunction(Point(1, meanTimeStamps), Point(1), linear);
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", normalization function=" << normalizationFunction);
  }
  else if (normalizationMethod == "MinMax")
  {
    const Scalar minTimeStamps = timeStamps.getMin()[0];
    const Scalar maxTimeStamps = timeStamps.getMax()[0];
    SymmetricMatrix linear(1);
    linear(0, 0) = (minTimeStamps < maxTimeStamps ? 1.0 / (maxTimeStamps - minTimeStamps) : 1.0);
    normalizationFunction = LinearFunction(Point(1, minTimeStamps), Point(1), linear);
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", normalization function=" << normalizationFunction);
  }
  else if (normalizationMethod == "None")
  {
    LOGINFO("No normalization of the timeStamps");
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << normalizationMethod << " is invalid for the \"GeneralizedParetoFactory-NormalizationMethod\" key in ResourceMap. Valid values are \"MinMax\", \"CenterReduce\", \"None\"";

  // evaluate covariates from basis, without duplicates basis terms
  Indices allIndices;
  allIndices.add(sigmaIndices);
  allIndices.add(xiIndices);
  Indices uniqueIndices;
  Collection<Function> yBasis;
  for (UnsignedInteger j = 0; j < allIndices.getSize(); ++ j)
  {
    const UnsignedInteger currentIndex = allIndices[j];
    if (!uniqueIndices.contains(currentIndex))
    {
      const Function phi(basis[currentIndex]);
      yBasis.add(phi);
      uniqueIndices.add(currentIndex);
    }
  }
  Function timeToY = AggregatedFunction(yBasis);
  if (normalizationMethod != "None")
    timeToY = ComposedFunction(timeToY, normalizationFunction);
  const Sample covariates = timeToY(timeStamps);
  const CovariatesResult covariatesResult(buildCovariates(sample, u, covariates, sigmaIndices, xiIndices, sigmaLink, xiLink, initializationMethod, "None"));

  // compose the parameter function: t(->tau)->y->theta
  Function parameterFunction(covariatesResult.getParameterFunction());
  parameterFunction = ComposedFunction(parameterFunction, timeToY);

  const Distribution parameterDistribution(covariatesResult.getParameterDistribution());
  const Scalar logLikelihood = covariatesResult.getLogLikelihood();
  const TimeVaryingResult result(*this, sample, parameterFunction, timeStamps, parameterDistribution, normalizationFunction, logLikelihood);
  return result;
}


/* Return level */
Distribution GeneralizedParetoFactory::buildReturnLevelEstimator(const DistributionFactoryResult & result, const Sample & sample,
    const Scalar m, const Scalar theta) const
{
  // see coles2001 4.3.3 p81
  if (result.getDistribution().getImplementation()->getClassName() != "GeneralizedPareto")
    throw InvalidArgumentException(HERE) << "Return level can only be estimated from a GPD";
  if (!(m > 1.0))
    throw InvalidArgumentException(HERE) << "Return period should be > 1";
  const Scalar sigma = result.getDistribution().getParameter()[0];
  const Scalar xi = result.getDistribution().getParameter()[1];
  const Scalar u = result.getDistribution().getParameter()[2];

  UnsignedInteger k = 0;
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Return level estimation requires a sample of dimension 1";
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (sample(i, 0) > u)
      ++ k;
  if (!k)
    throw InvalidArgumentException(HERE) << "Return level estimation requires sample values > u";
  const Scalar zeta_u = k * 1.0 / size;
  const Scalar zeta = zeta_u * theta;

  // (sigma, xi) are Gaussian, u can be a Dirac
  if (result.getParameterDistribution().getMarginal(Indices({0, 1})).getImplementation()->getClassName() == "Normal")
  {
    Scalar xm = 0.0;
    Matrix dxm(3, 1);
    if (std::abs(xi) < SpecFunc::Precision)
    {
      xm = u + sigma * std::log(m * zeta);
      dxm(0, 0) = sigma / zeta;
      dxm(1, 0) = std::log(m * zeta);
    }
    else
    {
      xm = u + sigma * (std::pow(m * zeta, xi) - 1.0) / xi;
      dxm(0, 0) = sigma * std::pow(m, xi) * std::pow(zeta, xi - 1.0);
      dxm(1, 0) = (std::pow(m * zeta, xi) - 1.0) / xi;
      dxm(2, 0) = sigma / xi * (std::pow(m * zeta, xi) * std::log(m * zeta) - dxm(1, 0));
    }
    const Matrix Vn(result.getParameterDistribution().getCovariance());
    Matrix V(3, 3);
    V(0, 0) = zeta * (1.0 - zeta) / size;
    V(1, 1) = Vn(0, 0);
    V(1, 2) = Vn(0, 1);
    V(2, 1) = Vn(1, 0);
    V(2, 2) = Vn(1, 1);
    const Scalar varXm = (dxm.transpose() * V * dxm)(0, 0);
    return Normal(xm, std::sqrt(varXm));
  }
  else
  {
    // sample input distribution + kernel smoothing
    throw NotYetImplementedException(HERE) << "GPD parameter distribution is not Gaussian";
  }
}


class GeneralizedParetoReturnLevelProfileLikelihoodEvaluation2 : public EvaluationImplementation
{
public:
  GeneralizedParetoReturnLevelProfileLikelihoodEvaluation2(const Sample & sample, const Scalar u, const Scalar m, const Scalar zeta)
    : EvaluationImplementation()
    , llh_(new GeneralizedParetoLikelihoodEvaluation(sample, u))
    , u_(u)
    , m_(m)
    , zeta_(zeta)
  {
    // Nothing to do
  }

  GeneralizedParetoReturnLevelProfileLikelihoodEvaluation2 * clone() const override
  {
    return new GeneralizedParetoReturnLevelProfileLikelihoodEvaluation2(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & zParameter) const override
  {
    const Scalar zm = zParameter[0];
    const Scalar xi = zParameter[1];
    const Scalar sigma = (std::abs(xi) < SpecFunc::Precision) ? (zm - u_) / std::log(m_ * zeta_) : (zm - u_) * xi / (std::pow(m_ * zeta_, xi) - 1.0);
    const Point nativeParameter = {sigma, xi};

    const Point llh(llh_(nativeParameter));
    return llh;
  }

private:
  Function llh_;
  Scalar u_ = 0.0;
  Scalar m_ = 0.0;
  Scalar zeta_ = 0.0;
};

class GeneralizedParetoReturnLevelProfileLikelihoodEvaluation1 : public EvaluationImplementation
{
public:
  GeneralizedParetoReturnLevelProfileLikelihoodEvaluation1(const Sample & sample,
      const Scalar u,
      const Scalar xi0,
      const Scalar zeta,
      const Scalar m,
      const OptimizationAlgorithm & solver)
    : EvaluationImplementation()
    , sample_(sample)
    , u_(u)
    , xi0_(xi0)
    , zeta_(zeta)
    , m_(m)
    , solver_(solver)
  {
    // Nothing to do
  }

  GeneralizedParetoReturnLevelProfileLikelihoodEvaluation1 * clone() const override
  {
    return new GeneralizedParetoReturnLevelProfileLikelihoodEvaluation1(*this);
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
    return {"zm"};
  }

  Point operator() (const Point & parameter) const override
  {
    const Function objective(new GeneralizedParetoReturnLevelProfileLikelihoodEvaluation2(sample_, u_, m_, zeta_));
    const ParametricFunction objectiveZm(objective, Indices({0}), parameter);
    OptimizationProblem problem(objectiveZm);
    problem.setMinimization(false);

    const Point x0({xi0_});

    // solve optimization problem
    OptimizationAlgorithm solver(solver_);
    solver.setProblem(problem);
    solver.setStartingPoint(x0);
    try
    {
      solver.run();
      optimalPoint_ = solver.getResult().getOptimalPoint();
      const Point optimalValue(solver.getResult().getOptimalValue());
      return optimalValue;
    }
    catch (const Exception &)
    {
      return {-SpecFunc::LogMaxScalar};
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  Scalar u_ = 0.0;
  Scalar xi0_ = 0.0;
  Scalar zeta_ = 0.0;
  Scalar m_ = 0.0;
  mutable Point optimalPoint_;
  OptimizationAlgorithm solver_;
};

ProfileLikelihoodResult GeneralizedParetoFactory::buildReturnLevelProfileLikelihoodEstimator(const Sample & sample, const Scalar u,
    const Scalar m, const Scalar theta) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (!(m > 1.0))
    throw InvalidArgumentException(HERE) << "Return period should be > 1";

  UnsignedInteger k = 0;
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Return level estimation requires a sample of dimension 1";
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (sample(i, 0) > u)
      ++ k;
  if (!k)
    throw InvalidArgumentException(HERE) << "Return level estimation requires sample values > u";
  const Scalar zeta_u = k * 1.0 / size;
  const Scalar zeta = zeta_u * theta;

  // start from maximum likelihood
  const Distribution ref(buildMethodOfLikelihoodMaximization(sample, u));
  const Scalar sigma0 = ref.getParameter()[0];
  const Scalar xi0 = ref.getParameter()[1];
  const Scalar zm0 = u + sigma0 / xi0 * (std::pow(m * zeta, xi0) - 1.0);
  const Point x0({zm0});

  const GeneralizedParetoReturnLevelProfileLikelihoodEvaluation1 profileLikelihoodEvaluation(sample, u, xi0, zeta, m, solver_);
  const Function objective(profileLikelihoodEvaluation.clone());

  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
  solver.setStartingPoint(x0);
  solver.run();

  // rerun once to get optimal xi at optimal zm
  const Scalar zm = solver.getResult().getOptimalPoint()[0];
  profileLikelihoodEvaluation(solver.getResult().getOptimalPoint());
  const Scalar xi = profileLikelihoodEvaluation.getOptimalPoint()[0];
  const Scalar sigma = (zm - u) * xi / (std::pow(m * zeta, xi) - 1.0);
  const Point optimalParameter({sigma, xi, u});

  const Distribution distribution(buildAsGeneralizedPareto(optimalParameter));
  const Distribution nativeParameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));

  // delta method to transport native parametrization into zm parametrization
  Matrix dzm(IdentityMatrix(3));
  if (std::abs(xi) < SpecFunc::Precision)
  {
    dzm(0, 0) = std::log(m * zeta);
    dzm(2, 0) = 1.0;
  }
  else
  {
    dzm(0, 0) = (std::pow(m * zeta, xi) - 1.0) / xi;
    dzm(1, 0) = - sigma / (xi * xi) - std::exp(xi * std::log(m * zeta)) * std::log(m * zeta);
    dzm(2, 0) = 1.0;
  }
  const Matrix Vn(nativeParameterDistribution.getCovariance());
  const Matrix covZm = (dzm.transpose() * Vn * dzm);
  Normal parameterDistribution(optimalParameter, CovarianceMatrix(covZm.getImplementation()));
  parameterDistribution.setDescription({"zm", "sigma", "xi"});
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];

  const Scalar zmMin = -SpecFunc::Infinity;
  const Scalar zmMax =  SpecFunc::Infinity;
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, zm, zmMin, zmMax);
  return result;
}

GeneralizedPareto GeneralizedParetoFactory::buildReturnLevelProfileLikelihood(const Sample & sample, const Scalar u,
    const Scalar m, const Scalar theta) const
{
  const Distribution distribution(buildReturnLevelProfileLikelihoodEstimator(sample, u, m, theta).getDistribution());
  return buildAsGeneralizedPareto(distribution.getParameter());
}


END_NAMESPACE_OPENTURNS
