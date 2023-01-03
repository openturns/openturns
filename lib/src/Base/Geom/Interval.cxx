//                                               -*- C++ -*-
/**
 *  @brief Interval is defined as the cartesian product of n 1D intervals ]low_1, up_1]x...x]low_n,up_n]
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Interval.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Interval)

static const Factory<Interval> Factory_Interval;

/* Default constructor */
Interval::Interval(const UnsignedInteger dimension)
  : DomainImplementation(dimension)
  , lowerBound_(dimension, 0.0)
  , upperBound_(dimension, 1.0)
  , finiteLowerBound_(dimension, true)
  , finiteUpperBound_(dimension, true)
{
  // Nothing to do
}

/* Parameters constructor, simplified interface for 1D case */
Interval::Interval(const Scalar lowerBound,
                   const Scalar upperBound)
  : DomainImplementation(1)
  , lowerBound_(1, lowerBound)
  , upperBound_(1, upperBound)
  , finiteLowerBound_(1, true)
  , finiteUpperBound_(1, true)
{
  // Check if the bounds are finite
  setLowerBound(Point(1, lowerBound));
  setUpperBound(Point(1, upperBound));
}

/* Parameters constructor */
Interval::Interval(const Point & lowerBound,
                   const Point & upperBound)
  : DomainImplementation(lowerBound.getDimension())
  , lowerBound_(lowerBound)
  , upperBound_(upperBound)
  , finiteLowerBound_(getDimension(), true)
  , finiteUpperBound_(getDimension(), true)
{
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build an Interval from two Point of different dimensions";
  // Check if the bounds are finite
  setLowerBound(lowerBound);
  setUpperBound(upperBound);
}

/* Parameters constructor */
Interval::Interval(const Point & lowerBound,
                   const Point & upperBound,
                   const BoolCollection & finiteLowerBound,
                   const BoolCollection & finiteUpperBound)
  : DomainImplementation(lowerBound.getDimension())
  , lowerBound_(lowerBound)
  , upperBound_(upperBound)
  , finiteLowerBound_(finiteLowerBound)
  , finiteUpperBound_(finiteUpperBound)
{
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build an Interval from two Point of different dimensions";
  if ((finiteLowerBound.getSize() != getDimension()) || (finiteUpperBound.getSize() != getDimension())) throw InvalidArgumentException(HERE) << "Error: cannot build an interval with lower bound flags or upper bound flags of improper dimension";
  // Check if the bounds are finite
  setLowerBound(lowerBound);
  setUpperBound(upperBound);
}

/* Clone method */
Interval * Interval::clone() const
{
  return new Interval(*this);
}

