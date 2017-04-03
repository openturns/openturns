//                                               -*- C++ -*-
/**
 *  @brief The evaluation part of a hermite piecewise scalar function
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
  CLASSNAME;
public:

  // friend class Factory<PiecewiseHermiteEvaluation>;

  /** Default constructor */
  PiecewiseHermiteEvaluation();

  /** Parameter constructor */
  PiecewiseHermiteEvaluation(const NumericalPoint & locations,
      const NumericalPoint & values,
      const NumericalPoint & derivatives);

  /** Parameter constructor */
  PiecewiseHermiteEvaluation(const NumericalPoint & locations,
      const NumericalSample & values,
      const NumericalSample & derivatives);

  /** Virtual constructor */
  virtual PiecewiseHermiteEvaluation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Evaluation operator */
  using EvaluationImplementation::operator ();
  NumericalPoint operator () (const NumericalPoint & inP) const;

  /** Compute the derivative */
  NumericalPoint derivate(const NumericalPoint & inP) const;

  /** Locations accessor */
  NumericalPoint getLocations() const;
  void setLocations(const NumericalPoint & locations);

  /** Values accessor */
  NumericalSample getValues() const;
  void setValues(const NumericalSample & values);

  /** Derivatives accessor */
  NumericalSample getDerivatives() const;
  void setDerivatives(const NumericalSample & derivatives);

  /** Simultaneous locations and values acessor */
  void setLocationsValuesAndDerivatives(const NumericalPoint & locations,
                                        const NumericalSample & values,
                                        const NumericalSample & derivatives);

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

  // The derivatives
  NumericalSample derivatives_;

  // Are locations regularly spaced?
  Bool isRegular_;
} ; /* class PiecewiseHermiteEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISEHERMITEEVALUATION_HXX */
