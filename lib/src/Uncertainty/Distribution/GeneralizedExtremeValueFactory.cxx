//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedExtremeValue distribution
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
#include "openturns/TNC.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/ComposedFunction.hxx"
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

class GeneralizedExtremeValueLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueLikelihoodEvaluation(const Sample & sample)
  : EvaluationImplementation()
  , sample_(sample)
  {
  }

  GeneralizedExtremeValueLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedExtremeValueLikelihoodEvaluation(*this);
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
    const UnsignedInteger size = sample_.getSize();
    const Scalar mu = parameter[0];
    const Scalar sigma = parameter[1];
    const Scalar xi = parameter[2];

    if (sigma <= 0.0)
      return Point(1, -SpecFunc::MaxScalar);

    // beware we cannot write -size, we need to cast to float first
    Scalar ll = -1.0 * size * std::log(sigma);

    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const Scalar yi = (sample_(i, 0) - mu) / sigma;
      if (std::abs(xi) < SpecFunc::Precision)
      {
        ll += -yi - std::exp(yi);
      }
      else
      {
        const Scalar c1 = 1.0 + xi * yi;
        if (c1 <= SpecFunc::Precision) // can be slightly off
          return Point(1, -SpecFunc::MaxScalar);
        ll += -(1.0 + 1.0 / xi) * std::log(c1) - std::pow(c1, -1.0 / xi);
      }
    }
    return Point(1, ll);
  }

private:
  Sample sample_;
};


static Scalar GeneralizedExtremeValueFactoryPWM(const Sample & sample, const UnsignedInteger r)
{
  Scalar s = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = r; i < size; ++ i)
    s += std::exp(SpecFunc::LogGamma(1.0 * (i + 1)) - SpecFunc::LogGamma(1.0 * (i + 1 - r))) * sample(i, 0);
  return s / std::exp(SpecFunc::LogGamma(size + 1.0) - SpecFunc::LogGamma(1.0 * (size - r)));
}


