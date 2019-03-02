//                                               -*- C++ -*-
/**
 *  @brief LeastSquaresProblem allows to describe an optimization problem
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

void LeastSquaresProblem::setResidualFunction(const Function & residualFunction)
{
  // r = ||(f)||^2/2
  const UnsignedInteger residualSize = residualFunction.getOutputDimension();
  residualFunction_ = residualFunction;
  const Point center(residualSize);
  const Point constant(1);
  const Matrix linear(residualSize, 1);
  SymmetricTensor quadratic(residualSize, 1);
  for (UnsignedInteger i = 0; i < residualSize; ++ i)
    quadratic(i, i, 0) = 0.5;
  const QuadraticFunction sumQuad(center, constant, linear, quadratic);
  objective_ = ComposedFunction(sumQuad, residualFunction);
  dimension_ = residualFunction.getInputDimension();
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
