//                                               -*- C++ -*-
/**
 *  @brief BonminProblem implements the interface between OT and Bonmin as
 * for optimization problems. It is derived from Bonmin::TMINLP to ensure
 * compatibility with Bonmin algorithms.
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BonminProblem.hxx"
#include "openturns/SpecFunc.hxx"

#include <algorithm>

using namespace Bonmin;
using namespace Ipopt;

BEGIN_NAMESPACE_OPENTURNS

/** Constructor with parameters */
BonminProblem::BonminProblem( const OptimizationProblem & optimProblem,
                              const Point & startingPoint,
                              const UnsignedInteger maximumCallsNumber,
                              const Scalar maximumTimeDuration,
                              const std::chrono::steady_clock::time_point & t0)
  : TMINLP()
  , core_(optimProblem, startingPoint, maximumCallsNumber, maximumTimeDuration, t0)
{
  // Nothing to do
}

/** Retrieving objective function input.output history */
Sample BonminProblem::getInputHistory() const
{
  return core_.getInputHistory();
}

Sample BonminProblem::getOutputHistory() const
{
  return core_.getOutputHistory();
}

UnsignedInteger BonminProblem::getGradientCallsNumber() const
{
  return core_.getGradientCallsNumber();
}

UnsignedInteger BonminProblem::getHessianCallsNumber() const
{
  return core_.getHessianCallsNumber();
}

/** Normalized constraint view */
Bool BonminProblem::hasEqualityConstraint() const
{
  return core_.hasEqualityConstraint();
}

Bool BonminProblem::hasInequalityConstraint() const
{
  return core_.hasInequalityConstraint();
}

Function BonminProblem::getEqualityConstraint() const
{
  return core_.getEqualityConstraint();
}

Function BonminProblem::getInequalityConstraint() const
{
  return core_.getInequalityConstraint();
}

OptimizationProblem BonminProblem::getNormalizedProblem() const
{
  return core_.getNormalizedProblem();
}

/** Overloading functions from bonmin/Ipopt
    See Ipopt online documentation at
    https://coin-or.github.io/Ipopt/INTERFACES.html#INTERFACE_CPP
    for description */

bool BonminProblem::get_nlp_info(int & n,
                                 int & m,
                                 int & nnz_jac_g, // Number of non-zero components in the Jacobian of g
                                 int & nnz_h_lag, // Number of non-zero components in Hessian of Lagrangean
                                 Ipopt::TNLP::IndexStyleEnum & index_style)
{
  // Number of variables
  n = core_.getDimension();

  // Number of constraints
  m = core_.getEqualityDimension() + core_.getInequalityDimension();

  // All components of the jacobian and lagrangian's hessian are assumed to be non-zero
  nnz_jac_g = n * m;
  nnz_h_lag = n * n;

  // Index style is C-like
  index_style = Ipopt::TNLP::C_STYLE;

  return true;
}

bool BonminProblem::get_variables_types(int n, VariableTypeTable var_types)
{
  // Conversion from OptimizationProblemImplementation::VariableType to TMINLP::VariableType
  for (int i = 0; i < n; ++i)
  {
    switch (core_.getProblem().getVariablesType()[i])
    {
      case OptimizationProblemImplementation::CONTINUOUS:
      {
        var_types[i] = TMINLP::CONTINUOUS;
        break;
      }
      case OptimizationProblemImplementation::INTEGER:
      {
        var_types[i] = TMINLP::INTEGER;
        break;
      }
      case OptimizationProblemImplementation::BINARY:
      {
        var_types[i] = TMINLP::BINARY;
        break;
      }
    }
  }

  return true;
}

