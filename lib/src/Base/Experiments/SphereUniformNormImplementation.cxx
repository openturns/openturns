//                                               -*- C++ -*-
/**
 *  @brief Implementation of a generic grid iterator.
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SphereUniformNormImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

class SphereUniformNorm1D
  : public SphereUniformNormImplementation
{
public:

  SphereUniformNorm1D(const Point & step, const Bool symmetric)
    : SphereUniformNormImplementation(step, symmetric)
  {
    if (step.getDimension() != 1) throw InvalidDimensionException(HERE) << "Expected a point of dimension 1";
    stepX_ = step[0];
  }

  Sample getPoints(const UnsignedInteger distance) const
  {
    const UnsignedInteger size = getSizeOfLevel(distance);
    Sample points(size, 1);
    if (distance == 0)
    {
      points[0] = Point(1, 0.0);
    }
    else
    {
      const NumericalScalar cx = distance * stepX_;
      Point element(1, cx);
      element[0] = cx;
      points[0] = element;
      if (!isSymmetric())
      {
        element[0] = -cx;
        points[1] = element;
      }
    }
    return points;
  }

  UnsignedInteger getOffsetLevel(const UnsignedInteger distance) const
  {
    if (distance == 0) return 0;
    if (isSymmetric())
      return distance;
    else
      return 2 * distance - 1;
  }

private:
  UnsignedInteger getSizeOfLevel(const UnsignedInteger distance) const
  {
    if (distance == 0 || isSymmetric()) return 1;
    else return 2;
  }

  NumericalScalar stepX_;
};

class SphereUniformNorm2D
  : public SphereUniformNormImplementation
{
public:

  SphereUniformNorm2D(const Point & step, const Bool symmetric)
    : SphereUniformNormImplementation(step, symmetric)
  {
    // nothing to do
    if (step.getDimension() != 2) throw InvalidDimensionException(HERE) << "Expected a point of dimension 2";
    stepX_ = step[0];
    stepY_ = step[1];
  }

  Sample getPoints(const UnsignedInteger distance) const
  {
    // Basic method that uses Sample
    // TODO use of iterators ?
    if (distance == 0) return Sample(1, 2);

    const UnsignedInteger size = getSizeOfLevel(distance);
    Sample points(size, 2);
    UnsignedInteger iterator = 0;
    const NumericalScalar cx = distance * stepX_;
    const NumericalScalar cy = distance * stepY_;
    if (isSymmetric())
    {
      Point element(2, 0.0);
      element[1] = cy;
      // 1) top
      for (UnsignedInteger ix = 0; ix < 2 * distance; ++ix, ++iterator)
      {
        element[0] = ix * stepX_ - cx;
        points[iterator] = element;
      }
      // 2) right
      element[0] = cx;
      for (UnsignedInteger iy = 2 * distance; iy > 0; --iy, ++iterator)
      {
        element[1] = iy * stepY_ - cy;
        points[iterator] = element;
      }
    }
    else
    {
      Point element(2, 0.0);
      // 1) left
      element[0] = - cx;
      for (UnsignedInteger iy = 0; iy < 2 * distance; ++iy, ++iterator)
      {
        element[1] = iy * stepY_ - cy;
        points[iterator] = element;
      }
      // 2) top
      element[1] = cy;
      for (UnsignedInteger ix = 0; ix < 2 * distance; ++ix, ++iterator)
      {
        element[0] = ix * stepX_ - cx;
        points[iterator] = element;
      }
      // 3) right
      element[0] = cx;
      for (UnsignedInteger iy = 2 * distance; iy > 0; --iy, ++iterator)
      {
        element[1] = iy * stepY_ - cy;
        points[iterator] = element;
      }
      // 4) bottom
      element[1] = - cy;
      for (UnsignedInteger ix = 2 * distance; ix > 0; --ix, ++iterator)
      {
        element[0] = ix * stepX_ - cx;
        points[iterator] = element;
      }
    }
    if (iterator != size) throw InternalException(HERE) << "Error in SphereUniformNorm2D::getPoints";
    return points;
  }

  UnsignedInteger getOffsetLevel(const UnsignedInteger distance) const
  {
    if (distance == 0) return 0;
    if (isSymmetric())
      return 2 * distance * (distance - 1) + 1;
    else
      return (2 * distance - 1) * (2 * distance - 1);
  }

private:
  UnsignedInteger getSizeOfLevel(const UnsignedInteger distance) const
  {
    if (distance == 0) return 1;
    if (isSymmetric())
      return 4 * distance;
    else
      return 8 * distance;
  }

  NumericalScalar stepX_;
  NumericalScalar stepY_;
};

class SphereUniformNorm3D
  : public SphereUniformNormImplementation
{

public:

  SphereUniformNorm3D(const Point & step, const Bool symmetric)
    : SphereUniformNormImplementation(step, symmetric)
  {
    // nothing to do
    if (step.getDimension() != 3) throw InvalidDimensionException(HERE) << "Expected a point of dimension 3";
    stepX_ = step[0];
    stepY_ = step[1];
    stepZ_ = step[2];
  }

  Sample getPoints(const UnsignedInteger distance) const
  {
    // Basic method that uses Sample
    // TODO use of iterators ?
    if (distance == 0) return Sample(1, 3);

    const UnsignedInteger size = getSizeOfLevel(distance);
    Sample points(size, 3);
    UnsignedInteger iterator = 0;
    const NumericalScalar cx = distance * stepX_;
    const NumericalScalar cy = distance * stepY_;
    const NumericalScalar cz = distance * stepZ_;
    if (isSymmetric())
    {
      Point element(3, 0.0);
      // 1) contour (like 2D case) for each z
      for (UnsignedInteger iz = 0; iz <= 2 * distance; ++iz)
      {
        // b) front
        element[0] = cx;
        element[2] = iz * stepZ_ - cz;
        for (UnsignedInteger iy = 0; iy < 2 * distance; ++iy, ++iterator)
        {
          element[1] = iy * stepY_ - cy;
          points[iterator] = element;
        }
        // c) right
        element[1] = cy;
        for (UnsignedInteger ix = 2 * distance; ix > 0; --ix, ++iterator)
        {
          element[0] = ix * stepX_ - cx;
          points[iterator] = element;
        }
      }
      // 2) top
      element[2] = distance * stepZ_;
      for (UnsignedInteger ix = 1; ix < 2 * distance; ++ix)
      {
        element[0] = ix * stepX_ - cx;
        for (UnsignedInteger iy = 1; iy < 2 * distance; ++iy, ++iterator)
        {
          element[1] = iy * stepY_ - cy;
          points[iterator] = element;
        }
      }
    }
    else
    {
      Point element(3, 0.0);
      // 1) contour (like 2D case) for each z
      for (UnsignedInteger iz = 0; iz <= 2 * distance; ++iz)
      {
        // a) left
        element[1] = - cy;
        element[2] = iz * stepZ_ - cz;
        for (UnsignedInteger ix = 0; ix < 2 * distance; ++ix, ++iterator)
        {
          element[0] = ix * stepX_ - cx;
          points[iterator] = element;
        }
        // b) front
        element[0] = cx;
        for (UnsignedInteger iy = 0; iy < 2 * distance; ++iy, ++iterator)
        {
          element[1] = iy * stepY_ - cy;
          points[iterator] = element;
        }
        // c) right
        element[1] = cy;
        for (UnsignedInteger ix = 2 * distance; ix > 0; --ix, ++iterator)
        {
          element[0] = ix * stepX_ - cx;
          points[iterator] = element;
        }
        // d) rear
        element[0] = - cx;
        element[2] = iz * stepZ_ - cz;
        for (UnsignedInteger iy = 2 * distance; iy > 0; --iy, ++iterator)
        {
          element[1] = iy * stepY_ - cy;
          points[iterator] = element;
        }
      }
      // 2) squares for |z| = distance
      for (UnsignedInteger iz = 0; iz <= 2 * distance; iz += 2 * distance)
      {
        element[2] = iz * stepZ_ - cz;
        for (UnsignedInteger ix = 1; ix < 2 * distance; ++ix)
        {
          element[0] = ix * stepX_ - cx;
          for (UnsignedInteger iy = 1; iy < 2 * distance; ++iy, ++iterator)
          {
            element[1] = iy * stepY_ - cy;
            points[iterator] = element;
          }
        }
      }
    } // else not symmetric
    if (iterator != size) throw InternalException(HERE) << "Error in SphereUniformNorm3D::getPoints";
    return points;
  }

  UnsignedInteger getOffsetLevel(const UnsignedInteger distance) const
  {
    if (distance == 0) return 0;
    if (isSymmetric())
      return distance * (1 + (2 * distance - 1) * (2 * distance - 2));
    else
      return (2 * distance - 1) * (2 * distance - 1) * (2 * distance - 1);
  }

private:
  UnsignedInteger getSizeOfLevel(const UnsignedInteger distance) const
  {
    if (distance == 0) return 1;
    if (isSymmetric())
      return 12 * distance * distance + 1;
    else
      return 24 * distance * distance + 2;
  }

  NumericalScalar stepX_;
  NumericalScalar stepY_;
  NumericalScalar stepZ_;

};

CLASSNAMEINIT(SphereUniformNormImplementation);

/** Default constructor */
SphereUniformNormImplementation::SphereUniformNormImplementation(const Point & step, const Bool symmetric)
  : PersistentObject()
  , step_(step)
  , symmetric_(symmetric)
{
  // Nothing to do
}

