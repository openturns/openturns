//                                               -*- C++ -*-
/**
 *  @brief Factory for LogUniform distribution
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
#include "openturns/LogUniformFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LogUniformFactory)

static const Factory<LogUniformFactory> Factory_LogUniformFactory;

/* Default constructor */
LogUniformFactory::LogUniformFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
LogUniformFactory * LogUniformFactory::clone() const
{
  return new LogUniformFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution LogUniformFactory::build(const Sample & sample) const
{
  return buildAsLogUniform(sample).clone();
}

Distribution LogUniformFactory::build(const Point & parameters) const
{
  return buildAsLogUniform(parameters).clone();
}

Distribution LogUniformFactory::build() const
{
  return buildAsLogUniform().clone();
}

LogUniform LogUniformFactory::buildAsLogUniform(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a LogUniform distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a LogUniform distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar a = xMin - std::abs(xMin) / (2.0 + size);
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a LogUniform distribution from a sample that contains non positive values.";
  Scalar aLog = std::log(a);
  const Scalar xMax = sample.getMax()[0];
  const Scalar b = xMax + std::abs(xMax) / (2.0 + size);
  Scalar bLog = std::log(b);
  if (!SpecFunc::IsNormal(aLog) || !SpecFunc::IsNormal(bLog)) throw InvalidArgumentException(HERE) << "Error: cannot build a LogUniform distribution if data contains NaN or Inf";
  if (xMin == xMax)
  {
    aLog *= 1.0 - SpecFunc::ScalarEpsilon;
    bLog *= 1.0 + SpecFunc::ScalarEpsilon;
  }
  LogUniform result(aLog, bLog);
  result.setDescription(sample.getDescription());
  return result;
}

LogUniform LogUniformFactory::buildAsLogUniform(const Point & parameters) const
{
  try
  {
    LogUniform distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a LogUniform distribution from the given parameters";
  }
}

LogUniform LogUniformFactory::buildAsLogUniform() const
{
  return LogUniform();
}

END_NAMESPACE_OPENTURNS
