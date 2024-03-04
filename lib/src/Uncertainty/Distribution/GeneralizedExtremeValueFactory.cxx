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
#include "openturns/DistributionFactory.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/Brent.hxx"
#include "openturns/AggregatedFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Normal.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedExtremeValueFactory)

static const Factory<GeneralizedExtremeValueFactory> Factory_GeneralizedExtremeValueFactory;

/* Default constructor */
GeneralizedExtremeValueFactory::GeneralizedExtremeValueFactory()
  : DistributionFactoryImplementation()
{
  solver_ = OptimizationAlgorithm::Build(ResourceMap::GetAsString("GeneralizedExtremeValueFactory-DefaultOptimizationAlgorithm"));
  solver_.setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedExtremeValueFactory-MaximumCallsNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("GeneralizedExtremeValueFactory-MaximumConstraintError"));
  Cobyla* cobyla = dynamic_cast<Cobyla *>(solver_.getImplementation().get());
  if (cobyla)
    cobyla->setCheckStatus(false);
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
  return buildMethodOfLikelihoodMaximization(sample);
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
      const UnsignedInteger r,
      const Scalar mean,
      const Scalar bpwm1,
      const Scalar zMin,
      const Scalar zMax,
      const OptimizationAlgorithm & solver)
    : EvaluationImplementation()
    , sample_(sample)
    , r_(r)
    , mean_(mean)
    , bpwm1_(bpwm1)
    , solver_(solver)
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

    const Function objective(new GeneralizedExtremeValueRMaximaLikelihoodEvaluation(sample_, r_));
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
      return Point(1, -std::log(SpecFunc::MaxScalar));
    }
  }

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

private:
  Sample sample_;
  UnsignedInteger r_ = 0;
  Scalar mean_ = 0.0;
  Scalar zMin_ = 0.0;
  Scalar zMax_ = 0.0;
  Scalar bpwm1_ = 0.0;
  mutable Point optimalPoint_;
  OptimizationAlgorithm solver_;
};


ProfileLikelihoodResult GeneralizedExtremeValueFactory::buildMethodOfXiProfileLikelihoodEstimator(const Sample & sample, const UnsignedInteger rx) const
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

  const GeneralizedExtremeValueProfileLikelihoodEvaluation profileLikelihoodEvaluation(sample, r, mean, bpwm1, zMin, zMax, solver_);
  const Function objective(profileLikelihoodEvaluation.clone());
  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
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

