//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedExtremeValue distribution
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
#include "openturns/GeneralizedExtremeValueFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FrechetFactory.hxx"
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/GumbelFactory.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/Brent.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValueFactory)

static const Factory<GeneralizedExtremeValueFactory> Factory_GeneralizedExtremeValueFactory;

/* Default constructor */
GeneralizedExtremeValueFactory::GeneralizedExtremeValueFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
GeneralizedExtremeValueFactory * GeneralizedExtremeValueFactory::clone() const
{
  return new GeneralizedExtremeValueFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution GeneralizedExtremeValueFactory::build(const Sample & sample) const
{
  return buildAsGeneralizedExtremeValue(sample).clone();
}

Distribution GeneralizedExtremeValueFactory::build(const Point & parameters) const
{
  return buildAsGeneralizedExtremeValue(parameters).clone();
}

Distribution GeneralizedExtremeValueFactory::build() const
{
  return buildAsGeneralizedExtremeValue().clone();
}

DistributionFactoryResult GeneralizedExtremeValueFactory::buildEstimator(const Sample & sample) const
{
  const GeneralizedExtremeValue distribution(buildAsGeneralizedExtremeValue(sample));
  const Bool isRegular = (distribution.getXi() >= -1.0);
  return MaximumLikelihoodFactory::BuildEstimator(*this, sample, isRegular);
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  Collection<DistributionFactory> factoryCollection;
  factoryCollection.add(FrechetFactory());
  factoryCollection.add(GumbelFactory());
  factoryCollection.add(WeibullMaxFactory());
  Scalar bic = -1.0;
  return FittingTest::BestModelBIC(sample, factoryCollection, bic);
}


static Scalar GeneralizedExtremeValueFactoryPWM(const Sample & sample, const UnsignedInteger r)
{
  Scalar s = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = r; i < size; ++ i)
    s += std::exp(SpecFunc::LogGamma(1.0 * (i + 1)) - SpecFunc::LogGamma(1.0 * (i + 1 - r))) * sample(i, 0);
  return s / std::exp(SpecFunc::LogGamma(size + 1.0) - SpecFunc::LogGamma(1.0 * (size - r)));
}


class GeneralizedExtremeValueRMaximaLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueRMaximaLikelihoodEvaluation(const Sample & sample,
      const UnsignedInteger r)
    : EvaluationImplementation()
    , sample_(sample)
    , R_(sample.getDimension())
    , r_(r)
    , m_(sample.getSize())
  {
    if ((r < 1) || (r > R_))
      throw InvalidArgumentException(HERE) << "We must have 1<=r<=R";
  }

  GeneralizedExtremeValueRMaximaLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 3;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & parameter) const override
  {
    const Scalar mu = parameter[0];
    const Scalar sigma = parameter[1];
    const Scalar xi = parameter[2];
    Point value(1);
    if (sigma <= 0.0)
    {
      value[0] = -std::log(SpecFunc::MaxScalar);
      return value;
    }

    Scalar ll = -1.0 * m_ * r_ * std::log(sigma);

    if (std::abs(xi) < SpecFunc::Precision)
    {
      for (UnsignedInteger i = 0; i < m_; ++ i)
      {
        const Scalar yir = (sample_(i, r_ - 1) - mu) / sigma;
        ll += -(yir + std::exp(-yir));

        for (UnsignedInteger k = 0; k < r_ - 1; ++ k)
        {
          const Scalar yik = (sample_(i, k) - mu) / sigma;
          ll += -yik;
        }
      } // i
    } // std::abs(xi) < SpecFunc::Precision
    else
    {
      for (UnsignedInteger i = 0; i < m_; ++ i)
      {
        const Scalar yir = (sample_(i, r_ - 1) - mu) / sigma;
        const Scalar c1 = xi * yir;
        if (c1 <= SpecFunc::Precision - 1.0) // can be slightly off
        {
          ll += -std::log(SpecFunc::ActualMaxScalar);
          continue;
        }
        const Scalar log1pC1 = std::log1p(c1);
        ll += -(1.0 + 1.0 / xi) * log1pC1 - std::exp(-log1pC1 / xi);

        for (UnsignedInteger k = 0; k < r_ - 1; ++ k)
        {
          const Scalar yik = (sample_(i, k) - mu) / sigma;
          const Scalar c2 = xi * yik;
          if (c2 <= SpecFunc::Precision - 1.0) // can be slightly off
          {
            ll += -std::log(SpecFunc::ActualMaxScalar);
            continue;
          }
          ll += (-1.0 / xi - 1.0) * std::log1p(c2);
        } // k
      } // i
    } // std::abs(xi) >= SpecFunc::Precision
    value[0] = ll;
    return value;
  }

