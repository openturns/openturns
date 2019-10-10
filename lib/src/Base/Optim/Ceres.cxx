//                                               -*- C++ -*-
/**
 *  @brief Ceres solver
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
    throw InvalidArgumentException(HERE) << "Error: Ceres line search algorithms do not support bound constraints";

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
    *mutable_parameter_block_sizes() = std::vector<ceres::int32>(1, problem.getDimension()); 
    set_num_residuals(problem.getResidualFunction().getOutputDimension());
  }

  virtual bool Evaluate(double const* const* parameters,
                        double* residuals,
                        double** jacobians) const
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
      std::copy(&gradient(0, 0), &gradient(n-1, m-1)+1, jacobians[0]);
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

  virtual int NumParameters() const
  {
    return algorithm_.getProblem().getDimension();
  }

  virtual bool Evaluate(const double * const x,
                        double * cost,
                        double * jacobian) const
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
      std::copy(&gradient(0, 0), &gradient(n-1, 0)+1, jacobian);
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
  Ceres & algorithm_;
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
        if (finiteLowerBound[i]) problem.SetParameterLowerBound(&x[0], i, lowerBound[i]);
        if (finiteUpperBound[i]) problem.SetParameterUpperBound(&x[0], i, upperBound[i]);
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
    if (options_.hasKey("line_search_type") && !ceres::StringToLineSearchType(options_.get("line_search_type"), &options.line_search_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_type";
    if (options_.hasKey("nonlinear_conjugate_gradient_type") && !ceres::StringToNonlinearConjugateGradientType(options_.get("nonlinear_conjugate_gradient_type"), &options.nonlinear_conjugate_gradient_type))
      throw InvalidArgumentException(HERE) << "Invalid value for nonlinear_conjugate_gradient_type";
    
    if (options_.hasKey("max_lbfgs_rank"))
      options.max_lbfgs_rank = options_.getAsUnsignedInteger("max_lbfgs_rank");
    
    if (options_.hasKey("use_approximate_eigenvalue_bfgs_scaling"))
      options.use_approximate_eigenvalue_bfgs_scaling = options_.getAsBool("use_approximate_eigenvalue_bfgs_scaling");
    
    if (options_.hasKey("line_search_interpolation_type") && !ceres::StringToLineSearchInterpolationType(options_.get("line_search_interpolation_type"), &options.line_search_interpolation_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_interpolation_type";
    
    if (options_.hasKey("min_line_search_step_size"))
      options.min_line_search_step_size = options_.getAsScalar("min_line_search_step_size");
    
    if (options_.hasKey("line_search_sufficient_function_decrease"))
      options.line_search_sufficient_function_decrease = options_.getAsScalar("line_search_sufficient_function_decrease");
    
    if (options_.hasKey("max_line_search_step_contraction"))
      options.max_line_search_step_contraction = options_.getAsScalar("max_line_search_step_contraction");
    
    if (options_.hasKey("min_line_search_step_contraction"))
      options.min_line_search_step_contraction = options_.getAsScalar("min_line_search_step_contraction");
    
    if (options_.hasKey("max_num_line_search_step_size_iterations"))
      options.max_num_line_search_step_size_iterations = options_.getAsUnsignedInteger("max_num_line_search_step_size_iterations");
    
    if (options_.hasKey("max_num_line_search_direction_restarts"))
      options.max_num_line_search_direction_restarts = options_.getAsUnsignedInteger("max_num_line_search_direction_restarts");
    
    if (options_.hasKey("line_search_sufficient_curvature_decrease"))
      options.line_search_sufficient_curvature_decrease = options_.getAsScalar("line_search_sufficient_curvature_decrease");
    
    if (options_.hasKey("max_line_search_step_expansion"))
      options.max_line_search_step_expansion = options_.getAsScalar("max_line_search_step_expansion");
    
    if (options_.hasKey("dogleg_type") && !ceres::StringToDoglegType(options_.get("dogleg_type"), &options.dogleg_type))
      throw InvalidArgumentException(HERE) << "Invalid value for dogleg_type";
    
    if (options_.hasKey("use_nonmonotonic_steps"))
      options.use_nonmonotonic_steps = options_.getAsBool("use_nonmonotonic_steps");
    
    if (options_.hasKey("max_consecutive_nonmonotonic_steps"))
      options.max_consecutive_nonmonotonic_steps = options_.getAsUnsignedInteger("max_consecutive_nonmonotonic_steps");
    
    if (options_.hasKey("max_num_iterations"))
      options.max_num_iterations = options_.getAsUnsignedInteger("max_num_iterations");
    
    if (options_.hasKey("max_solver_time_in_seconds"))
      options.max_solver_time_in_seconds = options_.getAsScalar("max_solver_time_in_seconds");
    
    if (options_.hasKey("num_threads"))
      options.num_threads = options_.getAsUnsignedInteger("num_threads");
    
    if (options_.hasKey("initial_trust_region_radius"))
      options.initial_trust_region_radius = options_.getAsScalar("initial_trust_region_radius");
    
    if (options_.hasKey("max_trust_region_radius"))
      options.max_trust_region_radius = options_.getAsScalar("max_trust_region_radius");
    
    if (options_.hasKey("min_trust_region_radius"))
      options.min_trust_region_radius = options_.getAsScalar("min_trust_region_radius");
    
    if (options_.hasKey("min_relative_decrease"))
      options.min_relative_decrease = options_.getAsScalar("min_relative_decrease");
    
    if (options_.hasKey("min_lm_diagonal"))
      options.min_lm_diagonal = options_.getAsScalar("min_lm_diagonal");
    
    if (options_.hasKey("max_lm_diagonal"))
      options.max_lm_diagonal = options_.getAsScalar("max_lm_diagonal");
    
    if (options_.hasKey("max_num_consecutive_invalid_steps"))
      options.max_num_consecutive_invalid_steps = options_.getAsUnsignedInteger("max_num_consecutive_invalid_steps");
    
    if (options_.hasKey("function_tolerance"))
      options.function_tolerance = options_.getAsScalar("function_tolerance");
    
    if (options_.hasKey("gradient_tolerance"))
      options.gradient_tolerance = options_.getAsScalar("gradient_tolerance");
    
    if (options_.hasKey("parameter_tolerance"))
      options.parameter_tolerance = options_.getAsScalar("parameter_tolerance");
    
    if (options_.hasKey("linear_solver_type") && !ceres::StringToLinearSolverType(options_.get("linear_solver_type"), &options.linear_solver_type))
      throw InvalidArgumentException(HERE) << "Invalid value for linear_solver_type";
    
    if (options_.hasKey("preconditioner_type") && !ceres::StringToPreconditionerType(options_.get("preconditioner_type"), &options.preconditioner_type))
      throw InvalidArgumentException(HERE) << "Invalid value for preconditioner_type";
    
    if (options_.hasKey("visibility_clustering_type") && !ceres::StringToVisibilityClusteringType(options_.get("visibility_clustering_type"), &options.visibility_clustering_type))
      throw InvalidArgumentException(HERE) << "Invalid value for visibility_clustering_type";
    
    if (options_.hasKey("dense_linear_algebra_library_type") && !ceres::StringToDenseLinearAlgebraLibraryType(options_.get("dense_linear_algebra_library_type"), &options.dense_linear_algebra_library_type))
      throw InvalidArgumentException(HERE) << "Invalid value for dense_linear_algebra_library_type";
    
    if (options_.hasKey("sparse_linear_algebra_library_type") && !ceres::StringToSparseLinearAlgebraLibraryType(options_.get("sparse_linear_algebra_library_type"), &options.sparse_linear_algebra_library_type))
      throw InvalidArgumentException(HERE) << "Invalid value for sparse_linear_algebra_library_type";
    
    if (options_.hasKey("num_linear_solver_threads"))
      options.num_linear_solver_threads = options_.getAsUnsignedInteger("num_linear_solver_threads");
    
    if (options_.hasKey("use_explicit_schur_complement"))
      options.use_explicit_schur_complement = options_.getAsBool("use_explicit_schur_complement");
    
    if (options_.hasKey("use_postordering"))
      options.use_postordering = options_.getAsBool("use_postordering");
    
    if (options_.hasKey("dynamic_sparsity"))
      options.dynamic_sparsity = options_.getAsBool("dynamic_sparsity");
    
    if (options_.hasKey("min_linear_solver_iterations"))
        options.min_linear_solver_iterations = options_.getAsUnsignedInteger("min_linear_solver_iterations");
    
    if (options_.hasKey("max_linear_solver_iterations"))
        options.max_linear_solver_iterations = options_.getAsUnsignedInteger("max_linear_solver_iterations");
    
    if (options_.hasKey("eta"))
      options.eta = options_.getAsScalar("eta");
    
    if (options_.hasKey("jacobi_scaling"))
      options.jacobi_scaling = options_.getAsBool("jacobi_scaling");
    
    if (options_.hasKey("use_inner_iterations"))
      options.use_inner_iterations = options_.getAsBool("use_inner_iterations");
    
    if (options_.hasKey("inner_iteration_tolerance"))
      options.inner_iteration_tolerance = options_.getAsScalar("inner_iteration_tolerance");

    // logging_type: https://github.com/ceres-solver/ceres-solver/issues/470
    options.logging_type = ceres::SILENT;
    
    if (options_.hasKey("minimizer_progress_to_stdout"))
      options.minimizer_progress_to_stdout = options_.getAsBool("minimizer_progress_to_stdout");
    
    // trust_region_problem_dump_directory/trust_region_problem_dump_format_type: https://github.com/ceres-solver/ceres-solver/issues/470
    if (options_.hasKey("check_gradients"))
      options.check_gradients = options_.getAsBool("check_gradients");
    
    if (options_.hasKey("gradient_check_relative_precision"))
      options.gradient_check_relative_precision = options_.getAsScalar("gradient_check_relative_precision");
    
    if (options_.hasKey("gradient_check_numeric_derivative_relative_step_size"))
      options.gradient_check_numeric_derivative_relative_step_size = options_.getAsScalar("gradient_check_numeric_derivative_relative_step_size");
    
    if (options_.hasKey("update_state_every_iteration"))
      options.update_state_every_iteration = options_.getAsBool("update_state_every_iteration");

    options.callbacks.push_back(new IterationCallbackInterface(*this));
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
      LOGWARN("Unconstrained optimization only allows line search methods, using default line search");

    options.max_num_iterations = getMaximumIterationNumber();
    options.function_tolerance = getMaximumResidualError();
    options.parameter_tolerance = getMaximumRelativeError();

    // Set remaining options from ResourceMap
    if (hasOption("line_search_type") && !ceres::StringToLineSearchType(options_.get("line_search_type"), &options.line_search_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_type";
    
    if (hasOption("nonlinear_conjugate_gradient_type") && !ceres::StringToNonlinearConjugateGradientType(options_.get("nonlinear_conjugate_gradient_type"), &options.nonlinear_conjugate_gradient_type))
      throw InvalidArgumentException(HERE) << "Invalid value for nonlinear_conjugate_gradient_type";
    
    if (options_.hasKey("max_lbfgs_rank"))
      options.max_lbfgs_rank = options_.getAsUnsignedInteger("max_lbfgs_rank");
    
    if (options_.hasKey("use_approximate_eigenvalue_bfgs_scaling"))
      options.use_approximate_eigenvalue_bfgs_scaling = options_.getAsBool("use_approximate_eigenvalue_bfgs_scaling");
    
    if (options_.hasKey("line_search_interpolation_type") && !ceres::StringToLineSearchInterpolationType(options_.get("line_search_interpolation_type"), &options.line_search_interpolation_type))
      throw InvalidArgumentException(HERE) << "Invalid value for line_search_interpolation_type";

    if (options_.hasKey("min_line_search_step_size"))
      options.min_line_search_step_size = options_.getAsScalar("min_line_search_step_size");
    
    if (options_.hasKey("line_search_sufficient_function_decrease"))
      options.line_search_sufficient_function_decrease = options_.getAsScalar("line_search_sufficient_function_decrease");
    
    if (options_.hasKey("max_line_search_step_contraction"))
      options.max_line_search_step_contraction = options_.getAsScalar("max_line_search_step_contraction");
    
    if (options_.hasKey("min_line_search_step_contraction"))
      options.min_line_search_step_contraction = options_.getAsScalar("min_line_search_step_contraction");
    
    if (options_.hasKey("max_num_line_search_step_size_iterations"))
      options.max_num_line_search_step_size_iterations = options_.getAsUnsignedInteger("max_num_line_search_step_size_iterations");
    
    if (options_.hasKey("max_num_line_search_direction_restarts"))
      options.max_num_line_search_direction_restarts = options_.getAsUnsignedInteger("max_num_line_search_direction_restarts");
    
    if (options_.hasKey("line_search_sufficient_curvature_decrease"))
      options.line_search_sufficient_curvature_decrease = options_.getAsScalar("line_search_sufficient_curvature_decrease");
    
    if (options_.hasKey("max_line_search_step_expansion"))
      options.max_line_search_step_expansion = options_.getAsScalar("max_line_search_step_expansion");
    
    if (options_.hasKey("max_num_iterations"))
      options.max_num_iterations = options_.getAsUnsignedInteger("max_num_iterations");
    
    if (options_.hasKey("max_solver_time_in_seconds"))
      options.max_solver_time_in_seconds = options_.getAsScalar("max_solver_time_in_seconds");
    
    if (options_.hasKey("function_tolerance"))
      options.function_tolerance = options_.getAsScalar("function_tolerance");
    
    if (options_.hasKey("gradient_tolerance"))
      options.gradient_tolerance = options_.getAsScalar("gradient_tolerance");
    
    if (options_.hasKey("parameter_tolerance"))
      options.parameter_tolerance = options_.getAsScalar("parameter_tolerance");
    
    // logging_type: https://github.com/ceres-solver/ceres-solver/issues/470
    options.logging_type = ceres::SILENT;
    
    if (options_.hasKey("minimizer_progress_to_stdout"))
      options.minimizer_progress_to_stdout = options_.getAsBool("minimizer_progress_to_stdout");

    options.callbacks.push_back(new IterationCallbackInterface(*this));
    ceres::GradientProblemSolver::Summary summary;
    ceres::GradientProblem problem(new FirstOrderFunctionInterface(*this));
    ceres::Solve(options, problem, &x[0], &summary);

    LOGINFO(OSS() << summary.BriefReport());
    if (summary.termination_type != ceres::CONVERGENCE)
      LOGWARN(OSS() << "Ceres terminated with " << ceres::TerminationTypeToString(summary.termination_type));

    optimalValue = getProblem().isMinimization() ? summary.final_cost : -summary.final_cost;
    iterationNumber = summary.iterations.size();
  }

  OptimizationResult result(dimension, 1);
  result.setProblem(getProblem());

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
  result.setLagrangeMultipliers(computeLagrangeMultipliers(x));
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

/** Options accessors */
void Ceres::setOptionAsString(const String & key, const String & value)
{
  options_.setAsString(key, value);
}
  
String Ceres::getOptionAsString(const String & key)
{
  return options_.getAsString(key);
}

void Ceres::setOptionAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  options_.setAsUnsignedInteger(key, value);
}

UnsignedInteger Ceres::getOptionAsUnsignedInteger(const String & key)
{
  return options_.getAsUnsignedInteger(key);
}

void Ceres::setOptionAsScalar(const String & key, const Scalar value)
{
  options_.setAsScalar(key, value);
}

Scalar Ceres::getOptionAsScalar(const String & key)
{
  return options_.getAsScalar(key);
}

void Ceres::setOptionAsBool(const String & key, const Bool value)
{
  options_.setAsBool(key, value);
}

Bool Ceres::getOptionAsBool(const String & key)
{
  return options_.getAsBool(key);
}

Bool Ceres::hasOption(const String & key)
{
  return options_.hasKey(key);
}

String Ceres::getOptionType(const String & key)
{
  return options_.getType(key);
}

String Ceres::getOption(const String & key)
{
  return options_.get(key);
}

void Ceres::removeOption(const String & key)
{
  options_.removeKey(key);
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

END_NAMESPACE_OPENTURNS