/* Returns the interval equals to the intersection between the interval and another one */
Interval Interval::intersect(const Interval & other) const
{
  // If one intersect the interval with itself
  if (this == &other) return (*this);
  // else check dimension compatibility
  if (other.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot intersect intervals of different dimensions";
  // Extract other elements
  const Point otherLower(other.getLowerBound());
  const Point otherUpper(other.getUpperBound());
  const BoolCollection otherFiniteLower(other.getFiniteLowerBound());
  const BoolCollection otherFiniteUpper(other.getFiniteUpperBound());
  // Built intersection elements
  Point intersectLower(getDimension());
  Point intersectUpper(getDimension());
  BoolCollection intersectFiniteLower(getDimension());
  BoolCollection intersectFiniteUpper(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    intersectLower[i] = std::max(lowerBound_[i], otherLower[i]);
    intersectUpper[i] = std::min(upperBound_[i], otherUpper[i]);
    intersectFiniteLower[i] = finiteLowerBound_[i] || otherFiniteLower[i];
    intersectFiniteUpper[i] = finiteUpperBound_[i] || otherFiniteUpper[i];
  }
  return Interval(intersectLower, intersectUpper, intersectFiniteLower, intersectFiniteUpper);
}

/* Returns the interval equals to the union between the interval and another one */
Interval Interval::join(const Interval & other) const
{
  // If one intersect the interval with itself
  if (this == &other) return (*this);
  // else check dimension compatibility
  if (other.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot intersect intervals of different dimensions";
  // Extract other elements
  const Point otherLower(other.getLowerBound());
  const Point otherUpper(other.getUpperBound());
  const BoolCollection otherFiniteLower(other.getFiniteLowerBound());
  const BoolCollection otherFiniteUpper(other.getFiniteUpperBound());
  // Built intersection elements
  Point intersectLower(getDimension());
  Point intersectUpper(getDimension());
  BoolCollection intersectFiniteLower(getDimension());
  BoolCollection intersectFiniteUpper(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    intersectLower[i] = std::min(lowerBound_[i], otherLower[i]);
    intersectUpper[i] = std::max(upperBound_[i], otherUpper[i]);
    intersectFiniteLower[i] = finiteLowerBound_[i] && otherFiniteLower[i];
    intersectFiniteUpper[i] = finiteUpperBound_[i] && otherFiniteUpper[i];
  }
  return Interval(intersectLower, intersectUpper, intersectFiniteLower, intersectFiniteUpper);
}

/* Check if the interval is empty, i.e. if we have lowerBound >= upperBound for at least one component with finite bounds. */
Bool Interval::isEmpty() const
{
  for (UnsignedInteger i = 0; i < getDimension(); ++i) if (finiteLowerBound_[i] && finiteUpperBound_[i] && (lowerBound_[i] > upperBound_[i])) return true;
  return false;
}

/* Check if the interval is numerically empty, i.e. its volume is zero */
Bool Interval::isNumericallyEmpty() const
{
  return getVolume() <= ResourceMap::GetAsScalar("Domain-SmallVolume");
}

/* Check if the given point is inside of the closed interval */
Bool Interval::contains(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // Check against the lower bound
    if (finiteLowerBound_[i] && (point[i] < lowerBound_[i])) return false;
    // Check against the upper bound
    if (finiteUpperBound_[i] && (point[i] > upperBound_[i])) return false;
  }
  return true;
}

/* Compute the Euclidean distance from a given point to the domain */
Scalar Interval::computeDistance(const Point & point) const
{
  const UnsignedInteger pointDimension = point.getDimension();
  const UnsignedInteger intervalDimension = getDimension();

  if (pointDimension != intervalDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << pointDimension << ", got dimension=" << intervalDimension;

  if (isEmpty()) return SpecFunc::MaxScalar;

  Point lowerBound = getLowerBound();
  Point upperBound = getUpperBound();
  BoolCollection finiteLowerBound = getFiniteLowerBound();
  BoolCollection finiteUpperBound = getFiniteUpperBound();

  Scalar squaredDistance = 0.0;

  for (UnsignedInteger i = 0; i < intervalDimension; ++i)
  {
    if (finiteLowerBound[i] && point[i] < lowerBound[i]) squaredDistance += (lowerBound[i] - point[i]) * (lowerBound[i] - point[i]);
    else if (finiteUpperBound[i] && point[i] > upperBound[i]) squaredDistance += (point[i] - upperBound[i]) * (point[i] - upperBound[i]);
  }

  return sqrt(squaredDistance);
}

/* Compute the numerical volume of the interval */
Scalar Interval::getVolume() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 0) return 0.0;
  Scalar volume = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    volume *= upperBound_[i] - lowerBound_[i];
    if (volume <= 0.0) return 0.0;
  }
  return volume;
}

/* Check if the given point is numerically inside of the closed interval, i.e. using only the bounds part of the interval */
Bool Interval::numericallyContains(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension << ", got dimension=" << point.getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] < lowerBound_[i]) || (point[i] > upperBound_[i])) return false;
  return true;
}

