//                                               -*- C++ -*-
/**
 *  @brief Result implements the results obtained from the First Order Reliability Method
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
#include "openturns/FORM.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FORMResult)

static const Factory<FORMResult> Factory_FORMResult;

typedef PersistentCollection<PointWithDescription> PersistentSensitivity;

/*
 * @brief  Standard constructor: the class is defined by an optimisation algorithm, a failure event and a physical starting point
 */
FORMResult::FORMResult(const Point & standardSpaceDesignPoint,
                       const RandomVector & limitStateVariable,
                       const Bool isStandardPointOriginInFailureSpace):
  AnalyticalResult(standardSpaceDesignPoint, limitStateVariable, isStandardPointOriginInFailureSpace),
  eventProbability_(0.),
  generalisedReliabilityIndex_(0.),
  eventProbabilitySensitivity_(Sensitivity(0)),
  isAlreadyComputedEventProbabilitySensitivity_(false)
{
  computeEventProbability();
  computeGeneralisedReliabilityIndex();
}

/* Default constructor */
FORMResult::FORMResult():
  AnalyticalResult(),
  eventProbability_(0.),
  generalisedReliabilityIndex_(0.),
  eventProbabilitySensitivity_(Sensitivity(0)),
  isAlreadyComputedEventProbabilitySensitivity_(false)
{
  // Nothing to do
}

/* Virtual constructor */
FORMResult * FORMResult::clone() const
{
  return new FORMResult(*this);
}

/* The function that actually evaluates the event probability with FORM approximation */
void FORMResult::computeEventProbability() const
{
  /* evaluate the event probability */
  /* Be careful! computeCDF method takes an Point as an input argument */
  /* in the standard space all marginals of the standard distribution are identical */
  eventProbability_ = getLimitStateVariable().getImplementation()->getAntecedent().getDistribution().getStandardDistribution().getMarginal(0).computeCDF(Point(1, -getHasoferReliabilityIndex()));

  if (getIsStandardPointOriginInFailureSpace())
  {
    // isStandardPointOriginInFailureSpace is true: unusual case
    eventProbability_ = 1.0 - eventProbability_;
  }
}

/* EventProbability accessor */
Scalar FORMResult::getEventProbability() const
{
  return eventProbability_;
}

/* EventProbability accessor */
void FORMResult::setEventProbability(const Scalar & eventProbability)
{
  eventProbability_ = eventProbability;
}

/* The function that actually evaluates the generalised reliability index with FORM approximation */
void  FORMResult::computeGeneralisedReliabilityIndex() const
{
  /* GeneralisedReliabilityIndex is defined by : - Inverse standard marginal CDF (eventProbability). It
     will thus be negative if the eventProbability is > 0.5. */
  generalisedReliabilityIndex_ = -getLimitStateVariable().getImplementation()->getAntecedent().getDistribution().getStandardDistribution().getMarginal(0).computeQuantile(eventProbability_)[0];
}


/* GeneralisedReliabilityIndex accessor */
Scalar FORMResult::getGeneralisedReliabilityIndex() const
{
  return generalisedReliabilityIndex_;
}

/* GeneralisedReliabilityIndex accessor */
void FORMResult::setGeneralisedReliabilityIndex(const Scalar & generalisedReliabilityIndex)
{
  generalisedReliabilityIndex_ = generalisedReliabilityIndex;
}

/* The function that actually evaluates the  event probability sensitivity with FORM approximation */
void FORMResult::computeEventProbabilitySensitivity() const
{
  Point correctedReliabilityIndex(1, (getIsStandardPointOriginInFailureSpace() ? getHasoferReliabilityIndex() : -getHasoferReliabilityIndex()));
  Distribution antecedent(getLimitStateVariable().getImplementation()->getAntecedent().getDistribution());
  UnsignedInteger dimension = antecedent.getDimension();

  /* Be carefull! computeCDF method takes an Point as an input argument */
  /* in the standard space all marginals of the standard distribution are identical */
  /* evaluate one marginal at the reliability index : the marginal is symmetric with respect to zero */
  const Distribution standardMarginalDistribution(antecedent.getStandardDistribution().getMarginal(0));
  Scalar correctedReliabilityIndexDensity = standardMarginalDistribution.computePDF(correctedReliabilityIndex);

  if (! getIsStandardPointOriginInFailureSpace())
  {
    // isStandardPointOriginInFailureSpace is false : usual case
    correctedReliabilityIndexDensity *= -1.0;
  }
  /* We initialize eventProbabilitySensitivity_ this way in order to inherit from the name and description of hasoferReliabilityIndexSensitivity */
  eventProbabilitySensitivity_ = getHasoferReliabilityIndexSensitivity();
  /* sensitivity with respect to the parameters influencing beta (beta is not sensitive to the parameters relative to the copula type) */
  UnsignedInteger size = eventProbabilitySensitivity_.getSize();
  for(UnsignedInteger i = 0; i < size; ++i) eventProbabilitySensitivity_[i] *= correctedReliabilityIndexDensity;
  /* sensitivity with respect to the parameters of the density generator of the standard distribution */
  PointWithDescriptionCollection standardMarginalParametersCollection(standardMarginalDistribution.getParametersCollection());
  UnsignedInteger parametersDimension = standardMarginalParametersCollection[0].getDimension();
  /* there 2 parameters generic to 1D elliptical distributions : mean and standard deviation */
  UnsignedInteger genericParametersNumber = 2;
  if (antecedent.getImplementation()->hasEllipticalCopula() && parametersDimension > genericParametersNumber)
  {
    const Point standardParametersGradient(standardMarginalDistribution.computeCDFGradient(correctedReliabilityIndex));
    /* shift is the number of parameters of the correlation matrix (upper triangle) for elliptical copula*/
    const UnsignedInteger shift = dimension * (dimension - 1) / 2;
    for (UnsignedInteger index = genericParametersNumber; index < standardParametersGradient.getDimension(); ++index) eventProbabilitySensitivity_[dimension][index + shift - genericParametersNumber] = standardParametersGradient[index];
  }
  isAlreadyComputedEventProbabilitySensitivity_ = true;

}// end computeEventProbabilitySensitivity()