/** Virtual constructor */
SphereUniformNormImplementation * SphereUniformNormImplementation::clone() const
{
  return new SphereUniformNormImplementation(*this);
}

/** Factory of SphereUniformNormImplementation from grid steps */
SphereUniformNormImplementation::Implementation SphereUniformNormImplementation::GetFromGridSteps(const Point & step, const Bool symmetric)
{
  switch(step.getDimension())
  {
    case 1:
      return new SphereUniformNorm1D(step, symmetric);
      break;
    case 2:
      return new SphereUniformNorm2D(step, symmetric);
      break;
    case 3:
      return new SphereUniformNorm3D(step, symmetric);
      break;
    default:
      throw InvalidDimensionException(HERE) << "Expected a point of dimension 1, 2 or 3";
  }
}

UnsignedInteger SphereUniformNormImplementation::getOffsetLevel(const UnsignedInteger distance) const
{
  throw NotYetImplementedException(HERE) << "In SphereUniformNormImplementation::getOffsetLevel(const UnsignedInteger distance) const";
}

Sample SphereUniformNormImplementation::getPoints(const UnsignedInteger distance) const
{
  throw NotYetImplementedException(HERE) << "In SphereUniformNormImplementation::getPoints(const UnsignedInteger distance) const";
}

/* String converter */
String SphereUniformNormImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " step=" << step_
         << " symmetric=" << symmetric_;
}


String SphereUniformNormImplementation::__str__(const String & offset) const
{
  return __repr__();
}


/* Method save() stores the object through the StorageManager */
void SphereUniformNormImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "step_", step_);
  adv.saveAttribute( "symmetric_", symmetric_);
}


/* Method load() reloads the object from the StorageManager */
void SphereUniformNormImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "step_", step_);
  adv.loadAttribute( "symmetric_", symmetric_);
}

END_NAMESPACE_OPENTURNS

