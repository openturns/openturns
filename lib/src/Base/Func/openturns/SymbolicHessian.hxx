//                                               -*- C++ -*-
/**
 *  @brief The class that implements the Hessian of analytical functions
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

#ifndef OPENTURNS_SYMBOLICHESSIAN_HXX
#define OPENTURNS_SYMBOLICHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/Hessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SymbolicHessian
 *
 * The class that implement the Hessian of analytical functions.
 */
class OT_API SymbolicHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SymbolicHessian();

  /** Default constructor */
  SymbolicHessian(const SymbolicEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit SymbolicHessian(const Pointer<SymbolicEvaluation> & evaluation);
#endif

  /** Virtual constructor */
  virtual SymbolicHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const SymbolicHessian & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using HessianImplementation::hessian;
  SymmetricTensor hessian(const Point & inP) const;

  /** Accessor to a specific formula */
  String getFormula(const UnsignedInteger i,
                    const UnsignedInteger j,
                    const UnsignedInteger k) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Hessian getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Hessian getMarginal(const Indices & indices) const;

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

}; /* class SymbolicHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYMBOLICHESSIAN_HXX */
