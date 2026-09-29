//                                               -*- C++ -*-
/**
 *  @brief SCIP mixed-integer nonlinear solver
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
#include "openturns/SCIP.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"

#ifdef OPENTURNS_HAVE_SCIP
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <scip/cons_linear.h>
#include <scip/cons_nonlinear.h>
#include <scip/expr_var.h>
#include <scip/expr_sum.h>
#include <scip/intervalarith.h>
#include <scip/pub_event.h>
#include <scip/pub_expr.h>
#include <scip/scip_nlpi.h>
#include <scip/scip_numerics.h>
#include <objscip/objexprhdlr.h>
#endif

#include <algorithm>
#include <chrono>
#include <cmath>

#ifdef OPENTURNS_HAVE_SCIP
namespace
{

/* Expression data attached to each black-box expression: one Function marginal */
struct OTExprData
{
  OTExprData(const OT::Function & function, const OT::UnsignedInteger marginalIndex)
    : function_(function), marginalIndex_(marginalIndex) {}
  OT::Function function_;
  OT::UnsignedInteger marginalIndex_;
};

/* Collect current point from already evaluated children */
OT::Point GetChildrenPoint(::SCIP_EXPR * expr)
{
  const int nChildren = SCIPexprGetNChildren(expr);
  ::SCIP_EXPR ** children = SCIPexprGetChildren(expr);
  OT::Point x(nChildren);
  for (int i = 0; i < nChildren; ++ i)
    x[i] = SCIPexprGetEvalValue(children[i]);
  return x;
}

/* Black-box expression handler forwarding eval/grad/hess to an OT Function */
class OTExprHdlr
  : public scip::ObjExprhdlr
{
public:
  OTExprHdlr(::SCIP * scip)
    : scip::ObjExprhdlr(scip, "otfunc", "openturns black-box function",
                        100000,
                        TRUE, TRUE, FALSE, FALSE, FALSE, FALSE,
                        TRUE, TRUE, TRUE, FALSE, TRUE, FALSE,
                        FALSE, FALSE, FALSE, FALSE, FALSE, FALSE)
  {}

  OTExprHdlr * clone(::SCIP * scip) const override
  {
    return new OTExprHdlr(scip);
  }

  SCIP_Bool iscloneable() const override
  {
    return TRUE;
  }

  SCIP_DECL_EXPREVAL(scip_eval) override
  {
    (void) scip;
    (void) sol;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    try
    {
      const OT::Point x(GetChildrenPoint(expr));
      const OT::Point y(data->function_(x));
      *val = y[data->marginalIndex_];
    }
    catch (const std::exception &)
    {
      *val = SCIP_INVALID;
    }
    return SCIP_OKAY;
  }

  SCIP_DECL_EXPRBWDIFF(scip_bwdiff) override
  {
    (void) scip;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    try
    {
      const OT::Point x(GetChildrenPoint(expr));
      const OT::Matrix grad(data->function_.gradient(x));
      *val = grad(childidx, data->marginalIndex_);
    }
    catch (const std::exception &)
    {
      *val = 0.0;
    }
    return SCIP_OKAY;
  }

  SCIP_DECL_EXPRFWDIFF(scip_fwdiff) override
  {
    (void) scip;
    (void) direction;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    try
    {
      const OT::Point x(GetChildrenPoint(expr));
      const OT::Matrix grad(data->function_.gradient(x));
      ::SCIP_EXPR ** children = SCIPexprGetChildren(expr);
      const int nChildren = SCIPexprGetNChildren(expr);
      OT::Scalar dotValue = 0.0;
      for (int i = 0; i < nChildren; ++ i)
        dotValue += grad(i, data->marginalIndex_) * SCIPexprGetDot(children[i]);
      *dot = dotValue;
    }
    catch (const std::exception &)
    {
      *dot = 0.0;
    }
    return SCIP_OKAY;
  }

  SCIP_DECL_EXPRBWFWDIFF(scip_bwfwdiff) override
  {
    (void) scip;
    (void) direction;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    try
    {
      const OT::Point x(GetChildrenPoint(expr));
      const OT::SymmetricMatrix hess(data->function_.hessian(x).getSheet(data->marginalIndex_));
      ::SCIP_EXPR ** children = SCIPexprGetChildren(expr);
      const int nChildren = SCIPexprGetNChildren(expr);
      OT::Scalar bardotValue = 0.0;
      for (int i = 0; i < nChildren; ++ i)
        bardotValue += hess(i, childidx) * SCIPexprGetDot(children[i]);
      *bardot = bardotValue;
    }
    catch (const std::exception &)
    {
      *bardot = 0.0;
    }
    return SCIP_OKAY;
  }

  SCIP_DECL_EXPRCOPYDATA(scip_copydata) override
  {
    (void) targetscip;
    (void) targetexprhdlr;
    (void) sourcescip;
    OTExprData * sourceData = reinterpret_cast<OTExprData *>(SCIPexprGetData(sourceexpr));
    try
    {
      *targetexprdata = reinterpret_cast<::SCIP_EXPRDATA *>(new OTExprData(*sourceData));
    }
    catch (const std::exception &)
    {
      return SCIP_NOMEMORY;
    }
    return SCIP_OKAY;
  }

  SCIP_DECL_EXPRFREEDATA(scip_freedata) override
  {
    (void) scip;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    delete data;
    SCIPexprSetData(expr, nullptr);
    return SCIP_OKAY;
  }

  /* Mean value form: the first order Taylor expansion at the reference point, shifted by
     the sum of the gradients magnitudes weighted by the radii of the global bounds */
  SCIP_DECL_EXPRESTIMATE(scip_estimate) override
  {
    (void) localbounds;
    (void) branchcand;
    (void) targetvalue;
    OTExprData * data = reinterpret_cast<OTExprData *>(SCIPexprGetData(expr));
    const int nChildren = SCIPexprGetNChildren(expr);
    try
    {
      const OT::Point reference(refpoint, refpoint + nChildren);
      const OT::Point y(data->function_(reference));
      const OT::Matrix grad(data->function_.gradient(reference));
      OT::Scalar deviation = 0.0;
      for (int i = 0; i < nChildren; ++ i)
      {
        const OT::Scalar radius = std::max(std::abs(SCIPintervalGetInf(globalbounds[i]) - reference[i]),
                                           std::abs(SCIPintervalGetSup(globalbounds[i]) - reference[i]));
        // Without finite bounds the mean value form is meaningless, and its slack
        // would degrade the conditioning of the linear relaxation
        if (!std::isfinite(radius) || SCIPisInfinity(scip, radius))
        {
          *success = FALSE;
          return SCIP_OKAY;
        }
        coefs[i] = grad(i, data->marginalIndex_);
        deviation += std::abs(coefs[i]) * radius;
      }
      *constant = y[data->marginalIndex_] + (overestimate ? deviation : -deviation);
      *islocal = FALSE;
      *success = TRUE;
    }
    catch (const std::exception &)
    {
      *success = FALSE;
    }
    return SCIP_OKAY;
  }
}; /* class OTExprHdlr */