private:
  Sample sample_;
  UnsignedInteger R_ = 0;
  UnsignedInteger r_ = 0;
  UnsignedInteger m_ = 0;
};


class GeneralizedExtremeValueProfileLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueProfileLikelihoodEvaluation(const Sample & sample,
      const Scalar mean,
      const Scalar bpwm1,
      const Scalar zMin,
      const Scalar zMax)
    : EvaluationImplementation()
    , sample_(sample)
    , mean_(mean)
    , bpwm1_(bpwm1)
  {
    zMin_ = zMin;
    zMax_ = zMax;
  }

  GeneralizedExtremeValueProfileLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedExtremeValueProfileLikelihoodEvaluation(*this);
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

    const Function objective(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample_, 1));
    const ParametricFunction objectiveXi(objective, Indices({2}), parameter);
    OptimizationProblem problem(objectiveXi);
    problem.setMinimization(false);

    // sigma > 0
    const Point lowerBound({-SpecFunc::MaxScalar, SpecFunc::Precision});
    const Point upperBound(2, SpecFunc::MaxScalar);
    const Interval::BoolCollection finiteLowerBound({false, true});
    const Interval::BoolCollection finiteUpperBound(2, false);
    problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

    // 1+xi(zi-mu)/sigma > 0
    Description formulas(2);
    formulas[0] = OSS() << "sigma + " << xi0 << " * (" << zMax_ << " - mu)";
    formulas[1] = OSS() << "sigma + " << xi0 << " * (" << zMin_ << " - mu)";
    const SymbolicFunction constraint(Description({"mu", "sigma"}), formulas);
    problem.setInequalityConstraint(constraint);

    // heuristic for the starting point, see fit.gev function from R mev package
    const Scalar gamma1mXi0 = (xi0 < 1.0) ? SpecFunc::Gamma(1.0 - xi0) : 10.0;
    const Scalar sigma0 = -(2.0 * bpwm1_ - mean_) * xi0 / (gamma1mXi0 * (1.0 - std::pow(2.0, xi0)));
    const Scalar mu0 = mean_ - sigma0 * (gamma1mXi0 - 1.0) / xi0;
    Point x0({mu0, sigma0});

    // Adapt mu
    const Point cv(constraint(x0));
    if (xi0 < 0.0)
    {
      if (cv[0] <= 0.0)
        x0[0] = zMax_;
    }
    else
    {
      if (cv[1] <= 0.0)
        x0[0] = zMin_;
    }
    // solve optimization problem
    Cobyla solver(problem);
    solver.setIgnoreFailure(true);
    solver.setProblem(problem);
    solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
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
      return Point(1, -std::log(SpecFunc::MaxScalar));
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  Scalar mean_ = 0.0;
  Scalar zMin_ = 0.0;
  Scalar zMax_ = 0.0;
  Scalar bpwm1_ = 0.0;
  mutable Point optimalPoint_;
};


ProfileLikelihoodResult GeneralizedExtremeValueFactory::buildMethodOfProfileLikelihoodMaximizationEstimator(const Sample & sample) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Scalar zMin = sample.getMin()[0];
  const Scalar zMax = sample.getMax()[0];
  const Scalar mean = sample.computeMean()[0];

  // method of probability weighted moments for the starting point, see fit.gev function from R mev package
  const Sample sorted(sample.sort());
  const Scalar bpwm1 = GeneralizedExtremeValueFactoryPWM(sorted, 1);
  const Scalar bpwm2 = GeneralizedExtremeValueFactoryPWM(sorted, 2);
  const Scalar kst = (2.0 * bpwm1 - mean) / (3.0 * bpwm2 - mean) - std::log(2.0) / std::log(3.0);
  const Scalar xi0 = -(7.859 + 2.9554 * kst) * kst;
  const Point x0({xi0});

  const GeneralizedExtremeValueProfileLikelihoodEvaluation profileLikelihoodEvaluation(sample, mean, bpwm1, zMin, zMax);
  const Function objective(profileLikelihoodEvaluation.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  Cobyla solver(problem);
  solver.setIgnoreFailure(true);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();

  // rerun once to get optimal (mu, sigma) at optimal xi
  const Scalar xi = solver.getResult().getOptimalPoint()[0];
  profileLikelihoodEvaluation(solver.getResult().getOptimalPoint());
  Point optimalParameter(profileLikelihoodEvaluation.getOptimalPoint());
  optimalParameter.add(xi);

  const Distribution distribution(buildAsGeneralizedExtremeValue(optimalParameter));
  Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));
  parameterDistribution.setDescription({"mu", "sigma", "xi"});
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
  const Scalar xiMin = -SpecFunc::MaxScalar;
  const Scalar xiMax = SpecFunc::MaxScalar;
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, xi, xiMin, xiMax);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildMethodOfProfileLikelihoodMaximization(const Sample & sample) const
{
  const Distribution distribution(buildMethodOfProfileLikelihoodMaximizationEstimator(sample).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
}



/* R largest order statistics */
DistributionFactoryLikelihoodResult GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample,
    const UnsignedInteger rx) const
{
  const UnsignedInteger R = sample.getDimension();
  // r=0 means r=R
  const UnsignedInteger r = (rx > 0) ? rx : R;
  const UnsignedInteger size = sample.getSize();
  if (r > R)
    throw InvalidArgumentException(HERE) << "r(" << r << ") should be < R (" << R << ")";
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of size>=2, here size=" << sample.getSize();

  // Check if order statistics are sorted the right way
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < r - 1; ++j)
      if (sample(i, j) < sample(i, j + 1))
        throw InvalidArgumentException(HERE) << "The maxima of bloc #" << (i + 1) << "/" << size << " are not sorted in decreasing order";

  const Function objective(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample, r));
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // sigma > 0
  const Point lowerBound({-SpecFunc::MaxScalar, SpecFunc::Precision, -SpecFunc::MaxScalar});
  const Point upperBound(3, SpecFunc::MaxScalar);
  const Interval::BoolCollection finiteLowerBound({false, true, false});
  const Interval::BoolCollection finiteUpperBound(3, false);
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

  // 1+xi(zi-mu)/sigma > 0 for all order statistics taken into account
  const Point allZMin = sample.getMin();
  const Point allZMax = sample.getMax();
  Scalar zMin = SpecFunc::MaxScalar;
  Scalar zMax = -SpecFunc::MaxScalar;
  for (UnsignedInteger i = 0; i < r; ++i)
  {
    zMin = std::min(zMin, allZMin[i]);
    zMax = std::max(zMax, allZMax[i]);
  }
  const Sample sample0(r == 1 ? sample : sample.getMarginal(0));
  const Scalar mean = sample0.computeMean()[0];
  Description formulas(2);
  formulas[0] = OSS() << "sigma + xi * (" << zMax << " - mu)";
  formulas[1] = OSS() << "sigma + xi * (" << zMin << " - mu)";
  const SymbolicFunction constraint(Description({"mu", "sigma", "xi"}), formulas);
  problem.setInequalityConstraint(constraint);

  // pwm for the starting point, see fit.gev function from R mev package
  const Sample sorted(sample0.sort());
  const Scalar bpwm1 = GeneralizedExtremeValueFactoryPWM(sorted, 1);
  const Scalar bpwm2 = GeneralizedExtremeValueFactoryPWM(sorted, 2);
  const Scalar kst = (2.0 * bpwm1 - mean) / (3.0 * bpwm2 - mean) - std::log(2.0) / std::log(3.0);
  const Scalar xi0 = -(7.859 + 2.9554 * kst) * kst;
  const Scalar gamma1mXi0 = SpecFunc::Gamma(1.0 - xi0);
  const Scalar sigma0 = -(2.0 * bpwm1 - mean) * xi0 / (gamma1mXi0 * (1.0 - std::pow(2.0, xi0)));
  const Scalar mu0 = mean - sigma0 * (gamma1mXi0 - 1.0) / xi0;
  const Point x0({mu0, sigma0, xi0});

  // solve optimization problem
  Cobyla solver(problem);
  solver.setIgnoreFailure(true);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());
  const Distribution distribution(buildAsGeneralizedExtremeValue(optimalParameter));
  // Only the maxima are representative of the estimated distribution.
  const Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample.getMarginal(0)));
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  DistributionFactoryLikelihoodResult result(distribution, parameterDistribution, logLikelihood);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximization(const Sample & sample, const UnsignedInteger rx) const
{
  const Distribution distribution(buildMethodOfLikelihoodMaximizationEstimator(sample, rx).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
}


class GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation(const Sample & sample,
      const Sample & timeStamps,
      const Function & thetaFunction,
      const Scalar startingValue)
    : EvaluationImplementation()
    , sample_(sample)
    , timeStamps_(timeStamps)
    , thetaFunction_(thetaFunction)
    , startingValue_(startingValue)
  {
    // Nothing to do
  }

  GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return thetaFunction_.getParameter().getSize();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 3;
  }

  Point operator() (const Point & parameter) const override
  {
    Function thetaFunction(thetaFunction_);
    thetaFunction.setParameter(parameter);

    Scalar ll = startingValue_;
    Scalar minSigma = SpecFunc::MaxScalar;
    Scalar minC1 = SpecFunc::MaxScalar;
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
    {
      const Point t(timeStamps_[i]);
      const Point theta(thetaFunction(t));

      const Scalar mu = theta[0];
      const Scalar sigma = theta[1];
      const Scalar xi = theta[2];
      minSigma = std::min(minSigma, sigma);

      if (sigma <= 0.0)
      {
        ll += -std::log(SpecFunc::ActualMaxScalar);
        continue;
      }

      ll += -std::log(sigma);
      const Scalar yi = (sample_(i, 0) - mu) / sigma;
      const Scalar c1 = xi * yi;
      minC1 = std::min(minC1, 1.0 + c1);
      if (c1 <= SpecFunc::Precision - 1.0) // can be slightly off
      {
        ll += -std::log(SpecFunc::ActualMaxScalar);
        continue;
      }
      const Scalar log1pC1 = std::log1p(c1);
      ll += -(1.0 + 1.0 / xi) * log1pC1 - std::exp(-log1pC1 / xi);
    }
    Point value(3);
    value[0] = ll;
    value[1] = minSigma;
    value[2] = minC1;
    LOGINFO(OSS(false) << "time varying log-likelihood parameter=" << parameter << ", log-likelihood=" << ll << ", min_t sigma(t)=" << minSigma << ", min_t c1(t)=" << minC1);
    return value;
  }

