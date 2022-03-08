//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/RandomVectorMetropolisHastings.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(RandomVectorMetropolisHastings)

static const Factory<RandomVectorMetropolisHastings> Factory_RandomVectorMetropolisHastings;

/* Default constructor */
RandomVectorMetropolisHastings::RandomVectorMetropolisHastings()
  : MetropolisHastingsImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
RandomVectorMetropolisHastings::RandomVectorMetropolisHastings(const RandomVector & randomVector,
                                                              const Point & initialState,
                                                              const Indices & marginalIndices,
                                                              const Function & linkFunction)
  : MetropolisHastingsImplementation(initialState, marginalIndices)
{
  setRandomVector(randomVector);
  if (linkFunction.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (linkFunction.getInputDimension() != initialState.getDimension())
      throw InvalidDimensionException(HERE) << "The link function input dimension (" << linkFunction.getInputDimension()
                                            << ") does not match the dimension of the state (" << initialState.getDimension() << ").";
    if (linkFunction.getOutputDimension() != randomVector.getParameter().getDimension())
      throw InvalidDimensionException(HERE) << "The link function output dimension (" << linkFunction.getOutputDimension()
                                            << ") does not match the parameter dimension of the randomVector (" << randomVector.getParameter().getDimension() << ").";
    randomVectorLinkFunction_ = linkFunction;
  }
}


/* String converter */
String RandomVectorMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << RandomVectorMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MetropolisHastingsImplementation::__repr__()
         << " randomVector=" << randomVector_;
}


RandomVectorMetropolisHastings* RandomVectorMetropolisHastings::clone() const
{
  return new RandomVectorMetropolisHastings(*this);
}

Point RandomVectorMetropolisHastings::getCandidate() const
{
  if (randomVectorLinkFunction_.getEvaluation().getImplementation()->isActualImplementation())
  {
    const Point parameter(randomVectorLinkFunction_(currentState_));
    randomVector_.setParameter(parameter);
  }
  const Point prop(randomVector_.getRealization());
  Point newState(currentState_);
  for (UnsignedInteger j = 0; j < marginalIndices_.getSize(); ++ j)
    newState[marginalIndices_[j]] = prop[j];
  return newState;
}


void RandomVectorMetropolisHastings::setRandomVector(const RandomVector & randomVector)
{
  if (randomVector.getDimension() != marginalIndices_.getSize())
    throw InvalidArgumentException(HERE) << "The random variable dimension (" << randomVector.getDimension()
                                         << ") does not match the block size.(" << marginalIndices_.getSize() << ")";
  randomVector_ = randomVector;
}


RandomVector RandomVectorMetropolisHastings::getRandomVector() const
{
  return randomVector_;
}

/* Method save() stores the object through the StorageManager */
void RandomVectorMetropolisHastings::save(Advocate & adv) const
{
  MetropolisHastingsImplementation::save(adv);
  adv.saveAttribute("randomVector_", randomVector_);
  adv.saveAttribute("randomVectorLinkFunction_", randomVectorLinkFunction_);
}

/* Method load() reloads the object from the StorageManager */
void RandomVectorMetropolisHastings::load(Advocate & adv)
{
  MetropolisHastingsImplementation::load(adv);
  adv.loadAttribute("randomVector_", randomVector_);
  adv.loadAttribute("randomVectorLinkFunction_", randomVectorLinkFunction_);
}


END_NAMESPACE_OPENTURNS
