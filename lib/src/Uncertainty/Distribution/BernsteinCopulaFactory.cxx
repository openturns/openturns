//                                               -*- C++ -*-
/**
 *  @brief This class implements a non parametric density estimator for copulas
 *         based on the Bernstein copula approximation of the empirical copula
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
#include "openturns/BernsteinCopulaFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KFoldSplitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BernsteinCopulaFactory
 *
 * The class describes the probabilistic concept of BernsteinCopulaFactory.
 */

CLASSNAMEINIT(BernsteinCopulaFactory)

static const Factory<BernsteinCopulaFactory> Factory_BernsteinCopulaFactory;

/* Default constructor */
BernsteinCopulaFactory::BernsteinCopulaFactory()
  : DistributionFactoryImplementation()
{
  setName("BernsteinCopulaFactory");
}

/* Virtual constructor */
BernsteinCopulaFactory * BernsteinCopulaFactory::clone() const
{
  return new BernsteinCopulaFactory(*this);
}

/* Compute the number of bins according to the inverse power rule */
UnsignedInteger BernsteinCopulaFactory::ComputeAMISEBinNumber(const Sample & sample)
{
  return static_cast<UnsignedInteger>(1.0 + pow(sample.getSize(), 2.0 / (4.0 + sample.getDimension())));
}

// Build all the learning/validation partitions of the given sample
void BernsteinCopulaFactory::BuildCrossValidationSamples(const Sample & sample,
    const UnsignedInteger kFraction,
    Collection<Sample> & learningCollection,
    Collection<Sample> & validationCollection)
{
  if (kFraction < 2) throw InvalidArgumentException(HERE) << "Error: the fraction number must be greater or equal to 2, here kFraction=" << kFraction;
  const UnsignedInteger size = sample.getSize();
  if (kFraction >= size) throw InvalidArgumentException(HERE) << "Error: the fraction number must be less than the sample size, here kFraction=" << kFraction << " and sample size=" << size;

  // Create the samples
  validationCollection = Collection<Sample>(0);
  learningCollection = Collection<Sample>(0);
  KFoldSplitter splitter(size, kFraction);
  for (UnsignedInteger k = 0; k < kFraction; ++k)
  {
    // Select the points
    Indices indices2;
    Indices indices1(splitter.generate(indices2));
    Sample validationSample(sample.select(indices2));
    Sample learningSample(sample.select(indices1));

    // No need to rank the validation sample as it is supposed to
    // be distributed according to an unknown copula
    validationCollection.add(validationSample);
    // Rank the learning sample as it is mandatory for the creation of an
    // EmpiricalBernsteinCopula (hence the name: empirical)
    learningSample = (learningSample.rank() + 1.0) / learningSample.getSize();
    learningCollection.add(learningSample);
  } // k
}

namespace
{
// Class used to compute the mean log-likelihood of Bernstein copulas
// learnt over a set of learning samples and evaluated over a corresponding
// set of validation samples
class LogLikelihoodObjective: public FunctionImplementation
{
public:
  LogLikelihoodObjective(const Collection<Sample> & learningSamples,
                         const Collection<Sample> & validationSamples)
    : FunctionImplementation()
    , learningSamples_(learningSamples)
    , validationSamples_(validationSamples)
    , kFraction_(learningSamples.getSize())
  {
    // Nothing to do
  }

  LogLikelihoodObjective * clone() const
  {
    return new LogLikelihoodObjective(*this);
  }

  Point operator() (const Point & point) const
  {
    const UnsignedInteger m = static_cast<UnsignedInteger>(point[0]);
    return Point(1, computeLogLikelihood(m));
  }

  Scalar computeLogLikelihood(const UnsignedInteger m) const
  {
    LOGINFO(OSS() << "In computeLogLikelihood, m=" << m);
    if (m == 1) return 0.0;
    Scalar result = 0.0;
    for (UnsignedInteger k = 0; k < kFraction_; ++k)
    {
      const Sample learning(learningSamples_[k]);
      const Sample validation(validationSamples_[k]);
      LOGINFO("Build copula");
      const EmpiricalBernsteinCopula copula(learning, m, true);
      LOGINFO("Compute log-PDF");
      result -= copula.computeLogPDF(validation).computeMean()[0];
    } // k
    return result / kFraction_;
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description(1, "m");
  }

