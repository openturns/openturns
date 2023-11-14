//                                               -*- C++ -*-
/**
 *  @brief Classification algorithm interface
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CLASSIFIER_HXX
#define OPENTURNS_CLASSIFIER_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/ClassifierImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class Classifier
 *
 * Classification algorithm interface
 */
class OT_API Classifier
  : public TypedInterfaceObject<ClassifierImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<ClassifierImplementation>::Implementation Implementation;

  /** Default constructor */
  Classifier();

  /** Constructor from implementation */
  Classifier(const ClassifierImplementation & implementation);

  /** Constructor from implementation */
  Classifier(const Implementation & p_implementation);

  /** Constructor from sample */
  Classifier(const Sample & inS, const Indices & outC);

  UnsignedInteger getNumberOfClasses() const;

  /** Associate a point with a class */
  UnsignedInteger classify(const Point & inP) const;
  Indices classify(const Sample & inS) const;

  /** Grade a point as if it were associated to a class */
  Scalar grade(const Point & inP,
               UnsignedInteger outC) const;
  Point grade(const Sample & inS,
              const Indices & outC) const;

  /** Parallelization flag accessor */
  void setParallel(const Bool flag);
  Bool isParallel() const;

  /** Input space dimension */
  UnsignedInteger getDimension() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class Classifier */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CLASSIFIER_HXX */
