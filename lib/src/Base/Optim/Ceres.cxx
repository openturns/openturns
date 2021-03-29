//                                               -*- C++ -*-
/**
 *  @brief Ceres solver
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Ceres.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#ifdef OPENTURNS_HAVE_CERES
#include <ceres/ceres.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Ceres)

static const Factory<Ceres> Factory_Ceres;

Description Ceres::GetAlgorithmNames()
{
  static Description AlgorithmNames;
  if (!AlgorithmNames.getSize())
  {
    // trust-region methods, not for general optimization
    AlgorithmNames.add("LEVENBERG_MARQUARDT");// default nlls method, list it first
    AlgorithmNames.add("DOGLEG");

    // line search methods, available for both least-squares and general optimization
    AlgorithmNames.add("STEEPEST_DESCENT");
    AlgorithmNames.add("NONLINEAR_CONJUGATE_GRADIENT");
    AlgorithmNames.add("LBFGS");
    AlgorithmNames.add("BFGS");
  }
  return AlgorithmNames;
}


/* Default constructor */
Ceres::Ceres(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , algoName_(algoName)
{
  if (!GetAlgorithmNames().contains(algoName))
    throw InvalidArgumentException(HERE) << "Unknown algorithm name, should be one of " << GetAlgorithmNames();
}

Ceres::Ceres(const OptimizationProblem & problem,
             const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
  , algoName_(algoName)
{
  if (!GetAlgorithmNames().contains(algoName))
    throw InvalidArgumentException(HERE) << "Unknown algorithm name, should be one of " << GetAlgorithmNames();
  checkProblem(problem);
}

/* Virtual constructor */
Ceres * Ceres::clone() const
{
  return new Ceres(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void Ceres::checkProblem(const OptimizationProblem & problem) const
{
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support multi-objective optimization";

  if (problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support nearest-point problems";

  if (problem.hasBounds() && (algoName_ != "LEVENBERG_MARQUARDT" && algoName_ != "DOGLEG"))
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " line search algorithms do not support bound constraints";

  if (!problem.hasResidualFunction() && (algoName_ == "LEVENBERG_MARQUARDT" || algoName_ == "DOGLEG"))
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " trust-region algorithms do not support general optimization";

  if (problem.hasInequalityConstraint())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support inequality constraints";

  if (problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support equality constraints";

  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Error: " << getClassName() << " does not support non continuous problems";
}

#ifdef OPENTURNS_HAVE_CERES
class CostFunctionInterface : public ceres::CostFunction
{
public:
  explicit CostFunctionInterface(Ceres & algorithm)
    : ceres::CostFunction()
    , algorithm_(algorithm)
  {
    const OptimizationProblem problem(algorithm_.getProblem());
    *mutable_parameter_block_sizes() = std::vector<int32_t>(1, problem.getDimension());
    set_num_residuals(problem.getResidualFunction().getOutputDimension());
  }

  virtual bool Evaluate(double const* const* parameters,
                        double* residuals,
                        double** jacobians) const override
  {
    const OptimizationProblem problem(algorithm_.getProblem());
    const UnsignedInteger n = problem.getDimension();
    const UnsignedInteger m = problem.getResidualFunction().getOutputDimension();
    Point inP(n);
    const double * x = parameters[0];
    std::copy(x, x + n, inP.begin());

    // evaluation
    const Point outP(problem.getResidualFunction()(inP));
    std::copy(outP.begin(), outP.end(), residuals);
    algorithm_.evaluationInputHistory_.add(inP);
    algorithm_.evaluationOutputHistory_.add(Point(1, 0.5 * outP.normSquare()));

    // gradient
    if (jacobians)
    {
      const Matrix gradient(problem.getResidualFunction().gradient(inP));
      std::copy(&gradient(0, 0), &gradient(n - 1, m - 1) + 1, jacobians[0]);
    }
    return true;
  }

protected:
  Ceres & algorithm_;
};


class FirstOrderFunctionInterface : public ceres::FirstOrderFunction
{
public:
  explicit FirstOrderFunctionInterface(Ceres & algorithm)
    : ceres::FirstOrderFunction()
    , algorithm_(algorithm) {}

  virtual int NumParameters() const override
  {
    return algorithm_.getProblem().getDimension();
  }

  virtual bool Evaluate(const double * const x,
                        double * cost,
                        double * jacobian) const override
  {
    const OptimizationProblem problem(algorithm_.getProblem());
    const UnsignedInteger n = problem.getDimension();
    Point inP(n);
    std::copy(x, x + n, inP.begin());

    // evaluation
    const Point outP(problem.getObjective()(inP));
    *cost = problem.isMinimization() ? outP[0] : -outP[0];
    algorithm_.evaluationInputHistory_.add(inP);
    algorithm_.evaluationOutputHistory_.add(outP);

    // gradient
    if (jacobian)
    {
      const Matrix gradient(problem.isMinimization() ? problem.getObjective().gradient(inP) : -1.0 * problem.getObjective().gradient(inP));
      std::copy(&gradient(0, 0), &gradient(n - 1, 0) + 1, jacobian);
    }
    return true;
  }

protected:
  Ceres & algorithm_;
};


class IterationCallbackInterface : public ceres::IterationCallback
{
public:
  explicit IterationCallbackInterface(Ceres & algorithm)
    : ceres::IterationCallback()
    , algorithm_(algorithm) {}

  virtual ceres::CallbackReturnType operator()(const ceres::IterationSummary & summary)
  {
    if (algorithm_.progressCallback_.first)
      algorithm_.progressCallback_.first(100.0 * summary.iteration / algorithm_.getMaximumIterationNumber(), algorithm_.progressCallback_.second);
    if (algorithm_.stopCallback_.first && algorithm_.stopCallback_.first(algorithm_.stopCallback_.second))
      return ceres::SOLVER_ABORT;
    else
      return ceres::SOLVER_CONTINUE;
  }

protected:
  const Ceres & algorithm_;
};

#endif

/* Performs the actual computation by calling the Ceres library
 */
void Ceres::run()
{
#ifdef OPENTURNS_HAVE_CERES
  const UnsignedInteger dimension = getProblem().getDimension();
  Point x(getStartingPoint());
  if (x.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << x.getDimension() << "), expected " << dimension;

  // initialize history
  evaluationInputHistory_ = Sample(0, dimension);
  evaluationOutputHistory_ = Sample(0, 1);

  double optimalValue = 0.0;
  UnsignedInteger iterationNumber = 0;

  if (getProblem().hasResidualFunction())
  {
    // Build the problem.
    ceres::Problem problem;
    ceres::CostFunction* cost_function = new CostFunctionInterface(*this);
    problem.AddResidualBlock(cost_function, NULL, &x[0]);

    if (getProblem().hasBounds())
    {
      Interval bounds(getProblem().getBounds());
      if (!bounds.contains(x))
        LOGWARN(OSS() << "Starting point is not inside bounds x=" << x.__str__() << " bounds=" << bounds);
      Interval::BoolCollection finiteLowerBound(bounds.getFiniteLowerBound());
      Interval::BoolCollection finiteUpperBound(bounds.getFiniteUpperBound());
      Point lowerBound(bounds.getLowerBound());
      Point upperBound(bounds.getUpperBound());
      for (UnsignedInteger i = 0; i < dimension; ++ i)
      {
        if (finiteLowerBound[i]) problem.SetParameterLowerBound(&(*x.begin()), i, lowerBound[i]);
        if (finiteUpperBound[i]) problem.SetParameterUpperBound(&(*x.begin()), i, upperBound[i]);
      }
    }

    // Run the solver!
    ceres::Solver::Options options;

    // Switch trust region / line search depending on algoName as it's the union
    if (ceres::StringToTrustRegionStrategyType(algoName_, &options.trust_region_strategy_type))
      options.minimizer_type = ceres::TRUST_REGION;
    else if (ceres::StringToLineSearchDirectionType(algoName_, &options.line_search_direction_type))
      options.minimizer_type = ceres::LINE_SEARCH;
    else
      throw InvalidArgumentException(HERE) << "Could not set minimizer_type";

    options.max_num_iterations = getMaximumIterationNumber();
    options.function_tolerance = getMaximumResidualError();
    options.parameter_tolerance = getMaximumRelativeError();

    // Set remaining options from ResourceMap
    if (ResourceMap::HasKey("Ceres-line_search_type") && !ceres::StringToLineSearchType(ResourceMap::Get("Ceres-line_search_type"), &options.line_search_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_type";
    if (ResourceMap::HasKey("Ceres-nonlinear_conjugate_gradient_type") && !ceres::StringToNonlinearConjugateGradientType(ResourceMap::Get("Ceres-nonlinear_conjugate_gradient_type"), &options.nonlinear_conjugate_gradient_type))
      throw InvalidArgumentException(HERE) << "Invalid value for nonlinear_conjugate_gradient_type";
    if (ResourceMap::HasKey("Ceres-max_lbfgs_rank"))
      options.max_lbfgs_rank = ResourceMap::GetAsUnsignedInteger("Ceres-max_lbfgs_rank");
    if (ResourceMap::HasKey("Ceres-use_approximate_eigenvalue_bfgs_scaling"))
      options.use_approximate_eigenvalue_bfgs_scaling = ResourceMap::GetAsBool("Ceres-use_approximate_eigenvalue_bfgs_scaling");
    if (ResourceMap::HasKey("Ceres-line_search_interpolation_type") && !ceres::StringToLineSearchInterpolationType(ResourceMap::Get("Ceres-line_search_interpolation_type"), &options.line_search_interpolation_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_interpolation_type";
    if (ResourceMap::HasKey("Ceres-min_line_search_step_size"))
      options.min_line_search_step_size = ResourceMap::GetAsScalar("Ceres-min_line_search_step_size");
    if (ResourceMap::HasKey("Ceres-line_search_sufficient_function_decrease"))
      options.line_search_sufficient_function_decrease = ResourceMap::GetAsScalar("Ceres-line_search_sufficient_function_decrease");
    if (ResourceMap::HasKey("Ceres-max_line_search_step_contraction"))
      options.max_line_search_step_contraction = ResourceMap::GetAsScalar("Ceres-max_line_search_step_contraction");
    if (ResourceMap::HasKey("Ceres-min_line_search_step_contraction"))
      options.min_line_search_step_contraction = ResourceMap::GetAsScalar("Ceres-min_line_search_step_contraction");
    if (ResourceMap::HasKey("Ceres-max_num_line_search_step_size_iterations"))
      options.max_num_line_search_step_size_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_line_search_step_size_iterations");
    if (ResourceMap::HasKey("Ceres-max_num_line_search_direction_restarts"))
      options.max_num_line_search_direction_restarts = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_line_search_direction_restarts");
    if (ResourceMap::HasKey("Ceres-line_search_sufficient_curvature_decrease"))
      options.line_search_sufficient_curvature_decrease = ResourceMap::GetAsScalar("Ceres-line_search_sufficient_curvature_decrease");
    if (ResourceMap::HasKey("Ceres-max_line_search_step_expansion"))
      options.max_line_search_step_expansion = ResourceMap::GetAsScalar("Ceres-max_line_search_step_expansion");
    if (ResourceMap::HasKey("Ceres-dogleg_type") && !ceres::StringToDoglegType(ResourceMap::Get("Ceres-dogleg_type"), &options.dogleg_type))
      throw InvalidArgumentException(HERE) << "Invalid value for dogleg_type";
    if (ResourceMap::HasKey("Ceres-use_nonmonotonic_steps"))
      options.use_nonmonotonic_steps = ResourceMap::GetAsBool("Ceres-use_nonmonotonic_steps");
    if (ResourceMap::HasKey("Ceres-max_consecutive_nonmonotonic_steps"))
      options.max_consecutive_nonmonotonic_steps = ResourceMap::GetAsUnsignedInteger("Ceres-max_consecutive_nonmonotonic_steps");
    if (ResourceMap::HasKey("Ceres-max_num_iterations"))
      options.max_num_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_iterations");
    if (ResourceMap::HasKey("Ceres-max_solver_time_in_seconds"))
      options.max_solver_time_in_seconds = ResourceMap::GetAsScalar("Ceres-max_solver_time_in_seconds");
    if (ResourceMap::HasKey("Ceres-num_threads"))
      options.num_threads = ResourceMap::GetAsUnsignedInteger("Ceres-num_threads");
    if (ResourceMap::HasKey("Ceres-initial_trust_region_radius"))
      options.initial_trust_region_radius = ResourceMap::GetAsScalar("Ceres-initial_trust_region_radius");
    if (ResourceMap::HasKey("Ceres-max_trust_region_radius"))
      options.max_trust_region_radius = ResourceMap::GetAsScalar("Ceres-max_trust_region_radius");
    if (ResourceMap::HasKey("Ceres-min_trust_region_radius"))
      options.min_trust_region_radius = ResourceMap::GetAsScalar("Ceres-min_trust_region_radius");
    if (ResourceMap::HasKey("Ceres-min_relative_decrease"))
      options.min_relative_decrease = ResourceMap::GetAsScalar("Ceres-min_relative_decrease");
    if (ResourceMap::HasKey("Ceres-min_lm_diagonal"))
      options.min_lm_diagonal = ResourceMap::GetAsScalar("Ceres-min_lm_diagonal");
    if (ResourceMap::HasKey("Ceres-max_lm_diagonal"))
      options.max_lm_diagonal = ResourceMap::GetAsScalar("Ceres-max_lm_diagonal");
    if (ResourceMap::HasKey("Ceres-max_num_consecutive_invalid_steps"))
      options.max_num_consecutive_invalid_steps = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_consecutive_invalid_steps");
    if (ResourceMap::HasKey("Ceres-function_tolerance"))
      options.function_tolerance = ResourceMap::GetAsScalar("Ceres-function_tolerance");
    if (ResourceMap::HasKey("Ceres-gradient_tolerance"))
      options.gradient_tolerance = ResourceMap::GetAsScalar("Ceres-gradient_tolerance");
    if (ResourceMap::HasKey("Ceres-parameter_tolerance"))
      options.parameter_tolerance = ResourceMap::GetAsScalar("Ceres-parameter_tolerance");
    if (ResourceMap::HasKey("Ceres-linear_solver_type") && !ceres::StringToLinearSolverType(ResourceMap::Get("Ceres-linear_solver_type"), &options.linear_solver_type))
      throw InvalidArgumentException(HERE) << "Invalid value for linear_solver_type";
    if (ResourceMap::HasKey("Ceres-preconditioner_type") && !ceres::StringToPreconditionerType(ResourceMap::Get("Ceres-preconditioner_type"), &options.preconditioner_type))
      throw InvalidArgumentException(HERE) << "Invalid value for preconditioner_type";
    if (ResourceMap::HasKey("Ceres-visibility_clustering_type") && !ceres::StringToVisibilityClusteringType(ResourceMap::Get("Ceres-visibility_clustering_type"), &options.visibility_clustering_type))
      throw InvalidArgumentException(HERE) << "Invalid value for visibility_clustering_type";
    if (ResourceMap::HasKey("Ceres-dense_linear_algebra_library_type") && !ceres::StringToDenseLinearAlgebraLibraryType(ResourceMap::Get("Ceres-dense_linear_algebra_library_type"), &options.dense_linear_algebra_library_type))
      throw InvalidArgumentException(HERE) << "Invalid value for dense_linear_algebra_library_type";
    if (ResourceMap::HasKey("Ceres-sparse_linear_algebra_library_type") && !ceres::StringToSparseLinearAlgebraLibraryType(ResourceMap::Get("Ceres-sparse_linear_algebra_library_type"), &options.sparse_linear_algebra_library_type))
      throw InvalidArgumentException(HERE) << "Invalid value for sparse_linear_algebra_library_type";
    if (ResourceMap::HasKey("Ceres-use_explicit_schur_complement"))
      options.use_explicit_schur_complement = ResourceMap::GetAsBool("Ceres-use_explicit_schur_complement");
    if (ResourceMap::HasKey("Ceres-use_postordering"))
      options.use_postordering = ResourceMap::GetAsBool("Ceres-use_postordering");
    if (ResourceMap::HasKey("Ceres-dynamic_sparsity"))
      options.dynamic_sparsity = ResourceMap::GetAsBool("Ceres-dynamic_sparsity");
    if (ResourceMap::HasKey("Ceres-min_linear_solver_iterations"))
      options.min_linear_solver_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-min_linear_solver_iterations");
    if (ResourceMap::HasKey("Ceres-max_linear_solver_iterations"))
      options.max_linear_solver_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-max_linear_solver_iterations");
    if (ResourceMap::HasKey("Ceres-eta"))
      options.eta = ResourceMap::GetAsScalar("Ceres-eta");
    if (ResourceMap::HasKey("Ceres-jacobi_scaling"))
      options.jacobi_scaling = ResourceMap::GetAsBool("Ceres-jacobi_scaling");
    if (ResourceMap::HasKey("Ceres-use_inner_iterations"))
      options.use_inner_iterations = ResourceMap::GetAsBool("Ceres-use_inner_iterations");
    if (ResourceMap::HasKey("Ceres-inner_iteration_tolerance"))
      options.inner_iteration_tolerance = ResourceMap::GetAsScalar("Ceres-inner_iteration_tolerance");
    // logging_type: https://github.com/ceres-solver/ceres-solver/issues/470
    options.logging_type = ceres::SILENT;
    if (ResourceMap::HasKey("Ceres-minimizer_progress_to_stdout"))
      options.minimizer_progress_to_stdout = ResourceMap::GetAsBool("Ceres-minimizer_progress_to_stdout");
    // trust_region_problem_dump_directory/trust_region_problem_dump_format_type: https://github.com/ceres-solver/ceres-solver/issues/470
    if (ResourceMap::HasKey("Ceres-check_gradients"))
      options.check_gradients = ResourceMap::GetAsBool("Ceres-check_gradients");
    if (ResourceMap::HasKey("Ceres-gradient_check_relative_precision"))
      options.gradient_check_relative_precision = ResourceMap::GetAsScalar("Ceres-gradient_check_relative_precision");
    if (ResourceMap::HasKey("Ceres-gradient_check_numeric_derivative_relative_step_size"))
      options.gradient_check_numeric_derivative_relative_step_size = ResourceMap::GetAsScalar("Ceres-gradient_check_numeric_derivative_relative_step_size");
    if (ResourceMap::HasKey("Ceres-update_state_every_iteration"))
      options.update_state_every_iteration = ResourceMap::GetAsBool("Ceres-update_state_every_iteration");

    Pointer<IterationCallbackInterface> p_iterationCallbackInterface = new IterationCallbackInterface(*this);
    options.callbacks.push_back(p_iterationCallbackInterface.get());
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
    LOGINFO(OSS() << summary.BriefReport());
    if (summary.termination_type == ceres::FAILURE)
      throw InternalException(HERE) << "Ceres terminated with failure.";
    else if (summary.termination_type != ceres::CONVERGENCE)
      LOGWARN(OSS() << "Ceres terminated with " << ceres::TerminationTypeToString(summary.termination_type));

    optimalValue = summary.final_cost;
    iterationNumber = summary.iterations.size();

  }
  else
  {
    // general optimization

    ceres::GradientProblemSolver::Options options;
    // check that algoName is a line search method
    if (!ceres::StringToLineSearchDirectionType(algoName_, &options.line_search_direction_type))
      throw InvalidArgumentException(HERE) << "Unconstrained optimization only allows line search methods";

    options.max_num_iterations = getMaximumIterationNumber();
    options.function_tolerance = getMaximumResidualError();
    options.parameter_tolerance = getMaximumRelativeError();

    // Set remaining options from ResourceMap
    if (ResourceMap::HasKey("Ceres-line_search_type") && !ceres::StringToLineSearchType(ResourceMap::Get("Ceres-line_search_type"), &options.line_search_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_type";
    if (ResourceMap::HasKey("Ceres-nonlinear_conjugate_gradient_type") && !ceres::StringToNonlinearConjugateGradientType(ResourceMap::Get("Ceres-nonlinear_conjugate_gradient_type"), &options.nonlinear_conjugate_gradient_type))
      throw InvalidArgumentException(HERE) << "Invalid value for nonlinear_conjugate_gradient_type";
    if (ResourceMap::HasKey("Ceres-max_lbfgs_rank"))
      options.max_lbfgs_rank = ResourceMap::GetAsUnsignedInteger("Ceres-max_lbfgs_rank");
    if (ResourceMap::HasKey("Ceres-use_approximate_eigenvalue_bfgs_scaling"))
      options.use_approximate_eigenvalue_bfgs_scaling = ResourceMap::GetAsBool("Ceres-use_approximate_eigenvalue_bfgs_scaling");
    if (ResourceMap::HasKey("Ceres-line_search_interpolation_type") && !ceres::StringToLineSearchInterpolationType(ResourceMap::Get("Ceres-line_search_interpolation_type"), &options.line_search_interpolation_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_interpolation_type";

    if (ResourceMap::HasKey("Ceres-min_line_search_step_size"))
      options.min_line_search_step_size = ResourceMap::GetAsScalar("Ceres-min_line_search_step_size");
    if (ResourceMap::HasKey("Ceres-line_search_sufficient_function_decrease"))
      options.line_search_sufficient_function_decrease = ResourceMap::GetAsScalar("Ceres-line_search_sufficient_function_decrease");
    if (ResourceMap::HasKey("Ceres-max_line_search_step_contraction"))
      options.max_line_search_step_contraction = ResourceMap::GetAsScalar("Ceres-max_line_search_step_contraction");
    if (ResourceMap::HasKey("Ceres-min_line_search_step_contraction"))
      options.min_line_search_step_contraction = ResourceMap::GetAsScalar("Ceres-min_line_search_step_contraction");
    if (ResourceMap::HasKey("Ceres-max_num_line_search_step_size_iterations"))
      options.max_num_line_search_step_size_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_line_search_step_size_iterations");
    if (ResourceMap::HasKey("Ceres-max_num_line_search_direction_restarts"))
      options.max_num_line_search_direction_restarts = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_line_search_direction_restarts");
    if (ResourceMap::HasKey("Ceres-line_search_sufficient_curvature_decrease"))
      options.line_search_sufficient_curvature_decrease = ResourceMap::GetAsScalar("Ceres-line_search_sufficient_curvature_decrease");
    if (ResourceMap::HasKey("Ceres-max_line_search_step_expansion"))
      options.max_line_search_step_expansion = ResourceMap::GetAsScalar("Ceres-max_line_search_step_expansion");
    if (ResourceMap::HasKey("Ceres-max_num_iterations"))
      options.max_num_iterations = ResourceMap::GetAsUnsignedInteger("Ceres-max_num_iterations");
    if (ResourceMap::HasKey("Ceres-max_solver_time_in_seconds"))
      options.max_solver_time_in_seconds = ResourceMap::GetAsScalar("Ceres-max_solver_time_in_seconds");
    if (ResourceMap::HasKey("Ceres-function_tolerance"))
      options.function_tolerance = ResourceMap::GetAsScalar("Ceres-function_tolerance");
    if (ResourceMap::HasKey("Ceres-gradient_tolerance"))
      options.gradient_tolerance = ResourceMap::GetAsScalar("Ceres-gradient_tolerance");
    if (ResourceMap::HasKey("Ceres-parameter_tolerance"))
      options.parameter_tolerance = ResourceMap::GetAsScalar("Ceres-parameter_tolerance");
    // logging_type: https://github.com/ceres-solver/ceres-solver/issues/470
    options.logging_type = ceres::SILENT;
    if (ResourceMap::HasKey("Ceres-minimizer_progress_to_stdout"))
      options.minimizer_progress_to_stdout = ResourceMap::GetAsBool("Ceres-minimizer_progress_to_stdout");

    Pointer<IterationCallbackInterface> p_iterationCallbackInterface = new IterationCallbackInterface(*this);
    options.callbacks.push_back(p_iterationCallbackInterface.get());
    ceres::GradientProblemSolver::Summary summary;
    ceres::GradientProblem problem(new FirstOrderFunctionInterface(*this));
    ceres::Solve(options, problem, &x[0], &summary);

    LOGINFO(OSS() << summary.BriefReport());
    if (summary.termination_type != ceres::CONVERGENCE)
      LOGWARN(OSS() << "Ceres terminated with " << ceres::TerminationTypeToString(summary.termination_type));

    optimalValue = getProblem().isMinimization() ? summary.final_cost : -summary.final_cost;
    iterationNumber = summary.iterations.size();
  }

  OptimizationResult result(getProblem());

  const UnsignedInteger size = evaluationInputHistory_.getSize();

  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(evaluationInputHistory_[i]);
    const Point outP(evaluationOutputHistory_[i]);
    constraintError = 0.0;
    if (getProblem().hasBounds())
    {
      const Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        if (bounds.getFiniteLowerBound()[j])
          constraintError = std::max(constraintError, bounds.getLowerBound()[j] - inP[j]);
        if (bounds.getFiniteUpperBound()[j])
          constraintError = std::max(constraintError, inP[j] - bounds.getUpperBound()[j]);
      }
    }
    if (i > 0)
    {
      const Point inPM(evaluationInputHistory_[i - 1]);
      const Point outPM(evaluationOutputHistory_[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }
    result.store(inP, outP, absoluteError, relativeError, residualError, constraintError);
  }

  result.setEvaluationNumber(size);
  result.setIterationNumber(iterationNumber);
  result.setOptimalPoint(x);
  result.setOptimalValue(Point(1, optimalValue));
  setResult(result);
#else
  throw NotYetImplementedException(HERE) << "No Ceres support";
#endif
}

/* String converter */
String Ceres::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}

/* String converter */
String Ceres::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << getClassName();
  return oss;
}

void Ceres::setAlgorithmName(const String algoName)
{
  algoName_ = algoName;
}

String Ceres::getAlgorithmName() const
{
  return algoName_;
}

/* Method save() stores the object through the StorageManager */
void Ceres::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
}

/* Method load() reloads the object from the StorageManager */
void Ceres::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algoName_", algoName_);
}

Bool Ceres::IsAvailable()
{
#ifdef OPENTURNS_HAVE_CERES
  return true;
#else
  return false;
#endif
}

void Ceres::Initialize()
{
#ifdef OPENTURNS_HAVE_CERES
  google::InitGoogleLogging("openturns");
#endif
}

END_NAMESPACE_OPENTURNS