bool BonminProblem::get_variables_linearity( int n,
    LinearityTypeTable var_types)
{
  Function objective(core_.getProblem().getObjective());

  Indices objectiveLinearity(n, Ipopt::TNLP::NON_LINEAR);
  if (objective.getEvaluation().getImplementation()->isActualImplementation())
    for (UnsignedInteger i = 0; i < objective.getInputDimension(); ++i)
      if (objective.isLinearlyDependent(i))
        objectiveLinearity[i] = Ipopt::TNLP::LINEAR;

  Indices equalityConstraintsLinearity(n, Ipopt::TNLP::LINEAR);
  if (core_.hasEqualityConstraint())
    for (UnsignedInteger i = 0; i < objective.getInputDimension(); ++i)
      if (!core_.getEqualityConstraint().isLinearlyDependent(i))
        equalityConstraintsLinearity[i] = Ipopt::TNLP::NON_LINEAR;

  Indices inequalityConstraintsLinearity(n, Ipopt::TNLP::LINEAR);
  if (core_.hasInequalityConstraint())
    for (UnsignedInteger i = 0; i < objective.getInputDimension(); ++i)
      if (!core_.getInequalityConstraint().isLinearlyDependent(i))
        inequalityConstraintsLinearity[i] = Ipopt::TNLP::NON_LINEAR;

  for (int i = 0; i < n; ++i)
    if (  objectiveLinearity[i] == Ipopt::TNLP::LINEAR
          && equalityConstraintsLinearity[i] == Ipopt::TNLP::LINEAR
          && inequalityConstraintsLinearity[i] == Ipopt::TNLP::LINEAR)
      var_types[i] = Ipopt::TNLP::LINEAR;
    else
      var_types[i] = Ipopt::TNLP::NON_LINEAR;

  return true;
}

bool BonminProblem::get_constraints_linearity( int /*m*/,
    LinearityTypeTable const_types)
{
  // Evaluation of constraints linearity to TMINLP::LinearityType
  UnsignedInteger k = 0;

  // "Linear" functions in Bonmin are of the form f(x) = A*x,
  // i.e. f is linear and f(0) = 0
  const Point zero(core_.getDimension());

  if (core_.hasEqualityConstraint())
  {
    const Function equalityConstraint(core_.getEqualityConstraint());
    for (UnsignedInteger i = 0; i < core_.getEqualityDimension(); ++i)
    {
      const Function marginal(equalityConstraint.getMarginal(i));
      if (marginal.isLinear() && (marginal(zero)[0] == 0))
        const_types[k] = Ipopt::TNLP::LINEAR;
      else
        const_types[k] = Ipopt::TNLP::NON_LINEAR;
      ++k;
    }
  }

  if (core_.hasInequalityConstraint())
  {
    const Function inequalityConstraint(core_.getInequalityConstraint());
    for (UnsignedInteger i = 0; i < core_.getInequalityDimension(); ++i)
    {
      const Function marginal(inequalityConstraint.getMarginal(i));
      if (marginal.isLinear() && (marginal(zero)[0] == 0))
        const_types[k] = Ipopt::TNLP::LINEAR;
      else
        const_types[k] = Ipopt::TNLP::NON_LINEAR;
      ++k;
    }
  }

  return true;
}

bool BonminProblem::get_bounds_info( int n,
                                     double* x_l,  // Lower bounds
                                     double* x_u,  // Upper bounds
                                     int /*m*/,
                                     double* g_l,  // Lower bounds
                                     double* g_u)  // Upper bounds
{
  // BOUNDS:
  // Conversion from OT::Interval to double array
  if (core_.getProblem().hasBounds())
  {
    const Interval bounds(core_.getProblem().getBounds());

    for (int i = 0; i < n; ++i)
    {
      if (!bounds.getFiniteLowerBound()[i])
        x_l[i] = SpecFunc::LowestScalar;
      else
        x_l[i] = bounds.getLowerBound()[i];

      if (!bounds.getFiniteUpperBound()[i])
        x_u[i] = SpecFunc::MaxScalar;
      else
        x_u[i] = bounds.getUpperBound()[i];
    }
  }
  else
  {
    for (int i = 0; i < n; ++i)
    {
      x_l[i] = SpecFunc::LowestScalar;
      x_u[i] = SpecFunc::MaxScalar;
    }
  }

  // CONSTRAINTS:
  int k = 0;
  // Equality constraints
  for (UnsignedInteger i = 0; i < core_.getEqualityDimension(); ++i)
  {
    g_l[k] = 0.0;   // OT constraints are expressed as g(x) = 0 and h(x) >= 0
    g_u[k] = 0.0;
    ++k;
  }

  // Inequality constraints
  for (UnsignedInteger i = 0; i < core_.getInequalityDimension(); ++i)
  {
    g_l[k] = 0.0;   // OT constraints are expressed as g(x) = 0 and h(x) >= 0
    g_u[k] = SpecFunc::MaxScalar;
    ++k;
  }

  return true;
}

