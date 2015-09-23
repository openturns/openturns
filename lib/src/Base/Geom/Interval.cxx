//                                               -*- C++ -*-
/**
 *  @brief Interval is defined as the cartesian product of n 1D intervalls ]low_1, up_1]x...x]low_n,up_n]
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
#include "Interval.hxx"
#include "PersistentObjectFactory.hxx"
#include "Log.hxx"
#include "Os.hxx"
#include "Exception.hxx"
#include "Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Interval);

static const Factory<Interval> RegisteredFactory;

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
Interval::Interval(const NumericalScalar lowerBound,
                   const NumericalScalar upperBound)
  : DomainImplementation(1)
  , lowerBound_(1, lowerBound)
  , upperBound_(1, upperBound)
  , finiteLowerBound_(1, true)
  , finiteUpperBound_(1, true)
{
  // Nothing to do
}

/* Parameters constructor */
Interval::Interval(const NumericalPoint & lowerBound,
                   const NumericalPoint & upperBound)
  : DomainImplementation(lowerBound.getDimension())
  , lowerBound_(lowerBound)
  , upperBound_(upperBound)
  , finiteLowerBound_(getDimension(), true)
  , finiteUpperBound_(getDimension(), true)
{
  if (getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an interval of dimension 0";
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build an Interval from two NumericalPoint of different dimensions";
}

/* Parameters constructor */
Interval::Interval(const NumericalPoint & lowerBound,
                   const NumericalPoint & upperBound,
                   const BoolCollection & finiteLowerBound,
                   const BoolCollection & finiteUpperBound)
  : DomainImplementation(lowerBound.getDimension())
  , lowerBound_(lowerBound)
  , upperBound_(upperBound)
  , finiteLowerBound_(finiteLowerBound)
  , finiteUpperBound_(finiteUpperBound)
{
  if (getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an interval of dimension 0";
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build an Interval from two NumericalPoint of different dimensions";
  if ((finiteLowerBound.getSize() != getDimension()) || (finiteUpperBound.getSize() != getDimension())) throw InvalidArgumentException(HERE) << "Error: cannot build an interval with lower bound flags or upper bound flags of improper dimension";
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
  const NumericalPoint otherLower(other.getLowerBound());
  const NumericalPoint otherUpper(other.getUpperBound());
  const BoolCollection otherFiniteLower(other.getFiniteLowerBound());
  const BoolCollection otherFiniteUpper(other.getFiniteUpperBound());
  // Built intersection elements
  NumericalPoint intersectLower(getDimension());
  NumericalPoint intersectUpper(getDimension());
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
  const NumericalPoint otherLower(other.getLowerBound());
  const NumericalPoint otherUpper(other.getUpperBound());
  const BoolCollection otherFiniteLower(other.getFiniteLowerBound());
  const BoolCollection otherFiniteUpper(other.getFiniteUpperBound());
  // Built intersection elements
  NumericalPoint intersectLower(getDimension());
  NumericalPoint intersectUpper(getDimension());
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

/* Check if the given point is inside of the closed interval */
Bool Interval::contains(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
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

/* Compute the numerical volume of the interval */
void Interval::computeVolume() const
{
  const UnsignedInteger dimension(getDimension());
  if (dimension == 0)
  {
    volume_ = 0.0;
    return;
  }
  volume_ = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    volume_ *= upperBound_[i] - lowerBound_[i];
    if (volume_ <= 0.0)
    {
      volume_ = 0.0;
      return;
    }
  }
}

/* Check if the given point is numerically inside of the closed interval, i.e. using only the bounds part of the interval */
Bool Interval::numericallyContains(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
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
  const NumericalPoint lowerBound(lowerBound_ + rhs.getLowerBound());
  const NumericalPoint upperBound(upperBound_ + rhs.getUpperBound());
  Interval::BoolCollection finiteLowerBound(rhs.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(rhs.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound[i] = finiteLowerBound[i] && finiteLowerBound_[i];
    finiteUpperBound[i] = finiteUpperBound[i] && finiteUpperBound_[i];
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
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


/* Substraction operator */
Interval Interval::operator -(const Interval & rhs) const
{
  if (getDimension() != rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be added (LHS dimension = "
        << getDimension()
        << "; RHS dimension = "
        << rhs.getDimension();
  const NumericalPoint lowerBound(lowerBound_ - rhs.getUpperBound());
  const NumericalPoint upperBound(upperBound_ - rhs.getLowerBound());
  Interval::BoolCollection finiteLowerBound(rhs.getFiniteLowerBound());
  Interval::BoolCollection finiteUpperBound(rhs.getFiniteUpperBound());
  for(UnsignedInteger i = 0; i < (getDimension()); ++i)
  {
    finiteLowerBound[i] = finiteLowerBound[i] && finiteUpperBound_[i];
    finiteUpperBound[i] = finiteUpperBound[i] && finiteLowerBound_[i];
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}



/* In-place substraction operator */
Interval & Interval::operator -=(const Interval & other)
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Intervals of different dimensions cannot be substracted (LHS dimension = "
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
Interval Interval::operator *(const NumericalScalar scalar) const
{
  // Special case for multiplication by 0. We assume that 0 x (+/-inf) = 0.
  if (scalar == 0.0) return Interval(NumericalPoint(getDimension(), 0.0), NumericalPoint(getDimension(), 0.0));
  NumericalPoint lowerBound(lowerBound_);
  NumericalPoint upperBound(upperBound_);
  Interval::BoolCollection finiteLowerBound(finiteLowerBound_);
  Interval::BoolCollection finiteUpperBound(finiteUpperBound_);
  if (scalar > 0.0) return Interval(scalar * lowerBound, scalar * upperBound, finiteLowerBound, finiteUpperBound);
  return Interval(scalar * upperBound, scalar * lowerBound, finiteUpperBound, finiteLowerBound);
}

/*  In-place product operator */
Interval & Interval::operator *=(const NumericalScalar scalar)
{
  // Special case for multiplication by 0. We assume that 0 x (+/-inf) = 0.
  if (scalar == 0.0)
  {
    lowerBound_ = NumericalPoint(getDimension());
    upperBound_ = NumericalPoint(getDimension());
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
  const NumericalPoint tmpBound(lowerBound_);
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

/* Lower bound accessor */
NumericalPoint Interval::getLowerBound() const
{
  return lowerBound_;
}

void Interval::setLowerBound(const NumericalPoint & lowerBound)
{
  if (lowerBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given lower bound has a dimension incompatible with the interval dimension.";
  lowerBound_ = lowerBound;
}

/* Upper bound accessor */
NumericalPoint Interval::getUpperBound() const
{
  return upperBound_;
}

void Interval::setUpperBound(const NumericalPoint & upperBound)
{
  if (upperBound.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given upper bound has a dimension incompatible with the interval dimension.";
  upperBound_ = upperBound;
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
    if (i > 0) oss << Os::GetEndOfLine();
    if (finiteLowerBound_[i]) oss << offset << "[" << lowerBound_[i] << ", ";
    else oss << offset << "]-inf (" << lowerBound_[i] << "), ";
    if (finiteUpperBound_[i]) oss << upperBound_[i] << "]";
    else oss << offset << "(" << upperBound_[i] << ") +inf[";
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