  Description getOutputDescription() const
  {
    return Description(1, "LogLikelihoodObjective");
  }

  String __repr__() const
  {
    OSS oss;
    oss << "LogLikelihoodObjective(" << learningSamples_.__str__() << ", " << validationSamples_.__str__() << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "LogLikelihoodObjective(" << learningSamples_.__str__() << ", " << validationSamples_.__str__() << ")";
    return oss;
  }

private:
  const Collection<Sample> & learningSamples_;
  const Collection<Sample> & validationSamples_;
  UnsignedInteger kFraction_;
};  // class MeanObjective

// Class used to compute the penalized csiszar divergence objective
class PenalizedCsiszarDivergenceObjective: public FunctionImplementation
{
public:
  PenalizedCsiszarDivergenceObjective(const Sample & sample,
                                      const Function & objective,
                                      const Scalar alpha)
    : FunctionImplementation()
    , sample_(sample)
    , objective_(objective)
    , alpha_(alpha)
    , N_(ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-SamplingSize"))
    , rhoS_(*sample.computeSpearmanCorrelation().getImplementation())
  {
    // Nothing to do
  }

  PenalizedCsiszarDivergenceObjective * clone() const
  {
    return new PenalizedCsiszarDivergenceObjective(*this);
  }

  Scalar computeMC(const EmpiricalBernsteinCopula & copula,
                   const Sample & sample) const
  {
    const UnsignedInteger size = sample.getSize();
    const Sample pdfSample(copula.computePDF(sample));
    Scalar value = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar pdf = pdfSample(i, 0);
      if (pdf > 0.0) value += pdf * objective_(Point(1, 1.0 / pdf))[0];
    } // i
    return value / size;
  }

  Point operator() (const Point & point) const
  {
    const UnsignedInteger m = static_cast<UnsignedInteger>(point[0]);
    return Point(1, computeCsiszar(m));
  }

  Scalar computeCsiszar(const UnsignedInteger m) const
  {
    const EmpiricalBernsteinCopula copula(sample_, m, false);
    const MatrixImplementation rhoM(*copula.getSpearmanCorrelation().getImplementation());
    const Scalar rHat = computeMC(copula, copula.getSample(N_));
    const Scalar rTilde = computeMC(copula, sample_);
    const Scalar delta = rHat - rTilde;
    return delta * delta + alpha_ * Point(rhoS_ - rhoM).normSquare() / std::pow(copula.getDimension(), 2.0);
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description(1, "m");
  }

  Description getOutputDescription() const
  {
    return Description(1, "PenalizedCsiszarDivergenceObjective");
  }

  String __repr__() const
  {
    OSS oss;
    oss << "PenalizedCsiszarDivergenceObjective(" << sample_.__str__() << ", " << objective_.__str__() << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "PenalizedCsiszarDivergenceObjective(" << sample_.__str__() << ", " << objective_.__str__() << ")";
    return oss;
  }

private:
  const Sample & sample_;
  const Function & objective_;
  Scalar alpha_;
  UnsignedInteger N_;
  MatrixImplementation rhoS_;
};  // class PenalizedCsiszarDivergenceObjective
} // anonymous namespace

// Find the best bin number using an exhaustive search between two bounds given through ResourceMap, or between 1 and size/2 if the bounds are in reverse order
UnsignedInteger BernsteinCopulaFactory::FindBestBinNumberSequential(const Function & mObjective,
    const UnsignedInteger mMin,
    const UnsignedInteger mMax)
{
  Scalar bestMValue = SpecFunc::MaxScalar;
  UnsignedInteger bestM = 0;
  for (UnsignedInteger m = mMin; m < mMax; ++m)
  {
    const Scalar mValue = mObjective(Point(1, m))[0];
    LOGINFO(OSS() << "Searching for the best bin number, m=" << m << ", objective=" << mValue << ", best so far=" << bestM << ", best objective=" << bestMValue);
    if (mValue < bestMValue)
    {
      bestMValue = mValue;
      bestM = m;
    }
  } // m
  return bestM;
}