private:
  Sample sample_;
  Sample timeStamps_;
  Function thetaFunction_;
  Scalar startingValue_;
};


TimeVaryingResult GeneralizedExtremeValueFactory::buildTimeVarying(const Sample & sample,
    const Sample & timeStamps,
    const BasisCollection & basisCollection,
    const Function & inverseLinkFunction,
    const String & initializationMethod,
    const String & normalizationMethod) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (timeStamps.getSize() != sample.getSize())
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << timeStamps.getSize();
  if (timeStamps.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << timeStamps.getDimension();
  if (basisCollection.getSize() != 3)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << timeStamps.getSize();

  // inverseLinkFunction is optional
  if (inverseLinkFunction.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (inverseLinkFunction.getInputDimension() != 3)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 3, here dimension=" << inverseLinkFunction.getInputDimension();
    if (inverseLinkFunction.getOutputDimension() != 3)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 3, here dimension=" << inverseLinkFunction.getInputDimension();
  }

  // Get an initial guest for (mu, sigma, xi) as if they were constant
  Point initialGuess(3);
  LOGINFO(OSS() << "Initialization method is \"" << initializationMethod << "\"");
  if (initializationMethod == "Gumbel")
  {
    const Scalar mean = sample.computeMean()[0];
    const Scalar std = sample.computeStandardDeviation()[0];
    initialGuess[0] = mean - SpecFunc::EULERSQRT6_PI * std;
    initialGuess[1] = std / SpecFunc::PI_SQRT6;
    initialGuess[2] = 0.1;
  }
  else if (initializationMethod == "Static")
  {
    initialGuess = buildMethodOfLikelihoodMaximization(sample).getParameter();
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << initializationMethod << " is invalid for the \"GeneralizedExtremeValueFactory-InitializationMethod\" key in ResourceMap. Valid values are \"Static\" and \"Gumbel\"";
  LOGINFO(OSS(false) << "In buildTimeVarying, initial guess=" << initialGuess);
  // Check if the timeStamps have to be normalized
  Bool mustNormalize = false;
  LinearFunction normalizationFunction(Point(1), Point(1), IdentityMatrix(1));
  if (normalizationMethod == "CenterReduce")
  {
    mustNormalize = true;
    const Scalar meanTimeStamps = timeStamps.computeMean()[0];
    const Scalar stdTimeStamps = timeStamps.computeStandardDeviation()[0];
    SymmetricMatrix matrix(1);
    matrix(0, 0) = (stdTimeStamps > 0.0 ? 1.0 / stdTimeStamps : 1.0);
    normalizationFunction = LinearFunction(Point(1, meanTimeStamps), Point(1), matrix);
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", normalization function=" << normalizationFunction);
  }
  else if (normalizationMethod == "MinMax")
  {
    mustNormalize = true;
    const Scalar minTimeStamps = timeStamps.getMin()[0];
    const Scalar maxTimeStamps = timeStamps.getMax()[0];
    SymmetricMatrix matrix(1);
    matrix(0, 0) = (minTimeStamps < maxTimeStamps ? 1.0 / (maxTimeStamps - minTimeStamps) : 1.0);
    normalizationFunction = LinearFunction(Point(1, minTimeStamps), Point(1), matrix);
    LOGINFO(OSS() << "Normalization method=" << normalizationMethod << ", normalization function=" << normalizationFunction);
  }
  else if (normalizationMethod == "None")
  {
    LOGINFO("No normalization of the timeStamps");
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << normalizationMethod << " is invalid for the \"GeneralizedExtremeValueFactory-NormalizationMethod\" key in ResourceMap. Valid values are \"MinMax\", \"CenterReduce\", \"None\"";
  // build the parametric function [beta],t->theta(t)=mu(t),sigma(t),xi(t)
  Collection<Function> thetaFunctions(3);
  UnsignedInteger nP = 0;
  Point x0;
  for (UnsignedInteger i = 0; i < 3; ++ i)
  {
    const UnsignedInteger nI = basisCollection[i].getSize();
    nP += nI;
    // initialize first coefficient of basis to 1, 0 elsewhere
    Point x0i(nI);
    x0i[0] = initialGuess[i];
    x0.add(x0i);
    const Description betaVars(Description::BuildDefault(nI, "beta"));
    const Description fVars(Description::BuildDefault(nI, "f"));
    Description inputVars(betaVars);
    inputVars.add(fVars);
    String formula;
    for (UnsignedInteger j = 0; j < nI; ++ j)
    {
      formula += betaVars[j] + " * " + fVars[j];
      if (j < nI - 1)
        formula += " + ";
    }
    const SymbolicFunction linearCombination(inputVars, Description(1, formula));
    Indices betaIndices(nI);
    betaIndices.fill();
    const ParametricFunction parametric(linearCombination, betaIndices, Point(nI));
    Collection<Function> coll(nI);
    for (UnsignedInteger j = 0; j < nI; ++ j)
    {
      if (mustNormalize)
        coll[j] = ComposedFunction(basisCollection[i][j], normalizationFunction);
      else
        coll[j] = basisCollection[i][j];
    }
    const AggregatedFunction aggregated(coll);
    ComposedFunction composed(parametric, aggregated);
    composed.setOutputDescription({build().getParameterDescription()[i] + "(t)"});
    thetaFunctions[i] = composed;
  }
  Function thetaFunction = AggregatedFunction(thetaFunctions);
  if (inverseLinkFunction.getEvaluation().getImplementation()->isActualImplementation())
    thetaFunction = ComposedFunction(inverseLinkFunction, thetaFunction);

  GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation evaluation(sample, timeStamps, thetaFunction, 0.0);
  // heuristic for feasible mu
  UnsignedInteger k = 0;
  const UnsignedInteger maxIter = ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-FeasibilityMaximumIterationNumber");
  const Scalar rho = ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-FeasibilityRhoFactor");
  Point value(evaluation(x0));
  while (((value[1] <= 0.0) || (value[2] <= 0)) && (k < maxIter))
  {
    x0[0] *= rho;
    value = evaluation(x0);
    ++ k;
  }
  LOGINFO(OSS(false) << "Starting points for the coefficients=" << x0);
  const Scalar startingValue = -evaluation(x0)[0];
  evaluation = GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation(sample, timeStamps, thetaFunction, startingValue);

  const Function objectiveAndConstraints(evaluation.clone());
  const Function objective(objectiveAndConstraints.getMarginal(0));
  const Function inequalities(objectiveAndConstraints.getMarginal(Indices({1, 2})));
  OptimizationProblem problem(objective);
  problem.setInequalityConstraint(inequalities);
  problem.setMinimization(false);

  Cobyla solver(problem);
  solver.setIgnoreFailure(true);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0] - startingValue;
  LOGINFO(OSS(false) << "Optimal coefficients=" << optimalParameter << ", optimal log-likelihood=" << logLikelihood);
  // estimate parameter distribution via the Fisher information matrix
  const UnsignedInteger size = sample.getSize();
  Matrix fisher(nP, nP);

  const Scalar epsilon = ResourceMap::GetAsScalar("Evaluation-ParameterEpsilon");
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    thetaFunction.setParameter(optimalParameter);
    Point param(thetaFunction(timeStamps[i]));
    const Scalar pdfIRef = buildAsGeneralizedExtremeValue(param).computeLogPDF(sample[i]);

    // evaluate dpdf/dbeta by finite-differences
    Matrix dpdfi(nP, 1);
    for (UnsignedInteger j = 0; j < nP; ++ j)
    {
      Point betaIj(optimalParameter);
      betaIj[j] += epsilon;
      thetaFunction.setParameter(betaIj);
      const Scalar pdfIj = buildAsGeneralizedExtremeValue(thetaFunction(timeStamps[i])).computeLogPDF(sample[i]);
      dpdfi(j, 0) = (pdfIj - pdfIRef) / epsilon;
    }
    fisher = fisher + dpdfi.computeGram(false);
  }
  thetaFunction.setParameter(optimalParameter); // reset before return

  const CovarianceMatrix covariance(SymmetricMatrix(fisher.getImplementation()).solveLinearSystem(IdentityMatrix(nP)).getImplementation());
  const Normal parameterDistribution(optimalParameter, covariance);
  const TimeVaryingResult result(*this, sample, thetaFunction, timeStamps, parameterDistribution, normalizationFunction, logLikelihood);
  return result;
}


