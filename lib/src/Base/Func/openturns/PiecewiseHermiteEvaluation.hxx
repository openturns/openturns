//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a hermite piecewise scalar function
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PIECEWISEHERMITEEVALUATION_HXX
#define OPENTURNS_PIECEWISEHERMITEEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PiecewiseHermiteEvaluation
 *
 * The evaluation part of a hermite piecewise scalar function
 *
 */

class OT_API PiecewiseHermiteEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  PiecewiseHermiteEvaluation();

  /** Parameter constructor */
  PiecewiseHermiteEvaluation(const Point & locations,
                             const Point & values,
                             const Point & derivatives);

  /** Parameter constructor */
  PiecewiseHermiteEvaluation(const Point & locations,
                             const Sample & values,
                             const Sample & derivatives);

  /** Virtual constructor */
  PiecewiseHermiteEvaluation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Evaluation operator */
  using EvaluationImplementation::operator ();
  Point operator () (const Point & inP) const override;
  Sample operator () (const Sample & inSample) const override;

  /** Compute the derivative */
  Point derivate(const Point & inP) const;

  /** Locations accessor */
  Point getLocations() const;
  void setLocations(const Point & locations);

  /** Values accessor */
  Sample getValues() const;
  void setValues(const Sample & values);

  /** Derivatives accessor */
  Sample getDerivatives() const;
  void setDerivatives(const Sample & derivatives);

  /** Simultaneous locations and values acessor */
  void setLocationsValuesAndDerivatives(const Point & locations,
                                        const Sample & values,
                                        const Sample & derivatives);

  /** Input dimension accessor */
  UnsignedInteger getInputDimension() const override;

  /** Output dimension accessor */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

private:
  // The locations
  Point locations_;

  // The values
  Sample values_;

  // The derivatives
  Sample derivatives_;

  // Are locations regularly spaced?
  Bool isRegular_;
} ; /* class PiecewiseHermiteEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISEHERMITEEVALUATION_HXX */
