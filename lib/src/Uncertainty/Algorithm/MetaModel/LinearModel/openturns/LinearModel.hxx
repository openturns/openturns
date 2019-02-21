//                                               -*- C++ -*-
/**
 *  @brief LinearModel implements the linear model
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
#ifndef OPENTURNS_LINEARMODEL_HXX
#define OPENTURNS_LINEARMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Interval.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/TestResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LinearModel
 * @deprecated
 *
 * LinearModel implements the notion of linear model
 */

class OT_API LinearModel :
  public PersistentObject
{
  CLASSNAME

public:

  typedef Collection<Scalar>                    ScalarCollection;
  typedef PersistentCollection<Scalar>          ScalarPersistentCollection;


  /** Default constructor is private */
  LinearModel();

  /** Standard constructor */
  LinearModel(const Point & vectorR,
              const Interval & intervals,
              const ScalarCollection & pValuesOfR);

  /** Constructor from Point */
  explicit LinearModel(const Point & vectorR);

  /** Virtual constructor */
  virtual LinearModel * clone() const;


  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** get vector, get intervals*/
  Point getRegression() const;
  Interval getConfidenceIntervals() const;
  ScalarCollection getPValues() const;

  /** getPredicted : build an sample of predicted values */
  Sample getPredicted(const Sample & predictor) const;

  /** getResidual */
  Sample getResidual(const Sample & predictor,
                     const Sample & measured) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

  /** The estimated vector */
  Point regression_;

  /** The confidence intervals */
  Interval confidenceIntervals_;

  /** The p-Values  */
  ScalarPersistentCollection pValues_;


}; /* class LinearModel */


/** Comparison operator */
OT_API Bool operator ==(const LinearModel & lhs, const LinearModel & rhs);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODEL_HXX */