// Compute the optimal bin number according to the maximum mean log-likelihood function computed using cross-validation
UnsignedInteger BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber(const Sample & sample,
    const UnsignedInteger kFraction)
{
  Collection<Sample> learningCollection(0);
  Collection<Sample> validationCollection(0);
  if (kFraction > 1)
    BuildCrossValidationSamples(sample, kFraction, learningCollection, validationCollection);
  else
  {
    learningCollection.add((sample.rank() + 1.0) / sample.getSize());
    validationCollection.add(sample);
  }
  UnsignedInteger mMin = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MinM");
  UnsignedInteger mMax = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MaxM");
  if (mMin > mMax)
  {
    mMin = 1;
    mMax = learningCollection[0].getSize();
  }
  return FindBestBinNumberSequential(LogLikelihoodObjective(learningCollection, validationCollection), mMin, mMax);
}

// Compute the optimal bin number according to the maximum mean log-likelihood function computed using cross-validation
UnsignedInteger BernsteinCopulaFactory::ComputePenalizedCsiszarDivergenceBinNumber(const Sample & sample,
    const Function & f,
    const Scalar alpha)
{
  UnsignedInteger mMin = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MinM");
  UnsignedInteger mMax = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MaxM");
  if (mMin > mMax)
  {
    mMin = 1;
    mMax = sample.getSize();
  }
  return FindBestBinNumberSequential(PenalizedCsiszarDivergenceObjective(sample, f, alpha), mMin, mMax);
}

/* Build a Bernstein copula based on the given sample. The bin number is computed according to the rule given by ResourceMap */
Distribution BernsteinCopulaFactory::build() const
{
  return buildAsEmpiricalBernsteinCopula().clone();
}

Distribution BernsteinCopulaFactory::build(const Sample & sample) const
{
  return buildAsEmpiricalBernsteinCopula(sample, ResourceMap::GetAsString("BernsteinCopulaFactory-BinNumberSelectionMethod")).clone();
}

Distribution BernsteinCopulaFactory::build(const Sample & sample,
    const String & method,
    const Function & objective) const
{
  return buildAsEmpiricalBernsteinCopula(sample, method, objective).clone();
}

Distribution BernsteinCopulaFactory::build(const Sample & sample,
    const UnsignedInteger binNumber) const
{
  return buildAsEmpiricalBernsteinCopula(sample, binNumber).clone();
}

/* Build a Bernstein copula based on the given sample. */
EmpiricalBernsteinCopula BernsteinCopulaFactory::buildAsEmpiricalBernsteinCopula() const
{
  return EmpiricalBernsteinCopula();
}

EmpiricalBernsteinCopula BernsteinCopulaFactory::buildAsEmpiricalBernsteinCopula(const Sample & sample,
    const String & method,
    const Function & objective) const
{
  const UnsignedInteger minM = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MinM");
  const UnsignedInteger maxM = ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-MaxM");
  UnsignedInteger m = 0;
  if (minM == maxM)
    m = minM;
  else
  {
    if (method == "AMISE")
      m = ComputeAMISEBinNumber(sample);
    else if (method == "LogLikelihood")
      m = ComputeLogLikelihoodBinNumber(sample, ResourceMap::GetAsUnsignedInteger("BernsteinCopulaFactory-kFraction"));
    else if (method == "PenalizedCsiszarDivergence")
      m = ComputePenalizedCsiszarDivergenceBinNumber(sample, objective, ResourceMap::GetAsScalar("BernsteinCopulaFactory-alpha"));
    else throw InvalidArgumentException(HERE) << "Error: the given method=" << method << " is not valid.";
  }
  LOGINFO(OSS() << "m=" << m);
  return EmpiricalBernsteinCopula(sample, m);
}

EmpiricalBernsteinCopula BernsteinCopulaFactory::buildAsEmpiricalBernsteinCopula(const Sample & sample,
    const UnsignedInteger binNumber) const
{
  if (binNumber == 0) throw InvalidDimensionException(HERE) << "Error: the bin number must be positive for the BernsteinCopulaFactory";
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidDimensionException(HERE) << "Error: cannot build a copula using the Bernstein copula factory based on an empty sample";
  if (binNumber > size) throw InvalidArgumentException(HERE) << "Error: cannot build a copula using the Bernstein copula factory when the bin number is greater than the sample size";
  return EmpiricalBernsteinCopula(sample, binNumber);
}

END_NAMESPACE_OPENTURNS
