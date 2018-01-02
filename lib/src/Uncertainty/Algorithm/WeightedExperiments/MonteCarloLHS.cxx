//                                               -*- C++ -*-
/**
 *  @brief MonteCarloLHS
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/MonteCarloLHS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SpaceFillingC2.hxx"
#include "openturns/SpaceFillingPhiP.hxx"
#include "openturns/SpaceFillingMinDist.hxx"


namespace OT
{

CLASSNAMEINIT(MonteCarloLHS)

static const Factory<MonteCarloLHS> Factory_MonteCarloLHS;

/* Default constructor */
MonteCarloLHS::MonteCarloLHS()
  : OptimalLHSExperiment()
{
}

/* Default constructor */
MonteCarloLHS::MonteCarloLHS(const LHSExperiment & lhs, const UnsignedInteger N, const SpaceFilling & spaceFilling)
  : OptimalLHSExperiment(lhs, spaceFilling),
    N_(N)
{
  // Nothing to do
}

/* Virtual constructor method */
MonteCarloLHS * MonteCarloLHS::clone() const
{
  return new MonteCarloLHS(*this);
}

/* Generate a Sample. */
Sample MonteCarloLHS::generateWithWeights(Point & weights) const
{
  LHSResult result(spaceFilling_);
  Sample history(N_, 1);
  Description historyDescription(1);
  historyDescription[0] = spaceFilling_.getImplementation()->getName() + " criterion";
  history.setDescription(historyDescription);
  // initialing algo
  Sample optimalDesign;
  Scalar optimalValue = spaceFilling_.isMinimizationProblem() ? SpecFunc::MaxScalar : -SpecFunc::MaxScalar;

  for (UnsignedInteger i = 0; i < N_; ++i)
  {
    const Sample design(lhs_.generate());
    const Scalar value = spaceFilling_.evaluate(rankTransform(design));
    history[i][0] = value;
    if (spaceFilling_.isMinimizationProblem() && (value < optimalValue))
    {
      optimalDesign = design;
      optimalValue = value;
    }
    if (!spaceFilling_.isMinimizationProblem() && (value > optimalValue))
    {
      optimalDesign = design;
      optimalValue = value;
    }
  }
  Sample optimalDesignU(rankTransform(optimalDesign));
  result.add(optimalDesign, optimalValue, SpaceFillingC2().evaluate(optimalDesignU), SpaceFillingPhiP().evaluate(optimalDesignU), SpaceFillingMinDist().evaluate(optimalDesignU), history);
  result_ = result;
  weights = Point(size_, 1.0 / size_);
  return result.getOptimalDesign();
}

/* String converter */
String MonteCarloLHS::__repr__() const
{
  OSS oss;
  oss << "class=" << MonteCarloLHS::GetClassName()
      << " name=" << getName()
      << " lhs=" << lhs_
      << " spaceFilling=" << spaceFilling_
      << " simulation size=" << N_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void MonteCarloLHS::save(Advocate & adv) const
{
  OptimalLHSExperiment::save( adv );
  adv.saveAttribute( "N_", N_ );
}

/* Method load() reloads the object from the StorageManager */
void MonteCarloLHS::load(Advocate & adv)
{
  OptimalLHSExperiment::load( adv );
  adv.loadAttribute( "N_", N_ );
}


} /* namespace OT */
