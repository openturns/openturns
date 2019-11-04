//                                               -*- C++ -*-
/**
 *  @brief SORMResult implements the results obtained from the Second Order Reliability Method
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

#include "openturns/SORM.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/Function.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Log.hxx"
#include "openturns/Normal.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(SORMResult)

static const Factory<SORMResult> Factory_SORMResult;

/*
 * @brief  Standard constructor: the class is defined by an optimisation algorithm, a failure event and a physical starting point
 */
SORMResult::SORMResult(const Point & standardSpaceDesignPoint,
                       const RandomVector & limitStateVariable,
                       const Bool isStandardPointOriginInFailureSpace):
  AnalyticalResult(standardSpaceDesignPoint, limitStateVariable, isStandardPointOriginInFailureSpace),
  hessianLimitStateFunction_(),
  gradientLimitStateFunction_(),
  sortedCurvatures_(),
  isAlreadyComputedSortedCurvatures_(false),
  eventProbabilityBreitung_(-1.0),
  eventProbabilityHohenBichler_(-1.0),
  eventProbabilityTvedt_(-1.0),
  generalisedReliabilityIndexBreitung_(SpecFunc::MaxScalar),
  generalisedReliabilityIndexHohenBichler_(SpecFunc::MaxScalar),
  generalisedReliabilityIndexTvedt_(SpecFunc::MaxScalar),
  standardDistribution_(limitStateVariable.getImplementation()->getAntecedent().getImplementation()->getDistribution().getStandardDistribution()),
  standardMarginal_(standardDistribution_.getMarginal(0))
{
  /* get the physical Limite State Function */
  const Function limitStateFunction(StandardEvent(limitStateVariable).getImplementation()->getFunction());
  /* compute its gradient */
  const Matrix gradient(limitStateFunction.gradient(getStandardSpaceDesignPoint()));
  /* Get the first column */
  gradientLimitStateFunction_ = gradient * Point(1, 1.0);
  /* compute its hessian */
  const SymmetricTensor hessian(limitStateFunction.hessian(getStandardSpaceDesignPoint()));
  /* Get the first sheet */
  hessianLimitStateFunction_ = SquareMatrix(hessian.getNbRows());
  for (UnsignedInteger i = 0; i < hessian.getNbRows(); ++i)
    for (UnsignedInteger j = 0; j < hessian.getNbColumns(); ++j)
      hessianLimitStateFunction_(i, j) = hessian(i, j, 0);
} // end SORMResult::Result

/* Default constructor */
SORMResult::SORMResult():
  AnalyticalResult(),
  hessianLimitStateFunction_(),
  gradientLimitStateFunction_(),
  sortedCurvatures_(),
  isAlreadyComputedSortedCurvatures_(false),
  eventProbabilityBreitung_(-1.0),
  eventProbabilityHohenBichler_(-1.0),
  eventProbabilityTvedt_(-1.0),
  generalisedReliabilityIndexBreitung_(SpecFunc::MaxScalar),
  generalisedReliabilityIndexHohenBichler_(SpecFunc::MaxScalar),
  generalisedReliabilityIndexTvedt_(SpecFunc::MaxScalar),
  standardDistribution_(Normal(1)),
  standardMarginal_(Normal(1))
{
  // Nothing to do
}

/* Virtual constructor */
SORMResult * SORMResult::clone() const
{
  return new SORMResult(*this);
}

