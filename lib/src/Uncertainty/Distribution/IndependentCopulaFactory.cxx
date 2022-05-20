//                                               -*- C++ -*-
/**
 *  @brief Factory for IndependentCopula distribution
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
#include <cmath>
#include "openturns/IndependentCopulaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndependentCopulaFactory)

static const Factory<IndependentCopulaFactory> Factory_IndependentCopulaFactory;

/* Default constructor */
IndependentCopulaFactory::IndependentCopulaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
IndependentCopulaFactory * IndependentCopulaFactory::clone() const
{
  return new IndependentCopulaFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution IndependentCopulaFactory::build(const Sample & sample) const
{
  return buildAsIndependentCopula(sample).clone();
}

Distribution IndependentCopulaFactory::build(const Point & parameters) const
{
  return buildAsIndependentCopula(parameters).clone();
}

Distribution IndependentCopulaFactory::build() const
{
  return buildAsIndependentCopula().clone();
}

IndependentCopula IndependentCopulaFactory::buildAsIndependentCopula(const Sample &) const
{
  return buildAsIndependentCopula();
}

IndependentCopula IndependentCopulaFactory::buildAsIndependentCopula(const Point &) const
{
  return buildAsIndependentCopula();
}

IndependentCopula IndependentCopulaFactory::buildAsIndependentCopula() const
{
  return IndependentCopula();
}

END_NAMESPACE_OPENTURNS