GeneralizedExtremeValue GeneralizedExtremeValueFactory::buildMethodOfXiProfileLikelihood(const Sample & sample, const UnsignedInteger r) const
{
  const Distribution distribution(buildMethodOfXiProfileLikelihoodEstimator(sample, r).getDistribution());
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
	OptimizationAlgorithm solver(solver_);
	solver.setProblem(problem);
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


class GeneralizedExtremeValueCovariatesLikelihoodEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValueCovariatesLikelihoodEvaluation(const Sample & sample,
      const Matrix & muCovariates,
      const Matrix & sigmaCovariates,
      const Matrix & xiCovariates,
      const Function & muLink,
      const Function & sigmaLink,
      const Function & xiLink,
      const Scalar startingValue)
    : EvaluationImplementation()
    , sample_(sample)
    , muCovariates_(muCovariates)
    , sigmaCovariates_(sigmaCovariates)
    , xiCovariates_(xiCovariates)
    , muLink_(muLink.getEvaluation().getImplementation()->isActualImplementation() ? muLink : IdentityFunction(1))
    , sigmaLink_(sigmaLink.getEvaluation().getImplementation()->isActualImplementation() ? sigmaLink : IdentityFunction(1))
    , xiLink_(xiLink.getEvaluation().getImplementation()->isActualImplementation() ? xiLink : IdentityFunction(1))
    , muDim_(muCovariates.getNbColumns())
    , sigmaDim_(sigmaCovariates.getNbColumns())
    , xiDim_(xiCovariates.getNbColumns())
    , startingValue_(startingValue)
  {
    // Nothing to do
  }

  GeneralizedExtremeValueCovariatesLikelihoodEvaluation * clone() const override
  {
    return new GeneralizedExtremeValueCovariatesLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return muDim_ + sigmaDim_ + xiDim_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 3;
  }

  Point operator() (const Point & beta) const override
  {
    // Mu
    Point betaMu(muDim_);
    std::copy(beta.begin(), beta.begin() + muDim_, betaMu.begin());
    const Sample muT(Sample::BuildFromPoint(muCovariates_ * betaMu));
    UnsignedInteger shift = muDim_;
    // Sigma
    Point betaSigma(sigmaDim_);
    std::copy(beta.begin() + shift, beta.begin() + shift + sigmaDim_, betaSigma.begin());
    const Sample sigmaT(Sample::BuildFromPoint(sigmaCovariates_ * betaSigma));
    shift += sigmaDim_;
    // Xi
    Point betaXi(xiDim_);
    std::copy(beta.begin() + shift, beta.begin() + shift + xiDim_, betaXi.begin());
    const Sample xiT(Sample::BuildFromPoint(xiCovariates_ * betaXi));

    Scalar ll = startingValue_;
    Scalar minSigma = SpecFunc::ActualMaxScalar;
    Scalar minC1 = SpecFunc::ActualMaxScalar;
    for (UnsignedInteger i = 0; i < sample_.getSize(); ++ i)
    {
      const Scalar mu = muLink_(muT[i])[0];
      const Scalar sigma = sigmaLink_(sigmaT[i])[0];
      const Scalar xi = xiLink_(xiT[i])[0];
      const Scalar yi = (sample_(i, 0) - mu) / sigma;
      LOGDEBUG(OSS() << "i=" << i << ", mu=" << mu << ", sigma=" << sigma << ", xi=" << xi << ", yi=" << yi);
      if (std::abs(xi) < SpecFunc::Precision)
      {
        ll += -(yi + std::exp(-yi));
      } // std::abs(xi) < SpecFunc::Precision
      else
      {
        minSigma = std::min(minSigma, sigma);

        if (sigma <= 0.0)
        {
          ll += -std::log(SpecFunc::ActualMaxScalar);
          continue;
        }

        ll += -std::log(sigma);
        const Scalar c1 = xi * yi;
        minC1 = std::min(minC1, 1.0 + c1);
        if (c1 <= SpecFunc::Precision - 1.0) // can be slightly off
        {
          ll += -std::log(SpecFunc::ActualMaxScalar);
          continue;
        }
        const Scalar log1pC1 = std::log1p(c1);
        ll += -(1.0 + 1.0 / xi) * log1pC1 - std::exp(-log1pC1 / xi);
      } // std::abs(xi) >= SpecFunc::Precision
    } // i

    LOGTRACE(OSS(false) << "covariates log-likelihood beta=" << beta << ", log-likelihood=" << ll << ", min_t sigma(t)=" << minSigma << ", min_t c1(t)=" << minC1);
    return {ll, minSigma, minC1};
  }

  void setStartingValue(const Scalar startingValue)
  {
    startingValue_ = startingValue;
  }

private:
  Sample sample_;
  Matrix muCovariates_;
  Matrix sigmaCovariates_;
  Matrix xiCovariates_;
  Function muLink_;
  Function sigmaLink_;
  Function xiLink_;
  UnsignedInteger muDim_ = 0;
  UnsignedInteger sigmaDim_ = 0;
  UnsignedInteger xiDim_ = 0;
  Scalar startingValue_ = 0.0;
};



class GeneralizedExtremeValuePDFEvaluation : public EvaluationImplementation
{
public:
  GeneralizedExtremeValuePDFEvaluation()
    : EvaluationImplementation()
  {
    // Nothing to do
  }

  GeneralizedExtremeValuePDFEvaluation * clone() const override
  {
    return new GeneralizedExtremeValuePDFEvaluation(*this);
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
    const Scalar logPDF = factory_.buildAsGeneralizedExtremeValue(theta).computeLogPDF(X_);
    return {logPDF};
  }

  static void SetX(const Point & x)
  {
    X_ = x;
  }

private:
  GeneralizedExtremeValueFactory factory_;
  static Point X_;
};

Point GeneralizedExtremeValuePDFEvaluation::X_;