/* Data shared with the stop callback event handler, alive during the whole solve */
struct OTStopData
{
  const std::pair<OT::OptimizationAlgorithmImplementation::StopCallback, void *> * stopCallback_;
  OT::Bool * interrupted_;
};

/* Catch the node events during the initialization of the solving process */
SCIP_DECL_EVENTINIT(otEventInit)
{
  return SCIPcatchEvent(scip, SCIP_EVENTTYPE_NODEFOCUSED, eventhdlr,
                        reinterpret_cast<::SCIP_EVENTDATA *>(SCIPeventhdlrGetData(eventhdlr)), nullptr);
}

/* Poll the stop callback at each node and interrupt the search if the user asks to stop */
SCIP_DECL_EVENTEXEC(otEventExec)
{
  (void) eventhdlr;
  (void) event;
  OTStopData * data = reinterpret_cast<OTStopData *>(eventdata);
  if (data->stopCallback_->first && data->stopCallback_->first(data->stopCallback_->second))
  {
    *data->interrupted_ = TRUE;
    SCIPinterruptSolve(scip);
  }
  return SCIP_OKAY;
}


/* Throw on SCIP error */
void CheckRetcode(const SCIP_RETCODE ret, const char * context)
{
  if (ret != SCIP_OKAY)
    throw OT::InternalException(HERE) << "SCIP error in " << context << " (code " << ret << ")";
}