DistributionFactoryLikelihoodResult GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximizationEstimator(const Sample & sample) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Function objective(new GeneralizedExtremeValueLikelihoodEvaluation(sample));
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  const Scalar zmin = sample.getMin()[0];
  const Scalar zmax = sample.getMax()[0];
  const Scalar mean = sample.computeMean()[0];

  // sigma > 0
  const Point lowerBound({-SpecFunc::MaxScalar, SpecFunc::Precision, -SpecFunc::MaxScalar});
  const Point upperBound(3, SpecFunc::MaxScalar);
  const Interval::BoolCollection finiteLowerBound({false, true, false});
  const Interval::BoolCollection finiteUpperBound(3, false);
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));  

  // 1+xi(zi-mu)/sigma > 0
  Description formulas(2);
  formulas[0] = OSS() << "sigma + xi * (" << zmax << " - mu)";
  formulas[1] = OSS() << "sigma + xi * (" << zmin << " - mu)";
  const SymbolicFunction constraint(Description({"mu", "sigma", "xi"}), formulas);
  problem.setInequalityConstraint(constraint);

  // pwm for the starting point, see fit.gev function from R mev package
  const Sample sorted(sample.sort());
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
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());

  const Distribution distribution(buildAsGeneralizedExtremeValue(optimalParameter));
  const Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, sample));
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  DistributionFactoryLikelihoodResult result(distribution, parameterDistribution, logLikelihood);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildMethodOfLikelihoodMaximization(const Sample & sample) const
{
  const Distribution distribution(buildMethodOfLikelihoodMaximizationEstimator(sample).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
}

class GeneralizedExtremeValueProfileLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueProfileLikelihoodEvaluation(const Sample & sample,
                                                      const Scalar mean,
                                                      const Scalar bpwm1)
  : EvaluationImplementation()
  , sample_(sample)
  , mean_(mean)
  , bpwm1_(bpwm1)
  {
    zmin_ = sample.getMin()[0];
    zmax_ = sample.getMax()[0];
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

    const Function objective(new GeneralizedExtremeValueLikelihoodEvaluation(sample_));
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
    formulas[0] = OSS() << "sigma + " << xi0 << " * (" << zmax_ << " - mu)";
    formulas[1] = OSS() << "sigma + " << xi0 << " * (" << zmin_ << " - mu)";
    const SymbolicFunction constraint(Description({"mu", "sigma"}), formulas);
    problem.setInequalityConstraint(constraint);

    // heuristic for the starting point, see fit.gev function from R mev package
    const Scalar gamma1mXi0 = (xi0 < 1.0) ? SpecFunc::Gamma(1.0 - xi0) : 10.0;
    const Scalar sigma0 = -(2.0 * bpwm1_ - mean_) * xi0 / (gamma1mXi0 * (1.0 - std::pow(2.0, xi0)));
    const Scalar mu0 = mean_ - sigma0 * (gamma1mXi0 - 1.0) / xi0;
    Point x0({mu0, sigma0});

    // make mu great again
    Point cv(constraint(x0));
    if (cv[0] < 0.0)
      x0[0] += cv[0] / xi0;
    else if (cv[1]<0)
      x0[0] += cv[1] / xi0;

    // solve optimization problem
    Cobyla solver(problem);
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
      return Point(1, -SpecFunc::MaxScalar);
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  Scalar mean_ = 0.0;
  Scalar zmin_ = 0.0;
  Scalar zmax_ = 0.0;
  Scalar bpwm1_ = 0.0;
  mutable Point optimalPoint_;
};


ProfileLikelihoodResult GeneralizedExtremeValueFactory::buildMethodOfProfileLikelihoodMaximizationEstimator(const Sample & sample) const
{
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Scalar mean = sample.computeMean()[0];

  // method of probability weighted moments for the starting point, see fit.gev function from R mev package
  const Sample sorted(sample.sort());
  const Scalar bpwm1 = GeneralizedExtremeValueFactoryPWM(sorted, 1);
  const Scalar bpwm2 = GeneralizedExtremeValueFactoryPWM(sorted, 2);
  const Scalar kst = (2.0 * bpwm1 - mean) / (3.0 * bpwm2 - mean) - std::log(2.0) / std::log(3.0);
  const Scalar xi0 = -(7.859 + 2.9554 * kst) * kst;
  const Point x0({xi0});

  const GeneralizedExtremeValueProfileLikelihoodEvaluation profileLikelihoodEvaluation(sample, mean, bpwm1);
  const Function objective(profileLikelihoodEvaluation.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  Cobyla solver(problem);
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
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, xi);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildMethodOfProfileLikelihoodMaximization(const Sample & sample) const
{
  const Distribution distribution(buildMethodOfProfileLikelihoodMaximizationEstimator(sample).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
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

    if (sigma <= 0.0)
      return Point(1, -SpecFunc::MaxScalar);

    Scalar ll = -1.0 * m_ * r_ * std::log(sigma);
    for (UnsignedInteger i = 0; i < m_; ++ i)
    {
      const Scalar yir = (sample_(i, r_ - 1) - mu) / sigma;
      const Scalar c1 = 1.0 + xi * yir;
      if (c1 <= SpecFunc::Precision) // can be slightly off
        return Point(1, -SpecFunc::MaxScalar);
      ll += -std::pow(c1, -1.0 / xi);

      for (UnsignedInteger k = 0; k < r_; ++ k)
      {
        const Scalar yik = (sample_(i, k) - mu) / sigma;
        const Scalar c2 = 1.0 + xi * yik;
        if (c2 <= SpecFunc::Precision) // can be slightly off
          return Point(1, -SpecFunc::MaxScalar);
        ll += (-1.0 / xi - 1.0) * std::log(c2);
      }
    }
    return Point(1, ll);
  }

private:
  Sample sample_;
  UnsignedInteger R_ = 0;
  UnsignedInteger r_ = 0;
  UnsignedInteger m_ = 0;
};

/* R largest order statistics */
DistributionFactoryResult GeneralizedExtremeValueFactory::buildRMaximaEstimator(const Sample & sample,
                                                                                const UnsignedInteger rx)
{
  const UnsignedInteger R = sample.getDimension();
  // r=0 means r=R
  const UnsignedInteger r = rx ? rx : R;
  const UnsignedInteger size = sample.getSize();
  if (r > R)
    throw InvalidArgumentException(HERE) << "r(" << r << ") should be < R (" << R << ")";
  if (size < 2)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of size>=2, here size=" << sample.getSize();
  const UnsignedInteger m = sample.getSize();

  for (UnsignedInteger i = 0; i < m; ++ i)
    for (UnsignedInteger j = 0; j < r - 1; ++ j)
      if (sample(i, j) < sample(i, j + 1))
        throw InvalidArgumentException(HERE) << "The maxima of bloc #" << (i + 1) << "/" << m << " are not sorted in decreasing order";

  const Function objective(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample, r));
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // sigma > 0
  const Point lowerBound({-SpecFunc::MaxScalar, SpecFunc::Precision, -SpecFunc::MaxScalar});
  const Point upperBound(3, SpecFunc::MaxScalar);
  const Interval::BoolCollection finiteLowerBound({false, true, false});
  const Interval::BoolCollection finiteUpperBound(3, false);
  problem.setBounds(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));

  // pwm + 1d search for the starting point, see gevrFit function from R eva package
  const Sample rMax(sample.sort(0));
  Sample rMax1(rMax);
  Sample rMax2(rMax);
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    rMax1(i, 0) *= 1.0 * i / (m - 1.0);
    rMax2(i, 0) *= 1.0 * i * (i - 1.0) / (m - 1.0) / (m - 2.0);
  }
  const Scalar mom0 = rMax.computeMean()[0];
  const Scalar mom1 = rMax1.computeMean()[0];
  const Scalar mom2 = rMax2.computeMean()[0];
  const SymbolicFunction solveShape(Description({"sh", "mom0", "mom1", "mom2"}),
                                    Description(1, "(3^sh - 1) / (2^sh - 1) - (3 * mom2 - mom0) / (2 * mom1 - mom0)"));
  const ParametricFunction f(solveShape, Indices({1, 2, 3}), Point({mom0, mom1, mom2}));
  const Scalar a = ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-XiSearchLowerBound");
  const Scalar b = ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-XiSearchUpperBound");
  const Scalar fA = f(Point(1, a))[0];
  const Scalar fB = f(Point(1, b))[0];
  const Brent solver1d;
  const Scalar xi0 = solver1d.solve(f, 0.0, a, b, fA, fB);
  const Scalar sigma0 = ((2.0 * mom1 - mom0) * xi0) / (SpecFunc::Gamma(1.0 - xi0) * (std::pow(2.0, xi0) - 1.0));
  const Scalar mu0 = mom0 + sigma0 * (1.0 - SpecFunc::Gamma(1.0 - xi0)) / xi0;
  const Point x0({mu0, sigma0, xi0});

  // solve optimization problem
  TNC solver(problem);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(solver.getResult().getOptimalPoint());

  const Distribution distribution(buildAsGeneralizedExtremeValue(optimalParameter));
  Sample flat(0, 1);
  for (UnsignedInteger i = 0; i < m; ++ i)
    flat.add(Sample::BuildFromPoint(sample[i]));
  const Distribution parameterDistribution(MaximumLikelihoodFactory::BuildGaussianEstimator(distribution, flat));
  DistributionFactoryResult result(distribution, parameterDistribution);
  return result;
}

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildRMaxima(const Sample & sample, const UnsignedInteger rx)
{
  const Distribution distribution(buildRMaximaEstimator(sample, rx).getDistribution());
  return buildAsGeneralizedExtremeValue(distribution.getParameter());
}