/** Covariates */
CovariatesResult GeneralizedExtremeValueFactory::buildCovariates(const Sample & sample,
    const Sample & covariates0,
    const Indices & muIndices0,
    const Indices & sigmaIndices0,
    const Indices & xiIndices0,
    const Function & muLink,
    const Function & sigmaLink,
    const Function & xiLink,
    const String & initializationMethod,
    const String & normalizationMethod) const
{
  const UnsignedInteger size = sample.getSize();

  if (size < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedExtremeValue distribution from a sample of size < 3";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  if (covariates0.getSize() != size)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only if the sample of covariates has the same size as the sample of observations";

  UnsignedInteger covariatesDimension = covariates0.getDimension();
  if (!muIndices0.check(covariatesDimension))
    throw InvalidArgumentException(HERE) << "Error: the indices for mu are not compatible with the covariates dimension";
  if (!sigmaIndices0.check(covariatesDimension))
    throw InvalidArgumentException(HERE) << "Error: the indices for sigma are not compatible with the covariates dimension";
  if (!xiIndices0.check(covariatesDimension))
    throw InvalidArgumentException(HERE) << "Error: the indices for xi are not compatible with the covariates dimension";

  // h is optional
  if (muLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (muLink.getInputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << muLink.getInputDimension();
    if (muLink.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << muLink.getInputDimension();
  }
  if (sigmaLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (sigmaLink.getInputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << sigmaLink.getInputDimension();
    if (sigmaLink.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << sigmaLink.getInputDimension();
  }
  if (xiLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (xiLink.getInputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << xiLink.getInputDimension();
    if (xiLink.getOutputDimension() != 1)
      throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from an inverse link function of input dimension 1, here dimension=" << xiLink.getInputDimension();
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
  Indices muIndices(muIndices0);
  Indices sigmaIndices(sigmaIndices0);
  Indices xiIndices(xiIndices0);
  if (constantCovariateIndex == covariatesDimension)
  {
    covariates.stack(Sample(size, Point({1.0})));
    muIndices.add(covariatesDimension);
    sigmaIndices.add(covariatesDimension);
    xiIndices.add(covariatesDimension);
    ++ covariatesDimension;
  }

  // the provided constant covariate must be non-null
  if (std::abs(covariates(0, constantCovariateIndex)) < SpecFunc::Precision)
    throw InvalidArgumentException(HERE) << "Null constant covariate at index " << constantCovariateIndex;

  // indices must at least reference the constant covariate
  if (!muIndices.contains(constantCovariateIndex))
    muIndices.add(constantCovariateIndex);
  if (!sigmaIndices.contains(constantCovariateIndex))
    sigmaIndices.add(constantCovariateIndex);
  if (!xiIndices.contains(constantCovariateIndex))
    xiIndices.add(constantCovariateIndex);

  // Get an initial guest for (mu, sigma, xi) as if they were constant
  Point initialGuess;
  LOGINFO(OSS() << "Initialization method is \"" << initializationMethod << "\"");
  const UnsignedInteger muDim = muIndices.getSize();
  const UnsignedInteger sigmaDim = sigmaIndices.getSize();
  const UnsignedInteger xiDim = xiIndices.getSize();
  if (initializationMethod == "Gumbel")
  {
    const Scalar mean = sample.computeMean()[0];
    const Scalar std = sample.computeStandardDeviation()[0];
    initialGuess = {mean - SpecFunc::EULERSQRT6_PI * std, std / SpecFunc::PI_SQRT6, 0.1};
  }
  else if (initializationMethod == "Static")
  {
    initialGuess = buildMethodOfLikelihoodMaximization(sample).getParameter();
  }
  else throw InvalidArgumentException(HERE) << "Error: the value " << initializationMethod << " is invalid for the \"GeneralizedExtremeValueFactory-InitializationMethod\" key in ResourceMap. Valid values are \"Static\" and \"Gumbel\"";
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
  else throw InvalidArgumentException(HERE) << "Error: the value " << normalizationMethod << " is invalid for the \"GeneralizedExtremeValueFactory-NormalizationMethod\" key in ResourceMap. Valid values are \"MinMax\", \"CenterReduce\", \"None\"";

  // normalization should not nullify constant column
  if (constantCovariateIndex < covariatesDimension)
    center[constantCovariateIndex] = 0.0;

  const LinearFunction normalizationFunction(center, constant, linear);
  const Sample normalizedCovariates(normalizationFunction(covariates));

  // Extract the 3 matrices corresponding to the covariates for mu, sigma and xi
  const Matrix muCovariates(Matrix(muIndices.getSize(), normalizedCovariates.getSize(), normalizedCovariates.getMarginal(muIndices).getImplementation()->getData()).transpose());
  const Matrix sigmaCovariates(Matrix(sigmaIndices.getSize(), normalizedCovariates.getSize(), normalizedCovariates.getMarginal(sigmaIndices).getImplementation()->getData()).transpose());
  const Matrix xiCovariates(Matrix(xiIndices.getSize(), normalizedCovariates.getSize(), normalizedCovariates.getMarginal(xiIndices).getImplementation()->getData()).transpose());

  // Conpute the log-likelihood associated to the initial point with a zero reference value in order to find a feasible initial point
  GeneralizedExtremeValueCovariatesLikelihoodEvaluation evaluation(sample, muCovariates, sigmaCovariates, xiCovariates, muLink, sigmaLink, xiLink, 0.0);

  // set initial guess on coefficients for constant covariate
  Point x0(muDim + sigmaDim + xiDim);
  if (muIndices.contains(constantCovariateIndex))
    x0[muIndices.find(constantCovariateIndex)] = initialGuess[0];
  UnsignedInteger shift = muDim;
  if (sigmaIndices.contains(constantCovariateIndex))
    x0[shift + sigmaIndices.find(constantCovariateIndex)] = initialGuess[1];
  shift += sigmaDim;
  if (xiIndices.contains(constantCovariateIndex))
    x0[shift + xiIndices.find(constantCovariateIndex)] = initialGuess[2];

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
  const UnsignedInteger nP = muDim + sigmaDim + xiDim;
  Point alpha(nP);
  Point gamma(nP);
  for (UnsignedInteger i = 0; i < muDim; ++ i)
  {
    gamma[i] = center[muIndices[i]];
    alpha[i] = linear(muIndices[i], muIndices[i]);
  }
  shift = muDim;
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
  {
    gamma[shift + i] = center[sigmaIndices[i]];
    alpha[shift + i] = linear(sigmaIndices[i], sigmaIndices[i]);
  }
  shift += sigmaDim;
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
  {
    gamma[shift + i] = center[xiIndices[i]];
    alpha[shift + i] = linear(xiIndices[i], xiIndices[i]);
  }

  // compute the beta coefficients from the beta coefficients matching the normalized covariates
  Point optimalBeta(optimalParameter);
  Scalar offset = 0.0;
  for (UnsignedInteger i = 0; i < muDim; ++ i)
  {
    if (muIndices[i] != constantCovariateIndex)
    {
      optimalBeta[i] *= alpha[i];
      offset += optimalParameter[i] * alpha[i] * gamma[i];
    }
  }

  // report the centering coefficients on the constant term
  for (UnsignedInteger i = 0; i < muDim; ++ i)
    if (muIndices[i] == constantCovariateIndex)
      optimalBeta[i] -= offset;

  shift = muDim;
  offset = 0.0;
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
  {
    if (sigmaIndices[i] != constantCovariateIndex)
    {
      optimalBeta[shift + i] *= alpha[shift + i];
      offset += optimalParameter[shift + i] * alpha[shift + i] * gamma[shift + i];
    }
  }

  // report the centering coefficients on the constant term
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
    if (sigmaIndices[i] == constantCovariateIndex)
      optimalBeta[shift + i] -= offset;

  shift += sigmaDim;
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

  // Build the theta function which maps a dim(covariates) vector into a (mu, sigma, xi) vector.
  const Description muBetaDesc(Description::BuildDefault(muDim, "muBeta"));
  const Description sigmaBetaDesc(Description::BuildDefault(sigmaDim, "sigmaBeta"));
  const Description xiBetaDesc(Description::BuildDefault(xiDim, "xiBeta"));
  const Description yDesc(Description::BuildDefault(covariatesDimension, "y"));
  Description muVars(muBetaDesc);
  Description sigmaVars(sigmaBetaDesc);
  Description xiVars(xiBetaDesc);
  muVars.add(yDesc);
  sigmaVars.add(yDesc);
  xiVars.add(yDesc);
  String muFormula;
  String sigmaFormula;
  String xiFormula;
  for (UnsignedInteger i = 0; i < muDim; ++ i)
    muFormula += OSS() << muBetaDesc[i] << " * " << yDesc[muIndices[i]] << (i < muDim - 1 ? " + " : "");
  for (UnsignedInteger i = 0; i < sigmaDim; ++ i)
    sigmaFormula += OSS() << sigmaBetaDesc[i] << " * " << yDesc[sigmaIndices[i]] << (i < sigmaDim - 1 ? " + " : "");
  for (UnsignedInteger i = 0; i < xiDim; ++ i)
    xiFormula += OSS() << xiBetaDesc[i] << " * " << yDesc[xiIndices[i]] << (i < xiDim - 1 ? " + " : "");
  Function muBetaFunction = SymbolicFunction(muVars, {muFormula});
  Function sigmaBetaFunction = SymbolicFunction(sigmaVars, {sigmaFormula});
  Function xiBetaFunction = SymbolicFunction(xiVars, {xiFormula});

  // use beta variables as parameters
  Description thetaBetaVars(muBetaDesc);
  thetaBetaVars.add(sigmaBetaDesc);
  thetaBetaVars.add(xiBetaDesc);
  thetaBetaVars.add(yDesc);
  Indices betaVarsIndices(muDim + sigmaDim + xiDim);
  betaVarsIndices.fill();
  SymbolicFunction thetaBetaFunction(thetaBetaVars, {muFormula, sigmaFormula, xiFormula});
  Function thetaFunction = ParametricFunction(thetaBetaFunction, betaVarsIndices, optimalBeta);

  Indices muVarsIndices(muDim);
  Indices sigmaVarsIndices(sigmaDim);
  Indices xiVarsIndices(xiDim);
  muVarsIndices.fill();
  sigmaVarsIndices.fill();
  xiVarsIndices.fill();
  muBetaFunction = ParametricFunction(muBetaFunction, muVarsIndices, Point(muDim, 1.0));
  sigmaBetaFunction = ParametricFunction(sigmaBetaFunction, sigmaVarsIndices, Point(sigmaDim, 1.0));
  xiBetaFunction = ParametricFunction(xiBetaFunction, xiVarsIndices, Point(xiDim, 1.0));
 
  // The theta function is the composition between the inverse link function and the linear function
  if (muLink.getEvaluation().getImplementation()->isActualImplementation()
    || sigmaLink.getEvaluation().getImplementation()->isActualImplementation()
    || xiLink.getEvaluation().getImplementation()->isActualImplementation())
  {
    Function link1(muLink.getEvaluation().getImplementation()->isActualImplementation() ? muLink : IdentityFunction(1));
    link1 = ComposedFunction(link1, SymbolicFunction({"x1", "x2", "x3"}, {"x1"}));
    Function link2(sigmaLink.getEvaluation().getImplementation()->isActualImplementation() ? sigmaLink : IdentityFunction(1));
    link2 = ComposedFunction(link2, SymbolicFunction({"x1", "x2", "x3"}, {"x2"}));
    Function link3(xiLink.getEvaluation().getImplementation()->isActualImplementation() ? xiLink : IdentityFunction(1));
    link3 = ComposedFunction(link3, SymbolicFunction({"x1", "x2", "x3"}, {"x3"}));
    AggregatedFunction thetaLink({link1, link2, link3});
    thetaFunction = ComposedFunction(thetaLink, thetaFunction);
  }

  // useful for the theta(y) graphs
  thetaFunction.setOutputDescription({"$\\mu$", "$\\sigma$", "$\\xi$"});

  // compose the y->theta->pdf function
  GeneralizedExtremeValuePDFEvaluation pdfFunction;
  ComposedFunction yToPDF(pdfFunction, thetaFunction);

  // estimate parameter distribution via the Fisher information matrix
  Matrix fisher(nP, nP);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // set the location through a global variable
    GeneralizedExtremeValuePDFEvaluation::SetX(sample[i]);

    // compute the jacobian wrt the beta coefficients
    const Matrix dpdfi(yToPDF.parameterGradient(covariates[i]));
    fisher = fisher + dpdfi.computeGram(false);
  }

  const CovarianceMatrix covariance(SymmetricMatrix(fisher.getImplementation()).solveLinearSystem(IdentityMatrix(nP) / size).getImplementation());
  const Normal parameterDistribution(optimalBeta, covariance);
  const CovariatesResult result(*this, thetaFunction, covariates, parameterDistribution, normalizationFunction, logLikelihood);
  return result;
}


TimeVaryingResult GeneralizedExtremeValueFactory::buildTimeVarying(const Sample & sample,
    const Sample & timeStamps,
    const Basis & basis,
    const Indices & muIndices,
    const Indices & sigmaIndices,
    const Indices & xiIndices,
    const Function & muLink,
    const Function & sigmaLink,
    const Function & xiLink,
    const String & initializationMethod,
    const String & normalizationMethod) const
{
  if (timeStamps.getSize() != sample.getSize())
    throw InvalidArgumentException(HERE) << "GeneralizedExtremeValue timeStamps size (" << timeStamps.getSize()<<") must match sample size (" << sample.getSize() << ")";
  if (timeStamps.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedExtremeValue distribution only from a sample of dimension 1, here dimension=" << timeStamps.getDimension();
  if (!basis.getSize())
    throw InvalidArgumentException(HERE) << "Basis is empty";
  if (!muIndices.check(basis.getSize()))
    throw InvalidArgumentException(HERE) << "Error: the indices for mu are not compatible with the basis size";
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
  else throw InvalidArgumentException(HERE) << "Error: the value " << normalizationMethod << " is invalid for the \"GeneralizedExtremeValueFactory-NormalizationMethod\" key in ResourceMap. Valid values are \"MinMax\", \"CenterReduce\", \"None\"";

  // evaluate covariates from basis, without duplicates basis terms
  Indices allIndices;
  allIndices.add(muIndices);
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
  const CovariatesResult covariatesResult(buildCovariates(sample, covariates, muIndices, sigmaIndices, xiIndices, muLink, sigmaLink, xiLink, initializationMethod, "None"));

  // compose the parameter function: t(->tau)->y->theta
  Function parameterFunction(covariatesResult.getParameterFunction());
  parameterFunction = ComposedFunction(parameterFunction, timeToY);

  const Distribution parameterDistribution(covariatesResult.getParameterDistribution());
  const Scalar logLikelihood = covariatesResult.getLogLikelihood();
  const TimeVaryingResult result(*this, sample, parameterFunction, timeStamps, parameterDistribution, normalizationFunction, logLikelihood);
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
      const Scalar xi0,
      const Scalar m,
      const OptimizationAlgorithm & solver)
    : EvaluationImplementation()
    , sample_(sample)
    , sigma0_(sigma0)
    , xi0_(xi0)
    , m_(m)
    , solver_(solver)
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
  OptimizationAlgorithm solver_;
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

  const GeneralizedExtremeValueReturnLevelProfileLikelihoodEvaluation1 profileLikelihoodEvaluation(sample, sigma0, xi0, m, solver_);
  const Function objective(profileLikelihoodEvaluation.clone());

  OptimizationProblem problem(objective);
  problem.setMinimization(false);

  // solve optimization problem
  OptimizationAlgorithm solver(solver_);
  solver.setProblem(problem);
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
  const Scalar zmMin = -SpecFunc::ActualMaxScalar;
  const Scalar zmMax =  SpecFunc::ActualMaxScalar;
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

void GeneralizedExtremeValueFactory::setOptimizationAlgorithm(const OptimizationAlgorithm& solver)
{
  solver_ = solver;
}

OptimizationAlgorithm GeneralizedExtremeValueFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

END_NAMESPACE_OPENTURNS
