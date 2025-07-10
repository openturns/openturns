//                                               -*- C++ -*-
/**
 *  @brief HiGHS linear solver
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_HAVE_HIGHS
#include "Highs.h"
#endif

#include <chrono>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HiGHS)

static const Factory<HiGHS> Factory_HiGHS;

/* Constructor with no parameters */
HiGHS::HiGHS()
  : OptimizationAlgorithmImplementation()
{
  // Nothing to do here
}


/* Constructor that sets starting sample */
HiGHS::HiGHS(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)

{
  // Nothing to do here
}


/* Check whether this problem can be solved by this solver */
void HiGHS::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective()) throw InvalidArgumentException(HERE) << "HiGHS does not support multi-objective optimization";

  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "HiGHS does not support least squares or nearest point problems";

  if (problem.hasEqualityConstraint())
    throw InvalidArgumentException(HERE) << "HiGHS does not support equality constraints";
}


void HiGHS::run()
{
#ifdef OPENTURNS_HAVE_HIGHS
  const UnsignedInteger problemDimension = getProblem().getDimension();
  if (problemDimension == 0) throw InvalidArgumentException(HERE) << "No problem has been set.";
  result_ = OptimizationResult(getProblem());
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  const UnsignedInteger initialCallsNumber = getProblem().getObjective().getCallsNumber();

  HighsModel model;
  model.lp_.num_col_ = problemDimension;
  model.lp_.num_row_ = getProblem().getInequalityConstraint().getOutputDimension();

  // objective function
  const Point origin(problemDimension, 0.0);
  model.lp_.sense_ = getProblem().isMinimization() ? ObjSense::kMinimize : ObjSense::kMaximize;
  const Scalar f0 = getProblem().getObjective()(origin)[0];
  model.lp_.offset_ = f0;
  const Matrix grad0(getProblem().getObjective().gradient((origin)));
  model.lp_.col_cost_.resize(model.lp_.num_col_);
  for (UnsignedInteger i = 0; i < problemDimension; ++ i)
    model.lp_.col_cost_[i] = grad0(i, 0);

  // bound constraints
  model.lp_.col_lower_ = std::vector<double>(model.lp_.num_col_, -SpecFunc::MaxScalar);
  model.lp_.col_upper_ = std::vector<double>(model.lp_.num_col_, SpecFunc::MaxScalar);
  if (getProblem().hasBounds())
    for (int col = 0; col < model.lp_.num_col_; ++ col)
    {
      model.lp_.col_lower_[col] = getProblem().getBounds().getLowerBound()[col];
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

  // inequality constraints
  if (getProblem().hasInequalityConstraint())
  {
    model.lp_.a_matrix_.format_ = MatrixFormat::kColwise;
    model.lp_.a_matrix_.start_.resize(model.lp_.num_col_ + 1);
    for (int col = 0; col <= model.lp_.num_col_; ++ col)
      model.lp_.a_matrix_.start_[col] = col * model.lp_.num_row_;
    model.lp_.a_matrix_.index_.resize(model.lp_.num_col_ * model.lp_.num_row_);
    model.lp_.a_matrix_.value_.resize(model.lp_.num_col_ * model.lp_.num_row_);
    const Matrix ineqGrad0(getProblem().getInequalityConstraint().gradient((origin)));
    for (int col = 0; col < model.lp_.num_col_; ++ col)
      for (int row = 0; row < model.lp_.num_row_; ++ row)
      {
        model.lp_.a_matrix_.index_[col * model.lp_.num_row_ + row] = row;
        model.lp_.a_matrix_.value_[col * model.lp_.num_row_ + row] = ineqGrad0(col, row);
      }
    model.lp_.row_lower_.resize(model.lp_.num_row_);
    model.lp_.row_upper_.resize(model.lp_.num_row_);
    const Point ineq0(getProblem().getInequalityConstraint()(origin));
    for (int row = 0; row < model.lp_.num_row_; ++ row)
    {
      model.lp_.row_lower_[row] = -ineq0[row];
      model.lp_.row_upper_[row] = SpecFunc::MaxScalar;
    }
  }

  // Create a Highs instance
  Highs highs;
  highs.setOptionValue("output_flag", false);
  if (getMaximumTimeDuration() > 0.0)
    highs.setOptionValue("time_limit", getMaximumTimeDuration());

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
  if (return_status != HighsStatus::kOk)
    throw InvalidArgumentException(HERE) << "Cannot initialize highs model";

  // Solve the model
  return_status = highs.run();
  if (return_status != HighsStatus::kOk)
    throw InvalidArgumentException(HERE) << "Solve not ok";

  // Get the model status
  const HighsModelStatus& model_status = highs.getModelStatus();
  if (model_status != HighsModelStatus::kOptimal)
    throw InvalidArgumentException(HERE) << "Solve not optimal";

  std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
  result_.setTimeDuration(timeDuration);

  // Get the solution information
  const HighsInfo& info = highs.getInfo();
  if (!info.primal_solution_status)
    throw InvalidArgumentException(HERE) << "Primal status not ok";

  const HighsSolution& solution = highs.getSolution();
  Point optimalPoint(problemDimension);
  for (int col = 0; col < model.lp_.num_col_; col++) {
      optimalPoint[col] = solution.col_value[col];
  }
  result_.setOptimalPoint(optimalPoint);
  result_.setOptimalValue(Point({info.objective_function_value}));
  const UnsignedInteger callsNumber = getProblem().getObjective().getCallsNumber() - initialCallsNumber;
  result_.setCallsNumber(callsNumber);
  result_.setIterationNumber(info.simplex_iteration_count);

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
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}



/* Method save() stores the object through the StorageManager */
void HiGHS::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void HiGHS::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
