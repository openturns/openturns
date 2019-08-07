//                                               -*- C++ -*-
/**
 *  @brief Approximation algorithm for system events based on FORM
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

#include "openturns/SystemFORM.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FORM.hxx"
#include "openturns/IntersectionEvent.hxx"
#include "openturns/UnionEvent.hxx"
#include "openturns/Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SystemFORM);

static Factory<SystemFORM> Factory_SystemFORM;

SystemFORM::SystemFORM()
  : Analytical()
{
  // Nothing to do
}

SystemFORM::SystemFORM(const OptimizationAlgorithm & nearestPointAlgorithm,
                        const RandomVector & event,
                        const Point & physicalStartingPoint)
  : Analytical()
{
  setNearestPointAlgorithm(nearestPointAlgorithm);
  setPhysicalStartingPoint(physicalStartingPoint);
  setEvent(event);
}

/* Virtual constructor */
SystemFORM * SystemFORM::clone() const
{
  return new SystemFORM(*this);
}

/* Result accessor */
MultiFORMResult SystemFORM::getResult() const
{
  return multiFORMResult_;
}


/* String converter */
String SystemFORM::__repr__() const
{
  OSS oss;
  oss << "class=" << SystemFORM::GetClassName()
      << " " << Analytical::__repr__()
      << " result=" << multiFORMResult_;
  return oss;
}

void SystemFORM::setEvent(const RandomVector & event)
{
  // check that the event is in disjunctive normal form (union of intersections)
  const UnionEvent *unionEvent = dynamic_cast<UnionEvent*>(event.getImplementation().get());
  Collection<RandomVector> unionCollection;
  if (unionEvent)
    unionCollection = unionEvent->getEventCollection();
  else // just a single intersection
    unionCollection.add(event);
  for (UnsignedInteger i = 0; i < unionCollection.getSize(); ++ i)
  {
    if (unionCollection[i].getImplementation()->getClassName() == "IntersectionEvent")
    {
      const IntersectionEvent *intersectionEvent = dynamic_cast<IntersectionEvent*>(unionCollection[i].getImplementation().get());
      Collection<RandomVector> intersectionCollection(intersectionEvent->getEventCollection());
      for (UnsignedInteger j = 0; j < intersectionCollection.getSize(); ++ j)
        if (intersectionCollection[i].getImplementation()->getClassName() != "EventRandomVector")
          throw InvalidArgumentException(HERE) << "Event is not is disjunctive normal form";
    }
    else if (unionCollection[i].getImplementation()->getClassName() != "EventRandomVector")
      throw InvalidArgumentException(HERE) << "Event is not is disjunctive normal form";
  }
  Analytical::setEvent(event);
}