/* Return level */
Distribution GeneralizedExtremeValueFactory::buildReturnLevelEstimator(const DistributionFactoryResult & result, const Scalar m) const
{
  if (result.getDistribution().getImplementation()->getClassName() != "GeneralizedExtremeValue")
    throw InvalidArgumentException(HERE) << "Return level can only be estimated from a GEV";
  if (!(m > 1.0))
    throw InvalidArgumentException(HERE) << "Return period should be > 1";
  const Scalar p = 1.0 / m;
  const Scalar sigma = result.getDistribution().getParameter()[1];
  const Scalar xi = result.getDistribution().getParameter()[2];
  const Scalar zm = result.getDistribution().computeQuantile(p, true)[0];
  if (result.getParameterDistribution().getImplementation()->getClassName() == "Normal")
  {
    Matrix dzm(3, 1);
    dzm(0, 0) = 1.0;
    const Scalar yp = -std::log1p(-p);
    if (std::abs(xi) < SpecFunc::Precision)
      dzm(1, 0) = -std::log(yp);
    else
    {
      dzm(1, 0) = -1.0 / xi * (1.0 - std::pow(yp, -xi));
      dzm(2, 0) = sigma / (xi * xi) * (1.0 - std::pow(yp, -xi)) - sigma / xi * std::pow(yp, -xi) * std::log(yp);
    }
    const Matrix Vn(result.getParameterDistribution().getCovariance());
    const Scalar varZm = (dzm.transpose() * Vn * dzm)(0, 0);
    return Normal(zm, std::sqrt(varZm));
  }
  else
  {
    // sample input distribution + kernel smoothing
    throw NotYetImplementedException(HERE) << "GEV parameter distribution is not Gaussian";
  }
}



class GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation3 : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation3(const Sample & sample, const Scalar m)
    : EvaluationImplementation()
    , llh_(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample, 1))
    , logLog1pM_(std::log(-std::log1p(-1.0 / m)))
  {
    // Nothing to do
  }

  GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation3 * clone() const override
  {
    return new GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation3(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 3;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & zParameter) const override
  {
    const Scalar zm = zParameter[0];
    const Scalar sigma = zParameter[1];
    const Scalar xi = zParameter[2];

    if (sigma <= 0.0)
      return Point(1, -SpecFunc::MaxScalar);

    const Scalar mu = (std::abs(xi) < SpecFunc::Precision ? zm + sigma * logLog1pM_ : zm - sigma * std::expm1(-xi * logLog1pM_) / xi);

    Point nativeParameter(zParameter);
    nativeParameter[0] = mu;
    return llh_(nativeParameter);
  }

private:
  Function llh_;
  Scalar logLog1pM_ = 0.0;
};

class GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1 : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1(const Sample & sample,
      const Scalar sigma0,
      const Scalar xi0, const Scalar m)
    : EvaluationImplementation()
    , sample_(sample)
    , sigma0_(sigma0)
    , xi0_(xi0)
    , m_(m)
  {
    // Nothing to do
  }

  GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1 * clone() const override
  {
    return new GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1(*this);
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
    const Function objective(new GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation3(sample_, m_));
    const ParametricFunction objectiveZm(objective, Indices({0}), parameter);
    OptimizationProblem problem(objectiveZm);
    problem.setMinimization(false);


    // sigma > 0
    const Point lowerBound({SpecFunc::Precision, -SpecFunc::MaxScalar});
    const Point upperBound(2, SpecFunc::MaxScalar);
    const Interval::BoolCollection finiteLowerBound({true, false});
    const Interval::BoolCollection finiteUpperBound(2, false);
    problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

    const Point x0({sigma0_, xi0_});

    // solve optimization problem
    Cobyla solver(problem);
    solver.setIgnoreFailure(true);
    solver.setProblem(problem);
    solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
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
      return Point(1, -std::log(SpecFunc::MaxScalar));
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  Scalar sigma0_ = 0.0;
  Scalar xi0_ = 0.0;
  Scalar m_ = 0.0;
  mutable Point optimalPoint_;
};

