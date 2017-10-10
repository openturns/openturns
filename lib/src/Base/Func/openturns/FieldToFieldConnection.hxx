//                                               -*- C++ -*-
/**
 * @brief The class that implements the composition between field functions or 
 *        point to field and field to point functions
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

#ifndef OPENTURNS_FIELDTOFIELDCONNECTION_HXX
#define OPENTURNS_FIELDTOFIELDCONNECTION_HXX

#include "openturns/FieldFunction.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldToPointFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FieldToFieldConnection
 *
 * The class that implements the composition between field functions or 
 * point to field and field to point functions
 */
class OT_API FieldToFieldConnection
  : public FieldFunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FieldToFieldConnection();

  /** Parameter constructor */
  FieldToFieldConnection(const FieldFunction & leftFieldFunction,
			const FieldFunction & rightFieldFunction);


  /** parameter constructor */
  FieldToFieldConnection(const PointToFieldFunction & pointToFieldFunction,
			const FieldToPointFunction & fieldToPointFunction);


  /** Virtual constructor */
  virtual FieldToFieldConnection * clone() const;

  /** Comparison operator */
  Bool operator ==(const FieldToFieldConnection & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using FieldFunctionImplementation::operator();
  Field operator() (const Field & inF) const;

  ProcessSample operator() (const ProcessSample & inS) const;

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

  /** Composed implementation accessor */
  FieldFunction getLeftFieldFunction() const;
  FieldFunction getRightFieldFunction() const;
  PointToFieldFunction getPointToFieldFunction() const;
  FieldToPointFunction getFieldToPointFunction() const;

protected:


private:

  /** Flag to tell if it is a composition of functions or a
   * composition of point to field and field to point functions */
  Bool isFieldFunctionComposition_;
  
  /** The field function f in h = f o g */
  FieldFunction leftFieldFunction_;

  /** The field function g in h = f o g */
  FieldFunction rightFieldFunction_;

  /** The field to point function */
  FieldToPointFunction fieldToPointFunction_;

  /** The point to field function */
  PointToFieldFunction pointToFieldFunction_;

}; /* class FieldToFieldConnection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOFIELDCONNECTION_HXX */
