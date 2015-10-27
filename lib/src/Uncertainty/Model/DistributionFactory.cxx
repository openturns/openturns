//                                               -*- C++ -*-
/**
 * @brief Top-level class for all distribution factories
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DistributionFactory.hxx"
#include "Description.hxx"
#include "Path.hxx"
#include "Exception.hxx"
#include "AliMikhailHaqCopulaFactory.hxx"
#include "ArcsineFactory.hxx"
#include "BernoulliFactory.hxx"
#include "BetaFactory.hxx"
#include "BinomialFactory.hxx"
#include "BurrFactory.hxx"
#include "ChiFactory.hxx"
#include "ChiSquareFactory.hxx"
#include "ClaytonCopulaFactory.hxx"
#include "DiracFactory.hxx"
#include "DirichletFactory.hxx"
#include "ExponentialFactory.hxx"
#include "FarlieGumbelMorgensternCopulaFactory.hxx"
#include "FisherSnedecorFactory.hxx"
#include "FrankCopulaFactory.hxx"
#include "GammaFactory.hxx"
#include "GeneralizedParetoFactory.hxx"
#include "GeometricFactory.hxx"
#include "GumbelCopulaFactory.hxx"
#include "GumbelFactory.hxx"
#include "HistogramFactory.hxx"
#include "InverseNormalFactory.hxx"
#include "LaplaceFactory.hxx"
#include "LogisticFactory.hxx"
#include "LogNormalFactory.hxx"
#include "LogUniformFactory.hxx"
#include "MeixnerDistributionFactory.hxx"
#include "MultinomialFactory.hxx"
#include "NegativeBinomialFactory.hxx"
#include "NormalCopulaFactory.hxx"
#include "NormalFactory.hxx"
#include "PoissonFactory.hxx"
#include "RayleighFactory.hxx"
#include "RiceFactory.hxx"
#include "SkellamFactory.hxx"
#include "StudentFactory.hxx"
#include "TrapezoidalFactory.hxx"
#include "TriangularFactory.hxx"
#include "TruncatedNormalFactory.hxx"
#include "UniformFactory.hxx"
#include "UserDefinedFactory.hxx"
#include "WeibullFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionFactory);

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
  collection.add(RayleighFactory());
  collection.add(RiceFactory());
  collection.add(StudentFactory());
  collection.add(TrapezoidalFactory());
  collection.add(TriangularFactory());
  collection.add(TruncatedNormalFactory());
  collection.add(UniformFactory());
  collection.add(WeibullFactory());
  return collection;
}

DistributionFactory::DistributionFactoryCollection DistributionFactory::GetContinuousMultiVariateFactories()
{
  DistributionFactoryCollection collection(0);
  collection.add(AliMikhailHaqCopulaFactory());
  collection.add(ClaytonCopulaFactory());
  collection.add(DirichletFactory());
  collection.add(FarlieGumbelMorgensternCopulaFactory());
  collection.add(FrankCopulaFactory());
  collection.add(GumbelCopulaFactory());
  collection.add(NormalCopulaFactory());
  collection.add(NormalFactory());
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
  : TypedInterfaceObject<DistributionImplementationFactory>(new DistributionImplementationFactory(bootstrapSize))
{
  // Nothing to do
}

/* Parameter constructor */
DistributionFactory::DistributionFactory(const DistributionImplementationFactory & factory)
  : TypedInterfaceObject<DistributionImplementationFactory>(factory.clone())
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
Distribution DistributionFactory::build(const NumericalSample & sample) const
{
  return getImplementation()->build(sample);
}

Distribution DistributionFactory::build(const NumericalPoint & parameters) const
{
  return getImplementation()->build(parameters);
}

Distribution DistributionFactory::build() const
{
  return getImplementation()->build();
}

DistributionFactoryResult DistributionFactory::buildEstimator(const NumericalSample & sample) const
{
  return getImplementation()->buildEstimator(sample);
}

END_NAMESPACE_OPENTURNS