/* The function that actually evaluates the curvatures of the standard limite state function at the standard design point */
void SORMResult::computeSortedCurvatures() const
{
  /* see Mefisto v3.2 documentation */
  /* we compute the main curvatures */
  const UnsignedInteger dimension = getStandardSpaceDesignPoint().getDimension();
  // If the dimension is zero, throw an exception
  if (dimension == 0) throw NotDefinedException(HERE) << "Error: the curvatures cannot be computed when the dimension is zero.";
  const Scalar inverseGradientNorm = 1.0 / gradientLimitStateFunction_.norm();
  const Point unitGradientLimitStateFunction(inverseGradientNorm * gradientLimitStateFunction_);
  SquareMatrix kroneckerUnitGradientLimitStateFunction(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      kroneckerUnitGradientLimitStateFunction(i, j) = unitGradientLimitStateFunction[i] * unitGradientLimitStateFunction[j];
  /* W = (uGrad.uGrad^t -Id) * Hess(g) */
  const SquareMatrix id = IdentityMatrix(dimension);
  const SquareMatrix::ComplexCollection eigenValues(((kroneckerUnitGradientLimitStateFunction - id) * hessianLimitStateFunction_).computeEigenValues());
  Point realEigenValues(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) realEigenValues[i] = eigenValues[i].real();

  /* The curvatures are proportional to the eigenvalues of W
     If the normal of the boundary of the failure domain points to the origin at the design point, then we change
     the sign of the curvatures. It insure that a convexe failure domain will have positive curvatures */
  sortedCurvatures_ = ((gradientLimitStateFunction_.dot(getStandardSpaceDesignPoint()) > 0.0 ? 1.0 : -1.0) * inverseGradientNorm) * realEigenValues;
  /* we sort the curvatures with increasing order */
  std::sort(sortedCurvatures_.begin(), sortedCurvatures_.end());
  isAlreadyComputedSortedCurvatures_ = true;
} // end SORMResult::computeCurvatures

/* SortedCurvatures accessor */
Point SORMResult::getSortedCurvatures() const
{
  if (!isAlreadyComputedSortedCurvatures_) computeSortedCurvatures();
  return sortedCurvatures_;
}

/* The function that actually evaluates the event probability with SORM Breitung approximation
   We use flag values to avoid the explicit management of flags to tell if the probability has
   already been computed or if it cannot be computed. A value of -1.0 means that the value has
   not yet been computed and a value of -2.0 means that the value cannot be computed */
Scalar SORMResult::getEventProbabilityBreitung() const
{
  /* Quick return if the probability has already been computed */
  if ((eventProbabilityBreitung_ != -1.0) && (eventProbabilityBreitung_ != -2.0)) return eventProbabilityBreitung_;
  /* Make sure the curvatures have been computed*/
  getSortedCurvatures();
  const Scalar beta = getHasoferReliabilityIndex();
  const Point minusBeta(1, -beta);
  const Scalar standardCDFBeta = standardMarginal_.computeCDF(minusBeta);

  /* test if all curvatures verifie 1 + beta * curvature  > 0 */
  /* curvatures are sorted with increasing order and stocked in the attribute SortedCurvatures */
  if (1.0 + beta * sortedCurvatures_[0] < 0)
  {
    eventProbabilityBreitung_ = -2.0;
    throw NotDefinedException(HERE) << "Error: impossible to compute Breitung SORM probability, one of the curvatures is < -1/beta. beta=" << beta << ", curvature=" << sortedCurvatures_[0];
  }
  /* PBreitung = E(-beta)/Prod(sqrt(1+beta*curvature[i])) */
  eventProbabilityBreitung_ = standardCDFBeta;
  for (UnsignedInteger index = 0 ; index < sortedCurvatures_.getDimension(); ++index) eventProbabilityBreitung_ /= sqrt(1.0 + beta * sortedCurvatures_[index]);
  /* if the Standard Point Origin is in the failure space, take the complementry probability */
  if (getIsStandardPointOriginInFailureSpace()) eventProbabilityBreitung_ = 1.0 - eventProbabilityBreitung_;
  if ((eventProbabilityBreitung_ < 0.0) || (eventProbabilityBreitung_ > 1.0))
  {
    const Scalar badValue = eventProbabilityBreitung_;
    eventProbabilityBreitung_ = -2.0;
    throw NotDefinedException(HERE) << "Error: the probability computed using Breitung SORM approximation gives a value outside of [0, 1]:" << badValue;
  }
  return eventProbabilityBreitung_;
} // end SORMResult::getEventProbabilityBreitung

/* The function that actually evaluates the event probability with SORM HohenBichler approximation */
Scalar SORMResult::getEventProbabilityHohenBichler() const
{
  /* Quick return if the probability has already been computed */
  if ((eventProbabilityHohenBichler_ != -1.0) && (eventProbabilityHohenBichler_ != -2.0)) return eventProbabilityHohenBichler_;

  /* this formula is valid only for standard distribution with independent components */
  if (!standardDistribution_.hasIndependentCopula())
  {
    eventProbabilityHohenBichler_ = -2.0;
    throw NotDefinedException(HERE) << "Error: impossible to compute HohenBichler SORM probability for standard distributions with non independent components.";
  }
  /* Make sure the curvatures have been computed*/
  getSortedCurvatures();
  const Point minusBeta(1, -getHasoferReliabilityIndex());
  const Scalar standardPDFBeta = standardMarginal_.computePDF(minusBeta);
  const Scalar standardCDFBeta = standardMarginal_.computeCDF(minusBeta);

  const Scalar rho = standardPDFBeta / standardCDFBeta;

  /* test if all curvatures verifie 1 + rho * curvature  > 0 */
  /* curvatures are sorted with increasing order and stocked in the attribute SortedCurvatures */
  if (1.0 + rho * sortedCurvatures_[0] < 0)
  {
    eventProbabilityBreitung_ = -2.0;
    throw NotDefinedException(HERE) << "Error: impossible to compute HohenBichler SORM probability, one of the curvatures is < -1/rho. rho=" << rho << ", curvature=" << sortedCurvatures_[0];
  }
  /* P_hohenbichler = Phi(-beta)/Prod(sqrt(1+rho*curvature[i])) */
  eventProbabilityHohenBichler_ = standardCDFBeta;
  for (UnsignedInteger index = 0 ; index < sortedCurvatures_.getDimension(); ++index) eventProbabilityHohenBichler_ /= sqrt(1.0 + rho * sortedCurvatures_[index]);

  /* if the Standard Point Origin is in the failure space, take the complementry probability */
  if (getIsStandardPointOriginInFailureSpace()) eventProbabilityHohenBichler_ = 1.0 - eventProbabilityHohenBichler_;
  if ((eventProbabilityHohenBichler_ < 0.0) || (eventProbabilityHohenBichler_ > 1.0))
  {
    const Scalar badValue = eventProbabilityHohenBichler_;
    eventProbabilityHohenBichler_ = -2.0;
    throw NotDefinedException(HERE) << "Error: the probability computed using HohenBichler SORM approximation gives a value outside of [0, 1]:" << badValue;
  }
  return eventProbabilityHohenBichler_;
} // SORMResult::getEventProbabilityHohenBichler

/* The function that actually evaluates the event probability with SORM Tvedtapproximation */
Scalar SORMResult::getEventProbabilityTvedt() const
{
  /* Quick return if the probability has already been computed */
  if ((eventProbabilityTvedt_ != -1.0) && (eventProbabilityTvedt_ != -2.0)) return eventProbabilityTvedt_;
  /* this formula is valid only for standard distribution with independent components */
  if (!standardDistribution_.hasIndependentCopula())
  {
    eventProbabilityTvedt_ = -2.0;
    throw NotDefinedException(HERE) << "Error: impossible to compute Tvedt SORM probability for standard distributions with non independent components.";
  }

  /* Make sure the curvatures have been computed*/
  getSortedCurvatures();

  const Scalar beta = getHasoferReliabilityIndex();
  const Point minusBeta(1, -beta);

  /* test if all curvatures verifie 1 + (beta+1) * curvature  > 0 */
  /* curvatures are sorted with increasing order and stocked in the attribute SortedCurvatures */
  if (1.0 + (1 + beta) * sortedCurvatures_[0] < 0)
  {
    eventProbabilityTvedt_ = -2.0;
    throw NotDefinedException(HERE) << "Error: impossible to compute Tvedt SORM probability, one of the curvatures is < -1/(1+beta). beta=" << beta << ", curvature=" << sortedCurvatures_[0];
  }

  const Scalar standardPDFBeta = standardMarginal_.computePDF(minusBeta);
  const Scalar standardCDFBeta = standardMarginal_.computeCDF(minusBeta);

  /* compute the first term A1 */

  Scalar prod1 = 1.0;
  const UnsignedInteger dimension = sortedCurvatures_.getDimension();
  for (UnsignedInteger index = 0 ; index < dimension; ++index) prod1 /= sqrt(1.0 + beta * sortedCurvatures_[index]);
  const Scalar termA1 = standardCDFBeta * prod1;

  /* compute the second term A2 */

  const Scalar rho = beta * standardCDFBeta - standardPDFBeta;

  Scalar prod2 = 1.0;
  for (UnsignedInteger index = 0; index < dimension; ++index) prod2 /= sqrt(1.0 + (1.0 + beta) * sortedCurvatures_[index]);

  const Scalar termA2 = rho * (prod1 - prod2);

  /* compute the second term A3 */

  Complex complexProd3(1.0, 0.0);
  Complex iPlusBeta(beta, 1.0);

  for (UnsignedInteger index = 0; index < dimension; ++index) complexProd3 /= sqrt(1.0 + iPlusBeta * sortedCurvatures_[index]);
  const Scalar termA3 = (beta + 1.0) * rho * (prod1 - complexProd3.real());

  /* compute tvedt probability */

  eventProbabilityTvedt_ = termA1 + termA2 + termA3;
  /* if the Standard Point Origin is in the failure space, take the complementry probability */
  if (getIsStandardPointOriginInFailureSpace()) eventProbabilityTvedt_ = 1.0 - eventProbabilityTvedt_;
  if ((eventProbabilityTvedt_ < 0.0) || (eventProbabilityTvedt_ > 1.0))
  {
    const Scalar badValue = eventProbabilityTvedt_;
    eventProbabilityTvedt_ = -2.0;
    throw NotDefinedException(HERE) << "Error: the probability computed using Tvedt SORM approximation gives a value outside of [0, 1]:" << badValue;
  }
  return eventProbabilityTvedt_;
} // end SORMResult::getEventProbabilityTvedt

/* GeneralisedReliabilityIndexBreitung accessor */
Scalar SORMResult::getGeneralisedReliabilityIndexBreitung() const
{
  /* evaluate the GeneralisedReliabilityIndex */
  /* GeneralisedReliabilityIndex is defined by : - Inverse standard marginal CDF (eventProbability) in usual case or : + Inverse standard marginal CDF (eventProbability) in other case */
  // StandardPointOriginInFailureSpace is FALSE : usual case
  Scalar sign = 1.0;
  if (!getIsStandardPointOriginInFailureSpace()) sign = -1.0;

  /* Generalised reliability index with SORM Breitung approximation */
  /* Make sure that Breitung's approximation has been computed */
  getEventProbabilityBreitung();
  generalisedReliabilityIndexBreitung_ = sign * standardMarginal_.computeQuantile(eventProbabilityBreitung_)[0];
  return generalisedReliabilityIndexBreitung_;
} // end SORMResult::getGeneralisedReliabilityIndexBreitung

/* GeneralisedReliabilityIndex accessor */
Scalar SORMResult::getGeneralisedReliabilityIndexHohenBichler() const
{
  /* evaluate the GeneralisedReliabilityIndex */
  /* GeneralisedReliabilityIndex is defined by : - Inverse standard marginal CDF (eventProbability) in usual case or : + Inverse standard marginal CDF (eventProbability) in other case */
  // StandardPointOriginInFailureSpace is FALSE : usual case
  Scalar sign = 1.0;
  if (!getIsStandardPointOriginInFailureSpace()) sign = -1.0;

  /* Generalised reliability index with SORM HohenBichler approximation */
  /* Make sure that HohenBichler's approximation has been computed */
  getEventProbabilityHohenBichler();
  generalisedReliabilityIndexHohenBichler_ = sign * standardMarginal_.computeQuantile(eventProbabilityHohenBichler_)[0];
  return generalisedReliabilityIndexHohenBichler_;
}

/* GeneralisedReliabilityIndex accessor */
Scalar SORMResult::getGeneralisedReliabilityIndexTvedt() const
{
  /* evaluate the GeneralisedReliabilityIndex */
  /* GeneralisedReliabilityIndex is defined by : - Inverse standard marginal CDF (eventProbability) in usual case or : + Inverse standard marginal CDF (eventProbability) in other case */
  // StandardPointOriginInFailureSpace is FALSE : usual case
  Scalar sign = 1.0;
  if (!getIsStandardPointOriginInFailureSpace()) sign = -1.0;

  /* Generalised reliability index with SORM Tvedt approximation */
  /* Make sure that Tvedt's approximation has been computed */
  getEventProbabilityTvedt();
  generalisedReliabilityIndexTvedt_ = sign * standardMarginal_.computeQuantile(eventProbabilityTvedt_)[0];
  return generalisedReliabilityIndexTvedt_;
}

/* String converter */
String SORMResult::__repr__() const
{
  OSS oss;
  oss << "class=" << SORMResult::GetClassName()
      << " " << AnalyticalResult::__repr__()
      << " sortedCurvatures=" << sortedCurvatures_
      << " eventProbabilityBreitung=" << eventProbabilityBreitung_
      << " eventProbabilityHohenBichler=" << eventProbabilityHohenBichler_
      << " eventProbabilityTvedt=" << eventProbabilityTvedt_
      << " generalisedReliabilityIndexBreitung=" << generalisedReliabilityIndexBreitung_
      << " generalisedReliabilityIndexHohenBichler=" << generalisedReliabilityIndexHohenBichler_
      << " generalisedReliabilityIndexTvedt=" << generalisedReliabilityIndexTvedt_
      << " gradientLimitStateFunction_=" << gradientLimitStateFunction_
      << " hessianLimitStateFunction_=" << hessianLimitStateFunction_;
  return oss;
}

/* String converter */
String SORMResult::__str__(const String & offset) const
{
  OSS oss;
  try
  {
    oss << "Probability estimate    (Breitung)=" << getEventProbabilityBreitung() << Os::GetEndOfLine() << offset;
    oss << "Generalised reliability (Breitung)=" << getGeneralisedReliabilityIndexBreitung() << Os::GetEndOfLine() << offset;
  }
  catch (...)
  {
    oss << "Probability estimate and generalised reliability index (Breitung) not defined." << Os::GetEndOfLine() << offset;
  }
  try
  {
    oss << "Probability estimate    (HohenBichler)=" << getEventProbabilityHohenBichler() << Os::GetEndOfLine() << offset;
    oss << "Generalised reliability (HohenBichler)=" << getGeneralisedReliabilityIndexHohenBichler() << Os::GetEndOfLine() << offset;
  }
  catch (...)
  {
    oss << "Probability estimate and generalised reliability index (HohenBichler) not defined." << Os::GetEndOfLine() << offset;
  }
  try
  {
    oss << "Probability estimate (Tvedt)=" << getEventProbabilityTvedt() << Os::GetEndOfLine() << offset;
    oss << "Generalised reliability (Tvedt)" << getGeneralisedReliabilityIndexTvedt() << Os::GetEndOfLine() << offset;
  }
  catch (...)
  {
    oss << "Probability estimate and generalised reliability index (Tvedt) not defined." << Os::GetEndOfLine() << offset;
  }
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SORMResult::save(Advocate & adv) const
{
  AnalyticalResult::save(adv);
  adv.saveAttribute( "hessianLimitStateFunction_", hessianLimitStateFunction_ );
  adv.saveAttribute( "gradientLimitStateFunction_", gradientLimitStateFunction_ );
  adv.saveAttribute( "sortedCurvatures_", sortedCurvatures_ );
  adv.saveAttribute( "isAlreadyComputedSortedCurvatures_", isAlreadyComputedSortedCurvatures_ );
  adv.saveAttribute( "eventProbabilityBreitung_", eventProbabilityBreitung_ );
  adv.saveAttribute( "eventProbabilityHohenBichler_", eventProbabilityHohenBichler_ );
  adv.saveAttribute( "eventProbabilityTvedt_", eventProbabilityTvedt_ );
  adv.saveAttribute( "generalisedReliabilityIndexBreitung_", generalisedReliabilityIndexBreitung_ );
  adv.saveAttribute( "generalisedReliabilityIndexHohenBichler_", generalisedReliabilityIndexHohenBichler_ );
  adv.saveAttribute( "generalisedReliabilityIndexTvedt_", generalisedReliabilityIndexTvedt_ );
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "standardMarginal_", standardMarginal_ );
}

/* Method load() reloads the object from the StorageManager */
void SORMResult::load(Advocate & adv)
{
  AnalyticalResult::load(adv);
  adv.loadAttribute( "hessianLimitStateFunction_", hessianLimitStateFunction_ );
  adv.loadAttribute( "gradientLimitStateFunction_", gradientLimitStateFunction_ );
  adv.loadAttribute( "sortedCurvatures_", sortedCurvatures_ );
  adv.loadAttribute( "isAlreadyComputedSortedCurvatures_", isAlreadyComputedSortedCurvatures_ );
  adv.loadAttribute( "eventProbabilityBreitung_", eventProbabilityBreitung_ );
  adv.loadAttribute( "eventProbabilityHohenBichler_", eventProbabilityHohenBichler_ );
  adv.loadAttribute( "eventProbabilityTvedt_", eventProbabilityTvedt_ );
  adv.loadAttribute( "generalisedReliabilityIndexBreitung_", generalisedReliabilityIndexBreitung_ );
  adv.loadAttribute( "generalisedReliabilityIndexHohenBichler_", generalisedReliabilityIndexHohenBichler_ );
  adv.loadAttribute( "generalisedReliabilityIndexTvedt_", generalisedReliabilityIndexTvedt_ );
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "standardMarginal_", standardMarginal_ );
}

END_NAMESPACE_OPENTURNS
