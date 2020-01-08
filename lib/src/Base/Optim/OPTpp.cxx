//                                               -*- C++ -*-
/**
 *  @brief OPT++ solver
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
#include "openturns/OPTpp.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

#ifdef OPENTURNS_HAVE_OPTPP
#include "openturns/OPTppImplementation.hxx"
#include "openturns/OptGSS.hxx"
#endif

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(OPTpp)

static const Factory<OPTpp> Factory_OPTpp;


/* Static methods to access algorithm names and codes */
Description OPTpp::GetAlgorithmNames()
{
#ifdef OPENTURNS_HAVE_OPTPP
  return OPTppImplementation::GetAlgorithmNames();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}


void OPTpp::initialize(const String & algoName)
{
#ifdef OPENTURNS_HAVE_OPTPP
  if (algoName == "OptBaNewton")
    p_impl_ = new OptBaNewton(getProblem());
  else if (algoName == "OptBaQNewton")
    p_impl_ = new OptBaQNewton(getProblem());
  else if (algoName == "OptBCEllipsoid")
    p_impl_ = new OptBCEllipsoid(getProblem());
  else if (algoName == "OptBCFDNewton")
    p_impl_ = new OptBCFDNewton(getProblem());
  else if (algoName == "OptBCNewton")
    p_impl_ = new OptBCNewton(getProblem());
  else if (algoName == "OptBCQNewton")
    p_impl_ = new OptBCQNewton(getProblem());
  else if (algoName == "OptCG")
    p_impl_ = new OptCG(getProblem());
  else if (algoName == "OptConstrFDNewton")
    p_impl_ = new OptConstrFDNewton(getProblem());
  else if (algoName == "OptConstrNewton")
    p_impl_ = new OptConstrNewton(getProblem());
  else if (algoName == "OptConstrQNewton")
    p_impl_ = new OptConstrQNewton(getProblem());
  else if (algoName == "OptDHNIPS")
    p_impl_ = new OptDHNIPS(getProblem());
  else if (algoName == "OptFDNewton")
    p_impl_ = new OptFDNewton(getProblem());
  else if (algoName == "OptFDNIPS")
    p_impl_ = new OptFDNIPS(getProblem());
  else if (algoName == "OptGSS")
    p_impl_ = new OptGSS(getProblem());
  else if (algoName == "OptLBFGS")
    p_impl_ = new OptLBFGS(getProblem());
  else if (algoName == "OptNewton")
    p_impl_ = new OptNewton(getProblem());
  else if (algoName == "OptNIPS")
    p_impl_ = new OptNIPS(getProblem());
  else if (algoName == "OptPDS")
    p_impl_ = new OptPDS(getProblem());
  else if (algoName == "OptQNewton")
    p_impl_ = new OptQNewton(getProblem());
  else if (algoName == "OptQNIPS")
    p_impl_ = new OptQNIPS(getProblem());
  else
    throw InvalidArgumentException(HERE) << "Invalid solver name: " << algoName;
#else
  (void)algoName;
#endif
}

/* Default constructor */
OPTpp::OPTpp(const String & algoName)
  : OptimizationAlgorithmImplementation()
{
  initialize(algoName);
}

OPTpp::OPTpp(const OptimizationProblem & problem,
             const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
{
  initialize(algoName);
}

/* Virtual constructor */
OPTpp * OPTpp::clone() const
{
  return new OPTpp(*this);
}

/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void OPTpp::setProblem(const OptimizationProblem & problem)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setProblem(problem);
#else
  (void)problem;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}


/* Performs the actual computation by calling the OPTpp library
 */
void OPTpp::run()
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setStartingPoint(getStartingPoint());
  p_impl_->setMaximumAbsoluteError(getMaximumAbsoluteError());
  p_impl_->setMaximumConstraintError(getMaximumConstraintError());
  p_impl_->setMaximumEvaluationNumber(getMaximumEvaluationNumber());
  p_impl_->setMaximumIterationNumber(getMaximumIterationNumber());
  p_impl_->setMaximumRelativeError(getMaximumRelativeError());
  p_impl_->setMaximumResidualError(getMaximumResidualError());
  p_impl_->setProgressCallback(progressCallback_.first, progressCallback_.second);
  p_impl_->setStopCallback(stopCallback_.first, stopCallback_.second);
  p_impl_->run();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

OptimizationResult OPTpp::getResult() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getResult();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/** Minimum step accessor */
void OPTpp::setMinimumStep(const Scalar minimumStep)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setMinimumStep(minimumStep);
#else
  (void)minimumStep;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

Scalar OPTpp::getMinimumStep() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getMinimumStep();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/** Maximum step accessor */
void OPTpp::setMaximumStep(const Scalar maximumStep)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setMaximumStep(maximumStep);
#else
  (void)maximumStep;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

Scalar OPTpp::getMaximumStep() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getMaximumStep();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/** Maximum line search tolerance accessor */
void OPTpp::setLineSearchTolerance(const Scalar lineSearchTolerance)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setLineSearchTolerance(lineSearchTolerance);
#else
  (void)lineSearchTolerance;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

Scalar OPTpp::getLineSearchTolerance() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getLineSearchTolerance();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/** Maximum line search iteration number accessor */
void OPTpp::setMaximumLineSearchIterationNumber(const UnsignedInteger maximumLineSearchIterationNumber)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setMaximumLineSearchIterationNumber(maximumLineSearchIterationNumber);
#else
  (void)maximumLineSearchIterationNumber;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

UnsignedInteger OPTpp::getMaximumLineSearchIterationNumber() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getMaximumLineSearchIterationNumber();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/** Trust region size accessor */
void OPTpp::setTrustRegionSize(const Scalar trustRegionSize)
{
#ifdef OPENTURNS_HAVE_OPTPP
  p_impl_->setTrustRegionSize(trustRegionSize);
#else
  (void)trustRegionSize;
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

Scalar OPTpp::getTrustRegionSize() const
{
#ifdef OPENTURNS_HAVE_OPTPP
  return p_impl_->getTrustRegionSize();
#else
  throw NotYetImplementedException(HERE) << "No OPTpp support";
#endif
}

/* String converter */
String OPTpp::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
#ifdef OPENTURNS_HAVE_OPTPP
      << "implementation=" << p_impl_->__repr__()
#endif
      << " " << OptimizationAlgorithmImplementation::__repr__();
  return oss;
}

/* String converter */
String OPTpp::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << getClassName();
#ifdef OPENTURNS_HAVE_OPTPP
  oss << " " << p_impl_->__str__(offset);
#endif
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OPTpp::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
#ifdef OPENTURNS_HAVE_OPTPP
  adv.saveAttribute("implementation_", *p_impl_);
#endif
}

/* Method load() reloads the object from the StorageManager */
void OPTpp::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
#ifdef OPENTURNS_HAVE_OPTPP
  TypedInterfaceObject<OPTppImplementation> interface;
  adv.loadAttribute("implementation_", interface);
  p_impl_ = interface.getImplementation();
#endif
}

Bool OPTpp::IsAvailable()
{
#ifdef OPENTURNS_HAVE_OPTPP
  return true;
#else
  return false;
#endif
}


END_NAMESPACE_OPENTURNS

