//                                               -*- C++ -*-
/**
 *  @brief SimulatedAnnealingLHS
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimulatedAnnealingLHS.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpaceFillingC2.hxx"
#include "openturns/SpaceFillingPhiP.hxx"
#include "openturns/SpaceFillingMinDist.hxx"
#include "openturns/KPermutationsDistribution.hxx"


namespace OT
{

CLASSNAMEINIT(SimulatedAnnealingLHS);

static const Factory<SimulatedAnnealingLHS> Factory_SimulatedAnnealingLHS;


/** Geometric temperature profil */
SimulatedAnnealingLHS::SimulatedAnnealingLHS(const LHSExperiment & lhs, const TemperatureProfile & profile,
  const SpaceFilling & spaceFilling)
    : OptimalLHSExperiment(lhs, spaceFilling)
    , profile_(profile)
    , initialDesign_()
{
  if (spaceFilling.getImplementation()->getClassName() == "SpaceFillingMinDist")
  {
    LOGWARN("MinDist criterion replaced by PhiP in SimulatedAnnealingLHS");
    spaceFilling_ = SpaceFillingPhiP();
  }
}

/** SimulatedAnnealingLHS constructor with LHS*/
SimulatedAnnealingLHS::SimulatedAnnealingLHS (const Sample & initialDesign,
                                              const Distribution & distribution,
                                              const TemperatureProfile & profile,
                                              const SpaceFilling & spaceFilling)
    : OptimalLHSExperiment()
    , profile_(profile)
    , initialDesign_(initialDesign)
{
  if (spaceFilling.getImplementation()->getClassName() == "SpaceFillingMinDist")
  {
    LOGWARN("MinDist criterion replaced by PhiP in SimulatedAnnealingLHS");
    spaceFilling_ = SpaceFillingPhiP();
  }
  else
    spaceFilling_ = spaceFilling;
  if (initialDesign_.getSize() == 0) throw InvalidArgumentException(HERE) << "Initial design must not be empty";
  if (initialDesign_.getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Initial design dimension " << initialDesign_.getDimension() << " does not match distribution dimension " << distribution.getDimension();

  // dummy lhs, only distribution is needed
  lhs_ = LHSExperiment(distribution, 1);
}

/* Virtual constructor method */
SimulatedAnnealingLHS * SimulatedAnnealingLHS::clone() const
{
  return new SimulatedAnnealingLHS(*this);
}

/* Generate design method */
Sample SimulatedAnnealingLHS::generateWithWeights(Point & weights) const
{
  weights = Point(size_, 1.0 / size_);
  return generateWithRestart(0);
}

Sample SimulatedAnnealingLHS::generateWithRestart(UnsignedInteger nRestart) const
{
  // LHSResult returns resuls for all restarts
  LHSResult result(spaceFilling_, nRestart);
  for(UnsignedInteger index = 0; index < nRestart + 1; ++index)
  {
    // history has dimension 3 :crit, proba & temperature
    // Total size depends on convergency
    Sample history(0, 3);
    Description historyDescription(3);
    historyDescription[0] = spaceFilling_.getImplementation()->getName() + " criterion";
    historyDescription[1] = "Probability";
    historyDescription[2] = "Temperature";
    history.setDescription(historyDescription);
    LOGDEBUG("Starting simulated annealing process");

    // Starting sample, in the [0,1]^d space
    Sample optimalDesign(rankTransform(initialDesign_.getSize() > 0 ? initialDesign_ : lhs_.generate()));

    // Starting implementation
    UnsignedInteger iteration(0);
    Scalar T(profile_.getT0());
    Scalar optimalValue(spaceFilling_.evaluate(optimalDesign));
    const UnsignedInteger iMax(profile_.getIMax());
    while(iteration < iMax && T > 0)
    {
      LOGDEBUG(OSS() << "Current iteration =" <<  iteration << ", current temperature =" << T);
      // Generate rows & column indexes
      const UnsignedInteger columnIndex = RandomGenerator::IntegerGenerate(optimalDesign.getDimension());
      const UnsignedInteger row1 = RandomGenerator::IntegerGenerate(optimalDesign.getSize());
      const UnsignedInteger row2 = RandomGenerator::IntegerGenerate(optimalDesign.getSize());
      if (row1 == row2) continue;
      // WARNING: bernoulliTrial is computed here and not in the else clause below to avoid
      //          numerical discrepancies between x86 and x86_64.  When optimalValue and
      //          newCriterion are almost equal, architectures may go to different
      //          branches.  In order to have the same random generator state,
      //          RandomGenerator::Generate() is called here.
      const Scalar bernoulliTrial = RandomGenerator::Generate();
      const Scalar newCriterion = spaceFilling_.perturbLHS(optimalDesign, optimalValue, row1, row2, columnIndex);
      const Scalar criteriaDifference = std::min(std::exp((optimalValue-newCriterion)/T), 1.0);
      // Decision with respect to criteriaDifference
      if (optimalValue >= newCriterion)
      {
        std::swap(optimalDesign[row1][columnIndex], optimalDesign[row2][columnIndex]);
        optimalValue = newCriterion;
      }
      else
      {
        if (bernoulliTrial < criteriaDifference)
        {
          std::swap(optimalDesign[row1][columnIndex], optimalDesign[row2][columnIndex]);
          optimalValue = newCriterion;
        }
      }
      LOGDEBUG(OSS() << "Current optimal value =" << optimalValue);
      Point historyElement(3);
      historyElement[0] = optimalValue;
      historyElement[1] = criteriaDifference;
      historyElement[2] = T;
      history.add(historyElement);
      // Update iteration, temperature
      ++ iteration;
      T = profile_(iteration);
    }
    LOGDEBUG("End of simulated annealing process");
    // Transform again optimalDesign
    Sample optimalDesignX(inverseRankTransform(optimalDesign));
    // Add elements to result
    result.add(optimalDesignX, optimalValue, SpaceFillingC2().evaluate(optimalDesign), SpaceFillingPhiP().evaluate(optimalDesign), SpaceFillingMinDist().evaluate(optimalDesign), history);
  }
  result_ = result;
  return result.getOptimalDesign();
}

/* String converter */
String SimulatedAnnealingLHS::__repr__() const
{
  OSS oss;
  oss << "class=" << SimulatedAnnealingLHS::GetClassName()
      << " name=" << getName()
      << " bounds=" << lhs_.getDistribution().getRange()
      << " spaceFilling=" << spaceFilling_
      << " profile=" << profile_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimulatedAnnealingLHS::save(Advocate & adv) const
{
  OptimalLHSExperiment::save( adv );
  adv.saveAttribute( "profile_", profile_ );
  adv.saveAttribute( "initialDesign_", initialDesign_ );
}

/* Method load() reloads the object from the StorageManager */
void SimulatedAnnealingLHS::load(Advocate & adv)
{
  OptimalLHSExperiment::load( adv );
  adv.loadAttribute( "profile_", profile_ );
  adv.loadAttribute( "initialDesign_", initialDesign_ );
}


} /* namespace OT */
