//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel algorithms
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

Distribution MetaModelAlgorithm::BuildDistribution(const Sample & inputSample)
{
  // Recover the distribution, taking into account that we look for performance
  // so we avoid to rebuild expensive distributions as much as possible
  const UnsignedInteger inputDimension = inputSample.getDimension();
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
  Collection< Distribution > marginals(inputDimension);
  // The strategy for the marginals is to find the best continuous 1-d parametric model else fallback to a kernel smoothing
  KernelSmoothing ks;
  Collection< DistributionFactory > factories(DistributionFactory::GetContinuousUniVariateFactories());
  const Description inputDescription(inputSample.getDescription());
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    TestResult bestResult;
    // Here we remove the duplicate entries in the marginal sample as we are suppose to have a continuous distribution. The duplicates are mostly due to truncation in the file export.
    const Sample marginalSample(inputSample.getMarginal(i).sortUnique());
    Collection<Distribution> possibleDistributions(0);
    for (UnsignedInteger j = 0; j < factories.getSize(); ++j)
      try
      {
        possibleDistributions.add(factories[j].build(marginalSample));
      }
      catch (...)
      {
        // Just skip the factories incompatible with the current marginal sample
      }
    const Distribution candidate(FittingTest::BestModelKolmogorov(marginalSample, possibleDistributions, bestResult));
    // This threshold is somewhat arbitrary. It is here to avoid expensive kernel smoothing.
    if (bestResult.getPValue() >= ResourceMap::GetAsScalar("MetaModelAlgorithm-PValueThreshold")) marginals[i] = candidate;
    else marginals[i] = ks.build(marginalSample);
    marginals[i].setDescription(Description(1, inputDescription[i]));
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