/* EventProbabilitySensitivity accessor */
FORMResult::Sensitivity FORMResult::getEventProbabilitySensitivity() const
{
  if (! isAlreadyComputedEventProbabilitySensitivity_) computeEventProbabilitySensitivity();
  return eventProbabilitySensitivity_;
}


/* HasoferReliabilityIndexSensitivity Graph */
AnalyticalResult::GraphCollection FORMResult::drawEventProbabilitySensitivity(Scalar width) const
{
  GraphCollection eventProbabilitySensitivityGraphCollection(0);
  // To ensure that the eventProbabilitySensitivity_ are up to date
  if (! isAlreadyComputedEventProbabilitySensitivity_) computeEventProbabilitySensitivity();
  UnsignedInteger dimension = getStandardSpaceDesignPoint().getDimension();
  UnsignedInteger size = eventProbabilitySensitivity_.getSize();

  // The first graph shows the sensitivities with respect to the marginal parameters
  Sensitivity marginalSensitivity(dimension);
  for(UnsignedInteger i = 0; i < dimension; ++i) marginalSensitivity[i] = eventProbabilitySensitivity_[i];
  Graph eventProbabilitySensitivityGraphMarginal(drawSensitivity(marginalSensitivity, width));
  OSS oss1;
  oss1 << "FORM - Event Probability Sensitivities - Marginal parameters - " << getLimitStateVariable().getName() ;
  eventProbabilitySensitivityGraphMarginal.setTitle(oss1);
  eventProbabilitySensitivityGraphCollection.add(eventProbabilitySensitivityGraphMarginal);

  // The second graph shows the sensitivities with respect to the other parameters
  if (size > dimension)
  {
    Sensitivity otherSensitivity(size - dimension);
    for(UnsignedInteger i = dimension; i < size; ++i) otherSensitivity[i - dimension] = eventProbabilitySensitivity_[i];
    Graph eventProbabilitySensitivityGraphOther(drawSensitivity(otherSensitivity, width));
    OSS oss2;
    oss2 << "FORM - Event Probability Sensitivities - Other parameters - " << getLimitStateVariable().getName() ;
    eventProbabilitySensitivityGraphOther.setTitle(oss2);
    eventProbabilitySensitivityGraphCollection.add(eventProbabilitySensitivityGraphOther);
  }
  return eventProbabilitySensitivityGraphCollection;
}


/* String converter */
String FORMResult::__repr__() const
{
  OSS oss;
  oss << "class=" << FORMResult::GetClassName()
      << " " << AnalyticalResult::__repr__()
      << " eventProbability=" << eventProbability_
      << " generalisedReliabilityIndex=" << generalisedReliabilityIndex_
      << " eventProbabilitySensitivity=" << eventProbabilitySensitivity_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FORMResult::save(Advocate & adv) const
{
  PersistentSensitivity sensitivity(eventProbabilitySensitivity_);
  AnalyticalResult::save(adv);
  adv.saveAttribute( "eventProbability_", eventProbability_ );
  adv.saveAttribute( "generalisedReliabilityIndex_", generalisedReliabilityIndex_ );
  adv.saveAttribute( "eventProbabilitySensitivity_", sensitivity );
  adv.saveAttribute( "isAlreadyComputedEventProbabilitySensitivity_", isAlreadyComputedEventProbabilitySensitivity_ );
}

/* Method load() reloads the object from the StorageManager */
void FORMResult::load(Advocate & adv)
{
  PersistentSensitivity sensitivity;
  AnalyticalResult::load(adv);
  adv.loadAttribute( "eventProbability_", eventProbability_ );
  adv.loadAttribute( "generalisedReliabilityIndex_", generalisedReliabilityIndex_ );
  adv.loadAttribute( "eventProbabilitySensitivity_", sensitivity );
  adv.loadAttribute( "isAlreadyComputedEventProbabilitySensitivity_", isAlreadyComputedEventProbabilitySensitivity_ );
  eventProbabilitySensitivity_ = sensitivity;
}

END_NAMESPACE_OPENTURNS

