//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a linear piecewise scalar function
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
#ifndef OPENTURNS_PIECEWISELINEAREVALUATION_HXX
#define OPENTURNS_PIECEWISELINEAREVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"

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

  friend class PiecewiseHermiteEvaluation;

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
  PiecewiseLinearEvaluation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Evaluation operator */
  using EvaluationImplementation::operator ();
  Point operator () (const Point & inP) const override;
  Sample operator () (const Sample & inSample) const override;

  /** Locations accessor */
  Point getLocations() const;
  void setLocations(const Point & locations);

  /** enableExtrapolation accessor */
  Bool getEnableExtrapolation() const;
  void setEnableExtrapolation(const Bool & enableExtrapolation);

  /** Values accessor */
  Sample getValues() const;
  void setValues(const Point & values);
  void setValues(const Sample & values);

  /** Simultaneous locations and values acessor */
  void setLocationsAndValues(const Point & locations,
                             const Sample & values);

  /** Input dimension accessor */
  UnsignedInteger getInputDimension() const override;

  /** Output dimension accessor */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check if the locations grid is regular to the given tolerance */
  static Bool IsRegular(const Point & locations,
                        const Scalar & epsilon);

  /** Find the segment containing value by bisection */
  static UnsignedInteger FindSegmentIndex(const Point & locations,
                                          const Scalar value,
                                          const UnsignedInteger start,
                                          const Bool isRegular);

private:
  /** Enable extrapolation */
  Bool enableExtrapolation_ = false;

  // The locations
  Point locations_;

  // The values
  Sample values_;

  // Are locations regularly spaced?
  Bool isRegular_;

} ; /* class PiecewiseLinearEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISELINEAREVALUATION_HXX */
