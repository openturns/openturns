//                                               -*- C++ -*-
/**
 *  @brief The class that implements the gradient of an analytical function.
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

#ifndef OPENTURNS_SYMBOLICGRADIENT_HXX
#define OPENTURNS_SYMBOLICGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SymbolicGradient
 *
 * The class that implement the gradient of an analytical function.
 */
class OT_API SymbolicGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SymbolicGradient();

  /** Default constructor */
  SymbolicGradient(const SymbolicEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit SymbolicGradient(Pointer<SymbolicEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  virtual SymbolicGradient * clone() const;

  /** Comparison operator */
  Bool operator ==(const SymbolicGradient & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Accessor to a specific formula */
  String getFormula(const UnsignedInteger i,
                    const UnsignedInteger j) const;

  /** Get the i-th marginal function */
  Gradient getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Gradient getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /* Must initialize the parser at the first call to operator() as the
     reference associated with the variables may have change after the construction */
  void initialize() const;

  mutable Bool isInitialized_;
  mutable Bool isAnalytical_;
  Pointer<SymbolicEvaluation> p_evaluation_;

  /** A mathematical expression parser from the muParser library */
  mutable SymbolicParser parser_;

}; /* class SymbolicGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYMBOLICGRADIENT_HXX */
