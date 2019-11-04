//                                               -*- C++ -*-
/**
 *  @brief LHSResult
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
#include "openturns/LHSResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/SpecFunc.hxx"

#include <limits>



namespace OT
{

static const Factory<LHSResult> Factory_LHSResult;

CLASSNAMEINIT(LHSResult)

/* Default constructor */
LHSResult::LHSResult()
  : PersistentObject()
  , optimalIndex_(0)
  , criteria_(Sample(0, 4))
{
  optimalCriterion_ = spaceFilling_.isMinimizationProblem() ? SpecFunc::MaxScalar : -SpecFunc::MaxScalar;
}

/* Default constructor */
LHSResult::LHSResult(const SpaceFilling & spaceFilling, UnsignedInteger restart)
  : PersistentObject()
  , spaceFilling_(spaceFilling)
  , restart_(restart)
  , optimalIndex_(0)
  , criteria_(Sample(0, 4))
{
  optimalCriterion_ = spaceFilling_.isMinimizationProblem() ? SpecFunc::MaxScalar : -SpecFunc::MaxScalar;
}

LHSResult * LHSResult::clone() const
{
  return new LHSResult(*this);
}


void LHSResult::add(const Sample & optimalDesign, Scalar criterion,
                    Scalar C2, Scalar PhiP, Scalar MinDist, const Sample & algoHistory)
{
  if (spaceFilling_.isMinimizationProblem() && (criterion < optimalCriterion_))
  {
    // This result is better than current one, store it
    optimalIndex_ = criteria_.getSize();
    optimalCriterion_ = criterion;
  }
  if (!spaceFilling_.isMinimizationProblem() && (criterion > optimalCriterion_))
  {
    // This result is better than current one, store it
    optimalIndex_ = criteria_.getSize();
    optimalCriterion_ = criterion;
  }
  Point criteria(4);
  criteria[0] = criterion;
  criteria[1] = C2;
  criteria[2] = PhiP;
  criteria[3] = MinDist;

  criteria_.add(criteria);
  collDesigns_.add(optimalDesign);
  collAlgoHistory_.add(algoHistory);
}

/** Accessors */
UnsignedInteger LHSResult::getNumberOfRestarts() const
{
  return restart_;
}

/** Attributes for getting elements of result */
Sample LHSResult::getOptimalDesign() const
{
  return collDesigns_[optimalIndex_];
}

Sample LHSResult::getOptimalDesign(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return collDesigns_[restart];
}

Scalar LHSResult::getOptimalValue() const
{
  return criteria_(optimalIndex_, 0);
}

Scalar LHSResult::getOptimalValue(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return criteria_(restart, 0);
}

Sample LHSResult::getAlgoHistory() const
{
  return collAlgoHistory_[optimalIndex_];
}

Sample LHSResult::getAlgoHistory(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return collAlgoHistory_[restart];
}

Scalar LHSResult::getC2() const
{
  return criteria_(optimalIndex_, 1);
}

Scalar LHSResult::getC2(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return criteria_(restart, 1);
}

Scalar LHSResult::getPhiP() const
{
  return criteria_(optimalIndex_, 2);
}

Scalar LHSResult::getPhiP(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return criteria_(restart, 2);
}

Scalar LHSResult::getMinDist() const
{
  return criteria_(optimalIndex_, 3);
}

Scalar LHSResult::getMinDist(UnsignedInteger restart) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  return criteria_(restart, 3);
}

UnsignedInteger LHSResult::findDescription(const char *text) const
{
  Description description(collAlgoHistory_[0].getDescription());
  for (UnsignedInteger i = 0; i < description.getSize(); ++i)
  {
    if (description[i].find(text) != std::string::npos)
    {
      return i + 1;
    }
  }
  return 0;
}

Graph LHSResult::drawCurveData(const Sample & data, const String & title) const
{
  Curve curve(data);
  curve.setColor("red");
  curve.setLegend(title);
  curve.setLineStyle("solid");
  curve.setLineWidth(2);
  Graph graph(title, "Iterations", data.getDescription()[0], true, "");
  graph.add(curve);
  return graph;
}

Graph LHSResult::drawHistoryCriterion(const String & title) const
{
  String drawTitle(title);
  if (drawTitle.empty())
  {
    const UnsignedInteger idx(findDescription("criterion"));
    if (idx == 0)
      throw InvalidArgumentException(HERE) << "Could not draw criterion history, data not found";
    drawTitle = collAlgoHistory_[0].getDescription()[idx - 1] + " history of optimal design";
  }
  return drawHistoryCriterion(optimalIndex_, drawTitle);
}

