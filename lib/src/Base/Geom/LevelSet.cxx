//                                               -*- C++ -*-
/**
 *  @brief LevelSet is defined as the set of points such that f(x_1,...,x_n) <= level
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
#include "openturns/LevelSet.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/AggregatedFunction.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LevelSet)

static const Factory<LevelSet> Factory_LevelSet;

/* Default constructor */
LevelSet::LevelSet(const UnsignedInteger dimension)
  : DomainImplementation(dimension)
  , function_(SymbolicFunction(Description::BuildDefault(dimension, "x"), Description(1, "1.0")))
  , operator_(LessOrEqual())
  , level_(0.0)
  , lowerBound_(0)
  , upperBound_(0)
{
  // Nothing to do
}

/* Parameters constructor, simplified interface for 1D case */
LevelSet::LevelSet(const Function & function,
                   const ComparisonOperator & op,
                   const Scalar level)
  : DomainImplementation(function.getInputDimension())
  , function_(function)
  , operator_(op)
  , level_(level)
  , lowerBound_(0)
  , upperBound_(0)
{
  if (function.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot build a level set based on functions with output dimension different from 1. Here, output dimension=" << function.getOutputDimension();
}

/* Clone method */
LevelSet * LevelSet::clone() const
{
  return new LevelSet(*this);
}

/* Returns the levelSet equals to the intersection between the levelSet and another one */
LevelSet LevelSet::intersect(const LevelSet & other) const
{
  // If one intersect the levelSet with itself
  if (this == &other) return (*this);
  // else check dimension compatibility
  if (other.dimension_ != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot intersect level sets of different dimensions";
  // The intersectFunction is negative or zero if the given point is inside of the resulting level set, ie if both functions are less or equal to their respective level
  const String sign1 = operator_(1.0, 2.0) ? "" : "-";
  const String sign2 = other.operator_(1.0, 2.0) ? "" : "-";
  const SymbolicFunction intersectFunction(Description::BuildDefault(2, "x"), Description(1, (OSS() << "max(" << sign1 << "(x0 - " << level_ << "), " << sign2 << "(x1 - " << other.level_ << "))")));
  Function::FunctionCollection coll(2);
  coll[0] = function_;
  coll[1] = other.function_;
  LevelSet result(ComposedFunction(intersectFunction, AggregatedFunction(coll)), LessOrEqual(), 0.0);
  // Check if we can compute a bounding box
  if ((lowerBound_.getDimension() == dimension_) &&
      (upperBound_.getDimension() == dimension_) &&
      (other.lowerBound_.getDimension() == dimension_) &&
      (other.upperBound_.getDimension() == dimension_))
  {
    const Interval boundingBox(Interval(lowerBound_, upperBound_).intersect(Interval(other.lowerBound_, other.upperBound_)));
    result.setLowerBound(boundingBox.getLowerBound());
    result.setUpperBound(boundingBox.getUpperBound());
  }
  return result;
}

/* Returns the levelSet equals to the union between the levelSet and another one */
LevelSet LevelSet::join(const LevelSet & other) const
{
  // If one intersect the levelSet with itself
  if (this == &other) return (*this);
  // else check dimension compatibility
  if (other.dimension_ != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot intersect level sets of different dimensions";
  // The intersectFunction is negative or zero iff the given point is inside of the resulting level set, ie if at least on function is less or equal to its level
  const String sign1 = operator_(1.0, 2.0) ? "" : "-";
  const String sign2 = other.operator_(1.0, 2.0) ? "" : "-";
  const SymbolicFunction intersectFunction(Description::BuildDefault(2, "x"), Description(1, (OSS() << "min(" << sign1 << "(x0 - " << level_ << "), " << sign2 << "(x1 - " << other.level_ << "))")));
  Function::FunctionCollection coll(2);
  coll[0] = function_;
  coll[1] = other.function_;
  LevelSet result(ComposedFunction(intersectFunction, AggregatedFunction(coll)), LessOrEqual(), 0.0);
  // Check if we can compute a bounding box
  if ((lowerBound_.getDimension() == dimension_) &&
      (upperBound_.getDimension() == dimension_) &&
      (other.lowerBound_.getDimension() == dimension_) &&
      (other.upperBound_.getDimension() == dimension_))
  {
    const Interval boundingBox(Interval(lowerBound_, upperBound_).join(Interval(other.lowerBound_, other.upperBound_)));
    result.setLowerBound(boundingBox.getLowerBound());
    result.setUpperBound(boundingBox.getUpperBound());
  }
  return result;
}

/* Check if the given point is inside of the closed levelSet */
Bool LevelSet::contains(const Point & point) const
{
  if (point.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << point.getDimension();
  // If a bounding box has been computed/provided
  if ((lowerBound_.getDimension() == dimension_) && (upperBound_.getDimension() == dimension_) && !Interval(lowerBound_, upperBound_).contains(point)) return false;
  return operator_(function_(point)[0], level_);
}

/* Check if the given points are inside of the closed levelSet */
LevelSet::BoolCollection LevelSet::contains(const Sample & sample) const
{
  if (sample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << dimension_ << ", got dimension=" << sample.getDimension();
  const UnsignedInteger size(sample.getSize());
  BoolCollection result(size);
  // If a bounding box has been computed/provided, only check points inside this bounding box
  if ((lowerBound_.getDimension() == dimension_) && (upperBound_.getDimension() == dimension_))
  {
    Indices insideIndices(0);
    const DomainImplementation::BoolCollection isInsideBoundingBox(Interval(lowerBound_, upperBound_).contains(sample));
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      if (isInsideBoundingBox[i])
      {
        insideIndices.add(i);
      }
    }
    const Sample insidePoints(sample.select(insideIndices));
    const Sample values(function_(insidePoints));
    for(UnsignedInteger i = 0; i < insideIndices.getSize(); ++i)
      result[insideIndices[i]] = operator_(values(i, 0), level_);
  }
  else
  {
    const Sample values(function_(sample));
    for(UnsignedInteger i = 0; i < size; ++i)
      result[i] = operator_(values(i, 0), level_);
  }
  return result;
}

/* Comparison operator */
Bool LevelSet::operator == (const LevelSet & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_) && (operator_ == other.operator_) && (level_ == other.level_);
}

/* Functio accessor */
Function LevelSet::getFunction() const
{
  return function_;
}

void LevelSet::setFunction(const Function & function)
{
  if (function.getInputDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given function has an input dimension=" << function.getInputDimension() << " incompatible with the levelSet dimension=" << dimension_;
  function_ = function;
}

/* Operator accessor */
ComparisonOperator LevelSet::getOperator() const
{
  return operator_;
}

void LevelSet::setOperator(const ComparisonOperator & op)
{
  operator_ = op;
}

/* Level accessor */
Scalar LevelSet::getLevel() const
{
  return level_;
}

void LevelSet::setLevel(const Scalar level)
{
  level_ = level;
}

/* Lower bound of the bounding box */
void LevelSet::setLowerBound(const Point & bound)
{
  if (bound.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a lower bound of dimension=" << dimension_ << ", got dimension=" << bound.getDimension();
  lowerBound_ = bound;
}

Point LevelSet::getLowerBound() const
{
  return lowerBound_;
}

/* Upper bound of the bounding box */
void LevelSet::setUpperBound(const Point & bound)
{
  if (bound.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected an upper bound of dimension=" << dimension_ << ", got dimension=" << bound.getDimension();
  upperBound_ = bound;
}

Point LevelSet::getUpperBound() const
{
  return upperBound_;
}

/* String converter */
String LevelSet::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << dimension_
         << " function=" << function_.__repr__()
//          << " operator=" << operator_.__repr__()
         << " level=" << level_;
}

String LevelSet::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "{x | f(x) " << operator_.__str__() << " " << level_ << "} with f=" << Os::GetEndOfLine() << offset << function_.__str__(offset);
  return oss;
}

void LevelSet::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("function_", function_);
  adv.saveAttribute("operator_", operator_);
  adv.saveAttribute("level_", level_);
}

/* Method load() reloads the object from the StorageManager */
void LevelSet::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("function_", function_);
  adv.loadAttribute("operator_", operator_);
  adv.loadAttribute("level_", level_);
}

END_NAMESPACE_OPENTURNS
