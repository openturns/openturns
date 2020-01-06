//                                               -*- C++ -*-
/**
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
#include "openturns/BoxCoxTransform.hxx"
#include "openturns/BoxCoxEvaluation.hxx"
#include "openturns/BoxCoxGradient.hxx"
#include "openturns/BoxCoxHessian.hxx"
#include "openturns/InverseBoxCoxTransform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxTransform)

/* Default constructor */
BoxCoxTransform::BoxCoxTransform()
  : Function(BoxCoxEvaluation())
{
  // Nothing to do
}

/* Standard parameter constructor */
BoxCoxTransform::BoxCoxTransform(const Point & lambda)
  : Function()
{
  const Pointer<BoxCoxEvaluation> p_evaluation = new BoxCoxEvaluation(lambda);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new BoxCoxGradient(p_evaluation));
  setHessian(new BoxCoxHessian(p_evaluation));
}

/* ScalarCollection parameter constructor */
BoxCoxTransform::BoxCoxTransform(const Point & lambda,
                                 const Point & shift)
  : Function()
{
  const Pointer<BoxCoxEvaluation> p_evaluation = new BoxCoxEvaluation(lambda, shift);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new BoxCoxGradient(p_evaluation));
  setHessian(new BoxCoxHessian(p_evaluation));
}

/* 1D Scalar parameter constructor */
BoxCoxTransform::BoxCoxTransform(const Scalar & lambda)
  : Function()
{
  const Pointer<BoxCoxEvaluation> p_evaluation = new BoxCoxEvaluation(Point(1, lambda));
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new BoxCoxGradient(p_evaluation));
  setHessian(new BoxCoxHessian(p_evaluation));
}

BoxCoxTransform::BoxCoxTransform(const Scalar & lambda,
                                 const Scalar & shift)
  : Function()
{
  const Pointer<BoxCoxEvaluation> p_evaluation = new BoxCoxEvaluation(Point(1, lambda), Point(1, shift));
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new BoxCoxGradient(p_evaluation));
  setHessian(new BoxCoxHessian(p_evaluation));
}

/* Virtual constructor */
BoxCoxTransform * BoxCoxTransform::clone() const
{
  return new BoxCoxTransform(*this);
}

/* Lambda accessor */
Point BoxCoxTransform::getLambda() const
{
  return dynamic_cast< BoxCoxEvaluation* >(getEvaluation().getImplementation().get())->getLambda();
}

/* Shift accessor */
Point BoxCoxTransform::getShift() const
{
  return dynamic_cast< BoxCoxEvaluation* >(getEvaluation().getImplementation().get())->getShift();
}

/* Inverse accessor */
InverseBoxCoxTransform BoxCoxTransform::getInverse() const
{
  return InverseBoxCoxTransform(getLambda(), getShift());
}

END_NAMESPACE_OPENTURNS