Graph LHSResult::drawHistoryCriterion(UnsignedInteger restart, const String & title) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  const UnsignedInteger idx(findDescription("criterion"));
  if (idx == 0)
    throw InvalidArgumentException(HERE) << "Could not draw criterion history, data not found";
  if (collAlgoHistory_[restart].getSize() == 0)
    throw InvalidArgumentException(HERE) << "Could not draw criterion history, data are empty";

  const Sample data(collAlgoHistory_[restart].getMarginal(idx - 1));
  String drawTitle(title);
  if (drawTitle.empty()) drawTitle = String(OSS() << data.getDescription()[0] << " history of restart number=" << restart);
  return drawCurveData(data, drawTitle);
}

Graph LHSResult::drawHistoryTemperature(const String & title) const
{
  String drawTitle(title);
  if (drawTitle.empty()) drawTitle = "Temperature history of optimal design";
  return drawHistoryTemperature(optimalIndex_, drawTitle);
}

Graph LHSResult::drawHistoryTemperature(UnsignedInteger restart, const String & title) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  const UnsignedInteger idx(findDescription("Temperature"));
  if (idx == 0)
    throw InvalidArgumentException(HERE) << "Could not draw temperature history, data not found";
  if (collAlgoHistory_[restart].getSize() == 0)
    throw InvalidArgumentException(HERE) << "Could not draw temperature history, data are empty";

  const Sample data(collAlgoHistory_[restart].getMarginal(idx - 1));
  String drawTitle(title);
  if (drawTitle.empty()) drawTitle = String(OSS() << "Temperature history of restart number=" << restart);
  return drawCurveData(data, drawTitle);
}

Graph LHSResult::drawHistoryProbability(const String & title) const
{
  String drawTitle(title);
  if (drawTitle.empty()) drawTitle = "Probability history of optimal design";
  return drawHistoryProbability(optimalIndex_, drawTitle);
}

Graph LHSResult::drawHistoryProbability(UnsignedInteger restart, const String & title) const
{
  if (restart > restart_)
    throw InvalidArgumentException(HERE) << "The restart number must be in [0," << restart_ << "]";
  const UnsignedInteger idx(findDescription("Probability"));
  if (idx == 0)
    throw InvalidArgumentException(HERE) << "Could not draw probability history, data not found";
  if (collAlgoHistory_[restart].getSize() == 0)
    throw InvalidArgumentException(HERE) << "Could not draw probability history, data are empty";

  const UnsignedInteger size(collAlgoHistory_[restart].getSize());
  Sample data(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data(i, 0) = i;
    data(i, 1) = collAlgoHistory_[restart](i, idx - 1);
  }
  Description description(2);
  description[0] = "Iterations";
  description[1] = "Probability";
  data.setDescription(description);

  String drawTitle(title);
  if (drawTitle.empty()) drawTitle = String(OSS() << "Probability history of restart number=" << restart);

  Cloud cloud(data);
  cloud.setColor("red");
  cloud.setLegend(drawTitle);
  cloud.setLineStyle("solid");
  cloud.setLineWidth(2);
  Graph graph(drawTitle, description[0], description[1], true, "");
  graph.add(cloud);
  return graph;
}

/* String converter */
String LHSResult::__repr__() const
{
  OSS oss;
  oss << "class=" << LHSResult::GetClassName()
      << " spaceFilling=" << spaceFilling_
      << " restart=" << restart_
      << " criteria_=" << criteria_
      << " collDesigns_=" << collDesigns_
      << " collAlgoHistory_=" << collAlgoHistory_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LHSResult::save(Advocate & adv) const
{
  PersistentObject::save( adv );
  adv.saveAttribute("spaceFilling_", spaceFilling_);
  adv.saveAttribute("restart_", restart_);
  adv.saveAttribute("criteria_", criteria_);
  adv.saveAttribute("collDesigns_", collDesigns_);
  adv.saveAttribute("collAlgoHistory_", collAlgoHistory_);
}

/* Method load() reloads the object from the StorageManager */
void LHSResult::load(Advocate & adv)
{
  PersistentObject::load( adv );
  adv.loadAttribute("spaceFilling_", spaceFilling_);
  adv.loadAttribute("restart_", restart_);
  adv.loadAttribute("criteria_", criteria_);
  adv.loadAttribute("collDesigns_", collDesigns_);
  adv.loadAttribute("collAlgoHistory_", collAlgoHistory_);
}

} /* namespace OT */
