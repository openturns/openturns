//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MINIMUMVOLUMECLASSIFIER_HXX
#define OPENTURNS_MINIMUMVOLUMECLASSIFIER_HXX

#include "openturns/LevelSet.hxx"
#include "openturns/ClassifierImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MinimumVolumeClassifier
 *
 * Associate to a given point its class index
 */
class OT_API MinimumVolumeClassifier
  : public ClassifierImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MinimumVolumeClassifier();

  /** Parameters constructor */
  MinimumVolumeClassifier(const Distribution & distribution,
                          const Scalar alpha);

  /** Virtual constructor */
  MinimumVolumeClassifier * clone() const override;

  /** String converter */
  String __repr__() const override;

  UnsignedInteger getNumberOfClasses() const override;

  /** Associate a point to a class */
  using ClassifierImplementation::classify;
  UnsignedInteger classify(const Point & inP) const override;

  Indices classify(const Sample & inS) const override;

  /** Level set accessor */
  LevelSet getLevelSet() const;

  /** Grade a point as if it were associated to a class */
  Scalar grade(const Point & inP, const UnsignedInteger outC) const override;

  /** Distribution accessors */
  Distribution getDistribution() const;
  Scalar getThreshold() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  GridLayout drawContour(const Point & alpha) const;
  GridLayout drawSample(const Sample & sample, const Indices & classes) const;
  GridLayout drawContourAndSample(const Point & alpha, const Sample & sample, const Indices & classes) const;

protected:
  // the distribution and confidence level that defines the minimum volume domain
  Distribution distribution_;
  Scalar alpha_ = 0.0;

  mutable Bool isLevelSetComputed_ = false;
  mutable LevelSet levelSet_;
  mutable Scalar threshold_ = 0.0;

}; /* class MinimumVolumeClassifier */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MINIMUMVOLUMECLASSIFIER_HXX */
