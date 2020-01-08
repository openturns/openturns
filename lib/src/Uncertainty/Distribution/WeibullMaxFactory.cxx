//                                               -*- C++ -*-
/**
 *  @brief Factory for WeibullMax distribution
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
#include "openturns/WeibullMaxFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/WeibullMinFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMaxFactory)

static const Factory<WeibullMaxFactory> Factory_WeibullMaxFactory;

/* Default constructor */
WeibullMaxFactory::WeibullMaxFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
WeibullMaxFactory * WeibullMaxFactory::clone() const
{
  return new WeibullMaxFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution WeibullMaxFactory::build(const Sample & sample) const
{
  return buildAsWeibullMax(sample).clone();
}

Distribution WeibullMaxFactory::build(const Point & parameters) const
{
  return buildAsWeibullMax(parameters).clone();
}

Distribution WeibullMaxFactory::build() const
{
  return buildAsWeibullMax().clone();
}

WeibullMax WeibullMaxFactory::buildAsWeibullMax(const Sample & sample) const
{
  Distribution weibullMin(WeibullMinFactory().build(-1.0 * sample));
  Point parameter(weibullMin.getParameter());
  parameter[2] *= -1.0;// location parameter (gamma)
  return buildAsWeibullMax(parameter);
}

WeibullMax WeibullMaxFactory::buildAsWeibullMax(const Point & parameters) const
{
  try
  {
    WeibullMax distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a WeibullMax distribution from the given parameters";
  }
}

WeibullMax WeibullMaxFactory::buildAsWeibullMax() const
{
  return WeibullMax();
}


END_NAMESPACE_OPENTURNS
