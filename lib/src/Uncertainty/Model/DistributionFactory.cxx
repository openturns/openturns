//                                               -*- C++ -*-
/**
 * @brief Top-level class for all distribution factories
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/DistributionFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Path.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AliMikhailHaqCopulaFactory.hxx"
#include "openturns/ArcsineFactory.hxx"
#include "openturns/BernoulliFactory.hxx"
#include "openturns/BetaFactory.hxx"
#include "openturns/BernsteinCopulaFactory.hxx"
#include "openturns/BinomialFactory.hxx"
#include "openturns/BurrFactory.hxx"
#include "openturns/ChiFactory.hxx"
#include "openturns/ChiSquareFactory.hxx"
#include "openturns/ClaytonCopulaFactory.hxx"
#include "openturns/DiracFactory.hxx"
#include "openturns/DirichletFactory.hxx"
#include "openturns/ExponentialFactory.hxx"
#include "openturns/FarlieGumbelMorgensternCopulaFactory.hxx"
#include "openturns/FisherSnedecorFactory.hxx"
#include "openturns/FrankCopulaFactory.hxx"
#include "openturns/FrechetFactory.hxx"
#include "openturns/GammaFactory.hxx"
#include "openturns/GeneralizedParetoFactory.hxx"
#include "openturns/GeometricFactory.hxx"
#include "openturns/GumbelCopulaFactory.hxx"
#include "openturns/GumbelFactory.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/InverseNormalFactory.hxx"
#include "openturns/LaplaceFactory.hxx"
#include "openturns/LogisticFactory.hxx"
#include "openturns/LogNormalFactory.hxx"
#include "openturns/LogUniformFactory.hxx"
#include "openturns/MeixnerDistributionFactory.hxx"
#include "openturns/MultinomialFactory.hxx"
#include "openturns/NegativeBinomialFactory.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/ParetoFactory.hxx"
#include "openturns/PlackettCopulaFactory.hxx"
#include "openturns/PoissonFactory.hxx"
#include "openturns/RayleighFactory.hxx"
#include "openturns/RiceFactory.hxx"
#include "openturns/SkellamFactory.hxx"
#include "openturns/StudentFactory.hxx"
#include "openturns/TrapezoidalFactory.hxx"
#include "openturns/TriangularFactory.hxx"
#include "openturns/TruncatedNormalFactory.hxx"
#include "openturns/UniformFactory.hxx"
#include "openturns/UserDefinedFactory.hxx"
#include "openturns/VonMisesFactory.hxx"
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/WeibullMinFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionFactory)

/* Catalog of factories */
DistributionFactory::DistributionFactoryCollection DistributionFactory::GetContinuousUniVariateFactories()
{
  DistributionFactoryCollection collection(0);
  collection.add(ArcsineFactory());
  collection.add(BetaFactory());
  collection.add(BurrFactory());
  collection.add(ChiFactory());
  collection.add(ChiSquareFactory());
  collection.add(DirichletFactory());
  collection.add(ExponentialFactory());
  collection.add(FisherSnedecorFactory());
  collection.add(FrechetFactory());
  collection.add(GammaFactory());
  collection.add(GeneralizedParetoFactory());
  collection.add(GumbelFactory());
  collection.add(HistogramFactory());
  collection.add(InverseNormalFactory());
  collection.add(LaplaceFactory());
  collection.add(LogisticFactory());
  collection.add(LogNormalFactory());
  collection.add(LogUniformFactory());
  collection.add(MeixnerDistributionFactory());
  collection.add(NormalFactory());
  collection.add(ParetoFactory());
  collection.add(RayleighFactory());
  collection.add(RiceFactory());
  collection.add(StudentFactory());
  collection.add(TrapezoidalFactory());
  collection.add(TriangularFactory());
  collection.add(TruncatedNormalFactory());
  collection.add(UniformFactory());
  collection.add(VonMisesFactory());
  collection.add(WeibullMaxFactory());
  collection.add(WeibullMinFactory());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetContinuousMultiVariateFactories()
{
  DistributionFactoryCollection collection(0);
  collection.add(AliMikhailHaqCopulaFactory());
  collection.add(BernsteinCopulaFactory());
  collection.add(ClaytonCopulaFactory());
  collection.add(DirichletFactory());
  collection.add(FarlieGumbelMorgensternCopulaFactory());
  collection.add(FrankCopulaFactory());
  collection.add(GumbelCopulaFactory());
  collection.add(NormalCopulaFactory());
  collection.add(NormalFactory());
  collection.add(PlackettCopulaFactory());
  collection.add(StudentFactory());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetDiscreteUniVariateFactories()
{
  DistributionFactoryCollection collection(0);
  collection.add(BernoulliFactory());
  collection.add(BinomialFactory());
  collection.add(DiracFactory());
  collection.add(GeometricFactory());
  collection.add(MultinomialFactory());
  collection.add(NegativeBinomialFactory());
  collection.add(PoissonFactory());
  collection.add(UserDefinedFactory());
  collection.add(SkellamFactory());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetDiscreteMultiVariateFactories()
{
  DistributionFactoryCollection collection(0);
  collection.add(DiracFactory());
  collection.add(MultinomialFactory());
  collection.add(UserDefinedFactory());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetUniVariateFactories()
{
  DistributionFactoryCollection collection(DistributionFactory::GetContinuousUniVariateFactories());
  collection.add(DistributionFactory::GetDiscreteUniVariateFactories());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetMultiVariateFactories()
{
  DistributionFactoryCollection collection(DistributionFactory::GetContinuousMultiVariateFactories());
  collection.add(DistributionFactory::GetDiscreteMultiVariateFactories());
  return collection;
}

/* Default constructor */
DistributionFactory::DistributionFactory(const UnsignedInteger bootstrapSize)
  : TypedInterfaceObject<DistributionFactoryImplementation>(new DistributionFactoryImplementation(bootstrapSize))
{
  // Nothing to do
}

/* Parameter constructor */
DistributionFactory::DistributionFactory(const DistributionFactoryImplementation & factory)
  : TypedInterfaceObject<DistributionFactoryImplementation>(factory.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
DistributionFactory::DistributionFactory(DistributionFactoryImplementation * p_implementation)
  : TypedInterfaceObject<DistributionFactoryImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String DistributionFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << DistributionFactory::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String DistributionFactory::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Here is the interface that all derived class must implement */
Distribution DistributionFactory::build(const Sample & sample) const
{
  return getImplementation()->build(sample);
}

Distribution DistributionFactory::build(const Point & parameters) const
{
  return getImplementation()->build(parameters);
}

Distribution DistributionFactory::build() const
{
  return getImplementation()->build();
}

DistributionFactoryResult DistributionFactory::buildEstimator(const Sample & sample) const
{
  return getImplementation()->buildEstimator(sample);
}

DistributionFactoryResult DistributionFactory::buildEstimator(const Sample & sample, const DistributionParameters & parameters) const
{
  return getImplementation()->buildEstimator(sample, parameters);
}

END_NAMESPACE_OPENTURNS
