//                                               -*- C++ -*-
/**
 *  @brief LevelSet is defined as the set of points such that f(x_1,...,x_n) <= level
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Less.hxx"
#include "openturns/SpecFunc.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LevelSet)

static const Factory<LevelSet> Factory_LevelSet;

/* Default constructor */
LevelSet::LevelSet(const UnsignedInteger dimension)
  : DomainImplementation(dimension)
  , function_(SymbolicFunction(Description::BuildDefault(dimension, "x"), Description(1, "1.0")))
  , operator_(LessOrEqual())
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
{
  if (function.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot build a level set based on functions with output dimension different from 1. Here, output dimension=" << function.getOutputDimension();
}

/* Clone method */
LevelSet * LevelSet::clone() const
{
  return new LevelSet(*this);
}


class OT_API LevelSetBooleanOperationEvaluation : public EvaluationImplementation
{
public:
  explicit LevelSetBooleanOperationEvaluation(const Function::FunctionCollection & collFunc = Function::FunctionCollection(),
      const Collection<ComparisonOperator> & collOp = Collection<ComparisonOperator>(),
      const Point & level = Point(),
      const Bool intersection = true)
    : EvaluationImplementation()
    , collFunc_(collFunc)
    , collOp_(collOp)
    , level_(level)
    , intersection_(intersection)
  {
    const UnsignedInteger size = collFunc.getSize();
    const UnsignedInteger inputDimension = size ? collFunc[0].getInputDimension() : 0;

    for (UnsignedInteger i = 1; i < size; ++ i)
    {
      if (collFunc[i].getInputDimension() != inputDimension)
        throw InvalidArgumentException(HERE) << "Mismatching input dimension";
      if (collFunc[i].getOutputDimension() != 1)
        throw InvalidArgumentException(HERE) << "Mismatching output dimension";
    }
    if (collOp_.getSize() != size)
      throw InvalidArgumentException(HERE) << "Mismatching op size";
    if (level_.getSize() != size)
      throw InvalidArgumentException(HERE) << "Mismatching level size";
  }

  LevelSetBooleanOperationEvaluation * clone() const override
  {
    return new LevelSetBooleanOperationEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return collFunc_.getSize() ? collFunc_[0].getInputDimension() : 0;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator() (const Point & x) const override
  {
    const UnsignedInteger collSize = collFunc_.getSize();
    Scalar value = intersection_ ? -SpecFunc::MaxScalar : SpecFunc::MaxScalar;
    for (UnsignedInteger j = 0; j < collSize; ++ j)
    {
      const Scalar fx = collFunc_[j](x)[0];
      const Scalar sign = collOp_[j](1.0, 2.0) ? 1.0 : -1.0;
      const Scalar slack = sign * (fx - level_[j]);
      const Bool ok = collOp_[j](fx, level_[j]);
      value = intersection_ ? std::max(value, slack) : std::min(value, slack);
      // return if outside subdomain for intersection or inside subdomain for union
      if (intersection_ != ok)
        break;
    }
    return Point(1, value);
  }

  Sample operator() (const Sample & inS) const override
  {
    const UnsignedInteger size = inS.getSize();
    const UnsignedInteger collSize = collFunc_.getSize();
    Sample outS(size, Point(getOutputDimension(), intersection_ ? -SpecFunc::MaxScalar : SpecFunc::MaxScalar));
    Indices todo(size);
    todo.fill();
    for (UnsignedInteger j = 0; j < collSize; ++ j)
    {
      const Sample fx(collFunc_[j](inS.select(todo)));
      const Scalar sign = collOp_[j](1.0, 2.0) ? 1.0 : -1.0;
      for (SignedInteger i = todo.getSize() - 1; i >= 0; -- i)
      {
        const Scalar slack = sign * (fx(i, 0) - level_[j]);
        const Bool ok = collOp_[j](fx(i, 0), level_[j]);
        outS(todo[i], 0) = intersection_ ? std::max(outS(todo[i], 0), slack) : std::min(outS(todo[i], 0), slack);
        // return if outside subdomain for intersection or inside subdomain for union
        if (intersection_ != ok)
          todo.erase(todo.begin() + i, todo.begin() + i + 1);
      }
      // exit loop if all points are pruned
      if (!todo.getSize())
        break;
    }
    return outS;
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "LevelSetBooleanOperationEvaluation func=" << collFunc_ << " op=" << collOp_ << " level=" << level_ << " intersection=" << intersection_;
    return oss;
  }

  String __str__(const String & offset = "") const override
  {
    OSS oss;
    oss << offset;
    const UnsignedInteger size = collFunc_.getSize();
    // Scalar value = 0.0;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      oss << collFunc_[i].__str__() << " " << collOp_[i].__str__() << " " << level_[i];
      if (i != size - 1)
        oss << (intersection_ ? " and " : " or ");
    }
    return oss;
  }

  void save(Advocate & adv) const override
  {
    EvaluationImplementation::save(adv);
    adv.saveAttribute("collFunc_", collFunc_);
    adv.saveAttribute("collOp_", collOp_);
    adv.saveAttribute("level_", level_);
    adv.saveAttribute("intersection_", intersection_);
  }

  void load(Advocate & adv) override
  {
    EvaluationImplementation::load(adv);
    adv.loadAttribute("collFunc_", collFunc_);
    adv.loadAttribute("collOp_", collOp_);
    adv.loadAttribute("level_", level_);
    adv.loadAttribute("intersection_", intersection_);
  }

  Function::FunctionCollection getFunctionCollection() const
  {
    return collFunc_;
  }
  Collection<ComparisonOperator> getComparisonOperatorCollection() const
  {
    return collOp_;
  }
  Point getLevel() const
  {
    return level_;
  }
  Bool getIntersection() const
  {
    return intersection_;
  }
  String getClassName() const override
  {
    return "LevelSetBooleanOperationEvaluation";
  }
  static String GetClassName()
  {
    return "LevelSetBooleanOperationEvaluation";
  }

private:
  PersistentCollection<Function> collFunc_;
  PersistentCollection<ComparisonOperator> collOp_;
  Point level_;

  // intersection/union
  Bool intersection_ = true;
};

static const Factory<LevelSetBooleanOperationEvaluation> Factory_LevelSetBooleanOperationEvaluation;
static const Factory<PersistentCollection<ComparisonOperator> > Factory_PersistentCollection_ComparisonOperator;


/* Returns the levelSet equals to the intersection/union between the levelSet and another one */
LevelSet LevelSet::intersectOrJoin(const LevelSet & other, const Bool intersection) const
{
  // If one intersect the levelSet with itself
  if (this == &other) return (*this);

  // check dimension compatibility
  const String composeType = intersection ? "intersect" : "join";
  if (other.dimension_ != dimension_)
    throw InvalidArgumentException(HERE) << "Error: cannot " << composeType << " level sets of different dimensions";

  Collection<Function> collFunc;
  Collection<ComparisonOperator> collOp;
  Point level;
  const LevelSetBooleanOperationEvaluation *evaluation = dynamic_cast<LevelSetBooleanOperationEvaluation*>(function_.getEvaluation().getImplementation().get());
  if (evaluation && (intersection == evaluation->getIntersection()))
  {
    // merge evaluation
    collFunc.add(evaluation->getFunctionCollection());
    collFunc.add(other.function_);
    collOp.add(evaluation->getComparisonOperatorCollection());
    collOp.add(other.operator_);
    level.add(evaluation->getLevel());
    level.add(other.level_);
  }
  else
  {
    // standard case
    collFunc = {function_, other.function_};
    collOp = {operator_, other.operator_};
    level = {level_, other.level_};
  }
  const Function function(new LevelSetBooleanOperationEvaluation(collFunc, collOp, level, intersection));
  const ComparisonOperator op(operator_(1.0, 1.0) && other.operator_(1.0, 1.0) ? ComparisonOperator(LessOrEqual()) : ComparisonOperator(Less()));
  LevelSet result(function, op, 0.0);
  // Check if we can compute a bounding box
  if ((lowerBound_.getDimension() == dimension_) &&
      (upperBound_.getDimension() == dimension_) &&
      (other.lowerBound_.getDimension() == dimension_) &&
      (other.upperBound_.getDimension() == dimension_))
  {
    Interval boundingBox;
    if (intersection)
      boundingBox = Interval(lowerBound_, upperBound_).intersect(Interval(other.lowerBound_, other.upperBound_));
    else
      boundingBox = Interval(lowerBound_, upperBound_).join(Interval(other.lowerBound_, other.upperBound_));
    result.setLowerBound(boundingBox.getLowerBound());
    result.setUpperBound(boundingBox.getUpperBound());
  }
  return result;
}

/* Returns the levelSet equals to the intersection between the levelSet and another one */
LevelSet LevelSet::intersect(const LevelSet & other) const
{
  return intersectOrJoin(other, true);
}

/* Returns the levelSet equals to the union between the levelSet and another one */
LevelSet LevelSet::join(const LevelSet & other) const
{
  return intersectOrJoin(other, false);
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
  oss << "{x | f(x) " << operator_.__str__() << " " << level_ << "} with f=" << "\n" << offset << function_.__str__(offset);
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