bool BonminProblem::get_starting_point(int /*n*/,
                                       bool /*init_x*/,
                                       double* x,
                                       bool /*init_z*/,
                                       double* /*z_L*/,
                                       double* /*z_U*/,
                                       int /*m*/,
                                       bool /*init_lambda*/,
                                       double* /*lambda*/)
{
  // Conversion starting point from OT::Point to double array
  const Point startingPoint(core_.getStartingPoint());
  std::copy(startingPoint.begin(), startingPoint.end(), x);

  return true;
}


bool BonminProblem::eval_f(int n,
                           const double* x,
                           bool /*new_x*/,
                           double& obj_value)
{
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x, x + n, xPoint.begin());

  // Computing objective function value
  Scalar value;
  if (!core_.evaluateObjective(xPoint, value))
    return false;

  obj_value = core_.getProblem().isMinimization() ? value : -value;

  return true;
}


bool BonminProblem::eval_grad_f( int n,
                                 const double* x,
                                 bool /*new_x*/,
                                 double* grad_f)
{
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x, x + n, xPoint.begin());

  // Computing objective function gradient
  Matrix gradOT;
  if (!core_.evaluateObjectiveGradient(xPoint, gradOT))
    return false;

  // Conversion from OT::Matrix to double array
  for (int i = 0; i < n; ++i)
    if (core_.getProblem().isMinimization())
      grad_f[i] = gradOT(i, 0);
    else
      grad_f[i] = -gradOT(i, 0);

  return true;
}

bool BonminProblem::eval_g(int n,
                           const double* x,
                           bool /*new_x*/,
                           int /*m*/,
                           double* g)
{
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x, x + n, xPoint.begin());

  // Equality constraints
  Point values(core_.getEqualityDimension());
  if (!core_.evaluateEqualityConstraint(xPoint, values))
    return false;
  std::copy(values.begin(), values.end(), g);

  // Inequality constraints
  values = Point(core_.getInequalityDimension());
  if (!core_.evaluateInequalityConstraint(xPoint, values))
    return false;
  std::copy(values.begin(), values.end(), g + core_.getEqualityDimension());

  return true;
}

bool BonminProblem::eval_jac_g(int n,
                               const double* x,
                               bool /*new_x*/,
                               int m,
                               int /*nnz_jac*/,
                               int* iRow,
                               int *jCol,
                               double* values)
{
  /* Switch on first call / later calls */
  if (values == NULL)
  {
    // First call: initialization of iRow/jCol
    int k = 0;
    for (int i = 0; i < m; ++i)
      for (int j = 0; j < n; ++j)
      {
        iRow[k] = i;
        jCol[k] = j;
        ++k;
      }
  }
  else     // Later calls
  {
    // Convert x to OT::Point
    Point xPoint(n);
    std::copy(x, x + n, xPoint.begin());

    // Filling values array
    UnsignedInteger k = 0;
    Matrix jacobian;

    // Equality constraints
    if (!core_.evaluateEqualityConstraintGradient(xPoint, jacobian))
      return false;
    for (UnsignedInteger i = 0; i < core_.getEqualityDimension(); ++i)
      for (int j = 0; j < n; ++j)
      {
        values[k] = jacobian(j, i);
        ++k;
      }

    // Inequality constraints
    if (!core_.evaluateInequalityConstraintGradient(xPoint, jacobian))
      return false;
    for (UnsignedInteger i = 0; i < core_.getInequalityDimension(); ++i)
      for (int j = 0; j < n; ++j)
      {
        values[k] = jacobian(j, i);
        ++k;
      }
  }

  return true;
}


