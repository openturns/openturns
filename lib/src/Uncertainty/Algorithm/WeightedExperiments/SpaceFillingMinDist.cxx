//                                               -*- C++ -*-
/**
 *  @brief SpaceFillingMinDist
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
#include "openturns/SpaceFillingMinDist.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#include <limits>
#include <cmath>


namespace OT
{

CLASSNAMEINIT(SpaceFillingMinDist)

static const Factory<SpaceFillingMinDist> Factory_SpaceFillingMinDist;


/* Default constructor */
SpaceFillingMinDist::SpaceFillingMinDist()
  : SpaceFillingImplementation(false)
{
  setName("MinDist");
}

/* Virtual constructor method */
SpaceFillingMinDist * SpaceFillingMinDist::clone() const
{
  return new SpaceFillingMinDist(*this);
}

String SpaceFillingMinDist::__repr__() const
{
  OSS oss;
  oss << "class=" << SpaceFillingMinDist::GetClassName()
      << " minimization=" << isMinimizationProblem();
  return oss;
}

/** Evaluate criterion on a sample */
Scalar SpaceFillingMinDist::evaluate(const Sample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  const UnsignedInteger dimension(sample.getDimension());
  const Sample normalizedSample(normalize(sample));
  const Scalar* addr_sample = &normalizedSample(0, 0);
  Scalar minDist = std::numeric_limits<Scalar>::max();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar* ptI = addr_sample + dimension * i;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const Scalar* ptJ = addr_sample + dimension * j;
      Scalar squaredNorm = 0.0;
      for (UnsignedInteger d = 0; d < dimension; ++d)
      {
        const Scalar delta(ptI[d] - ptJ[d]);
        squaredNorm += delta * delta;
      }
      minDist = std::min(minDist, squaredNorm);
    }
  }
  return std::sqrt(minDist);
}

} /* namespace OT */
