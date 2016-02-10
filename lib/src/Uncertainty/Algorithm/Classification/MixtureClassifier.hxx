//                                               -*- C++ -*-
/**
 *  @brief K-Fold cross validation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MIXTURECLASSIFIER_HXX
#define OPENTURNS_MIXTURECLASSIFIER_HXX

#include "Mixture.hxx"
#include "ClassifierImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MixtureClassifier
 *
 * Associate to a given point its class index
 */
class OT_API MixtureClassifier
  : public ClassifierImplementation
{
  CLASSNAME;
public:

  /** Parameters constructor */
  MixtureClassifier();

  /** Default constructor */
  MixtureClassifier(const Mixture & mixture);

  /** Virtual constructor */
  virtual MixtureClassifier * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Associate a point with a class */
  virtual UnsignedInteger classify(const NumericalPoint & inP) const;

  /** Grade a point as if it were associated to a class */
  virtual NumericalScalar grade(const NumericalPoint & inP,
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