ProfileLikelihoodResult GeneralizedExtremeValueFactory::buildReturnLevelProfileLikelihoodEstimator(const Sample & sample, const Scalar m) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (!(m > 1.0))
    throw InvalidArgumentException(HERE) << "Return period should be > 1";
  const Scalar p = 1.0 / m;
  const Scalar logLog1pM = std::log(-std::log1p(-p));

  // start from maximum likelihood
  const Distribution ref(buildMethodOfLikelihoodMaximization(sample));

  const Scalar zm0 = ref.computeQuantile(p, true)[0];
  const Scalar sigma0 = ref.getParameter()[1];
  const Scalar xi0 = ref.getParameter()[2];
  const Point x0({zm0});

  const GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1 profileLikelihoodEvaluation(sample, sigma0, xi0, m);
  const Function objective(profileLikelihoodEvaluation.clone());

  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  Cobyla solver(problem);
  solver.setIgnoreFailure(true);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();

  // rerun once to get optimal (sigma, xi) at optimal zm
  const Scalar zm = solver.getResult().getOptimalPoint()[0];
  profileLikelihoodEvaluation(solver.getResult().getOptimalPoint());
  const Scalar sigma = profileLikelihoodEvaluation.getOptimalPoint()[0];
  const Scalar xi = profileLikelihoodEvaluation.getOptimalPoint()[1];
  const Scalar mu = zm - sigma * std::expm1(-xi * logLog1pM) / xi;
  const Point optimalParameter({mu, sigma, xi});

  const Distribution distribution(buildAsGeneralizedExtremeValue(optimalParameter));
  const Distribution nativeParameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));

  // delta method to transport native parametrization into zm parametrization
  Matrix dzm(IdentityMatrix(3));
  const Scalar yp = -std::log1p(-p);
  if (std::abs(xi) < SpecFunc::Precision)
    dzm(1, 0) = -std::log(yp);
  else
  {
    dzm(1, 0) = -1.0 / xi * (1.0 - std::pow(yp, -xi));
    dzm(2, 0) = sigma / (xi * xi) * (1.0 - std::pow(yp, -xi)) - sigma / xi * std::pow(yp, -xi) * std::log(yp);
  }
  const Matrix Vn(nativeParameterDistribution.getCovariance());
  const Matrix covZm = (dzm.transpose() * Vn * dzm);
  Normal parameterDistribution(optimalParameter, CovarianceMatrix(covZm.getImplementation()));
  parameterDistribution.setDescription({"zm", "sigma", "xi"});
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];

  // Compute the extreme possible values for zm given the sample and (mu, sigma)
  // As the function xi->zm(xi;mu,sigma,m) is increasing for all m>=2, we get
  // zmMin = mu + sigma * (exp(-xiMin * log(-log(1-1/m))) - 1) / xiMin
  // zmMin = mu + sigma * (exp(-xiMax * log(-log(1-1/m))) - 1) / xiMax
  // logLog1pM = log(-log(1-p)) p<=1/2, 1-p >= 1/2, log(1-p) >= -log(2), -log(1-p) <= log(2), log(-log(1-p)) <= log(log(2)), log(-log(1-p)) <= log(log(2)) < 0
  /* Unfortunately this nice idea fails...
    const Scalar zMin = sample.getMin()[0];
    const Scalar zMax = sample.getMax()[0];
    Scalar zmMin;
    if (zMax > mu)
    {
    const Scalar xiMin = sigma / (mu - zMax);
    zmMin = mu + sigma * std::expm1(-xiMin * logLog1pM) / xiMin;
    }
    else
    // mu + sigma * (0 - 1) / (-inf) = mu
    zmMin = mu;
    Scalar zmMax;
    if (zMin < mu)
    {
    const Scalar xiMax = sigma / (mu - zMin);
    zmMax = mu + sigma * std::expm1(-xiMax * logLog1pM) / xiMax;
    }
    else
    zmMax = SpecFunc::MaxScalar;
  */
  const Scalar zmMin = -SpecFunc::MaxScalar;
  const Scalar zmMax =  SpecFunc::MaxScalar;
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, zm, zmMin, zmMax);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildReturnLevelProfileLikelihood(const Sample & sample, const Scalar m) const
{
  const Distribution distribution(buildReturnLevelProfileLikelihoodEstimator(sample, m).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue(const Point & parameters) const
{
  try
  {
    GeneralizedExtremeValue distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from the given parameters";
  }
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildAsGeneralizedExtremeValue() const
{
  return GeneralizedExtremeValue();
}

END_NAMESPACE_OPENTURNS
