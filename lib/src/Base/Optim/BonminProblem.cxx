//                                               -*- C++ -*-
/**
 *  @brief BonminProblem implements the interface between OT and Bonmin as
 * for optimization problems. It is derived from Bonmin::TMINLP to ensure
 * compatibility with Bonmin algorithms.
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
#include "openturns/BonminProblem.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS
 
/** Constructor with parameters */
BonminProblem::BonminProblem( const OptimizationProblem & optimProblem,
                            const Point & startingPoint,
                            const UnsignedInteger maximumEvaluationNumber)
  : TMINLP()
  , optimProblem_(optimProblem)
  , startingPoint_(startingPoint)
  , objectiveFunction_(optimProblem.getObjective())
  , optimalPoint_(optimProblem_.getDimension())
  , optimalValue_(1)
  , maximumEvaluationNumber_(maximumEvaluationNumber)
  , progressCallback_(std::make_pair<OptimizationAlgorithmImplementation::ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<OptimizationAlgorithmImplementation::StopCallback, void *>(0, 0))
{
  // Nothing to do
}
  
/** Retrieving objective function input.output history */
Sample BonminProblem::getInputHistory() const
{
  return objectiveFunction_.getInputHistory();
}

Sample BonminProblem::getOutputHistory() const
{
  return objectiveFunction_.getOutputHistory();
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
  n = optimProblem_.getDimension();
  
  // Number of constraints
  m = 0;
  if (optimProblem_.hasEqualityConstraint())
    m += optimProblem_.getEqualityConstraint().getOutputDimension();
  if (optimProblem_.hasInequalityConstraint())
    m += optimProblem_.getInequalityConstraint().getOutputDimension();
  
  // All components of the jacobian and lagrangian's hessian are assumed to be non-zero
  nnz_jac_g = n * m;
  nnz_h_lag = n * n;

  // Index style is C-like
  index_style = Ipopt::TNLP::C_STYLE;
 
  return true;
}
        
