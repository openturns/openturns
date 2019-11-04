//                                               -*- C++ -*-
/**
 *  @brief Factory for Triangular distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/TriangularFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TriangularFactory)

static const Factory<TriangularFactory> Factory_TriangularFactory;

/* Default constructor */
TriangularFactory::TriangularFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
TriangularFactory * TriangularFactory::clone() const
{
  return new TriangularFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution TriangularFactory::build(const Sample & sample) const
{
  return buildAsTriangular(sample).clone();
}

Distribution TriangularFactory::build() const
{
  return buildAsTriangular().clone();
}

Distribution TriangularFactory::build(const Point & parameters) const
{
  return buildAsTriangular(parameters).clone();
}

Triangular TriangularFactory::buildAsTriangular(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Triangular distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Triangular distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  const Scalar xMin = sample.getMin()[0];
  const Scalar xMax = sample.getMax()[0];
  Scalar delta = xMax - xMin;
  const Scalar a = xMin - delta / (size + 2);
  const Scalar b = xMax + delta / (size + 2);
  if (!SpecFunc::IsNormal(a) || !SpecFunc::IsNormal(b)) throw InvalidArgumentException(HERE) << "Error: cannot build a Triangular distribution if data contains NaN or Inf";
  if (xMin == xMax)
  {
    delta = std::max(std::abs(xMin), 10.0) * SpecFunc::ScalarEpsilon;
    Triangular result(xMin - delta, xMin, xMax + delta);
    result.setDescription(sample.getDescription());
    return result;
  }
  const Scalar m = 3.0 * sample.computeMean()[0] - a - b;
  Triangular result(a, m, b);
  result.setDescription(sample.getDescription());
  return result;
}

Triangular TriangularFactory::buildAsTriangular(const Point & parameters) const
{
  try
  {
    Triangular distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Triangular distribution from the given parameters";
  }
}

Triangular TriangularFactory::buildAsTriangular() const
{
  return Triangular();
}

END_NAMESPACE_OPENTURNS
