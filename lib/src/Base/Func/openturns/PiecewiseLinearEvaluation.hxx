//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
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
#ifndef OPENTURNS_PIECEWISELINEAREVALUATION_HXX
#define OPENTURNS_PIECEWISELINEAREVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PiecewiseLinearEvaluation
 *
 * The evaluation part of a linear piecewise scalar function
 *
 */

class OT_API PiecewiseLinearEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  PiecewiseLinearEvaluation();

  /** Parameter constructor */
  PiecewiseLinearEvaluation(const Point & locations,
                            const Point & values);

  /** Parameter constructor */
  PiecewiseLinearEvaluation(const Point & locations,
                            const Sample & values);

  /** Virtual constructor */
  virtual PiecewiseLinearEvaluation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Evaluation operator */
  using EvaluationImplementation::operator ();
  Point operator () (const Point & inP) const;
  Sample operator () (const Sample & inSample) const;

  /** Locations accessor */
  Point getLocations() const;
  void setLocations(const Point & locations);

  /** Values accessor */
  Sample getValues() const;
  void setValues(const Point & values);
  void setValues(const Sample & values);

  /** Simultaneous locations and values acessor */
  void setLocationsAndValues(const Point & locations,
                             const Sample & values);

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
  Point locations_;

  // The values
  Sample values_;

  // Are locations regularly spaced?
  Bool isRegular_;
} ; /* class PiecewiseLinearEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISELINEAREVALUATION_HXX */