/* Create a black-box expression, freeing the attached data if SCIP rejects it */
void CreateBlackBoxExpr(::SCIP * scip,
                        ::SCIP_EXPR * & expr,
                        const OT::Function & function,
                        const OT::UnsignedInteger marginalIndex,
                        ::SCIP_EXPR ** children,
                        const OT::UnsignedInteger nChildren)
{
  OTExprData * data = new OTExprData(function, marginalIndex);
  const SCIP_RETCODE ret = SCIPcreateExpr(scip, &expr, SCIPfindExprhdlr(scip, "otfunc"),
                                          reinterpret_cast<::SCIP_EXPRDATA *>(data),
                                          static_cast<int>(nChildren), children, nullptr, nullptr);
  if (ret != SCIP_OKAY)
  {
    delete data;
    expr = nullptr;
    CheckRetcode(ret, "SCIPcreateExpr");
  }
}

/* Extract linear coefficients of a linear function: coef + constant */
OT::Bool GetLinearCoefficients(const OT::Function & function,
                               const OT::UnsignedInteger outputIndex,
                               OT::Point & coefficients,
                               OT::Scalar & constant)
{
  try
  {
    const OT::UnsignedInteger dimension = function.getInputDimension();
    const OT::Point zero(dimension, 0.0);
    const OT::Matrix grad(function.gradient(zero));
    const OT::Point value(function(zero));
    coefficients = OT::Point(dimension);
    for (OT::UnsignedInteger i = 0; i < dimension; ++ i)
      coefficients[i] = grad(i, outputIndex);
    constant = value[outputIndex];
  }
  catch (const std::exception &)
  {
    return false;
  }
  return true;
}

/* Bounds of a function over a box domain, based on the mean value form:
   |f(x) - f(x0)| <= 2 * sum_i |df/dxi(x0)| * radius_i, the factor 2 coming from both
   the linear term and the remainder of the Taylor expansion */
OT::Bool GetBoxBounds(const OT::Function & function,
                      const OT::UnsignedInteger outputIndex,
                      const OT::Interval & domain,
                      OT::Scalar & lowerBound,
                      OT::Scalar & upperBound)
{
  lowerBound = -OT::SpecFunc::MaxScalar;
  upperBound = OT::SpecFunc::MaxScalar;
  try
  {
    const OT::UnsignedInteger dimension = function.getInputDimension();
    if (domain.getDimension() != dimension) return false;
    // The center of the domain is the reference point of the Taylor expansion
    OT::Point reference(dimension);
    OT::Point radius(dimension);
    for (OT::UnsignedInteger i = 0; i < dimension; ++ i)
    {
      if (!(domain.getFiniteLowerBound()[i] && domain.getFiniteUpperBound()[i])) return false;
      reference[i] = 0.5 * (domain.getLowerBound()[i] + domain.getUpperBound()[i]);
      radius[i] = 0.5 * (domain.getUpperBound()[i] - domain.getLowerBound()[i]);
    }
    const OT::Scalar value = function(reference)[outputIndex];
    const OT::Matrix grad(function.gradient(reference));
    OT::Scalar deviation = 0.0;
    for (OT::UnsignedInteger i = 0; i < dimension; ++ i)
      deviation += std::abs(grad(i, outputIndex)) * radius[i];
    deviation *= 2.0;
    if (!(deviation < OT::SpecFunc::MaxScalar))
      return false;
    lowerBound = value - deviation;
    upperBound = value + deviation;
  }
  catch (const std::exception &)
  {
    return false;
  }
  return true;
}

/* Map OT variable type to SCIP variable type */
SCIP_VARTYPE GetSCIPVarType(const OT::UnsignedInteger variableType)
{
  if (variableType == OT::OptimizationProblemImplementation::BINARY)
    return SCIP_VARTYPE_BINARY;
  if (variableType == OT::OptimizationProblemImplementation::INTEGER)
    return SCIP_VARTYPE_INTEGER;
  return SCIP_VARTYPE_CONTINUOUS;
}

} /* anonymous namespace */

#endif


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SCIP)

static const Factory<SCIP> Factory_SCIP;

/* Constructor with no parameters */
SCIP::SCIP()
  : OptimizationAlgorithmImplementation()
{
  // One branch-and-bound node per iteration: MIP needs a much larger budget than the
  // default number of iterations of the iterative smooth solvers
  setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumIterationNumber"));
}


/* Constructor that sets starting sample */
SCIP::SCIP(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)

{
  setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumIterationNumber"));
  checkProblem(problem);
}


/* Name and version of the LP solver embedded in the SCIP library */
String SCIP::GetLPsolver()
{
#ifdef OPENTURNS_HAVE_SCIP
  return SCIPlpiGetSolverName();
#else
  return "";
#endif
}


