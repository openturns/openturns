//                                               -*- C++ -*-
/**
 *  @brief Implement the CUba mathod for multidimensional integration
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

int computeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata);

/**
 * @class CubaIntegration
 */

CLASSNAMEINIT(CubaIntegration)

static const Factory<CubaIntegration> Factory_CubaIntegration;

/* Constructor without parameters */
CubaIntegration::CubaIntegration()
  : IntegrationAlgorithmImplementation()
  , optRoutine_("vegas")
{
  // Nothing to do
}

/* Parameters constructor */
CubaIntegration::CubaIntegration(const String optRoutine)
  : IntegrationAlgorithmImplementation()
  , optRoutine_(optRoutine)
{
  // Check the name of the routine
  if (optRoutine != "cuhre" && optRoutine != "divonne" && optRoutine != "suave" && optRoutine != "vegas") throw InvalidArgumentException(HERE) << "Error: the name of the optimisation routine should be \"cuhre\", \"divonne\", \"suave\", or \"vegas\", here optRoutine=" << optRoutine;
}

/* Virtual constructor */
CubaIntegration * CubaIntegration::clone() const
{
  return new CubaIntegration(*this);
}

/*
 * The friend C routine to compute the integrand, that is provided to the Cuba routine.
 * It calls the function one wants to integrate after changing coordinates and
 * multiplying by the volume of the interval as Cuba routines perform integration
 * over the unit hypercube.
 */