/* Addition operator */
Interval Interval::operator +(const Interval & rhs) const
{
  if (getDimension() != rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << rhs.getDimension();
  const Point lowerBound(lowerBound_ + rhs.getLowerBound());
  const Point upperBound(upperBound_ + rhs.getUpperBound());
  Interval::BoolCollection finiteLowerBound(rhs.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(rhs.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound[i] = finiteLowerBound[i] && finiteLowerBound_[i];
    finiteUpperBound[i] = finiteUpperBound[i] && finiteUpperBound_[i];
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}


Interval Interval::getMarginal(const Indices & indices) const
{
  if (!indices.check(getDimension())) throw InvalidArgumentException(HERE) << "Marginal indices cannot exceed dimension";
  const UnsignedInteger size = indices.getSize();
  Point lowerBound(size);
  Point upperBound(size);
  Interval::BoolCollection finiteLowerBound(size);
  Interval::BoolCollection finiteUpperBound(size);
  for(UnsignedInteger i = 0; i < size; ++ i)
  {
    lowerBound[i] = lowerBound_[indices[i]];
    upperBound[i] = upperBound_[indices[i]];
    finiteLowerBound[i] = finiteLowerBound_[indices[i]];
    finiteUpperBound[i] = finiteUpperBound_[indices[i]];
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}


Interval Interval::getMarginal(const UnsignedInteger index) const
{
  if (!(index < getDimension())) throw InvalidArgumentException(HERE) << "Marginal index cannot exceed dimension, here index=" << index << " and dimension=" << getDimension();
  return Interval(Point(1, lowerBound_[index]),
                  Point(1, upperBound_[index]),
                  Interval::BoolCollection(1, finiteLowerBound_[index]),
                  Interval::BoolCollection(1, finiteUpperBound_[index]));
}


/* In-place addition operator */
Interval & Interval::operator +=(const Interval & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << other.getDimension();

  lowerBound_ += other.getLowerBound();
  upperBound_ += other.getUpperBound();
  Interval::BoolCollection finiteLowerBound(other.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(other.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound_[i] = finiteLowerBound[i] && finiteLowerBound_[i];
    finiteUpperBound_[i] = finiteUpperBound[i] && finiteUpperBound_[i];
  }
  return *this;
}


/* Subtraction operator */
Interval Interval::operator -(const Interval & rhs) const
{
  if (getDimension() != rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << rhs.getDimension();
  const Point lowerBound(lowerBound_ - rhs.getUpperBound());
  const Point upperBound(upperBound_ - rhs.getLowerBound());
  Interval::BoolCollection finiteLowerBound(rhs.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(rhs.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound[i] = finiteLowerBound[i] && finiteUpperBound_[i];
    finiteUpperBound[i] = finiteUpperBound[i] && finiteLowerBound_[i];
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}



/* In-place subtraction operator */
Interval & Interval::operator -=(const Interval & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be subtracted (LHS dimension = "
        << getDimension()
        << "; RHS dimension = " <<
        other.getDimension();

  lowerBound_ -= other.getUpperBound();
  upperBound_ -= other.getLowerBound();
  Interval::BoolCollection finiteLowerBound(other.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(other.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound_[i] = finiteLowerBound[i] && finiteUpperBound_[i];
    finiteUpperBound_[i] = finiteUpperBound[i] && finiteLowerBound_[i];
  }
  return *this;
}


/* Product operator */
Interval Interval::operator *(const Scalar scalar) const
{
  // Special case for multiplication by 0. We assume that 0 x (+/-inf) = 0.
  if (scalar == 0.0) return Interval(Point(getDimension(), 0.0), Point(getDimension(), 0.0));
  Point lowerBound(lowerBound_);
  Point upperBound(upperBound_);
  Interval::BoolCollection finiteLowerBound(finiteLowerBound_);
  Interval::BoolCollection finiteUpperBound(finiteUpperBound_);
  if (scalar > 0.0) return Interval(scalar * lowerBound, scalar * upperBound, finiteLowerBound, finiteUpperBound);
  return Interval(scalar * upperBound, scalar * lowerBound, finiteUpperBound, finiteLowerBound);
}

/*  In-place product operator */
Interval & Interval::operator *=(const Scalar scalar)
{
  // Special case for multiplication by 0. We assume that 0 x (+/-inf) = 0.
  if (scalar == 0.0)
  {
    lowerBound_ = Point(getDimension());
    upperBound_ = Point(getDimension());
    finiteLowerBound_ = BoolCollection(getDimension(), true);
    finiteUpperBound_ = BoolCollection(getDimension(), true);
    return *this;
  }
  if (scalar > 0.0)
  {
    lowerBound_ *= scalar;
    upperBound_ *= scalar;
    return *this;
  }
  const Point tmpBound(lowerBound_);
  lowerBound_ = scalar * upperBound_;
  upperBound_ = scalar * tmpBound;
  const BoolCollection tmpFiniteBound(finiteLowerBound_);
  finiteLowerBound_ = finiteUpperBound_;
  finiteUpperBound_ = tmpFiniteBound;
  return *this;
}


/* Comparison operator */
Bool Interval::operator == (const Interval & other) const
{
  if (this == &other) return true;
  return (lowerBound_ == other.lowerBound_)
         && (upperBound_ == other.upperBound_)
         && (finiteLowerBound_ == other.finiteLowerBound_)
         && (finiteUpperBound_ == other.finiteUpperBound_);
}

Bool Interval::operator != (const Interval & other) const
{
  return !operator==(other);
}

/* Lower bound accessor */
Point Interval::getLowerBound() const
{
  return lowerBound_;
}

void Interval::setLowerBound(const Point & lowerBound)
{
  if (lowerBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given lower bound has a dimension incompatible with the interval dimension.";
  lowerBound_ = lowerBound;
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    if (SpecFunc::IsInf(lowerBound[i]))
    {
      lowerBound_[i] = (lowerBound[i] > 0.0 ? SpecFunc::MaxScalar : SpecFunc::LowestScalar);
      finiteLowerBound_[i] = false;
    }
}

/* Upper bound accessor */
Point Interval::getUpperBound() const
{
  return upperBound_;
}

void Interval::setUpperBound(const Point & upperBound)
{
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given upper bound has a dimension incompatible with the interval dimension.";
  upperBound_ = upperBound;
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    if (SpecFunc::IsInf(upperBound[i]))
    {
      upperBound_[i] = (upperBound[i] > 0.0 ? SpecFunc::MaxScalar : SpecFunc::LowestScalar);
      finiteUpperBound_[i] = false;
    }
}

/* Lower bound flag accessor */
Interval::BoolCollection Interval::getFiniteLowerBound() const
{
  return finiteLowerBound_;
}

void Interval::setFiniteLowerBound(const BoolCollection & finiteLowerBound)
{
  if (finiteLowerBound.getSize() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given lower bound flag has a dimension incompatible with the interval dimension.";
  finiteLowerBound_ = finiteLowerBound;
}

/* Upper bound flag accessor */
Interval::BoolCollection Interval::getFiniteUpperBound() const
{
  return finiteUpperBound_;
}

void Interval::setFiniteUpperBound(const BoolCollection & finiteUpperBound)
{
  if (finiteUpperBound.getSize() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given upper bound flag has a dimension incompatible with the interval dimension.";
  finiteUpperBound_ = finiteUpperBound;
}

/* String converter */
String Interval::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " lower bound=" << lowerBound_.__repr__()
         << " upper bound=" << upperBound_.__repr__()
         << " finite lower bound=" << finiteLowerBound_.__repr__()
         << " finite upper bound=" << finiteUpperBound_.__repr__();
}

String Interval::__str__(const String & offset) const
{
  if (getDimension() == 0) return "empty";
  OSS oss(false);
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    if (i > 0) oss << Os::GetEndOfLine() << offset;
    if (finiteLowerBound_[i]) oss << "[" << lowerBound_[i] << ", ";
    else oss << "]-inf (" << lowerBound_[i] << "), ";
    if (finiteUpperBound_[i]) oss << upperBound_[i] << "]";
    else oss << "(" << upperBound_[i] << ") +inf[";
  }
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Interval::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("lowerBound_", lowerBound_);
  adv.saveAttribute("upperBound_", upperBound_);
  adv.saveAttribute("finiteLowerBound_", finiteLowerBound_);
  adv.saveAttribute("finiteUpperBound_", finiteUpperBound_);
}

/* Method load() reloads the object from the StorageManager */
void Interval::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("lowerBound_", lowerBound_);
  adv.loadAttribute("upperBound_", upperBound_);
  adv.loadAttribute("finiteLowerBound_", finiteLowerBound_);
  adv.loadAttribute("finiteUpperBound_", finiteUpperBound_);
}

END_NAMESPACE_OPENTURNS
