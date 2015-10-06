//                                               -*- C++ -*-
/**
 *  @brief LinearModel implements the linear model
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LINEARMODEL_HXX
#define OPENTURNS_LINEARMODEL_HXX

#include "PersistentObject.hxx"
#include "Description.hxx"
#include "NumericalPoint.hxx"
#include "NumericalSample.hxx"
#include "Interval.hxx"
#include "PersistentCollection.hxx"
#include "Collection.hxx"
#include "TestResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LinearModel
 *
 * LinearModel implements the notion of linear model
 */

class OT_API LinearModel :
  public PersistentObject
{
  CLASSNAME;

public:

  typedef Collection<NumericalScalar>                    NumericalScalarCollection;
  typedef PersistentCollection<NumericalScalar>          NumericalScalarPersistentCollection;


  /** Default constructor is private */
  LinearModel();

  /** Standard constructor */
  LinearModel(const NumericalPoint & vectorR,
              const Interval & intervals,
              const NumericalScalarCollection & pValuesOfR);

  /** Constructor from NumericalPoint */
  LinearModel(const NumericalPoint & vectorR);

  /** Virtual constructor */
  virtual LinearModel * clone() const;


  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** get vector, get intervals*/
  NumericalPoint getRegression() const;
  Interval getConfidenceIntervals() const;
  NumericalScalarCollection getPValues() const;

  /** getPredicted : build an sample of predicted values */
  NumericalSample getPredicted(const NumericalSample & predictor) const;

  /** getResidual */
  NumericalSample getResidual(const NumericalSample & predictor,
                              const NumericalSample & measured) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

  /** The estimated vector */
  NumericalPoint regression_;

  /** The confidence intervals */
  Interval confidenceIntervals_;

  /** The p-Values  */
  NumericalScalarPersistentCollection pValues_;


private:

  friend class Factory<LinearModel>;

}; /* class LinearModel */


/** Comparison operator */
OT_API Bool operator ==(const LinearModel & lhs, const LinearModel & rhs);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODEL_HXX */