int computeIntegrand(const int *ndim, const double x[], const int *ncomp, double f[], void *userdata)
{
  Point ptIn(*ndim);
  void** ppFunctionInterval = (void**)(userdata);
  const Function* const pFunction = (Function*)(ppFunctionInterval[0]);
  const Interval* const pInterval = (Interval*)(ppFunctionInterval[1]);
  const Scalar intervalVolume = pInterval->getVolume();
  const Point lb = pInterval->getLowerBound();
  const Point ub = pInterval->getUpperBound();
  for (UnsignedInteger i = 0 ; i < static_cast<UnsignedInteger>(*ndim) ; i++)
  {
    ptIn[i] = lb[i] + (ub[i] - lb[i]) * *(x + i);
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
#ifdef OPENTURNS_HAVE_CUBA
  if (function.getInputDimension() != interval.getDimension()) throw InvalidArgumentException(HERE) << "Error: interval dimension and input dimension have to match, here interval dimension="<< interval.getDimension() << " and input dimension=" << function.getInputDimension();
  const UnsignedInteger inputDimension = interval.getDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  /*
   * For the number of regions, of evaluations, and the failure indicator, pointers
   * to int, not long (aka SignedInteger) are expected by Cuba.
   */
  int nregions;
  int neval;
  int fail;
  Point integral(outputDimension);
  Point error(outputDimension);
  Point prob(outputDimension);

  /* Providing the integrator with pointers to the function and to the integration interval */
  void* ppFunctionInterval[2];
  ppFunctionInterval[0] = (void*)(&function);
  ppFunctionInterval[1] = (void*)(&interval);

  /* Setting constant parameters common to all routines */
  const UnsignedInteger nvec = 1; /* Maximum number of points to be given to the integrand at each invocation */
  const UnsignedInteger mineval = 0; /* The minimum number of integrand evaluations required */
  const SignedInteger seed = 0; /* The seed for the pseudo-random-number generator */

  /* Calling the chosen optimization routine, with specific parameters for each of them */
  if (optRoutine_ == "cuhre")
  {
    /* Cuhre-specific parameters */
    const UnsignedInteger key = 0; /* Default integration rule */

    Cuhre(inputDimension, outputDimension, computeIntegrand,
            (void*)(ppFunctionInterval), nvec, epsRel_, epsAbs_, flags_,
            mineval, maxeval_, key, NULL, NULL, &nregions, &neval, &fail,
            const_cast<double*>(integral.data()),
            const_cast<double*>(error.data()),
            const_cast<double*>(prob.data()));
  }
  else if (optRoutine_ == "divonne")
  {
    /* Divonne-specific parameters */
    const SignedInteger key1 = 47; /* Sampling parameter for the partitioning phase */
    const SignedInteger key2 = 1; /* Sampling parameter for the final integration phase */
    const SignedInteger key3 = 1; /* Strategy for the refinement phase */
    const SignedInteger maxpass = 5; /* Thoroughness of the partitioning phase */
    const Scalar border = 0.; /* Width of the border of the integration region */
    const Scalar maxchisq = 10.; /* Maximum chisq value a single subregion is allowed to have in the final integration phase */
    const Scalar mindeviation = 0.25; /* A bound related to further investigating a region */
    const SignedInteger ngiven = 0; /* No points at which peaks are expected */
    const SignedInteger nextra = 0; /* No peakfinder routine */

    Divonne(inputDimension, outputDimension, computeIntegrand,
              (void*)(ppFunctionInterval), nvec, epsRel_, epsAbs_, flags_,
              seed, mineval, maxeval_, key1, key2, key3, maxpass, border,
              maxchisq, mindeviation, ngiven, inputDimension, NULL, nextra,
              NULL, NULL, NULL, &nregions, &neval, &fail,
              const_cast<double*>(integral.data()),
              const_cast<double*>(error.data()),
              const_cast<double*>(prob.data()));
  }
  else if (optRoutine_ == "suave")
  {
    /* Suave-specific parameters */
    const SignedInteger nnew = 1000; /* Number of integrand evaluations in each subdivision */
    const SignedInteger nmin = 2; /* Minimum number of samples a former pass must contribute to a subregion */
    const Scalar flatness = 25.; /* Type of norm used to compute the fluctuation in a sample */

    Suave(inputDimension, outputDimension, computeIntegrand,
            (void*)(ppFunctionInterval), nvec, epsRel_, epsAbs_, flags_,
            seed, mineval, maxeval_, nnew, nmin, flatness, NULL, NULL,
            &nregions, &neval, &fail, const_cast<double*>(integral.data()),
            const_cast<double*>(error.data()), const_cast<double*>(prob.data()));
  }
  else if (optRoutine_ == "vegas")
  {
    /* Vegas-specific parameters */
    const SignedInteger nstart = 1000; /* Number of integrand evaluations per iteration to start with */
    const SignedInteger nincrease = 500; /* Increase in the number of evaluations per iteration */
    const SignedInteger nbatch = 1000; /* Batch size for sampling */
    const SignedInteger gridno = 0; /* Slot in the internal grid table */

    Vegas(inputDimension, outputDimension, computeIntegrand,
            (void*)(ppFunctionInterval), nvec, epsRel_, epsAbs_, flags_,
            seed, mineval, maxeval_, nstart, nincrease, nbatch, gridno, NULL,
            NULL, &neval, &fail, const_cast<double*>(integral.data()),
            const_cast<double*>(error.data()), const_cast<double*>(prob.data()));
  }
  else
  {
    throw InvalidArgumentException(HERE) << "Integration has been required with an unknown routine name: " << optRoutine_;
  }

  if (fail != 0)
  {
    throw InternalException(HERE) << "Error: integration failed: Cuba error code is " << fail << ".";
  }

  return integral;
#else
  throw NotYetImplementedException(HERE) << "No Cuba support";
#endif
}

/* epsRel accessor */
Scalar CubaIntegration::getEpsRel() const
{
  return epsRel_;
}

void CubaIntegration::setEpsRel(const Scalar epsRel)
{
  if (!(epsRel > 0)) throw InvalidArgumentException(HERE) << "Error: epsRel must be positive, here epsRel=" << epsRel;
  epsRel_ = epsRel;
}

/* epsAbs accessor */
Scalar CubaIntegration::getEpsAbs() const
{
  return epsAbs_;
}

void CubaIntegration::setEpsAbs(const Scalar epsAbs)
{
  if (!(epsAbs > 0)) throw InvalidArgumentException(HERE) << "Error: epsAbs must be positive, here epsAbs=" << epsAbs;
  epsAbs_ = epsAbs;
}

/* optRoutine accessor */
String CubaIntegration::getOptRoutine() const
{
  return optRoutine_;
}

void CubaIntegration::setOptRoutine(const String optRoutine)
{
  if (optRoutine != "cuhre" && optRoutine != "divonne" && optRoutine != "suave" && optRoutine != "vegas") throw InvalidArgumentException(HERE) << "Error: the name of the optimisation routine should be \"cuhre\", \"divonne\", \"suave\", or \"vegas\", here optRoutine=" << optRoutine;
  optRoutine_ = optRoutine;
}

/* maxeval accessor */
UnsignedInteger CubaIntegration::getMaxeval() const
{
  return maxeval_;
}

void CubaIntegration::setMaxeval(const UnsignedInteger maxeval)
{
  maxeval_ = maxeval;
}

/* flags accessor */
UnsignedInteger CubaIntegration::getFlags() const
{
  return flags_;
}

void CubaIntegration::setFlags(const UnsignedInteger flags)
{
  flags_ = flags;
}

/* String converter */
String CubaIntegration::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CubaIntegration::GetClassName()
      << ", epsRel=" << epsRel_
      << ", epsAbs=" << epsAbs_
      << ", optRoutine=" << optRoutine_
      << ", maxeval=" << maxeval_
      << ", flags=" << flags_;
  return oss;
}

/* String converter */
String CubaIntegration::__str__(const String & ) const
{
  OSS oss(false);
  oss << CubaIntegration::GetClassName()
      << "(epsRel=" << epsRel_
      << ", epsAbs=" << epsAbs_
      << ", optRoutine=" << optRoutine_
      << ", maxeval=" << maxeval_
      << ", flags=" << flags_
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CubaIntegration::save(Advocate & adv) const
{
  IntegrationAlgorithmImplementation::save(adv);
  adv.saveAttribute("epsRel_", epsRel_);
  adv.saveAttribute("epsAbs_", epsAbs_);
  adv.saveAttribute("optRoutine_", optRoutine_);
  adv.saveAttribute("maxeval_", maxeval_);
  adv.saveAttribute("flags_", flags_);
}

/* Method load() reloads the object from the StorageManager */
void CubaIntegration::load(Advocate & adv)
{
  IntegrationAlgorithmImplementation::load(adv);
  adv.loadAttribute("epsRel_", epsRel_);
  adv.loadAttribute("epsAbs_", epsAbs_);
  adv.loadAttribute("optRoutine_", optRoutine_);
  adv.loadAttribute("maxeval_", maxeval_);
  adv.loadAttribute("flags_", flags_);
}

END_NAMESPACE_OPENTURNS
