//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
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
#ifndef OPENTURNS_MIXTURECLASSIFIER_HXX
#define OPENTURNS_MIXTURECLASSIFIER_HXX

#include "openturns/Mixture.hxx"
#include "openturns/ClassifierImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MixtureClassifier
 *
 * Associate to a given point its class index
 */
class OT_API MixtureClassifier
  : public ClassifierImplementation
{
  CLASSNAME
public:

  /** Parameters constructor */
  MixtureClassifier();

  /** Default constructor */
  MixtureClassifier(const Mixture & mixture);

  /** Virtual constructor */
  MixtureClassifier * clone() const override;

  /** String converter */
  String __repr__() const override;

  UnsignedInteger getNumberOfClasses() const override;

  /** Associate a point to a class */
  using ClassifierImplementation::classify;
  UnsignedInteger classify(const Point & inP) const override;
  Indices classify(const Sample & inS) const override;
public:

  /** Grade a point as if it were associated to a class */
  using ClassifierImplementation::grade;
  Scalar grade(const Point &inP, const UnsignedInteger outC) const override;

  /** Mixture accessors */
  Mixture getMixture() const;
  void setMixture(const Mixture & mixture);

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The mixture that defines the MixtureClassifier */
  Mixture mixture_;


}; /* class MixtureClassifier */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MIXTURECLASSIFIER_HXX */
