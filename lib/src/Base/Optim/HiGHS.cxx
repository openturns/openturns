//                                               -*- C++ -*-
/**
 *  @brief HiGHS linear solver
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HiGHS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_HAVE_HIGHS
#include <Highs.h>
#endif

#include <algorithm>
#include <chrono>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HiGHS)

static const Factory<HiGHS> Factory_HiGHS;

/* Constructor with no parameters */
HiGHS::HiGHS(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , algoName_()
{
  setAlgorithmName(algoName);
}


/* Constructor with parameters */
HiGHS::HiGHS(const OptimizationProblem & problem,
             const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
  , algoName_()

{
  setAlgorithmName(algoName);
}


/* Algorithm names accessor */
Description HiGHS::GetAlgorithmNames()
{
  static const Description algoNames({"choose", "simplex", "ipm", "ipx", "hipo", "pdlp", "hipdlp"});
  return algoNames;
}


/* Algorithm name accessor */
void HiGHS::setAlgorithmName(const String & algoName)
{
  if (!GetAlgorithmNames().contains(algoName))
    throw InvalidArgumentException(HERE) << "Unknown HiGHS solver " << algoName << ", expected one of " << GetAlgorithmNames();
  const String previousAlgoName(algoName_);
  algoName_ = algoName;
  try
  {
    if (getProblem().getDimension() > 0)
      checkProblem(getProblem());
  }
  catch (...)
  {
    algoName_ = previousAlgoName;
    throw;
  }
}

String HiGHS::getAlgorithmName() const
{
  return algoName_;
}


/* Dual solution accessors (from last run) */
Point HiGHS::getDualPoint() const
{
  if (getResult().getStatusMessage().empty())
    throw InvalidArgumentException(HERE) << "No solution available, run the solver first";
  if (dualPoint_.getDimension() == 0)
    throw InvalidArgumentException(HERE) << "No dual solution available (only continuous problems with a valid dual solution provide one)";
  return dualPoint_;
}

Point HiGHS::getReducedCosts() const
{
  if (getResult().getStatusMessage().empty())
    throw InvalidArgumentException(HERE) << "No solution available, run the solver first";
  if (reducedCosts_.getDimension() == 0)
    throw InvalidArgumentException(HERE) << "No dual solution available (only continuous problems with a valid dual solution provide one)";
  return reducedCosts_;
}

Point HiGHS::getConstraintValues() const
{
  if (getResult().getStatusMessage().empty())
    throw InvalidArgumentException(HERE) << "No solution available, run the solver first";
  if (constraintValues_.getDimension() == 0)
    throw InvalidArgumentException(HERE) << "No constraint values available (only runs with linear constraints and a valid primal solution provide them)";
  return constraintValues_;
}


/* Check whether this problem can be solved by this solver */
void HiGHS::checkProblem(const OptimizationProblem & problem) const
{
  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction() || problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "HiGHS does not support multi-objective / least squares / nearest point problems";

  if (!problem.isLinear())
    throw InvalidArgumentException(HERE) << "HiGHS does not support non linear problems";

  // Discrete problems are solved by the MIP solver, which ignores the LP solver choice
  if (!problem.isContinuous() && (algoName_ != "choose"))
    throw InvalidArgumentException(HERE) << "HiGHS solver " << algoName_ << " only applies to continuous LP, use \"choose\" on problems with discrete variables";
}


void HiGHS::run()
{
#ifdef OPENTURNS_HAVE_HIGHS
  const UnsignedInteger problemDimension = getProblem().getDimension();
  if (problemDimension == 0) throw InvalidArgumentException(HERE) << "No problem has been set.";
  if (!GetAlgorithmNames().contains(algoName_))
    throw InvalidArgumentException(HERE) << "Unknown HiGHS solver " << algoName_;
  checkProblem(getProblem());
  result_ = OptimizationResult(getProblem());
  // clear cached outputs from a previous run
  dualPoint_.clear();
  reducedCosts_.clear();
  constraintValues_.clear();
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  const UnsignedInteger initialCallsNumber = getProblem().getObjective().getCallsNumber();

  HighsModel model;
  model.lp_.num_col_ = problemDimension;
  model.lp_.num_row_ = getProblem().getLinearConstraintCoefficients().getNbRows();

  // objective function
  model.lp_.sense_ = getProblem().isMinimization() ? ObjSense::kMinimize : ObjSense::kMaximize;
  model.lp_.offset_ = 0.0;
  model.lp_.col_cost_.resize(model.lp_.num_col_);
  for (UnsignedInteger i = 0; i < problemDimension; ++ i)
    model.lp_.col_cost_[i] = getProblem().getLinearCost()[i];

  // bound constraints
  model.lp_.col_lower_ = std::vector<double>(model.lp_.num_col_, -SpecFunc::MaxScalar);
  model.lp_.col_upper_ = std::vector<double>(model.lp_.num_col_, SpecFunc::MaxScalar);
  if (getProblem().hasBounds())
    for (int col = 0; col < model.lp_.num_col_; ++ col)
    {
      if (getProblem().getBounds().getFiniteLowerBound()[col])
        model.lp_.col_lower_[col] = getProblem().getBounds().getLowerBound()[col];
      if (getProblem().getBounds().getFiniteUpperBound()[col])
        model.lp_.col_upper_[col] = getProblem().getBounds().getUpperBound()[col];
    }

  // variable types
  model.lp_.integrality_.resize(model.lp_.num_col_);
  for (int col = 0; col < model.lp_.num_col_; ++ col)
    if (getProblem().getVariablesType()[col] == OptimizationProblemImplementation::CONTINUOUS)
      model.lp_.integrality_[col] = HighsVarType::kContinuous;
    else // INTEGER or BINARY
    {
      model.lp_.integrality_[col] = HighsVarType::kInteger;
      if (getProblem().getVariablesType()[col] == OptimizationProblemImplementation::BINARY)
      {
        model.lp_.col_lower_[col] = 0;
        model.lp_.col_upper_[col] = 1;
      }
    }

  // linear constraints, stored sparse column-wise (exact zeros skipped)
  if (model.lp_.num_row_ > 0)
  {
    const Matrix constraintCoefficients(getProblem().getLinearConstraintCoefficients());
    const UnsignedInteger nbRows = static_cast<UnsignedInteger>(model.lp_.num_row_);
    model.lp_.a_matrix_.format_ = MatrixFormat::kColwise;
    model.lp_.a_matrix_.start_.resize(model.lp_.num_col_ + 1);
    model.lp_.a_matrix_.start_[0] = 0;
    for (UnsignedInteger col = 0; col < problemDimension; ++ col)
    {
      for (UnsignedInteger row = 0; row < nbRows; ++ row)
      {
        const Scalar coefficient = constraintCoefficients(row, col);
        if (coefficient != 0.0)
        {
          model.lp_.a_matrix_.index_.push_back(static_cast<HighsInt>(row));
          model.lp_.a_matrix_.value_.push_back(coefficient);
        }
      }
      model.lp_.a_matrix_.start_[col + 1] = static_cast<HighsInt>(model.lp_.a_matrix_.index_.size());
    }
    model.lp_.row_lower_.resize(model.lp_.num_row_);
    model.lp_.row_upper_.resize(model.lp_.num_row_);
    for (int row = 0; row < model.lp_.num_row_; ++ row)
    {
      model.lp_.row_lower_[row] = -SpecFunc::MaxScalar;
      model.lp_.row_upper_[row] = SpecFunc::MaxScalar;
      if (getProblem().getLinearConstraintBounds().getFiniteLowerBound()[row])
        model.lp_.row_lower_[row] = getProblem().getLinearConstraintBounds().getLowerBound()[row] - getMaximumConstraintError();
      if (getProblem().getLinearConstraintBounds().getFiniteUpperBound()[row])
        model.lp_.row_upper_[row] = getProblem().getLinearConstraintBounds().getUpperBound()[row] + getMaximumConstraintError();
    }
  }

  // Create a Highs instance
  Highs highs;
  highs.setOptionValue("output_flag", Log::HasDebug());
  if (getMaximumTimeDuration() > 0.0)
    highs.setOptionValue("time_limit", getMaximumTimeDuration());
  highs.setOptionValue("simplex_iteration_limit", static_cast<HighsInt>(getMaximumIterationNumber()));
  highs.setOptionValue("ipm_iteration_limit", static_cast<HighsInt>(getMaximumIterationNumber()));
  highs.setOptionValue("pdlp_iteration_limit", static_cast<HighsInt>(getMaximumIterationNumber()));
  highs.setOptionValue("qp_iteration_limit", static_cast<HighsInt>(getMaximumIterationNumber()));
  // solver selected per instance; a HiGHS-solver ResourceMap key (if any) overrides it below
  if (highs.setOptionValue("solver", algoName_) != HighsStatus::kOk)
    throw InvalidArgumentException(HERE) << "HiGHS solver " << algoName_ << " is not supported by this HiGHS version";

  // pass options from ResourceMap
  std::vector<String> keys(ResourceMap::GetKeys());
  const UnsignedInteger nbKeys = keys.size();
  for (UnsignedInteger i = 0; i < nbKeys; ++i)
    if (keys[i].substr(0, 6) == "HiGHS-")
    {
      const String optionName(keys[i].substr(6));
      const String type(ResourceMap::GetType(keys[i]));
      HighsStatus status = HighsStatus::kOk;
      if (type == "str")
        status = highs.setOptionValue(optionName, ResourceMap::GetAsString(keys[i]));
      else if (type == "float")
        status = highs.setOptionValue(optionName, ResourceMap::GetAsScalar(keys[i]));
      else if (type == "int")
        status = highs.setOptionValue(optionName, static_cast<HighsInt>(ResourceMap::GetAsUnsignedInteger(keys[i])));
      else if (type == "bool")
        status = highs.setOptionValue(optionName, ResourceMap::GetAsBool(keys[i]));
      if (status != HighsStatus::kOk)
        throw InvalidArgumentException(HERE) << "Invalid HiGHS option " << optionName;
    }

  HighsStatus return_status = highs.passModel(model);
  if (return_status == HighsStatus::kError)
    throw InvalidArgumentException(HERE) << "Cannot initialize highs model: " << highsStatusToString(return_status) ;

  // Solve the model
  return_status = highs.run();
  if (return_status == HighsStatus::kError)
    throw InternalException(HERE) << "HiGHS solve failed: " << highsStatusToString(return_status);

  // Get the model status
  const HighsModelStatus model_status = highs.getModelStatus();
  const String modelStatus(highs.modelStatusToString(model_status));
  switch (model_status)
  {
    case HighsModelStatus::kOptimal:
      result_.setStatus(OptimizationResult::SUCCESS);
      break;
    case HighsModelStatus::kTimeLimit:
      result_.setStatus(OptimizationResult::TIMEOUT);
      break;
    case HighsModelStatus::kInterrupt:
      result_.setStatus(OptimizationResult::INTERRUPTION);
      break;
    case HighsModelStatus::kIterationLimit:
    case HighsModelStatus::kSolutionLimit:
      result_.setStatus(OptimizationResult::MAXIMUMCALLS);
      break;
    default:
      result_.setStatus(OptimizationResult::FAILURE);
      break;
  }
  result_.setStatusMessage(modelStatus);

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
  result_.setTimeDuration(timeDuration);

  // Get the solution information
  const HighsInfo & info = highs.getInfo();
  const HighsSolution & solution = highs.getSolution();
  // cache duals when available (continuous LP only, never for MIP or PDLP without crossover)
  if (solution.dual_valid)
  {
    dualPoint_ = Point(solution.row_dual.begin(), solution.row_dual.end());
    reducedCosts_ = Point(solution.col_dual.begin(), solution.col_dual.end());
  }
  if (solution.value_valid && !solution.row_value.empty())
    constraintValues_ = Point(solution.row_value.begin(), solution.row_value.end());

  if (solution.value_valid)
  {
    Point optimalPoint(problemDimension);
    for (UnsignedInteger col = 0; col < problemDimension; ++ col)
      optimalPoint[col] = solution.col_value[col];
    result_.setOptimalPoint(optimalPoint);
    result_.setOptimalValue(Point({info.objective_function_value}));
  }
  const UnsignedInteger callsNumber = getProblem().getObjective().getCallsNumber() - initialCallsNumber;
  result_.setCallsNumber(callsNumber);
  // unused solver iteration counts are set to -1 by HiGHS: clamp each to 0
  const HighsInt simplexIterations = std::max<HighsInt>(info.simplex_iteration_count, 0);
  const HighsInt ipmIterations = std::max<HighsInt>(info.ipm_iteration_count, 0);
  const HighsInt pdlpIterations = std::max<HighsInt>(info.pdlp_iteration_count, 0);
  const HighsInt qpIterations = std::max<HighsInt>(info.qp_iteration_count, 0);
  result_.setIterationNumber(simplexIterations + ipmIterations + pdlpIterations + qpIterations);

  if (result_.getStatus() != OptimizationResult::SUCCESS)
  {
    if (getCheckStatus())
      throw InternalException(HERE) << "HiGHS failed (" << modelStatus << ")";
    LOGWARN(OSS() << "HiGHS failed (" << modelStatus << ")");
  }

#else
  throw NotYetImplementedException(HERE) << "No HiGHS support";
#endif
}


/* Virtual constructor */
HiGHS * HiGHS::clone() const
{
  return new HiGHS(*this);
}

/* String converter */
String HiGHS::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " solver=" << algoName_
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}


/* String converter */
String HiGHS::__str__(const String &) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << " solver=" << algoName_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void HiGHS::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
  adv.saveAttribute("dualPoint_", dualPoint_);
  adv.saveAttribute("reducedCosts_", reducedCosts_);
  adv.saveAttribute("constraintValues_", constraintValues_);
}

/* Method load() reloads the object from the StorageManager */
void HiGHS::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  if (adv.hasAttribute("algoName_"))
    adv.loadAttribute("algoName_", algoName_);
  if (adv.hasAttribute("dualPoint_"))
    adv.loadAttribute("dualPoint_", dualPoint_);
  if (adv.hasAttribute("reducedCosts_"))
    adv.loadAttribute("reducedCosts_", reducedCosts_);
  if (adv.hasAttribute("constraintValues_"))
    adv.loadAttribute("constraintValues_", constraintValues_);
}

END_NAMESPACE_OPENTURNS