/* Names of the NLP solvers embedded in the SCIP library */
Description SCIP::GetNLPsolvers()
{
  Description nlpSolvers;
#ifdef OPENTURNS_HAVE_SCIP
  // The NLP solvers are plugins, they show up once the default ones are included
  ::SCIP * scip = nullptr;
  if (SCIPcreate(&scip) == SCIP_OKAY && scip != nullptr)
  {
    if (SCIPincludeDefaultPlugins(scip) == SCIP_OKAY)
    {
      const int numberOfNlpis = SCIPgetNNlpis(scip);
      ::SCIP_NLPI ** nlpis = SCIPgetNlpis(scip);
      for (int i = 0; i < numberOfNlpis; ++ i)
        nlpSolvers.add(SCIPnlpiGetName(nlpis[i]));
    }
    SCIPfree(&scip);
  }
#endif
  return nlpSolvers;
}


/* Check whether this problem can be solved by this solver */
void SCIP::checkProblem(const OptimizationProblem & problem) const
{
  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction() || problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "SCIP does not support multi-objective / least squares / nearest point problems";
}


void SCIP::run()
{
#ifdef OPENTURNS_HAVE_SCIP
  const UnsignedInteger problemDimension = getProblem().getDimension();
  if (problemDimension == 0) throw InvalidArgumentException(HERE) << "No problem has been set.";
  result_ = OptimizationResult(getProblem());
  const UnsignedInteger initialCallsNumber = getProblem().getObjective().getCallsNumber();
  const Scalar constraintTolerance = std::max(getMaximumConstraintError(), SpecFunc::MinScalar);

  ::SCIP * scip = nullptr;
  CheckRetcode(SCIPcreate(&scip), "SCIPcreate");
  try
  {
    CheckRetcode(SCIPincludeDefaultPlugins(scip), "SCIPincludeDefaultPlugins");
    {
      OTExprHdlr * exprHdlr = new OTExprHdlr(scip);
      const SCIP_RETCODE ret = SCIPincludeObjExprhdlr(scip, exprHdlr, TRUE);
      if (ret != SCIP_OKAY)
      {
        delete exprHdlr;
        CheckRetcode(ret, "SCIPincludeObjExprhdlr");
      }
    }
    // Event handler polling the stop callback at each node
    Bool interrupted = FALSE;
    OTStopData stopData = {&stopCallback_, &interrupted};
    ::SCIP_EVENTHDLR * eventHdlr = nullptr;
    CheckRetcode(SCIPincludeEventhdlrBasic(scip, &eventHdlr, "otstop", "openturns stop callback", otEventExec,
                                           reinterpret_cast<::SCIP_EVENTHDLRDATA *>(&stopData)), "SCIPincludeEventhdlrBasic");
    CheckRetcode(SCIPsetEventhdlrInit(scip, eventHdlr, otEventInit), "SCIPsetEventhdlrInit");
    CheckRetcode(SCIPcreateProbBasic(scip, "openturns"), "SCIPcreateProbBasic");
    CheckRetcode(SCIPsetObjsense(scip, getProblem().isMinimization() ? SCIP_OBJSENSE_MINIMIZE : SCIP_OBJSENSE_MAXIMIZE), "SCIPsetObjsense");

    const Scalar infinity = SCIPinfinity(scip);

    // Variables
    std::vector<::SCIP_VAR *> vars(problemDimension, nullptr);
    std::vector<Scalar> lowerBounds(problemDimension, 0.0);
    std::vector<Scalar> upperBounds(problemDimension, 0.0);
    for (UnsignedInteger i = 0; i < problemDimension; ++ i)
    {
      Scalar lb = -infinity;
      Scalar ub = infinity;
      if (getProblem().hasBounds())
      {
        if (getProblem().getBounds().getFiniteLowerBound()[i])
          lb = getProblem().getBounds().getLowerBound()[i];
        if (getProblem().getBounds().getFiniteUpperBound()[i])
          ub = getProblem().getBounds().getUpperBound()[i];
      }
      ::SCIP_VARTYPE vartype = GetSCIPVarType(getProblem().getVariablesType()[i]);
      if (vartype == SCIP_VARTYPE_BINARY)
      {
        lb = 0.0;
        ub = 1.0;
      }
      lowerBounds[i] = lb;
      upperBounds[i] = ub;
      OSS name;
      name << "x" << i;
      CheckRetcode(SCIPcreateVarBasic(scip, &vars[i], name.str().c_str(), lb, ub, 0.0, vartype), "SCIPcreateVarBasic");
      CheckRetcode(SCIPaddVar(scip, vars[i]), "SCIPaddVar");
    }

    // Variable expressions shared by all black-box expressions
    std::vector<::SCIP_EXPR *> varExprs(problemDimension, nullptr);
    for (UnsignedInteger i = 0; i < problemDimension; ++ i)
      CheckRetcode(SCIPcreateExprVar(scip, &varExprs[i], vars[i], nullptr, nullptr), "SCIPcreateExprVar");

    // Helper releasing captured child expressions after use, never throws
    auto releaseChildren = [scip](std::vector<::SCIP_EXPR *> & children)
    {
      for (::SCIP_EXPR * & child : children)
        if (child != nullptr)
        {
          (void) SCIPreleaseExpr(scip, &child);
          child = nullptr;
        }
    };

    // Objective
    const Function objective(getProblem().getObjective());
    Point linearCost;
    Scalar linearConstant = 0.0;
    const Bool linearObjective = objective.isLinear() && GetLinearCoefficients(objective, 0, linearCost, linearConstant);
    if (getProblem().isLinear())
    {
      const Point cost(getProblem().getLinearCost());
      for (UnsignedInteger i = 0; i < problemDimension; ++ i)
        CheckRetcode(SCIPchgVarObj(scip, vars[i], cost[i]), "SCIPchgVarObj");
    }
    else if (linearObjective)
    {
      for (UnsignedInteger i = 0; i < problemDimension; ++ i)
        CheckRetcode(SCIPchgVarObj(scip, vars[i], linearCost[i]), "SCIPchgVarObj");
      CheckRetcode(SCIPaddOrigObjoffset(scip, linearConstant), "SCIPaddOrigObjoffset");
    }
    else
    {
      // Nonlinear objective through auxiliary variable: minimize aux subject to aux == f(x).
      // The auxiliary variable is bounded using the range of the objective over the domain,
      // otherwise the relaxation stays unbounded and the branch-and-bound search is meaningless.
      Scalar auxLowerBound = -infinity;
      Scalar auxUpperBound = infinity;
      Scalar objectiveLowerBound = 0.0;
      Scalar objectiveUpperBound = 0.0;
      const Interval domain(Point(lowerBounds.begin(), lowerBounds.end()), Point(upperBounds.begin(), upperBounds.end()));
      if (GetBoxBounds(objective, 0, domain, objectiveLowerBound, objectiveUpperBound))
      {
        auxLowerBound = std::max(objectiveLowerBound, -infinity);
        auxUpperBound = std::min(objectiveUpperBound, infinity);
      }
      ::SCIP_VAR * auxVar = nullptr;
      CheckRetcode(SCIPcreateVarBasic(scip, &auxVar, "ot_obj_aux", auxLowerBound, auxUpperBound, 1.0, SCIP_VARTYPE_CONTINUOUS), "SCIPcreateVarBasic");
      CheckRetcode(SCIPaddVar(scip, auxVar), "SCIPaddVar");
      ::SCIP_EXPR * auxExpr = nullptr;
      CheckRetcode(SCIPcreateExprVar(scip, &auxExpr, auxVar, nullptr, nullptr), "SCIPcreateExprVar");
      ::SCIP_EXPR * funcExpr = nullptr;
      CreateBlackBoxExpr(scip, funcExpr, objective, 0, varExprs.data(), problemDimension);
      ::SCIP_EXPR * children[2] = {auxExpr, funcExpr};
      SCIP_Real coefs[2] = {1.0, -1.0};
      ::SCIP_EXPR * sumExpr = nullptr;
      CheckRetcode(SCIPcreateExprSum(scip, &sumExpr, 2, children, coefs, 0.0, nullptr, nullptr), "SCIPcreateExprSum");
      ::SCIP_CONS * objCons = nullptr;
      CheckRetcode(SCIPcreateConsBasicNonlinear(scip, &objCons, "ot_objective", sumExpr, 0.0, 0.0), "SCIPcreateConsBasicNonlinear");
      CheckRetcode(SCIPaddCons(scip, objCons), "SCIPaddCons");
      CheckRetcode(SCIPreleaseCons(scip, &objCons), "SCIPreleaseCons");
      CheckRetcode(SCIPreleaseExpr(scip, &sumExpr), "SCIPreleaseExpr");
      CheckRetcode(SCIPreleaseExpr(scip, &funcExpr), "SCIPreleaseExpr");
      CheckRetcode(SCIPreleaseExpr(scip, &auxExpr), "SCIPreleaseExpr");
      CheckRetcode(SCIPreleaseVar(scip, &auxVar), "SCIPreleaseVar");
    }

    // Constraints: linear functions go to linear constraints, others to nonlinear ones
    UnsignedInteger consIndex = 0;
    const auto addLinearRow = [&](const Point & row, const Scalar lhs, const Scalar rhs)
    {
      std::vector<SCIP_Real> vals(problemDimension);
      for (UnsignedInteger i = 0; i < problemDimension; ++ i)
        vals[i] = row[i];
      OSS name;
      name << "c" << (consIndex ++);
      ::SCIP_CONS * cons = nullptr;
      CheckRetcode(SCIPcreateConsBasicLinear(scip, &cons, name.str().c_str(), problemDimension, vars.data(), vals.data(), lhs, rhs), "SCIPcreateConsBasicLinear");
      CheckRetcode(SCIPaddCons(scip, cons), "SCIPaddCons");
      CheckRetcode(SCIPreleaseCons(scip, &cons), "SCIPreleaseCons");
    };
    const auto addNonlinearCons = [&](const Function & function, const UnsignedInteger outputIndex, const Scalar lhs, const Scalar rhs)
    {
      ::SCIP_EXPR * funcExpr = nullptr;
      CreateBlackBoxExpr(scip, funcExpr, function, outputIndex, varExprs.data(), problemDimension);
      OSS name;
      name << "c" << (consIndex ++);
      ::SCIP_CONS * cons = nullptr;
      CheckRetcode(SCIPcreateConsBasicNonlinear(scip, &cons, name.str().c_str(), funcExpr, lhs, rhs), "SCIPcreateConsBasicNonlinear");
      CheckRetcode(SCIPaddCons(scip, cons), "SCIPaddCons");
      CheckRetcode(SCIPreleaseCons(scip, &cons), "SCIPreleaseCons");
      CheckRetcode(SCIPreleaseExpr(scip, &funcExpr), "SCIPreleaseExpr");
    };
    const auto addFunctionConstraints = [&](const Function & function, const Bool isEquality)
    {
      // Linear marginals go to linear constraints, others to nonlinear ones
      const UnsignedInteger outputDimension = function.getOutputDimension();
      for (UnsignedInteger j = 0; j < outputDimension; ++ j)
      {
        Point row;
        Scalar constant = 0.0;
        if (function.getMarginal(j).isLinear() && GetLinearCoefficients(function, j, row, constant))
        {
          // SCIP models A.x + constant within [lhs, rhs]
          if (isEquality)
            addLinearRow(row, -constraintTolerance - constant, constraintTolerance - constant);
          else
            addLinearRow(row, -constraintTolerance - constant, infinity);
        }
        else
        {
          if (isEquality)
            addNonlinearCons(function, j, -constraintTolerance, constraintTolerance);
          else
            addNonlinearCons(function, j, -constraintTolerance, infinity);
        }
      }
    };
    if (getProblem().isLinear())
    {
      const Matrix coefficients(getProblem().getLinearConstraintCoefficients());
      const Interval constraintBounds(getProblem().getLinearConstraintBounds());
      const UnsignedInteger nbRows = coefficients.getNbRows();
      for (UnsignedInteger row = 0; row < nbRows; ++ row)
      {
        Point rowCoefficients(problemDimension);
        for (UnsignedInteger i = 0; i < problemDimension; ++ i)
          rowCoefficients[i] = coefficients(row, i);
        Scalar lhs = -infinity;
        Scalar rhs = infinity;
        if (constraintBounds.getFiniteLowerBound()[row])
          lhs = constraintBounds.getLowerBound()[row] - constraintTolerance;
        if (constraintBounds.getFiniteUpperBound()[row])
          rhs = constraintBounds.getUpperBound()[row] + constraintTolerance;
        addLinearRow(rowCoefficients, lhs, rhs);
      }
    }
    else
    {
      if (getProblem().hasEqualityConstraint())
        addFunctionConstraints(getProblem().getEqualityConstraint(), true);
      if (getProblem().hasInequalityConstraint())
        addFunctionConstraints(getProblem().getInequalityConstraint(), false);
    }

    releaseChildren(varExprs);
    for (::SCIP_VAR * & var : vars)
    {
      CheckRetcode(SCIPreleaseVar(scip, &var), "SCIPreleaseVar");
      var = nullptr;
    }

    // Limits
    if (getMaximumTimeDuration() > 0.0)
      CheckRetcode(SCIPsetRealParam(scip, "limits/time", getMaximumTimeDuration()), "limits/time");
    if (getMaximumIterationNumber() > 0)
      CheckRetcode(SCIPsetLongintParam(scip, "limits/nodes", static_cast<SCIP_Longint>(getMaximumIterationNumber())), "limits/nodes");

    // Options from ResourceMap, they override the limits above
    Bool userVerbosity = false;
    std::vector<String> keys(ResourceMap::GetKeys());
    for (const String & key : keys)
    {
      if (key == "SCIP-DefaultMaximumIterationNumber") continue;
      if (key.substr(0, 5) != "SCIP-")
        continue;
      const String optionName(key.substr(5));
      if (optionName.substr(0, 7) == "display/") userVerbosity = true;
      const String type(ResourceMap::GetType(key));
      ::SCIP_RETCODE ret = SCIP_OKAY;
      if (type == "str")
        ret = SCIPsetStringParam(scip, optionName.c_str(), ResourceMap::GetAsString(key).c_str());
      else if (type == "float")
        ret = SCIPsetRealParam(scip, optionName.c_str(), ResourceMap::GetAsScalar(key));
      else if (type == "int")
      {
        ret = SCIPsetIntParam(scip, optionName.c_str(), static_cast<int>(ResourceMap::GetAsUnsignedInteger(key)));
        if (ret != SCIP_OKAY)
          ret = SCIPsetLongintParam(scip, optionName.c_str(), static_cast<SCIP_Longint>(ResourceMap::GetAsUnsignedInteger(key)));
      }
      else if (type == "bool")
        ret = SCIPsetBoolParam(scip, optionName.c_str(), ResourceMap::GetAsBool(key) ? TRUE : FALSE);
      if (ret != SCIP_OKAY)
        throw InvalidArgumentException(HERE) << "Invalid SCIP option " << optionName;
    }

    // Verbosity, unless the user asked for a specific verbosity level
    SCIPsetMessagehdlrQuiet(scip, !Log::HasDebug() && !userVerbosity);

    // Starting point as solution hint, with auxiliary objective value if any
    const Bool useAuxObjective = !getProblem().isLinear() && !linearObjective;
    if (getStartingPoint().getDimension() == problemDimension)
    {
      Point startAuxValue;
      if (useAuxObjective)
      {
        try
        {
          startAuxValue = objective(getStartingPoint());
        }
        catch (const std::exception &)
        {
          startAuxValue = Point();
        }
      }
      ::SCIP_SOL * sol = nullptr;
      if (SCIPcreateSol(scip, &sol, nullptr) == SCIP_OKAY && sol != nullptr)
      {
        Bool allOk = TRUE;
        for (UnsignedInteger i = 0; i < problemDimension; ++ i)
          if (SCIPfindVar(scip, (OSS() << "x" << i).str().c_str()) == nullptr
              || SCIPsetSolVal(scip, sol, SCIPfindVar(scip, (OSS() << "x" << i).str().c_str()), getStartingPoint()[i]) != SCIP_OKAY)
          {
            allOk = FALSE;
            break;
          }
        if (allOk && useAuxObjective && startAuxValue.getDimension() == 1)
        {
          ::SCIP_VAR * auxVar = SCIPfindVar(scip, "ot_obj_aux");
          if (auxVar == nullptr || SCIPsetSolVal(scip, sol, auxVar, startAuxValue[0]) != SCIP_OKAY)
            allOk = FALSE;
        }
        if (allOk)
        {
          SCIP_Bool stored = FALSE;
          SCIPaddSolFree(scip, &sol, &stored);
        }
        if (sol != nullptr)
          SCIPfreeSol(scip, &sol);
      }
    }

    CheckRetcode(SCIPsolve(scip), "SCIPsolve");

    const ::SCIP_STATUS status = SCIPgetStatus(scip);
    ::SCIP_SOL * bestSol = SCIPgetBestSol(scip);
    // Solving interrupted by a limit: without a solution there is nothing to report
    const auto limitReached = [this, bestSol](String message, const OptimizationResult::Status limitStatus = OptimizationResult::FAILURE)
    {
      if (bestSol == nullptr)
        result_.setStatus(OptimizationResult::FAILURE);
      else
      {
        message += ", feasible solution found";
        result_.setStatus(limitStatus);
      }
      return message;
    };
    String statusMessage;
    switch (status)
    {
      case SCIP_STATUS_OPTIMAL:
        statusMessage = "Optimal";
        break;
      case SCIP_STATUS_INFEASIBLE:
        statusMessage = "Infeasible";
        result_.setStatus(OptimizationResult::FAILURE);
        break;
      case SCIP_STATUS_UNBOUNDED:
        statusMessage = "Unbounded";
        result_.setStatus(OptimizationResult::FAILURE);
        break;
      case SCIP_STATUS_INFORUNBD:
        statusMessage = "Infeasible or unbounded";
        result_.setStatus(OptimizationResult::FAILURE);
        break;
      case SCIP_STATUS_USERINTERRUPT:
      case SCIP_STATUS_TERMINATE:
        statusMessage = "User interrupt";
        result_.setStatus(OptimizationResult::INTERRUPTION);
        break;
      case SCIP_STATUS_TIMELIMIT:
        statusMessage = "Time limit";
        result_.setStatus(OptimizationResult::TIMEOUT);
        break;
      case SCIP_STATUS_NODELIMIT:
      case SCIP_STATUS_TOTALNODELIMIT:
      case SCIP_STATUS_STALLNODELIMIT:
        statusMessage = limitReached((OSS() << "Maximum number of nodes " << getMaximumIterationNumber() << " reached").str(), OptimizationResult::MAXIMUMCALLS);
        break;
      case SCIP_STATUS_MEMLIMIT:
        statusMessage = limitReached("Memory limit reached");
        break;
      case SCIP_STATUS_GAPLIMIT:
        statusMessage = limitReached("Gap limit reached");
        break;
      case SCIP_STATUS_SOLLIMIT:
      case SCIP_STATUS_BESTSOLLIMIT:
        statusMessage = limitReached("Solution limit reached");
        break;
      case SCIP_STATUS_PRIMALLIMIT:
        statusMessage = limitReached("Primal limit reached");
        break;
      default:
        // Other limits (restarts): the status is meaningless without a solution
        statusMessage = limitReached((OSS() << "Solving stopped with status " << static_cast<int>(status)).str());
        break;
    }
    // Whatever SCIP reports about an interrupted search is meaningless
    if (interrupted)
    {
      statusMessage = "User interrupt";
      result_.setStatus(OptimizationResult::INTERRUPTION);
    }
    result_.setStatusMessage(statusMessage);

    if (bestSol == nullptr)
    {
      if (result_.getStatus() == OptimizationResult::SUCCESS)
        result_.setStatus(OptimizationResult::FAILURE);
      // The user asked to stop, the absence of solution is not an error
      if (getCheckStatus() && result_.getStatus() != OptimizationResult::INTERRUPTION)
      {
        SCIPfree(&scip);
        throw InternalException(HERE) << "SCIP found no solution (" << statusMessage << ")";
      }
      SCIPfree(&scip);
      return;
    }
    Point optimalPoint(problemDimension);
    for (UnsignedInteger i = 0; i < problemDimension; ++ i)
      optimalPoint[i] = SCIPgetSolVal(scip, bestSol, SCIPfindVar(scip, (OSS() << "x" << i).str().c_str()));
    result_.setOptimalPoint(optimalPoint);
    result_.setOptimalValue(objective(optimalPoint));
    result_.setCallsNumber(getProblem().getObjective().getCallsNumber() - initialCallsNumber);
    result_.setIterationNumber(static_cast<UnsignedInteger>(SCIPgetNNodes(scip)));
    result_.setTimeDuration(SCIPgetSolvingTime(scip));

    if (result_.getStatus() != OptimizationResult::SUCCESS)
    {
      if (getCheckStatus())
      {
        SCIPfree(&scip);
        throw InternalException(HERE) << "Solving problem by SCIP method failed (" << result_.getStatusMessage() << ")";
      }
      LOGWARN(OSS() << "SCIP algorithm failed. The error message is " << result_.getStatusMessage());
    }
    CheckRetcode(SCIPfree(&scip), "SCIPfree");
  }
  catch (const std::exception &)
  {
    if (scip != nullptr)
      SCIPfree(&scip);
    throw;
  }
#else
  throw NotYetImplementedException(HERE) << "No SCIP support";
#endif
}


/* Virtual constructor */
SCIP * SCIP::clone() const
{
  return new SCIP(*this);
}

/* String converter */
String SCIP::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}



/* Method save() stores the object through the StorageManager */
void SCIP::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SCIP::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
