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
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Normal.hxx"

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
RandomVectorMetropolisHastings::RandomVectorMetropolisHastings(const RandomVector & source,
                                                              const Point & initialState,
                                                              const Indices & marginalIndices,
                                                              const Function & sourceLinkFunction)
  : MetropolisHastingsImplementation(Normal(initialState.getDimension()), initialState, marginalIndices)
{
  setSource(source);
  if (sourceLinkFunction.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (sourceLinkFunction.getInputDimension() != initialState.getDimension())
      throw InvalidDimensionException(HERE) << "The source link function input dimension (" << sourceLinkFunction.getInputDimension()
                                            << ") does not match the dimension of the state (" << initialState.getDimension() << ").";
    if (sourceLinkFunction.getOutputDimension() != source.getParameter().getDimension())
      throw InvalidDimensionException(HERE) << "The source link function output dimension (" << sourceLinkFunction.getOutputDimension()
                                            << ") does not match the parameter dimension of the source (" << source.getParameter().getDimension() << ").";
    sourceLinkFunction_ = sourceLinkFunction;
  }
}


/* String converter */
String RandomVectorMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << RandomVectorMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MetropolisHastingsImplementation::__repr__()
         << " source=" << source_;
}


RandomVectorMetropolisHastings* RandomVectorMetropolisHastings::clone() const
{
  return new RandomVectorMetropolisHastings(*this);
}

Scalar RandomVectorMetropolisHastings::computeLogPDFPrior(const Point & /*state*/) const
{
  return 0.0;
}

Point RandomVectorMetropolisHastings::getCandidate() const
{
  RandomVector source(source_);
  if (sourceLinkFunction_.getEvaluation().getImplementation()->isActualImplementation())
  {
    const Point parameter(sourceLinkFunction_(currentState_));
    source.setParameter(parameter);
  }
  const Point prop(source.getRealization());
  Point newState(currentState_);
  for (UnsignedInteger j = 0; j < marginalIndices_.getSize(); ++ j)
    newState[marginalIndices_[j]] = prop[j];
  return newState;
}


void RandomVectorMetropolisHastings::setSource(const RandomVector & source)
{
  if (source.getDimension() != marginalIndices_.getSize())
    throw InvalidArgumentException(HERE) << "The source random variable dimension (" << source.getDimension()
                                         << ") does not match the block size.(" << marginalIndices_.getSize() << ")";
  source_ = source;
}


RandomVector RandomVectorMetropolisHastings::getSource() const
{
  return source_;
}

/* Method save() stores the object through the StorageManager */
void RandomVectorMetropolisHastings::save(Advocate & adv) const
{
  MetropolisHastingsImplementation::save(adv);
  adv.saveAttribute("source_", source_);
  adv.saveAttribute("sourceLinkFunction_", sourceLinkFunction_);
}

/* Method load() reloads the object from the StorageManager */
void RandomVectorMetropolisHastings::load(Advocate & adv)
{
  MetropolisHastingsImplementation::load(adv);
  adv.loadAttribute("source_", source_);
  adv.loadAttribute("sourceLinkFunction_", sourceLinkFunction_);
}


END_NAMESPACE_OPENTURNS