bool BonminProblem::get_variables_types( int n,
                                        VariableTypeTable var_types)
{    
  Indices variablesTypes(optimProblem_.getVariablesType());
 
  // Conversion from OptimizationProblemImplementation::VariableType to TMINLP::VariableType
  for (int i=0; i<n; ++i)
  {
    switch (optimProblem_.getVariablesType()[i])
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
  Function objective(optimProblem_.getObjective());
  
  Indices objectiveLinearity(n, Ipopt::TNLP::NON_LINEAR);
  if (objective.getEvaluation().getImplementation()->isActualImplementation())
    for (UnsignedInteger i=0; i< objective.getInputDimension(); ++i)
      if (objective.isLinearlyDependent(i))
        objectiveLinearity[i] = Ipopt::TNLP::LINEAR;
    
  Indices equalityConstraintsLinearity(n, Ipopt::TNLP::LINEAR);
  if (optimProblem_.hasEqualityConstraint())
    for (UnsignedInteger i=0; i< objective.getInputDimension(); ++i)
      if (!optimProblem_.getEqualityConstraint().isLinearlyDependent(i))
        equalityConstraintsLinearity[i] = Ipopt::TNLP::NON_LINEAR;
      
  Indices inequalityConstraintsLinearity(n, Ipopt::TNLP::LINEAR);
  if (optimProblem_.hasInequalityConstraint())
    for (UnsignedInteger i=0; i< objective.getInputDimension(); ++i)
      if (!optimProblem_.getInequalityConstraint().isLinearlyDependent(i))
        inequalityConstraintsLinearity[i] = Ipopt::TNLP::NON_LINEAR;

  for (int i=0; i<n; ++i)
    if (  objectiveLinearity[i] == Ipopt::TNLP::LINEAR 
       && equalityConstraintsLinearity[i] == Ipopt::TNLP::LINEAR 
       && inequalityConstraintsLinearity[i] == Ipopt::TNLP::LINEAR)
      var_types[i] = Ipopt::TNLP::LINEAR;
    else
      var_types[i] = Ipopt::TNLP::NON_LINEAR;

  return true;
}

bool BonminProblem::get_constraints_linearity( int m,
                                              LinearityTypeTable const_types)
{  
  // Retrieving number of constraints
  const UnsignedInteger nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
  const UnsignedInteger nbInequalityConstraints = optimProblem_.getInequalityConstraint().getOutputDimension();
  
  // Evaluation of constraints linearity to TMINLP::LinearityType
  UnsignedInteger k = 0;
      
    // "Linear" functions in Bonmin are of the form f(x) = A*x,
    // i.e. f is linear and f(0) = 0
  
  Point zero(optimProblem_.getDimension());
  for (UnsignedInteger i=0; i<nbEqualityConstraints; ++i)
  {
    if (optimProblem_.getEqualityConstraint().getMarginal(i).isLinear()
        && (optimProblem_.getEqualityConstraint().getMarginal(i)(zero)[0] == 0))
      const_types[k] = Ipopt::TNLP::LINEAR;
    else
      const_types[k] = Ipopt::TNLP::NON_LINEAR;
    ++k;
  }
  
  for (UnsignedInteger i=0; i<nbInequalityConstraints; ++i)
  {
    if (optimProblem_.getInequalityConstraint().getMarginal(i).isLinear()
        && (optimProblem_.getInequalityConstraint().getMarginal(i)(zero)[0] == 0))
      const_types[k] = Ipopt::TNLP::LINEAR;
    else
      const_types[k] = Ipopt::TNLP::NON_LINEAR;
    ++k;
  }

  return true;
}

bool BonminProblem::get_bounds_info( int n,
                                    double* x_l,  // Lower bounds
                                    double* x_u,  // Upper bounds
                                    int m,
                                    double* g_l,  // Lower bounds
                                    double* g_u)  // Upper bounds
{
  // BOUNDS:
  // Conversion from OT::Interval to double array
  if (optimProblem_.hasBounds())
  {
    Interval bounds(optimProblem_.getBounds());
    
    for (int i=0; i<n; ++i)
    {
      if (!bounds.getFiniteLowerBound()[i]) 
        x_l[i] = -SpecFunc::MaxScalar;
      else 
        x_l[i] = bounds.getLowerBound()[i];
      
      if (!bounds.getFiniteUpperBound()[i])
        x_u[i] =  SpecFunc::MaxScalar;
      else
        x_u[i] = bounds.getUpperBound()[i];
    }
  }
  else
  {
    for (int i=0; i<n; ++i)
    {
      x_l[i] = -SpecFunc::MaxScalar;
      x_u[i] =  SpecFunc::MaxScalar;
    }
  }
  
  // CONSTRAINTS:
  int k=0;
  if (optimProblem_.hasEqualityConstraint())  // Equality constraints
    for (UnsignedInteger i=0; i < optimProblem_.getEqualityConstraint().getOutputDimension(); ++i)
    {
      g_l[k] = 0.0;   // OT constraints are expressed as g(x) = 0 and h(x) >= 0  
      g_u[k] = 0.0;
      ++k;
    }

  if (optimProblem_.hasInequalityConstraint()) // Inequality constraints
    for (UnsignedInteger i=0; i < optimProblem_.getInequalityConstraint().getOutputDimension(); ++i)
    {
      g_l[k] = 0.0;   // OT constraints are expressed as g(x) = 0 and h(x) >= 0  
      g_u[k] = SpecFunc::MaxScalar;
      ++k;
    }

  return true;
}

bool BonminProblem::get_starting_point(int n,
                                      bool init_x,
                                      double* x,
                                      bool init_z,
                                      double* z_L,
                                      double* z_U,
                                      int m,
                                      bool init_lambda,
                                      double* lambda)
{  
  // Retrieve number of constraints
  UnsignedInteger nbConstraints = 0;
  if (optimProblem_.hasEqualityConstraint())
    nbConstraints += optimProblem_.getEqualityConstraint().getOutputDimension();
  if (optimProblem_.hasInequalityConstraint())
    nbConstraints += optimProblem_.getInequalityConstraint().getOutputDimension();
  
  // Conversion starting point from OT::Point to double array
  std::copy(startingPoint_.begin(), startingPoint_.end(), x);

  return true;
}

bool BonminProblem::eval_f(int n,
                          const double* x,
                          bool new_x,
                          double& obj_value)
{  
  // Convert x to OT::Point 
  Point xPoint(n);
  std::copy(x, x+n, xPoint.begin());
  
  // Computing objective function value
  if (optimProblem_.isMinimization())
    obj_value = objectiveFunction_(xPoint)[0];
  else
    obj_value = -objectiveFunction_(xPoint)[0];
  
  // Check callbacks
  if (progressCallback_.first)
  {
    progressCallback_.first((100.0 * objectiveFunction_.getInputHistory().getSize()) / maximumEvaluationNumber_, progressCallback_.second);
  }
  if (stopCallback_.first)
  {
    Bool stop = stopCallback_.first(stopCallback_.second);
    if (stop)
      return false;
  }
  
  return (objectiveFunction_.getInputHistory().getSize() <= maximumEvaluationNumber_);
}


bool BonminProblem::eval_grad_f( int n,
                  const double* x,
                  bool new_x,
                  double* grad_f)
{    
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x,x+n,xPoint.begin());
   
  // Computing objective function gradient
  Matrix gradOT(objectiveFunction_.gradient(xPoint));
  
  // Conversion from OT::Matrix to double array
  for (int i=0; i<n; ++i)
    if (optimProblem_.isMinimization())
      grad_f[i] = gradOT(i,0);
    else
      grad_f[i] = -gradOT(i,0);

  return true;
}

