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
#include "openturns/Bonmin.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#ifdef OPENTURNS_HAVE_BONMIN
  #include "openturns/BonminProblem.hxx"
  #include "BonBonminSetup.hpp"
  #include "BonCbc.hpp"
  using namespace Bonmin;
  using namespace Ipopt;
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Bonmin)

static const Factory<Bonmin> Factory_Bonmin;

/** Constructors */
Bonmin::Bonmin(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , algoName_()
  {
    setAlgorithmName(algoName);
  }   

Bonmin::Bonmin( OptimizationProblem & problem,
                const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
  , algoName_()
  {
    setAlgorithmName(algoName);
  }

/* Virtual constructor */
Bonmin * Bonmin::clone() const
{
  return new Bonmin(*this);
}

  
  
  /** Bonmin static methods */
  Bool Bonmin::IsAvailable()
  {
    #ifdef OPENTURNS_HAVE_BONMIN
      return true;
    #else
      return false;
    #endif
  }
  
  Description Bonmin::GetAlgorithmNames()
  {
    Description algoNames(5);
    algoNames[0] = "B-BB";
    algoNames[1] = "B-OA";
    algoNames[2] = "B-QG";
    algoNames[3] = "B-Hyb";
    algoNames[4] = "B-iFP";
    
    return algoNames;
  }

  /** Accessors to Bonmin attributes */
  void Bonmin::setAlgorithmName(const String & algoName)
  {
    // Check algoName
    if (!GetAlgorithmNames().contains(algoName))
    {
      if (algoName == "B-Ecp")
        throw InternalException(HERE) << "B-Ecp solver is unavailable.";
      else
        throw InvalidArgumentException(HERE) << "Unknown solver " << algoName;
    }
    algoName_ = algoName;
  }
  
  String Bonmin::getAlgorithmName() const
  {
    return algoName_;
  }

  
/** Check whether this problem can be solved by this solver. */
void Bonmin::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective()) throw InvalidArgumentException(HERE) << "Error: Bonmin algorithm " << algoName_ << " does not support multi-objective optimization";
  
  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Bonmin does not support least squares or nearest point problems";
}

