//                                               -*- C++ -*-
/**
 *  @brief Implementation of the axial experiment plane
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/Axial.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Axial);

/* Default constructor */
Axial::Axial()
  : StratifiedExperiment()
{
  // Nothing to do
}

/* Constructor with parameters */
Axial::Axial(const NumericalPoint & center,
             const NumericalPoint & levels)
  : StratifiedExperiment(center, levels)
{
  // Nothing to do
}

/* Constructor with parameters */
Axial::Axial(const UnsignedInteger dimension,
             const NumericalPoint & levels)
  : StratifiedExperiment(NumericalPoint(dimension, 0.0), levels)
{
  // Nothing to do
}

/* Virtual constructor */
Axial * Axial::clone() const
{
  return new Axial(*this);
}

/* Experiment plane generation */
NumericalSample Axial::generate()
{
  /* Dimension of the realizations */
  const UnsignedInteger dimension = center_.getDimension();
  /* Number of level to be generated */
  const UnsignedInteger levelNumber = levels_.getDimension();
  /* Size of the sample to be generated: 1 + 2 x number of levels x dimension */
  const UnsignedInteger size = 1 + 2 * levelNumber * dimension;
  NumericalSample axialPlane(size, center_);
  axialPlane.setName("Axial plane");
  UnsignedInteger index = 1;
  /* For each level of the axial plane */
  for(UnsignedInteger levelIndex = 0; levelIndex < levelNumber; ++levelIndex)
  {
    const NumericalScalar levelValue = levels_[levelIndex];
    /* For each vertex of one half of the current level */
    for(UnsignedInteger vertex = 0; vertex < dimension; ++vertex)
    {
      /* Compute the coordinates of the positive vertex */
      axialPlane[index][vertex] += levelValue;
      ++index;
      /* Compute the coordinates of the negative vertex */
      axialPlane[index][vertex] -= levelValue;
      ++index;
    } // vertex
  } // levelIndex
  return axialPlane;
} // generate()

/* String converter */
String Axial::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " levels=" << levels_;
  return oss;
}

END_NAMESPACE_OPENTURNS