bool BonminProblem::eval_g(int n,
                          const double* x,
                          bool new_x,
                          int m,
                          double* g)
{  
  // Convert x to OT::Point
  Point xPoint(n);
  std::copy(x,x+n,xPoint.begin());
  
  // Retrieve number of constraints
  UnsignedInteger nbEqualityConstraints = 0;
  UnsignedInteger nbInequalityConstraints = 0;
  UnsignedInteger k = 0;
  
  if (optimProblem_.hasEqualityConstraint())
  {
    nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
    Point equalityConstraint(optimProblem_.getEqualityConstraint()(xPoint));
    std::copy(equalityConstraint.begin(), equalityConstraint.end(), g+k);
    k += nbEqualityConstraints;
  }
        
if (optimProblem_.hasInequalityConstraint())
 {
    nbInequalityConstraints = optimProblem_.getInequalityConstraint().getOutputDimension();
    Point inequalityConstraint(optimProblem_.getInequalityConstraint()(xPoint));
    std::copy(inequalityConstraint.begin(), inequalityConstraint.end(), g+k);
    k += nbInequalityConstraints;
  }
  
  return true;
} 
  
bool BonminProblem::eval_jac_g(int n,
                const double* x,
                bool new_x,
                int m,
                int nnz_jac,
                int* iRow,
                int *jCol,
                double* values)
{ 
  /* Switch on first call / later calls */
  if (values == NULL)
  {
    // First call: initialization of iRow/jCol
    int k = 0;
    for (int i=0; i<m; ++i)
      for (int j=0; j<n; ++j)
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
    std::copy(x, x+n, xPoint.begin());

    // Filling values array
    UnsignedInteger k = 0;
    UnsignedInteger nbEqualityConstraints = 0;
    UnsignedInteger nbInequalityConstraints = 0;
    
    if (optimProblem_.hasEqualityConstraint())
    {
      nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
      Matrix equalityConstraintGradient(optimProblem_.getEqualityConstraint().gradient(xPoint));
      for (UnsignedInteger i=0; i<nbEqualityConstraints; ++i)
        for (int j=0; j<n; ++j)
        {
          values[k] = equalityConstraintGradient(j,i);
          ++k;
        }
    }
      
    if (optimProblem_.hasInequalityConstraint())
    {
      nbInequalityConstraints = optimProblem_.getInequalityConstraint().getOutputDimension();
      Matrix inequalityConstraintGradient(optimProblem_.getInequalityConstraint().gradient(xPoint));
      for (UnsignedInteger i=0; i<nbInequalityConstraints; ++i)
        for (int j=0; j<n; ++j)
        {
          values[k] = inequalityConstraintGradient(j,i);
          ++k;
        }
    }
  }

  return true;
}


