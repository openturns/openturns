//                                               -*- C++ -*-
/**
 *  @brief LevelSet is defined as the set of points such that f(x_1,...,x_n) <= level
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
#include "openturns/LevelSet.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AnalyticalFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LevelSet);

static const Factory<LevelSet> Factory_LevelSet;

/* Default constructor */
LevelSet::LevelSet(const UnsignedInteger dimension)
  : DomainImplementation(dimension)
  , function_(AnalyticalFunction(Description::BuildDefault(dimension, "x"), Description(1, "1.0")))
  , level_(0.0)
  , lowerBound_(0)
  , upperBound_(0)
{
  // Nothing to do
}

/* Parameters constructor, simplified interface for 1D case */
LevelSet::LevelSet(const NumericalMathFunction & function,
                   const NumericalScalar level)
  : DomainImplementation(function.getInputDimension())
  , function_(function)
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
  // The intersectFunction is negative or zero iff the given point is inside of the resulting level set, ie if both functions are less or equal to their respective level
  const AnalyticalFunction intersectFunction(Description::BuildDefault(2, "x"), Description(1, (OSS() << "max(x0 - " << level_ << ", x1 - " << other.level_ << ")")));
  NumericalMathFunction::NumericalMathFunctionCollection coll(2);
  coll[0] = function_;
  coll[1] = other.function_;
  LevelSet result(ComposedFunction(intersectFunction, NumericalMathFunction(coll)), 0.0);
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
  const AnalyticalFunction intersectFunction(Description::BuildDefault(2, "x"), Description(1, (OSS() << "min(x0 - " << level_ << ", x1 - " << other.level_ << ")")));
  NumericalMathFunction::NumericalMathFunctionCollection coll(2);
  coll[0] = function_;
  coll[1] = other.function_;
  LevelSet result(ComposedFunction(intersectFunction, NumericalMathFunction(coll)), 0.0);
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
Bool LevelSet::contains(const NumericalPoint & point) const
{
  if (point.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << point.getDimension();
  // If a bounding box has been computed/provided
  if ((lowerBound_.getDimension() == dimension_) && (upperBound_.getDimension() == dimension_) && !Interval(lowerBound_, upperBound_).contains(point)) return false;
  return function_(point)[0] <= level_;
}

/* Comparison operator */
Bool LevelSet::operator == (const LevelSet & other) const
{
  if (this == &other) return true;
  return (function_ == other.function_) && (level_ == other.level_);
}

/* Functio accessor */
NumericalMathFunction LevelSet::getFunction() const
{
  return function_;
}

void LevelSet::setFunction(const NumericalMathFunction & function)
{
  if (function.getInputDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given function has an input dimension=" << function.getInputDimension() << " incompatible with the levelSet dimension=" << dimension_;
  function_ = function;
}

/* Level accessor */
NumericalScalar LevelSet::getLevel() const
{
  return level_;
}

void LevelSet::setLevel(const NumericalScalar level)
{
  level_ = level;
}

/* Lower bound of the bounding box */
void LevelSet::setLowerBound(const NumericalPoint & bound)
{
  if (bound.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a lower bound of dimension=" << dimension_ << ", got dimension=" << bound.getDimension();
  lowerBound_ = bound;
}

NumericalPoint LevelSet::getLowerBound() const
{
  if (lowerBound_.getDimension() != dimension_) computeLowerBound();
  return lowerBound_;
}

void LevelSet::computeLowerBound() const
{
  lowerBound_ = NumericalPoint(dimension_);
  LinearFunction translate(NumericalPoint(1, level_), NumericalPoint(1), IdentityMatrix(1));
  ComposedFunction equality(translate, function_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Matrix m(1, dimension_);
    m(0, i) = 1.0;
    LinearFunction coordinate(NumericalPoint(dimension_), NumericalPoint(1), m);
    OptimizationProblem problem(coordinate, equality, NumericalMathFunction(), Interval());
    problem.setMinimization(true);
    Cobyla solver(problem);
    solver.setStartingPoint(NumericalPoint(dimension_));
    solver.run();
    lowerBound_[i] = solver.getResult().getOptimalPoint()[i];
  }
}

/* Upper bound of the bounding box */
void LevelSet::setUpperBound(const NumericalPoint & bound)
{
  if (bound.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected an upper bound of dimension=" << dimension_ << ", got dimension=" << bound.getDimension();
  upperBound_ = bound;
}

NumericalPoint LevelSet::getUpperBound() const
{
  if (upperBound_.getDimension() != dimension_) computeUpperBound();
  return upperBound_;
}

void LevelSet::computeUpperBound() const
{
  upperBound_ = NumericalPoint(dimension_);
  LinearFunction translate(NumericalPoint(1, level_), NumericalPoint(1), IdentityMatrix(1));
  ComposedFunction equality(translate, function_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Matrix m(1, dimension_);
    m(0, i) = 1.0;
    LinearFunction coordinate(NumericalPoint(dimension_), NumericalPoint(1), m);
    OptimizationProblem problem(coordinate, equality, NumericalMathFunction(), Interval());
    problem.setMinimization(false);
    Cobyla solver(problem);
    solver.setStartingPoint(NumericalPoint(dimension_));
    solver.run();
    upperBound_[i] = solver.getResult().getOptimalPoint()[i];
  }
}

/* String converter */
String LevelSet::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << dimension_
         << " function=" << function_.__repr__()
         << " level=" << level_;
}

String LevelSet::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "{x | f(x) <= " << level_ << "} with f=\n" << function_.__str__(offset);
  return oss;
}

void LevelSet::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("function_", function_);
  adv.saveAttribute("level_", level_);
}

/* Method load() reloads the object from the StorageManager */
void LevelSet::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("function_", function_);
  adv.loadAttribute("level_", level_);
}

END_NAMESPACE_OPENTURNS