/** Performs the actual computation. */
void Bonmin::run()
{
  #ifdef OPENTURNS_HAVE_BONMIN
  // Check problem
  checkProblem(getProblem());
  
  // Check starting point
  if (getStartingPoint().getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << getStartingPoint().getDimension() << "), expected " << getProblem().getDimension();
  
  // Create BonminProblem
  Ipopt::SmartPtr<BonminProblem> tminlp = new BonminProblem(getProblem(),getStartingPoint(), getMaximumEvaluationNumber());
  tminlp->setProgressCallback(progressCallback_.first, progressCallback_.second);
  tminlp->setStopCallback(stopCallback_.first, stopCallback_.second);
                              
  // Create setup, initialize options
  BonminSetup bonmin;
  bonmin.initializeOptionsAndJournalist();
   
  // Retrieve user defined options
  Description optionNames(getOptions());
  for (UnsignedInteger i=0; i<options_.getSize(); ++i)
    if (options_.getType(optionNames[i]) == "string")
      bonmin.options()->SetStringValue(optionNames[i], options_.getAsString(optionNames[i]));
    else if (options_.getType(optionNames[i]) == "unsigned int")
      bonmin.options()->SetIntegerValue(optionNames[i], options_.getAsUnsignedInteger(optionNames[i]));
    else if (options_.getType(optionNames[i]) == "float")
      bonmin.options()->SetNumericValue(optionNames[i], options_.getAsScalar(optionNames[i]));
    else
      throw InternalException(HERE) << "Unsupported option type " << options_.getType(optionNames[i]);

  // Manage conflicts between user-defined options and Bonmin object attributes
  if (options_.hasKey("bonmin.algorithm") && options_.getAsString("bonmin.algorithm") != algoName_)
    LOGWARN(OSS() << "Algorithm name specified in local resource map (" << options_.getAsString("bonmin.algorithm") << ") will be ignored");
  bonmin.options()->SetStringValue("bonmin.algorithm", algoName_);
  
  if (options_.hasKey("max_iter") && options_.getAsUnsignedInteger("Bonmin-max_iter") != getMaximumIterationNumber())
    LOGWARN(OSS() << "Maximum iteration number specified in local resource map (" << options_.getAsUnsignedInteger("max_iter") << ") will be ignored");
  bonmin.options()->SetIntegerValue("max_iter", getMaximumIterationNumber());
 
  // Update setup with BonminProblem
  bonmin.initialize(GetRawPtr(tminlp));

  // Solve problem
  Bab solver;
  solver(bonmin);
  
  // Retrieve MemoizeFunction input/output history
  Sample inputHistory(tminlp->getInputHistory());
  Sample outputHistory(tminlp->getOutputHistory());
  
  // Create OptimizationResult, initialize error values
  OptimizationResult optimResult(getProblem().getDimension());
  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  /* Populate OptimizationResult from memoize history */
  
  for (UnsignedInteger i = 0; i < inputHistory.getSize(); ++ i)
  {
    const Point inP(inputHistory[i]);
    const Point outP(outputHistory[i]);
    constraintError = 0.0;
    
    // Constraint error on bounds    
    if (getProblem().hasBounds())
    {
      Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < getProblem().getDimension(); ++ j)
      {
        if (bounds.getFiniteLowerBound()[j])
          constraintError = std::max(constraintError, bounds.getLowerBound()[j] - inP[j]);
        if (bounds.getFiniteUpperBound()[j])
          constraintError = std::max(constraintError, inP[j] - bounds.getUpperBound()[j]);
      }
    }
    
    // Constraint error on equality constraints
    if (getProblem().hasEqualityConstraint())
    {
      const Point g(getProblem().getEqualityConstraint()(inP));
      constraintError = std::max(constraintError, g.normInf());
    }
    
    // Constraint error on inequality constraints    
    if (getProblem().hasInequalityConstraint())
    {
      Point h(getProblem().getInequalityConstraint()(inP));
      for (UnsignedInteger k = 0; k < getProblem().getInequalityConstraint().getOutputDimension(); ++ k)
      {
        h[k] = std::min(h[k], 0.0); // convention h(x)>=0 <=> admissibility
      }
      constraintError = std::max(constraintError, h.normInf());
    }
    
    // Computing errors, storing into OptimizationResult
    if (i > 0)
    {
      const Point inPM(inputHistory[i - 1]);
      const Point outPM(outputHistory[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }
    
    optimResult.store(inP,
                      outP,
                      absoluteError,
                      relativeError,
                      residualError,
                      constraintError);
  }
  
  // Optimum is not the last call to objective function
  optimResult.setOptimalPoint(tminlp->getOptimalPoint());
  optimResult.setOptimalValue(tminlp->getOptimalValue());
  
  setResult(optimResult);
  
  String allOptions;
  bonmin.options()->PrintList(allOptions);
  LOGINFO(allOptions);
  
#else
  
  throw NotYetImplementedException(HERE) << "No Bonmin support";

#endif
}

/** Options accessors */
Description Bonmin::getOptions() const
{
  Description keys(options_.getSize());
  std::vector<String> vectorKeys(options_.getKeys());
  
  std::copy(vectorKeys.begin(), vectorKeys.end(), keys.begin());
  
  return keys;
}

void Bonmin::setOptionAsString(const String & key, const String & value)
{
  options_.setAsString(key, value);
}
  
String Bonmin::getOptionAsString(const String & key) const
{
  return options_.getAsString(key);
}

void Bonmin::setOptionAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  options_.setAsUnsignedInteger(key, value);
}

UnsignedInteger Bonmin::getOptionAsUnsignedInteger(const String & key) const
{
  return options_.getAsUnsignedInteger(key);
}

void Bonmin::setOptionAsScalar(const String & key, const Scalar value)
{
  options_.setAsScalar(key, value);
}

Scalar Bonmin::getOptionAsScalar(const String & key) const
{
  return options_.getAsScalar(key);
}

Bool Bonmin::hasOption(const String & key) const
{
  return options_.hasKey(key);
}

String Bonmin::getOptionType(const String & key) const
{
  return options_.getType(key);
}

String Bonmin::getOption(const String & key) const
{
  return options_.get(key);
}

void Bonmin::removeOption(const String & key)
{
  options_.removeKey(key);
}

/** Description of object */
String Bonmin::__str__(const String &) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << "\nalgorithm=" << algoName_;
  return oss;
}

String Bonmin::__repr__() const
{
  OSS oss(false);
  oss << __str__();
  oss << "\noptions= ";
  oss << options_.__repr__();
    
  return oss;
}

END_NAMESPACE_OPENTURNS