UnsignedInteger GeneralizedExtremeValueFactory::buildBestRMaxima(const Sample & sample, const Indices & rx, Point & logLikelihoodOut)
{
  UnsignedInteger bestR = 0;
  Scalar bestLL = -SpecFunc::MaxScalar;
  logLikelihoodOut.resize(rx.getSize());
  for (UnsignedInteger i = 0; i < rx.getSize(); ++ i)
  {
    // r=0 means R
    const UnsignedInteger r = rx[i] ? rx[i] : sample.getDimension();
    const GeneralizedExtremeValue candidate(buildRMaxima(sample, r));
    const Function objective(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample, r));
    const Scalar candidateLL = objective(candidate.getParameter())[0];
    logLikelihoodOut[i] = candidateLL;
    if (candidateLL > bestLL)
    {
      bestR = r;
      bestLL = candidateLL;
    }
  }
  return bestR;
}


class GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation(const Sample & sample,
                                                         const Sample & meshValues,
                                                         const Function & thetaFunction,
                                                         const GeneralizedExtremeValueFactory::BasisCollection & basisCollection)
  : EvaluationImplementation()
  , sample_(sample)
  , meshValues_(meshValues)
  , thetaFunction_(thetaFunction)
  {
    // rescale constant terms parameters vs non-constant terms parameters
    Collection<Function> coll;
    for (UnsignedInteger i = 0; i < basisCollection.getSize(); ++ i)
      for (UnsignedInteger j = 0; j < basisCollection[i].getSize(); ++ j)
    {
      coll.add(basisCollection[i][j]);
    }
    AggregatedFunction aggregated(coll);
    Sample psi(aggregated(meshValues));
    const Scalar delta = meshValues.getMax()[0] - meshValues.getMin()[0];
    scale_ = psi.computeStandardDeviation() / delta;
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
    return 1;
  }

  Point uToX(const Point & inP) const
  {
    Point inP2(inP);
    for (UnsignedInteger j = 0; j < getInputDimension(); ++ j)
      if (scale_[j] > 0.0)
        inP2[j] *= scale_[j];
    return inP2;
  }

  Point operator() (const Point & parameter) const override
  {
    Function thetaFunction(thetaFunction_);
    thetaFunction.setParameter(uToX(parameter));

    Scalar ll = 0.0;
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
    {
      const Point t(meshValues_[i]);
      const Point theta(thetaFunction(t));

      const Scalar mu = theta[0];
      const Scalar sigma = theta[1];
      const Scalar xi = theta[2];

      if (sigma <= 0.0)
        return Point(1, -SpecFunc::MaxScalar);

      ll += -std::log(sigma);
      const Scalar yi = (sample_(i, 0) - mu) / sigma;
      const Scalar c1 = 1.0 + xi * yi;
      if (c1 <= SpecFunc::Precision) // can be slightly off
        return Point(1, -SpecFunc::MaxScalar);
      ll += -(1.0 + 1.0 / xi) * std::log(c1) - std::pow(c1, -1.0 / xi);     
    }
    return Point(1, ll);
  }

