//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between evaluation and
 *        point to field function or between point to field function and
 *        field function
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_POINTTOFIELDCONNECTION_HXX
#define OPENTURNS_POINTTOFIELDCONNECTION_HXX

#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/FieldFunction.hxx"
#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointToFieldConnection
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API PointToFieldConnection
  : public PointToFieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PointToFieldConnection();

  /** Parameter constructor */
  PointToFieldConnection(const FieldFunction & fieldFunction,
			const PointToFieldFunction & pointToFieldFunction);


  /** parameter constructor */
  PointToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
			const Function & function);


  /** Virtual constructor */
  virtual PointToFieldConnection * clone() const;

  /** Comparison operator */
  Bool operator ==(const PointToFieldConnection & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using PointToFieldFunctionImplementation::operator();
  Field operator() (const Point & inP) const;

  ProcessSample operator() (const Sample & inS) const;

  /** Get the i-th marginal function */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Composed implementation accessor */
  Function getFunction() const;
  FieldFunction getFieldFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;

protected:


private:

  /** Flag to tell if it is a composition between a field function and a point
      to field function (true) or a point to field function and a field function */
  Bool startByPointToFieldFunction_;
  
  /** The function f in h = f o g */
  Function function_;

  /** The function g in h = f o g */
  FieldFunction fieldFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class PointToFieldConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOFIELDCONNECTION_HXX */