bool BonminProblem::eval_h(int n,
                          const double* x,
                          bool new_x,
                          double obj_factor,
                          int m,
                          const double* lambda,
                          bool new_lambda,
                          int nele_hess,
                          int* iRow,
                          int* jCol,
                          double* values)
{      
  // Retrieve number of constraints
  UnsignedInteger nbEqualityConstraints = 0;
  UnsignedInteger nbInequalityConstraints = 0;
  if (optimProblem_.hasEqualityConstraint())
    nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
  if (optimProblem_.hasInequalityConstraint())
    nbInequalityConstraints = optimProblem_.getInequalityConstraint().getOutputDimension();

  /* Switch on first call / later calls */
  if (values == NULL) // First call: initialization of iRow/jCol
  {
    int k = 0;
    for (int i=0; i<n; ++i)
      for (int j=0; j<n; ++j)
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
    std::copy(x,x+n,xPoint.begin());
        
    // Compute objective hessian
    SymmetricMatrix objectiveHessian(obj_factor * optimProblem_.getObjective().hessian(xPoint).getSheet(0));
    
    // Compute constraints hessian
    int k = 0;
    SymmetricMatrix constraintsHessian(n,0.0);
    
    if (optimProblem_.hasEqualityConstraint())
    {
      SymmetricTensor equalityConstraintHessian(optimProblem_.getEqualityConstraint().hessian(xPoint));
      for (UnsignedInteger i=0; i<nbEqualityConstraints; ++i)
      {
        constraintsHessian = constraintsHessian + lambda[k] * equalityConstraintHessian.getSheet(i);
        ++k;
      }
    }
    
    if (optimProblem_.hasInequalityConstraint())
    {
      SymmetricTensor inequalityConstraintHessian(optimProblem_.getInequalityConstraint().hessian(xPoint));
      for (UnsignedInteger i=0; i<nbInequalityConstraints; ++i)
      {
        constraintsHessian = constraintsHessian + lambda[k] * inequalityConstraintHessian.getSheet(i);
        ++k;
      }
    }
      
    // Filling 'values' array
    SymmetricMatrix lagrangianHessian(objectiveHessian + constraintsHessian);
    k = 0;
    for (int i=0; i<n; ++i)
      for (int j=0; j<n; ++j)
      {
        values[k] = lagrangianHessian(i,j);
        ++k;
      }
  }
  
  return true;
}

bool BonminProblem::eval_gi(int n,
              const double* x,
              bool new_x,
              int i,
              double& gi)
{  
  // Convert x to OT::Point 
  Point xPoint(n);
  std::copy(x, x+n, xPoint.begin());
  
  // Retrieve number of constraints
  int nbEqualityConstraints = 0;
  if (optimProblem_.hasEqualityConstraint())
    nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
   
  // Computing constraints values
  if (i<nbEqualityConstraints)
    gi = optimProblem_.getEqualityConstraint().getMarginal(i)(xPoint)[0];
  else
    gi = optimProblem_.getInequalityConstraint().getMarginal(i-nbEqualityConstraints)(xPoint)[0];
  
  return true;
} 

bool BonminProblem::eval_grad_gi(int n,
                  const double* x,
                  bool new_x,
                  int i,
                  int& nele_grad_gi,
                  int* jCol,
                  double* values)
{
  // Convert x to OT::Point 
  Point xPoint(n);
  std::copy(x, x+n, xPoint.begin());
  
  // Computing constraint derivative
  nele_grad_gi = n;
  
  if (values==NULL) // First call
    for (int j=0; j<n; ++j)
      jCol[j] = j;
  else
  {
    // Retrieve number of constraints
    int nbEqualityConstraints = optimProblem_.getEqualityConstraint().getOutputDimension();
    
    if (i < nbEqualityConstraints)
    {
      Matrix equalityConstraintGradient(optimProblem_.getEqualityConstraint().getMarginal(i).gradient(xPoint));
      for (int j=0; j<n; ++j)
        values[j] = equalityConstraintGradient(j,0);
    }
    else
    {
      Matrix inequalityConstraintGradient(optimProblem_.getInequalityConstraint().getMarginal(i-nbEqualityConstraints).gradient(xPoint));
      for (int j=0; j<n; ++j)
        values[j] = inequalityConstraintGradient(j,0);
    }
  }
  return true;
}

void BonminProblem::finalize_solution( TMINLP::SolverReturn status,
                                      Ipopt::Index n,
                                      const Ipopt::Number* x,
                                      Ipopt::Number obj_value)
{
  // Check if solver succeeded
  Description bonminExitStatus(6);
  bonminExitStatus[0] = "SUCCESS";
  bonminExitStatus[1] = "INFEASIBLE";
  bonminExitStatus[2] = "CONTINUOUS_UNBOUNDED";
  bonminExitStatus[3] = "LIMIT_EXCEEDED";
  bonminExitStatus[4] = "USER_INTERRUPT";
  bonminExitStatus[5] = "MINLP_ERROR";
  if (status >= bonminExitStatus.getSize())
    throw InternalException(HERE) << "Bonmin solver exited with status UNKNOWN ERROR";

  if (status != SUCCESS)
    throw InternalException(HERE) << "Bonmin solver exited with status " << bonminExitStatus[status];
    
  // Convert x to OT::Point
  std::copy(x, x+n, optimalPoint_.begin());
  if (optimProblem_.isMinimization())
    optimalValue_[0] = obj_value;
  else
    optimalValue_[0] = -obj_value;
}


END_NAMESPACE_OPENTURNS