bool BonminProblem::eval_h(int n,
                           const double* x,
                           bool /*new_x*/,
                           double obj_factor,
                           int /*m*/,
                           const double* lambda,
                           bool /*new_lambda*/,
                           int /*nele_hess*/,
                           int* iRow,
                           int* jCol,
                           double* values)
{
  /* Switch on first call / later calls */
  if (values == NULL) // First call: initialization of iRow/jCol
  {
    int k = 0;
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
      {
        iRow[k] = i;
        jCol[k] = j;
        ++k;
      }
  }
  else // Later calls
  {
    // The definition of the Lagrangian used in Bonmin is available in Ipopt online documentation
    // at https://coin-or.github.io/Ipopt/classIpopt_1_1TNLP.html#a26b9145267e2574c53acc284fef1c354

    // Convert x to OT::Point
    Point xPoint(n);
    std::copy(x, x + n, xPoint.begin());

    // Compute objective hessian
    SymmetricMatrix objectiveHessian;
    if (!core_.evaluateObjectiveHessian(xPoint, objectiveHessian))
      return false;

    // Compute constraints hessian
    int k = 0;
    SymmetricMatrix constraintsHessian(n, 0.0);

    if (core_.hasEqualityConstraint())
    {
      SymmetricTensor equalityConstraintHessian;
      if (!core_.evaluateEqualityConstraintHessian(xPoint, equalityConstraintHessian))
        return false;
      for (UnsignedInteger i = 0; i < core_.getEqualityDimension(); ++i)
      {
        constraintsHessian = constraintsHessian + lambda[k] * equalityConstraintHessian.getSheet(i);
        ++k;
      }
    }

    if (core_.hasInequalityConstraint())
    {
      SymmetricTensor inequalityConstraintHessian;
      if (!core_.evaluateInequalityConstraintHessian(xPoint, inequalityConstraintHessian))
        return false;
      for (UnsignedInteger i = 0; i < core_.getInequalityDimension(); ++i)
      {
        constraintsHessian = constraintsHessian + lambda[k] * inequalityConstraintHessian.getSheet(i);
        ++k;
      }
    }

    // Filling 'values' array
    const SymmetricMatrix lagrangianHessian(obj_factor * objectiveHessian + constraintsHessian);
    k = 0;
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
      {
        values[k] = lagrangianHessian(i, j);
        ++k;
      }
  }

  return true;
}

bool BonminProblem::eval_gi(int n,
                            const double* x,
                            bool /*new_x*/,
                            int i,
                            double& gi)
{
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x, x + n, xPoint.begin());

  // Computing constraint value
  Scalar value;
  if (!core_.evaluateConstraintMarginal(i, xPoint, value))
    return false;
  gi = value;
  return true;
}

bool BonminProblem::eval_grad_gi(int n,
                                 const double* x,
                                 bool /*new_x*/,
                                 int i,
                                 int& nele_grad_gi,
                                 int* jCol,
                                 double* values)
{
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x, x + n, xPoint.begin());

  // Computing constraint derivative
  nele_grad_gi = n;

  if (values == NULL) // First call
    for (int j = 0; j < n; ++j)
      jCol[j] = j;
  else
  {
    Matrix gradient;
    if (!core_.evaluateConstraintMarginalGradient(i, xPoint, gradient))
      return false;
    for (int j = 0; j < n; ++j)
      values[j] = gradient(j, 0);
  }
  return true;
}

void BonminProblem::finalize_solution( TMINLP::SolverReturn status,
                                       Ipopt::Index /*n*/,
                                       const Ipopt::Number* /*x*/,
                                       Ipopt::Number /*obj_value*/)
{
  status_ = status;
}


END_NAMESPACE_OPENTURNS
