//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PIECEWISELINEAREVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_PIECEWISELINEAREVALUATIONIMPLEMENTATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PiecewiseLinearEvaluationImplementation
 *
 * The evaluation part of a linear piecewise scalar function
 *
 */

class OT_API PiecewiseLinearEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  // friend class Factory<PiecewiseLinearEvaluationImplementation>;

  /** Default constructor */
  PiecewiseLinearEvaluationImplementation();

  /** Parameter constructor */
  PiecewiseLinearEvaluationImplementation(const NumericalPoint & locations,
                                          const NumericalPoint & values);

  /** Parameter constructor */
  PiecewiseLinearEvaluationImplementation(const NumericalPoint & locations,
                                          const NumericalSample & values);

  /** Virtual constructor */
  virtual PiecewiseLinearEvaluationImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Evaluation operator */
  using NumericalMathEvaluationImplementation::operator ();
  NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Locations accessor */
  NumericalPoint getLocations() const;
  void setLocations(const NumericalPoint & locations);

  /** Values accessor */
  NumericalSample getValues() const;
  void setValues(const NumericalPoint & values);
  void setValues(const NumericalSample & values);

  /** Simultaneous locations and values acessor */
  void setLocationsAndValues(const NumericalPoint & locations,
                             const NumericalSample & values);

  /** Input dimension accessor */
  UnsignedInteger getInputDimension() const;

  /** Output dimension accessor */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  // The locations
  NumericalPoint locations_;

  // The values
  NumericalSample values_;

  // Are locations regularly spaced?
  Bool isRegular_;
} ; /* class PiecewiseLinearEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISELINEAREVALUATIONIMPLEMENTATION_HXX */
