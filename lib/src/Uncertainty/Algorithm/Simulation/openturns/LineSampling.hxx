//                                               -*- C++ -*-
/**
 *  @brief Line sampling simulation method
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LINESAMPLING_HXX
#define OPENTURNS_LINESAMPLING_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/RootStrategy.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API LineSampling
  : public EventSimulation
{
  CLASSNAME
public:

  /** Default Constructor */
  LineSampling();

  /** Constructor with parameters */
  LineSampling(const RandomVector & event,
               const Point & alpha,
               const RootStrategy & rootStrategy = RootStrategy());

  /** Virtual constructor */
  LineSampling * clone() const override;

  /** Initial alpha accessor */
  void setInitialAlpha(const Point & initialAlpha);
  Point getInitialAlpha() const;

  /** Root strategy accessor */
  void setRootStrategy(const RootStrategy & rootStrategy);
  RootStrategy getRootStrategy() const;

  /** Opposite direction flag accessor */
  void setSearchOppositeDirection(const Bool searchOppositeDirection);
  Bool getSearchOppositeDirection() const;

  /** Adaptive important direction flag accessor */
  void setAdaptiveImportantDirection(const Bool adaptiveImportantDirection);
  Bool getAdaptiveImportantDirection() const;

  /** History accessors */
  void setStoreHistory(const Bool storeHistory);
  Bool getStoreHistory() const;
  Sample getAlphaHistory() const;
  Collection<Sample> getRootPointsHistory() const;
  Collection<Point> getRootValuesHistory() const;

  /** Performs the actual computation. */
  void run() override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Compute the block sample */
  Sample computeBlockSample() override;

  /* Compute the contribution of the performance function along the direction */
  Scalar computeLineContribution(const Function & ray, Point & roots, Sample & rootPoints);

  /** Compute the contribution of a direction to the probability given the roots of the performance function along the direction */
  Scalar computeContribution(const Point & roots) const;

  // some parameters
  Point initialAlpha_;
  RootStrategy rootStrategy_;
  Bool searchOppositeDirection_ = true;
  Bool adaptiveImportantDirection_ = true;

  // histories
  Bool storeHistory_ = false;
  Sample alphaHistory_;
  PersistentCollection<Sample> rootPointsHistory_;
  PersistentCollection<Point> rootValuesHistory_;

  // attributes used for convenience, not to be saved/loaded
  StandardEvent standardEvent_; // the algorithm happens in U
  Point alpha_; // current important direction
  Point uStar_; // current nearest root point

}; /* class LineSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINESAMPLING_HXX */
