//                                               -*- C++ -*-
/**
 *  @brief Implement the Cuba mathod for multidimensional integration
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
#include "openturns/CubaIntegration.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#ifdef OPENTURNS_HAVE_CUBA
#include <cuba.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CubaIntegration
 */

CLASSNAMEINIT(CubaIntegration)

static const Factory<CubaIntegration> Factory_CubaIntegration;

/* Parameters constructor */
CubaIntegration::CubaIntegration(const String algorithmName)
  : IntegrationAlgorithmImplementation()
  , maximumRelativeError_(ResourceMap::GetAsScalar("CubaIntegration-DefaultMaximumRelativeError"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("CubaIntegration-DefaultMaximumAbsoluteError"))
  , maximumCallsNumber_(ResourceMap::GetAsUnsignedInteger("CubaIntegration-DefaultMaximumCallsNumber"))
{
  setAlgorithmName(algorithmName);
}

/* Virtual constructor */
CubaIntegration * CubaIntegration::clone() const
{
  return new CubaIntegration(*this);
}

/*
 * The static routine to compute the integrand, that is provided to the Cuba routine.
 * It calls the function one wants to integrate after changing coordinates and
 * multiplying by the volume of the interval as Cuba routines perform integration
 * over the unit hypercube.
 */
int CubaIntegration::ComputeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata)
{
  Point ptIn(*ndim);
  void** ppFunctionInterval = (void**)(userdata);
  const Function* const pFunction = (Function*)(ppFunctionInterval[0]);
  const Interval* const pInterval = (Interval*)(ppFunctionInterval[1]);
  const Scalar intervalVolume = pInterval->getVolume();
  const Point lb(pInterval->getLowerBound());
  const Point ub(pInterval->getUpperBound());
  for (UnsignedInteger i = 0 ; i < static_cast<UnsignedInteger>(*ndim) ; ++ i)
  {
    ptIn[i] = lb[i] + (ub[i] - lb[i]) * x[i];
  }
  /* Cuba computes an integral over the unit hypercube, one has to multiply its
     output by the volume of the true integration interval. */
  const Point ptOut(intervalVolume * (*pFunction)(ptIn));
  if (ptOut.getSize() != static_cast<UnsignedInteger>(*ncomp))
  {
    throw InvalidArgumentException(HERE) << "Error: expected number of components and dimension of the function output do not match, here expected number of components= " << *ncomp << " and dimension of the function output=" << ptOut.getSize();
  }
  std::copy(ptOut.begin(), ptOut.end(), f);

  return 0;
}

/* Compute an approximation of \int_{I}f(x)dx, where I
 * is an multidimensional interval and f a multidimensional function
 */
