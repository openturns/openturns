//                                               -*- C++ -*-
/**
 *  @brief Factory for WeibullMin distribution
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
#include "openturns/WeibullMinFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/WeibullMinMuSigma.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/TNC.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMinFactory)

static const Factory<WeibullMinFactory> Factory_WeibullMinFactory;

/* Default constructor */
WeibullMinFactory::WeibullMinFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
WeibullMinFactory * WeibullMinFactory::clone() const
{
  return new WeibullMinFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution WeibullMinFactory::build(const Sample & sample) const
{
  return buildAsWeibullMin(sample).clone();
}

Distribution WeibullMinFactory::build(const Point & parameters) const
{
  return buildAsWeibullMin(parameters).clone();
}

Distribution WeibullMinFactory::build() const
{
  return buildAsWeibullMin().clone();
}

WeibullMin WeibullMinFactory::buildMethodOfMoments(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a WeibullMin distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a WeibullMin distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  Scalar gamma = xMin - std::abs(xMin) / (2.0 + size);
  const Scalar mean = sample.computeMean()[0];
  const Scalar sigma = sample.computeStandardDeviation()[0];
  if (!SpecFunc::IsNormal(gamma)) throw InvalidArgumentException(HERE) << "Error: cannot build a WeibullMin distribution if data contains NaN or Inf";
  try
  {
    Point parameters(3);
    parameters[0] = mean;
    parameters[1] = sigma;
    parameters[2] = gamma;
    WeibullMin result(buildAsWeibullMin(WeibullMinMuSigma()(parameters)));
    result.setDescription(sample.getDescription());
    return result;
  }
  // Here we are in the case of a (nearly) Dirac distribution
  catch (InvalidArgumentException &)
  {
    if (gamma == 0.0) gamma = SpecFunc::ScalarEpsilon;
    WeibullMin result(100.0 * std::abs(gamma) * SpecFunc::ScalarEpsilon, 1.0, gamma);
    result.setDescription(sample.getDescription());
    return result;
  }
}

WeibullMin WeibullMinFactory::buildMethodOfLikelihoodMaximization(const Sample & sample) const
{
  LOGINFO("in WeibullMinFactory, using likelihood maximisation");
  const MaximumLikelihoodFactory factory(buildMethodOfMoments(sample));
  return buildAsWeibullMin(factory.build(sample).getParameter());
}

WeibullMin WeibullMinFactory::buildAsWeibullMin(const Sample & sample) const
{
  return buildMethodOfLikelihoodMaximization(sample);
}

WeibullMin WeibullMinFactory::buildAsWeibullMin(const Point & parameters) const
{
  try
  {
    WeibullMin distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a WeibullMin distribution from the given parameters";
  }
}

WeibullMin WeibullMinFactory::buildAsWeibullMin() const
{
  return WeibullMin();
}


END_NAMESPACE_OPENTURNS