/* Run */
void SystemFORM::run()
{
  // Collect the flat list of leaf events from the DNF event and the the lists of ids for each parallel region
  Collection<RandomVector> leafEventCollection;
  const UnionEvent *unionEvent = dynamic_cast<UnionEvent*>(getEvent().getImplementation().get());
  Collection<RandomVector> unionCollection;
  if (unionEvent)
    unionCollection = unionEvent->getEventCollection();
  else // just a single intersection
    unionCollection.add(getEvent());
  Collection<Indices> parallelRegionIdCollection;
  for (UnsignedInteger i = 0; i < unionCollection.getSize(); ++ i)
  {
    if (unionCollection[i].getImplementation()->getClassName() == "IntersectionEvent")
    {
      const IntersectionEvent *intersectionEvent = dynamic_cast<IntersectionEvent*>(unionCollection[i].getImplementation().get());
      Collection<RandomVector> intersectionCollection(intersectionEvent->getEventCollection());
      leafEventCollection.add(intersectionCollection);
      Indices parallelRegionId(intersectionCollection.getSize());
      for (UnsignedInteger j = 0; j < intersectionCollection.getSize(); ++ j)
      {
        parallelRegionId[j] = intersectionCollection[j].getId();
      }
      parallelRegionIdCollection.add(parallelRegionId);
    }
    else
    { // single event in parallel region
      leafEventCollection.add(unionCollection[i]);
      parallelRegionIdCollection.add(Indices(1, unionCollection[i].getId()));
    }
  }

  // for each leaf events perform FORM
  std::map<UnsignedInteger, Scalar> idToBetaMap;
  std::map<UnsignedInteger, Point> idToAlphaMap;
  Collection<FORMResult> formResultCollection;
  for (UnsignedInteger i = 0; i < leafEventCollection.getSize(); ++ i)
  {
    const UnsignedInteger id = leafEventCollection[i].getId();
    if (idToBetaMap.find(id) == idToBetaMap.end()) // if not already computed for this event
    {
      FORM algo(getNearestPointAlgorithm(), leafEventCollection[i], getPhysicalStartingPoint());
      algo.run();

      const FORMResult result(algo.getResult());
      const Scalar beta = result.getGeneralisedReliabilityIndex();
      idToBetaMap[id] = beta;
      idToAlphaMap[id] = result.getStandardSpaceDesignPoint() * (1.0 / beta);
      formResultCollection.add(result);
      LOGINFO(OSS() << "SystemFORM: event=" << id << " beta=" << result.getGeneralisedReliabilityIndex());
    }
  }

  // generate all poincarre terms
  Collection<Indices> poincarreRegion;
  UnsignedInteger numberOfPoincarreTerms = 0;
  for (UnsignedInteger k = 0; k < unionCollection.getSize(); ++ k)
  {
    // retrieve region
    const Indices region(parallelRegionIdCollection[k]);

    // recursive expansion of the Poincarre formula
    for (UnsignedInteger i = 0; i < numberOfPoincarreTerms; ++ i)
    {
      // intersection of region / poincarreRegion[i]
      Indices intersection(region);
      const UnsignedInteger poincarreRegionSize = poincarreRegion[i].getSize();
      for (UnsignedInteger j = 0; j < poincarreRegionSize; ++ j)
        if (!region.contains(poincarreRegion[i][j]))
          intersection.add(poincarreRegion[i][j]);
      poincarreRegion.add(intersection);
    }
    poincarreRegion.add(region);

    // update number of poincarre terms (=2^n-1)
    numberOfPoincarreTerms = 2 * numberOfPoincarreTerms + 1;
  }

  Scalar eventProbability = 0.0;
  for (UnsignedInteger k = 0; k < numberOfPoincarreTerms; ++ k)
  {
    // retrieve region
    const Indices region(poincarreRegion[k]);
    const UnsignedInteger regionSize = region.getSize();

    // set region beta
    Point regionBeta(regionSize);
    for (UnsignedInteger i = 0; i < regionSize; ++ i)
      regionBeta[i] = idToBetaMap[region[i]];

    // set the correlation matrix C
    CovarianceMatrix C(regionSize);
    for (UnsignedInteger i = 0; i < regionSize; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        C(i, j) = idToAlphaMap[region[i]].dot(idToAlphaMap[region[j]]);

    // Regularize C
    Scalar cumulatedScaling = 0.0;
    const Scalar startingScaling = ResourceMap::GetAsScalar("SystemFORM-StartingScaling");
    const Scalar maximalScaling = ResourceMap::GetAsScalar("SystemFORM-MaximalScaling");
    Scalar scaling = startingScaling;
    while (!C.isPositiveDefinite())
    {
      cumulatedScaling += scaling;
      for (UnsignedInteger index = 0; index < regionSize; ++index)
        C(index, index) += scaling;
      scaling *= 2.0;

      // No reasonable regularization succeeded
      if (cumulatedScaling >= maximalScaling)
        throw InvalidArgumentException(HERE) << "Could not regularize, scaling up to " << cumulatedScaling << " was not enough";
    }

    // compute the parallel region probability
    const Point mu(regionSize, 0.0);
    const Normal normal(mu, C);
    const Scalar poincarreProbability = normal.computeCDF(-1.0 * regionBeta);

    LOGINFO(OSS() << "SystemFORM: poincarre probability [" << k << "]=" << poincarreProbability);

    // trick to update the poincarre sum
    // we invert the sign of each consecutive term to follow the order of our recursive expansion of the Poincarre formula
    eventProbability = -eventProbability + poincarreProbability;
  }

  // store results
  multiFORMResult_ = MultiFORMResult(formResultCollection);
  multiFORMResult_.setEventProbability(eventProbability);
}


/* Method save() stores the object through the StorageManager */
void SystemFORM::save(Advocate & adv) const
{
  Analytical::save(adv);
  adv.saveAttribute("multiFORMResult_", multiFORMResult_);
}


/* Method load() reloads the object from the StorageManager */
void SystemFORM::load(Advocate & adv)
{
  Analytical::load(adv);
  adv.loadAttribute("multiFORMResult_", multiFORMResult_);
}

END_NAMESPACE_OPENTURNS