Point CubaIntegration::integrate(const Function & function,
                                 const Interval & interval) const
{
  if (function.getInputDimension() != interval.getDimension())
    throw InvalidArgumentException(HERE) << "Error: interval dimension and input dimension have to match, here interval dimension=" << interval.getDimension() << " and input dimension=" << function.getInputDimension();
#ifdef OPENTURNS_HAVE_CUBA
  const UnsignedInteger inputDimension = interval.getDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  /*
   * For the number of regions, of evaluations, and the failure indicator, pointers
   * to int, not long (aka SignedInteger) are expected by Cuba.
   */
  int nregions = 0;
  int neval = 0;
  int fail = 0;
  Point integral(outputDimension);
  Point error(outputDimension);
  Point prob(outputDimension);

  /* Providing the integrator with pointers to the function and to the integration interval */
  void* ppFunctionInterval[2];
  ppFunctionInterval[0] = (void*)(&function);
  ppFunctionInterval[1] = (void*)(&interval);

  /* Setting constant parameters common to all routines */
  const int nvec = 1; /* Maximum number of points to be given to the integrand at each invocation */
  const int mineval = ResourceMap::GetAsUnsignedInteger("CubaIntegration-mineval"); /* The minimum number of integrand evaluations required */
  const int flags = ResourceMap::GetAsUnsignedInteger("CubaIntegration-flags");
  const int seed = ResourceMap::GetAsUnsignedInteger("CubaIntegration-seed"); /* The seed for the pseudo-random-number generator */

  if ((inputDimension < 2) && Description({"cuhre", "divonne"}).contains(algorithmName_))
    throw InvalidArgumentException(HERE) << "Cuba minimum input dimension for cuhre/divonne is 2, got 1";
  if (inputDimension > 1024)
    throw InvalidArgumentException(HERE) << "Cuba maximum input dimension is 1024, got " << inputDimension;
  if (outputDimension > 1024)
    throw InvalidArgumentException(HERE) << "Cuba maximum output dimension is 1024, got " << outputDimension;

  /* Calling the chosen optimization routine, with specific parameters for each of them */
  if (algorithmName_ == "cuhre")
  {
    /* Cuhre-specific parameters */
    const UnsignedInteger key = ResourceMap::GetAsUnsignedInteger("CubaIntegration-cuhre-key"); /* Default integration rule */

    Cuhre(inputDimension, outputDimension, ComputeIntegrand,
          (void*)(ppFunctionInterval), nvec, maximumRelativeError_,
          maximumAbsoluteError_, flags, mineval, maximumCallsNumber_,
          key, NULL, NULL, &nregions, &neval, &fail,
          const_cast<double*>(integral.data()),
          const_cast<double*>(error.data()),
          const_cast<double*>(prob.data()));
  }
  else if (algorithmName_ == "divonne")
  {
    /* Divonne-specific parameters */
    const int key1 = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-key1"); /* Sampling parameter for the partitioning phase */
    const int key2 = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-key2"); /* Sampling parameter for the final integration phase */
    const int key3 = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-key3"); /* Strategy for the refinement phase */
    const int maxpass = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-maxpass"); /* Thoroughness of the partitioning phase */
    const cubareal border = ResourceMap::GetAsScalar("CubaIntegration-divonne-border"); /* Width of the border of the integration region */
    const cubareal maxchisq = ResourceMap::GetAsScalar("CubaIntegration-divonne-maxchisq"); /* Maximum chisq value a single subregion is allowed to have in the final integration phase */
    const cubareal mindeviation = ResourceMap::GetAsScalar("CubaIntegration-divonne-mindeviation"); /* A bound related to further investigating a region */
    const int ngiven = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-ngiven"); /* No points at which peaks are expected */
    const int nextra = ResourceMap::GetAsUnsignedInteger("CubaIntegration-divonne-nextra"); /* No peakfinder routine */

    Divonne(inputDimension, outputDimension, ComputeIntegrand,
            (void*)(ppFunctionInterval), nvec, maximumRelativeError_,
            maximumAbsoluteError_, flags, seed, mineval,
            maximumCallsNumber_, key1, key2, key3, maxpass, border,
            maxchisq, mindeviation, ngiven, inputDimension, NULL, nextra,
            NULL, NULL, NULL, &nregions, &neval, &fail,
            const_cast<double*>(integral.data()),
            const_cast<double*>(error.data()),
            const_cast<double*>(prob.data()));
  }
  else if (algorithmName_ == "suave")
  {
    /* Suave-specific parameters */
    const int nnew = ResourceMap::GetAsUnsignedInteger("CubaIntegration-suave-nnew"); /* Number of integrand evaluations in each subdivision */
    const int nmin = ResourceMap::GetAsUnsignedInteger("CubaIntegration-suave-nmin"); /* Minimum number of samples a former pass must contribute to a subregion */
    const cubareal flatness = ResourceMap::GetAsScalar("CubaIntegration-suave-flatness"); /* Type of norm used to compute the fluctuation in a sample */

    Suave(inputDimension, outputDimension, ComputeIntegrand,
          (void*)(ppFunctionInterval), nvec, maximumRelativeError_,
          maximumAbsoluteError_, flags, seed, mineval,
          maximumCallsNumber_, nnew, nmin, flatness, NULL, NULL,
          &nregions, &neval, &fail, const_cast<double*>(integral.data()),
          const_cast<double*>(error.data()), const_cast<double*>(prob.data()));
  }
  else if (algorithmName_ == "vegas")
  {
    /* Vegas-specific parameters */
    const int nstart = ResourceMap::GetAsUnsignedInteger("CubaIntegration-vegas-nstart"); /* Number of integrand evaluations per iteration to start with */
    const int nincrease = ResourceMap::GetAsUnsignedInteger("CubaIntegration-vegas-nincrease"); /* Increase in the number of evaluations per iteration */
    const int nbatch = ResourceMap::GetAsUnsignedInteger("CubaIntegration-vegas-nbatch"); /* Batch size for sampling */
    const int gridno = ResourceMap::GetAsUnsignedInteger("CubaIntegration-vegas-gridno"); /* Slot in the internal grid table */

    Vegas(inputDimension, outputDimension, ComputeIntegrand,
          (void*)(ppFunctionInterval), nvec, maximumRelativeError_,
          maximumAbsoluteError_, flags, seed, mineval,
          maximumCallsNumber_, nstart, nincrease, nbatch, gridno, NULL,
          NULL, &neval, &fail, const_cast<double*>(integral.data()),
          const_cast<double*>(error.data()), const_cast<double*>(prob.data()));
  }

  if (fail != 0)
    throw InternalException(HERE) << "Cuba integration failed with error code " << fail;

  return integral;
#else
  throw NotYetImplementedException(HERE) << "No Cuba support";
#endif
}

