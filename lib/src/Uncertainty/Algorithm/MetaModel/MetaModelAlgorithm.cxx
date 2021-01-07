//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel algorithms
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/HypothesisTest.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetaModelAlgorithm)

static const Factory<MetaModelAlgorithm> Factory_MetaModelAlgorithm;

/* Default constructor */
MetaModelAlgorithm::MetaModelAlgorithm()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
MetaModelAlgorithm::MetaModelAlgorithm(const Distribution & distribution,
                                       const Function & model)
  : PersistentObject()
  , distribution_(distribution)
  , model_(model)
{
  // Nothing to do
}

/* Virtual constructor */
MetaModelAlgorithm * MetaModelAlgorithm::clone() const
{
  return new MetaModelAlgorithm(*this);
}

/* String converter */
String MetaModelAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

struct TestedDistribution
{
  Distribution distribution_;
  Scalar score_;
  Scalar bic_;
  Scalar aic_;
  Scalar aicc_;
  String criterion_;

  TestedDistribution(const Distribution & distribution,
                     const Scalar score,
                     const Scalar bic,
                     const Scalar aic,
                     const Scalar aicc,
                     const String & criterion)
    : distribution_(distribution)
    , score_(score)
    , bic_(bic)
    , aic_(aic)
    , aicc_(aicc)
    , criterion_(criterion)
  {
    // Nothing to do
  }

  bool operator< (const TestedDistribution& other) const
  {
    if (criterion_ == "BIC")
      return bic_ < other.bic_;
    else if (criterion_ == "AIC")
      return aic_ < other.aic_;
    else if (criterion_ == "AICC")
      return aicc_ < other.aicc_;
    else
      return score_ > other.score_;
  }
};

Distribution MetaModelAlgorithm::BuildDistribution(const Sample & inputSample)
{
  // Recover the distribution, taking into account that we look for performance
  // so we avoid to rebuild expensive distributions as much as possible
  const UnsignedInteger inputDimension = inputSample.getDimension();
  Collection< Distribution > marginals(inputDimension);
  // The strategy for the marginals is to find the best continuous 1-d parametric model else fallback to a non-parametric one
  DistributionFactory nonParametricModel;
  const String nonParametric = ResourceMap::GetAsString("MetaModelAlgorithm-NonParametricModel");
  if (nonParametric == "Histogram")
    nonParametricModel = DistributionFactory(HistogramFactory());
  else
    nonParametricModel = DistributionFactory(KernelSmoothing());

  Collection< DistributionFactory > factories(DistributionFactory::GetContinuousUniVariateFactories());
  // Filter factories : remove histogram
  for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
  {
    if (factories[i].getImplementation()->getClassName() == "HistogramFactory")
    {
      factories.erase(factories.begin() + i);
      break;
    }
  }

  // Level for model qualification using KS test
  const Scalar level = ResourceMap::GetAsScalar("MetaModelAlgorithm-PValueThreshold");
  // Model selection using BIC/AIC/AICC criterion
  const String criterion = ResourceMap::GetAsString("MetaModelAlgorithm-ModelSelectionCriterion");

  const Description inputDescription(inputSample.getDescription());
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    // Here we remove the duplicate entries in the marginal sample as we are suppose to have a continuous distribution.
    // The duplicates are mostly due to truncation in the file export.
    const Sample marginalSample(inputSample.getMarginal(i).sortUnique());
    // First we estimate distribution using its factory
    // Once it is possible, we keep it only if Kolmogorov score exceed level
    std::vector<TestedDistribution> possibleDistributions;
    LOGINFO(OSS() << "In MetaModelAlgorithm::BuildDistribution, estimate distribution for marginal " << i);
    for (UnsignedInteger j = 0; j < factories.getSize(); ++j)
    {
      LOGINFO(OSS() << "BuildDistribution: trying " << factories[j]);
      try
      {
        const Distribution candidateDistribution(factories[j].build(marginalSample));
        const Scalar pValue = FittingTest::Kolmogorov(marginalSample, candidateDistribution, level).getPValue();
        const Bool isKSAccepted = (pValue >= level);
        if (isKSAccepted)
        {
          const Scalar BIC = FittingTest::BIC(marginalSample, candidateDistribution, candidateDistribution.getParameterDimension());
          const Scalar AIC = FittingTest::AIC(marginalSample, candidateDistribution, candidateDistribution.getParameterDimension());
          const Scalar AICC = FittingTest::AICC(marginalSample, candidateDistribution, candidateDistribution.getParameterDimension());
          TestedDistribution test(candidateDistribution, pValue, BIC, AIC, AICC, criterion);
          LOGINFO(OSS() << "Candidate distribution = " << candidateDistribution.getImplementation()->getClassName() << ", pValue=" << pValue << ", BIC=" << BIC << ", AIC=" << AIC << ", AICC=" << AICC);
          possibleDistributions.push_back(test);
        }
        else
          LOGINFO(OSS() << "Tested distribution & not selected = " << candidateDistribution.getImplementation()->getClassName() << ", pValue=" << pValue);
      }
      catch (...)
      {
        // Just skip the factories incompatible with the current marginal sample
        // or distribution that are not valid according to the KS test
      }
    } //for j in factories
    // Make sure the list is not empty
    // otherwise the candidate is a non-parametric model
    if (!(possibleDistributions.size() > 0))
      marginals[i] = nonParametricModel.build(marginalSample);
    else
    {
      // Now we return the "best" model according to a fixed criterion
      // This last one might be : BIC, AIC, AICC
      std::sort(possibleDistributions.begin(), possibleDistributions.end());
      marginals[i] = possibleDistributions[0].distribution_;
    }// else
    marginals[i].setDescription(Description(1, inputDescription[i]));
    LOGINFO(OSS() << "Selected distribution = " << marginals[i].getImplementation()->getClassName());
  }// for i

  // For the dependence structure, we use the Spearman independence test to decide between an independent and a Normal copula.
  Bool isIndependent = true;
  for (UnsignedInteger j = 0; j < inputDimension && isIndependent; ++ j)
  {
    const Sample marginalJ = inputSample.getMarginal(j);
    for (UnsignedInteger i = j + 1; i < inputDimension && isIndependent; ++ i)
    {
      TestResult testResult(HypothesisTest::Spearman(inputSample.getMarginal(i), marginalJ));
      isIndependent = isIndependent && testResult.getBinaryQualityMeasure();
    }
  }

  ComposedDistribution distribution(marginals);
  if (!isIndependent)
    distribution.setCopula(NormalCopulaFactory().build(inputSample));
  return distribution;
}


/* Distribution accessor */
void MetaModelAlgorithm::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}

Distribution MetaModelAlgorithm::getDistribution() const
{
  return distribution_;
}

/* Response surface computation */
void MetaModelAlgorithm::run()
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::run()";
}


Sample MetaModelAlgorithm::getInputSample() const
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::getInputSample() const";
}


Sample MetaModelAlgorithm::getOutputSample() const
{
  throw NotYetImplementedException(HERE) << "In MetaModelAlgorithm::getOutputSample() const";
}


/* Method save() stores the object through the StorageManager */
void MetaModelAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "model_", model_ );

}

/* Method load() reloads the object from the StorageManager */
void MetaModelAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "model_", model_ );
}

END_NAMESPACE_OPENTURNS
