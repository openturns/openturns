//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_SPATIALFUNCTION_HXX
#define OPENTURNS_SPATIALFUNCTION_HXX

#include "PersistentObject.hxx"
#include "DynamicalFunctionImplementation.hxx"
#include "NumericalMathEvaluationImplementation.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpatialFunction
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the spatial part
 * of a time series.
 */
class OT_API SpatialFunction
  : public DynamicalFunctionImplementation
{
  CLASSNAME;
public:

  typedef NumericalMathEvaluationImplementation::Implementation EvaluationImplementation;

  /** Default constructor */
  explicit SpatialFunction(const UnsignedInteger meshDimension = 1);

  /** Parameter constructor */
  explicit SpatialFunction(const NumericalMathFunction & function,
                           const UnsignedInteger meshDimension = 1);

#ifndef SWIG
  /** Parameter constructor */
  explicit SpatialFunction(const EvaluationImplementation & p_evaluation,
                           const UnsignedInteger meshDimension = 1);
#endif

  /** Parameter constructor */
  explicit SpatialFunction(const NumericalMathEvaluationImplementation & evaluation,
                           const UnsignedInteger meshDimension = 1);

  /** Virtual constructor */
  virtual SpatialFunction * clone() const;

  /** Comparison operator */
  Bool operator ==(const SpatialFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  Field operator() (const Field & inFld) const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Evaluation accessor */
  EvaluationImplementation getEvaluation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Pointer to the actual evaluation class */
  EvaluationImplementation p_evaluation_;

}; /* class SpatialFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPATIALFUNCTION_HXX */
