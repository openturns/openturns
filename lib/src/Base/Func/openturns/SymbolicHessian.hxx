//                                               -*- C++ -*-
/**
 * @brief The class that implements the Hessian of analytical functions
 *
 * Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_SYMBOLICHESSIAN_HXX
#define OPENTURNS_SYMBOLICHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/SymbolicEvaluation.hxx"

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
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

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
  SymbolicEvaluation evaluation_;

  /** A mathematical expression parser from the muParser library */
  mutable SymbolicParser parser_;

}; /* class SymbolicHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYMBOLICHESSIAN_HXX */
