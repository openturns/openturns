//                                               -*- C++ -*-
/**
 *  @brief Factory for FiniteDiscreteDistribution distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FiniteDiscreteDistributionFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FiniteDiscreteDistributionFactory)

static const Factory<FiniteDiscreteDistributionFactory> Factory_FiniteDiscreteDistributionFactory;
static const Factory<FiniteDiscreteDistributionFactory> Factory_UserDefinedFactory("UserDefinedFactory");

/* Default constructor */
FiniteDiscreteDistributionFactory::FiniteDiscreteDistributionFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
FiniteDiscreteDistributionFactory * FiniteDiscreteDistributionFactory::clone() const
{
  return new FiniteDiscreteDistributionFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution FiniteDiscreteDistributionFactory::build(const Sample & sample) const
{
  return buildAsFiniteDiscreteDistribution(sample, 0.0).clone();
}

Distribution FiniteDiscreteDistributionFactory::build(const Sample & sample,
    const Scalar epsilon) const
{
  return buildAsFiniteDiscreteDistribution(sample, epsilon).clone();
}

Distribution FiniteDiscreteDistributionFactory::build() const
{
  return buildAsFiniteDiscreteDistribution().clone();
}

FiniteDiscreteDistribution FiniteDiscreteDistributionFactory::buildAsFiniteDiscreteDistribution(const Sample & sample,
    const Scalar epsilon) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a FiniteDiscreteDistribution distribution from an empty sample";
  const Scalar mean = sample.computeMean()[0];
  if (!std::isfinite(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an FiniteDiscreteDistribution distribution if data contains NaN or Inf";
  const Scalar p = 1.0 / size;
  FiniteDiscreteDistribution result(sample, Point(size, p));
  result.compactSupport(epsilon);
  result.setDescription(sample.getDescription());
  return result;
}

FiniteDiscreteDistribution FiniteDiscreteDistributionFactory::buildAsFiniteDiscreteDistribution() const
{
  return FiniteDiscreteDistribution();
}


END_NAMESPACE_OPENTURNS