private:
  Sample sample_;
  Sample meshValues_;
  Function thetaFunction_;
  Point scale_;
};


TimeVaryingResult GeneralizedExtremeValueFactory::buildTimeVarying(const Sample & sample,
                                                                    const Mesh & mesh,
                                                                    const BasisCollection & basisCollection,
                                                                    const Function & inverseLinkFunction) const
{
  const Sample grid(mesh.getVertices());
  if (sample.getSize() < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (grid.getSize() != sample.getSize())
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << grid.getSize();
  if (grid.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << grid.getDimension();
  if (basisCollection.getSize() != 3)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << grid.getSize();

  // inverseLinkFunction is optional
  if (inverseLinkFunction.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (inverseLinkFunction.getInputDimension() != 3)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << inverseLinkFunction.getInputDimension();
    if (inverseLinkFunction.getOutputDimension() != 3)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << inverseLinkFunction.getInputDimension();
  }

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
    x0i[0] = 1.0;
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
      coll[j] = basisCollection[i][j];
    const AggregatedFunction aggregated(coll);
    ComposedFunction composed(parametric, aggregated);
    composed.setOutputDescription({build().getParameterDescription()[i] + "(t)"});
    thetaFunctions[i] = composed;
  }
  Function thetaFunction = AggregatedFunction(thetaFunctions);
  if (inverseLinkFunction.getEvaluation().getImplementation()->isActualImplementation())
    thetaFunction = ComposedFunction(inverseLinkFunction, thetaFunction);

  const GeneralizedExtremeValueTimeVaryingLikelihoodEvaluation evaluation(sample, grid, thetaFunction, basisCollection);
  const Function objective(evaluation.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // heuristic for feasible mu
  UnsignedInteger i = 0;
  const UnsignedInteger maxIter = ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-FeasibilityMaximumIterationNumber");
  const Scalar rho = ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-FeasibilityRhoFactor");
  while ((evaluation(x0)[0] == -SpecFunc::MaxScalar) && (i < maxIter))
  {
    x0[0] *= rho;
    ++ i;
  }

  TNC solver(problem);
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();
  const Point optimalParameter(evaluation.uToX(solver.getResult().getOptimalPoint()));

  // estimate parameter distribution via the Fisher information matrix
  const UnsignedInteger size = sample.getSize();
  Matrix fisher(nP, nP);
  const Scalar epsilon = ResourceMap::GetAsScalar("Evaluation-ParameterEpsilon");
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    thetaFunction.setParameter(optimalParameter);
    const Scalar pdfIRef = buildAsGeneralizedExtremeValue(thetaFunction(grid[i])).computePDF(sample[i]);

    // evaluate dpdf/dbeta by finite-differences
    Matrix dpdfi(nP, 1); 
    for (UnsignedInteger j = 0; j < nP; ++ j)
    {
      Point betaIj(optimalParameter);
      betaIj[j] += epsilon;
      thetaFunction.setParameter(betaIj);
      const Scalar pdfIj = buildAsGeneralizedExtremeValue(thetaFunction(grid[i])).computePDF(sample[i]);
      dpdfi(j, 0) = (pdfIj - pdfIRef) / epsilon;
    }
    dpdfi = dpdfi / pdfIRef;
    fisher = fisher + dpdfi * dpdfi.transpose() / size;
  }
  thetaFunction.setParameter(optimalParameter); // reset before return

  const CovarianceMatrix covariance(SymmetricMatrix(fisher.getImplementation()).solveLinearSystem(IdentityMatrix(nP) / size).getImplementation());

  const Normal parameterDistribution(optimalParameter, covariance);
  const Scalar logLikelihood = solver.getResult().getOptimalValue()[0];
  const TimeVaryingResult result(*this, thetaFunction, mesh, parameterDistribution, logLikelihood);
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
  , llh_(new GeneralizedExtremeValueLikelihoodEvaluation(sample))
  , m_(m)
  {
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

    const Scalar mu = zm + sigma / xi * (-std::expm1(xi * std::log(m_)));

    Point nativeParameter(zParameter);
    nativeParameter[0] = mu;
    return llh_(nativeParameter);
  }

private:
  Function llh_;
  Scalar m_ = 0.0;
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
      return Point(1, -SpecFunc::MaxScalar);
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
  solver.setProblem(problem);
  solver.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumEvaluationNumber"));
  solver.setStartingPoint(x0);
  solver.run();

  // rerun once to get optimal (sigma, xi) at optimal zm
  const Scalar zm = solver.getResult().getOptimalPoint()[0];
  profileLikelihoodEvaluation(solver.getResult().getOptimalPoint());
  const Scalar sigma = profileLikelihoodEvaluation.getOptimalPoint()[0];
  const Scalar xi = profileLikelihoodEvaluation.getOptimalPoint()[1];
  const Scalar mu = zm + sigma / xi * (-std::expm1(xi * std::log(m)));
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
  ProfileLikelihoodResult result(distribution, parameterDistribution, logLikelihood, objective, zm);
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
