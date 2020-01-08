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
  virtual MixtureClassifier * clone() const;

  /** String converter */
  virtual String __repr__() const;

  virtual UnsignedInteger getNumberOfClasses() const;

  /** Associate a point to a class */
  using ClassifierImplementation::classify;
  virtual UnsignedInteger classify(const Point & inP) const;

private:
  Indices classifySequential(const Sample & inS) const;
public:

  /** Grade a point as if it were associated to a class */
  virtual Scalar grade(const Point & inP,
                       const UnsignedInteger outC) const;

  /** Mixture accessors */
  Mixture getMixture() const;
  void setMixture(const Mixture & mixture);

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** The mixture that defines the MixtureClassifier */
  Mixture mixture_;


}; /* class MixtureClassifier */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MIXTURECLASSIFIER_HXX */
