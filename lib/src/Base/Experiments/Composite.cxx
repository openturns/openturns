//                                               -*- C++ -*-
/**
 *  @brief Implementation of the composite experiment plane
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
#include "Factorial.hxx"
#include "Axial.hxx"
#include "Composite.hxx"
#include "SpecFunc.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Composite);

/* Default constructor */
Composite::Composite()
  : StratifiedExperiment()
{
  // Nothing to do
}

/* Constructor with parameters */
Composite::Composite(const NumericalPoint & center,
                     const NumericalPoint & levels)
  : StratifiedExperiment(center, levels)
{
  // Nothing to do
}

/* Constructor with parameters */
Composite::Composite(const UnsignedInteger dimension,
                     const NumericalPoint & levels)
  : StratifiedExperiment(NumericalPoint(dimension, 0.0), levels)
{
  // Nothing to do
}

/* Virtual constructor */
Composite * Composite::clone() const
{
  return new Composite(*this);
}

/* Experiment plane generation */
NumericalSample Composite::generate()
{
  /* Dimension of the realizations */
  const UnsignedInteger dimension(center_.getDimension());
  /* Hypercube number of vertices */
  const UnsignedInteger verticesNumber((UnsignedInteger)round(pow(2.0, static_cast<int>(dimension))));
  /* Number of levels to be generated */
  const UnsignedInteger levelNumber(levels_.getDimension());
  /* Size of the sample to be generated: 1 + number of levels x (2 x dimension + 2^dimension) */
  UnsignedInteger size(1 + levelNumber * (2 * dimension + verticesNumber));
  /* A composite plane is made of an axial plane and a factorial plane with the same levels */
  /* First the axial plane */
  Axial axial(center_, levels_);
  const NumericalSample axialPlane(axial.generate());
  /* Second the factorial plane */
  Factorial factorial(center_, levels_);
  const NumericalSample factorialPlane(factorial.generate());
  /* We create the composite plane by copying the factorial plane (larger than the axial plane) then we append all the elements of the axial plane excepted its first one, which is the center of the plane and thus already present in the factorial plane */
  NumericalSample compositePlane(size, center_);
  compositePlane.setName("Composite plane");
  UnsignedInteger index(0);
  size = factorialPlane.getSize();
  for(UnsignedInteger factorialIndex = 0; factorialIndex < size; factorialIndex++)
  {
    compositePlane[index] = factorialPlane[factorialIndex];
    index++;
  }
  size = axialPlane.getSize();
  for(UnsignedInteger axialIndex = 1; axialIndex < size; axialIndex++)
  {
    compositePlane[index] = axialPlane[axialIndex];
    index++;
  }
  return compositePlane;
} // generate()

/* String converter */
String Composite::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " levels=" << levels_;
  return oss;
}

END_NAMESPACE_OPENTURNS