/* maximumRelativeError accessor */
Scalar CubaIntegration::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

void CubaIntegration::setMaximumRelativeError(const Scalar maximumRelativeError)
{
  if (!(maximumRelativeError > 0)) throw InvalidArgumentException(HERE) << "Error: maximumRelativeError must be positive, here maximumRelativeError=" << maximumRelativeError;
  maximumRelativeError_ = maximumRelativeError;
}

/* maximumAbsoluteError accessor */
Scalar CubaIntegration::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

void CubaIntegration::setMaximumAbsoluteError(const Scalar maximumAbsoluteError)
{
  if (!(maximumAbsoluteError > 0)) throw InvalidArgumentException(HERE) << "Error: maximumAbsoluteError must be positive, here maximumAbsoluteError=" << maximumAbsoluteError;
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* algorithmName accessor */
String CubaIntegration::getAlgorithmName() const
{
  return algorithmName_;
}

void CubaIntegration::setAlgorithmName(const String algorithmName)
{
  if (!GetAlgorithmNames().contains(algorithmName)) throw InvalidArgumentException(HERE) << "Error: the name of the optimisation routine should be \"cuhre\", \"divonne\", \"suave\", or \"vegas\", here optRoutine=" << algorithmName;
  algorithmName_ = algorithmName;
}

/* maximumCallsNumber accessor */
UnsignedInteger CubaIntegration::getMaximumCallsNumber() const
{
  return maximumCallsNumber_;
}

void CubaIntegration::setMaximumCallsNumber(const UnsignedInteger maximumCallsNumber)
{
  maximumCallsNumber_ = maximumCallsNumber;
}

/* Return the names of the available integration algorithms */
Description CubaIntegration::GetAlgorithmNames()
{
  return {"cuhre", "divonne", "suave", "vegas"};
}

/* String converter */
String CubaIntegration::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CubaIntegration::GetClassName()
      << ", maximumRelativeError=" << maximumRelativeError_
      << ", maximumAbsoluteError=" << maximumAbsoluteError_
      << ", algorithmName=" << algorithmName_
      << ", maximumCallsNumber=" << maximumCallsNumber_;
  return oss;
}

/* String converter */
String CubaIntegration::__str__(const String & ) const
{
  OSS oss(false);
  oss << CubaIntegration::GetClassName()
      << "(algorithmName=" << algorithmName_
      << ", maximumRelativeError=" << maximumRelativeError_
      << ", maximumAbsoluteError=" << maximumAbsoluteError_
      << ", maximumCallsNumber=" << maximumCallsNumber_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CubaIntegration::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute("algorithmName_", algorithmName_);
  adv.saveAttribute("maximumCallsNumber_", maximumCallsNumber_);
}

/* Method load() reloads the object from the StorageManager */
void CubaIntegration::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("maximumRelativeError_", maximumRelativeError_);
  adv.loadAttribute("maximumAbsoluteError_", maximumAbsoluteError_);
  adv.loadAttribute("algorithmName_", algorithmName_);
  adv.loadAttribute("maximumCallsNumber_", maximumCallsNumber_);
}

END_NAMESPACE_OPENTURNS
