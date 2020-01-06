//                                               -*- C++ -*-
/**
 *  @brief LeastSquaresProblem allows to describe an optimization problem
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeastSquaresProblem)

static const Factory<LeastSquaresProblem> Factory_LeastSquaresProblem;

/* Default constructor */
LeastSquaresProblem::LeastSquaresProblem()
  : OptimizationProblemImplementation()
{
  // Nothing to do
}

LeastSquaresProblem::LeastSquaresProblem(const Function & residualFunction)
  : OptimizationProblemImplementation()
{
  setResidualFunction(residualFunction);
}

/* Virtual constructor */
LeastSquaresProblem * LeastSquaresProblem::clone() const
{
  return new LeastSquaresProblem(*this);
}

/* Objective accessor */
Function LeastSquaresProblem::getResidualFunction() const
{
  return residualFunction_;
}

namespace LeastSquaresProblemFunctions
{
class HalfSquaredNormEvaluation: public EvaluationImplementation
{
public:
  HalfSquaredNormEvaluation(const UnsignedInteger dimension)
    : EvaluationImplementation()
    , dimension_(dimension)
  {
    // Nothing to do
  }

  HalfSquaredNormEvaluation * clone() const
  {
    return new HalfSquaredNormEvaluation(*this);
  }

  Point operator() (const Point & point) const
  {
    if (point.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << point.getDimension();
    return Point(1, 0.5 * point.normSquare());
  }

  UnsignedInteger getInputDimension() const
  {
    return dimension_;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(dimension_, "x");
  }

  Description getOutputDescription() const
  {
    return Description(1, "y");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  String __repr__() const
  {
    OSS oss;
    oss << "HalfSquaredNormEvaluation(" << dimension_ << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "HalfSquaredNormEvaluation(" << dimension_ << ")";
    return oss;
  }

private:
  const UnsignedInteger dimension_;
}; // class HalfSquaredNormEvaluation

class HalfSquaredNormGradient: public GradientImplementation
{
public:
  HalfSquaredNormGradient(const UnsignedInteger dimension)
    : GradientImplementation()
    , dimension_(dimension)
  {
    // Nothing to do
  }

  HalfSquaredNormGradient * clone() const
  {
    return new HalfSquaredNormGradient(*this);
  }

  Matrix gradient(const Point & point) const
  {
    if (point.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << point.getDimension();
    return Matrix(dimension_, 1, point);
  }

  UnsignedInteger getInputDimension() const
  {
    return dimension_;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(dimension_, "x");
  }

  Description getOutputDescription() const
  {
    return Description(1, "y");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  String __repr__() const
  {
    OSS oss;
    oss << "HalfSquaredNormGradient(" << dimension_ << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "HalfSquaredNormGradient(" << dimension_ << ")";
    return oss;
  }

private:
  const UnsignedInteger dimension_;

}; // class HalfSquaredNormGradient

class HalfSquaredNormHessian: public HessianImplementation
{
public:
  HalfSquaredNormHessian(const UnsignedInteger dimension)
    : HessianImplementation()
    , dimension_(dimension)
  {
    // Nothing to do
  }

  HalfSquaredNormHessian * clone() const
  {
    return new HalfSquaredNormHessian(*this);
  }

  SymmetricTensor hessian(const Point & point) const
  {
    if (point.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << point.getDimension();
    SymmetricTensor hess(dimension_, 1);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      hess(i, i, 0) = 1.0;
    return hess;
  }

  UnsignedInteger getInputDimension() const
  {
    return dimension_;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(dimension_, "x");
  }

  Description getOutputDescription() const
  {
    return Description(1, "y");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  String __repr__() const
  {
    OSS oss;
    oss << "HalfSquaredNormHessian(" << dimension_ << ")";
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss;
    oss << "HalfSquaredNormHessian(" << dimension_ << ")";
    return oss;
  }

private:
  const UnsignedInteger dimension_;

}; // class HalfSquaredNormHessian
} // namespace LeastSquaresProblemFunctions


void LeastSquaresProblem::setResidualFunction(const Function & residualFunction)
{
  // r = ||(f)||^2/2
  const UnsignedInteger residualSize = residualFunction.getOutputDimension();
  residualFunction_ = residualFunction;
  const Function halfSquaredNorm(LeastSquaresProblemFunctions::HalfSquaredNormEvaluation(residualSize).clone(),
                                 LeastSquaresProblemFunctions::HalfSquaredNormGradient(residualSize).clone(), LeastSquaresProblemFunctions::HalfSquaredNormHessian(residualSize).clone()
                                );
  objective_ = ComposedFunction(halfSquaredNorm, residualFunction);
  dimension_ = residualFunction.getInputDimension();

  setVariablesType(Indices(dimension_, CONTINUOUS));
}

Bool LeastSquaresProblem::hasResidualFunction() const
{
  return true;
}


/* String converter */
String LeastSquaresProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << LeastSquaresProblem::GetClassName();
  oss << " residual function=" << residualFunction_.__repr__();
  oss << " dimension=" << dimension_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LeastSquaresProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute( "residualFunction_", residualFunction_ );
}

/* Method load() reloads the object from the StorageManager */
void LeastSquaresProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute( "residualFunction_", residualFunction_ );
}

END_NAMESPACE_OPENTURNS
