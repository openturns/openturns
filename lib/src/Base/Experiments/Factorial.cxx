//                                               -*- C++ -*-
/**
 *  @brief Implementation of the factorial experiment plane
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
#include "openturns/OTprivate.hxx"
#include "openturns/Factorial.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx" // for boost.math.round
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Factorial)

static const Factory<Factorial> Factory_Factorial;

/* Default constructor */
Factorial::Factorial()
  : StratifiedExperiment()
{
  // Nothing to do
}

/* Constructor with parameters */
Factorial::Factorial(const Point & center,
                     const Point & levels)
  : StratifiedExperiment(center, levels)
{
  // Nothing to do
}

/* Constructor with parameters */
Factorial::Factorial(const UnsignedInteger dimension,
                     const Point & levels)
  : StratifiedExperiment(Point(dimension, 0.0), levels)
{
  // Nothing to do
}

/* Virtual constructor */
Factorial * Factorial::clone() const
{
  return new Factorial(*this);
}

/* Experiment plane generation */
Sample Factorial::generate() const
{
  /* Dimension of the realizations */
  const UnsignedInteger dimension = center_.getDimension();
  /* Hypercube number of vertices */
  const UnsignedInteger verticesNumber = (UnsignedInteger)round(pow(2.0, static_cast<int>(dimension)));
  /* Number of levels to be generated */
  const UnsignedInteger levelNumber = levels_.getDimension();
  /* Size of the sample to be generated: 1 + number of levels x 2^dimension */
  const UnsignedInteger size = 1 + levelNumber * verticesNumber;
  Sample factorialPlane(size, center_);
  factorialPlane.setName("Factorial plane");
  UnsignedInteger index = 1;
  /* For each level of the factorial plane */
  for(UnsignedInteger levelIndex = 0; levelIndex < levelNumber; ++levelIndex)
  {
    const Scalar levelValue = levels_[levelIndex];
    /* For each vertex of the current level */
    for(UnsignedInteger vertex = 0; vertex < verticesNumber; ++vertex)
    {
      /* Compute the coordinates of the current vertex */
      UnsignedInteger binaryIndex = vertex;
      for(UnsignedInteger coordinate = 0; coordinate < dimension; ++coordinate)
      {
        /* If the current bit of binaryIndex is 1, set the coordinate to center[coordinate]+levelValue, else set it to center[coordinate]-levelValue */
        factorialPlane(index, coordinate) += (2.0 * (binaryIndex % 2) - 1.0) * levelValue;
        binaryIndex /= 2;
      } // coordinate
      ++index;
    } // vertex
  } // levelIndex
  return factorialPlane;
} // generate()

/* String converter */
String Factorial::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " levels=" << levels_;
  return oss;
}

END_NAMESPACE_OPENTURNS
