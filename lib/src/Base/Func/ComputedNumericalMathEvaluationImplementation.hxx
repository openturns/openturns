//                                               -*- C++ -*-
/**
 * @brief Abstract top-level class for all ComputedNumerical math functions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_COMPUTEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_COMPUTEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "Pointer.hxx"
#include "NumericalPoint.hxx"
#include "WrapperFile.hxx"

BEGIN_NAMESPACE_OPENTURNS




/* Forward declaration */
class WrapperObject;

/**
 * @class ComputedNumericalMathEvaluationImplementation
 *
 * This class acts like an real numerical mathematical function
 * into the platform. It realizes the computation of the image vector
 * of a vector (aka NumericalPoint) through the function. The
 * function may be loaded in an external way through a wrapper.
 */
class OT_API ComputedNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
  friend class NumericalMathFunctionImplementation;

public:

  /* Some typedefs to ease reading */
  typedef Pointer<WrapperObject>                                          InternalFunction;
  typedef NumericalMathEvaluationImplementation::CacheKeyType             CacheKeyType;
  typedef NumericalMathEvaluationImplementation::CacheValueType           CacheValueType;
  typedef NumericalMathEvaluationImplementation::CacheType                CacheType;

  /** Default constructor */
  ComputedNumericalMathEvaluationImplementation(const String & name, const WrapperFile & file);

  /** Copy constructor */
  ComputedNumericalMathEvaluationImplementation(const ComputedNumericalMathEvaluationImplementation & other);

  /** Virtual constructor */
  virtual ComputedNumericalMathEvaluationImplementation * clone() const;

  /** Destructor */
  virtual ~ComputedNumericalMathEvaluationImplementation();

  /** Comparison operator */
  Bool operator ==(const ComputedNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  String __repr__() const;
  virtual String __str__(const String & offset = "") const;



  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;
  virtual NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for description */
  virtual Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

  /** The function object that is linked to the external code */
  InternalFunction p_function_;

  /** State management */
  void * getState() const;

private:

  ComputedNumericalMathEvaluationImplementation() {};
  friend class Factory<ComputedNumericalMathEvaluationImplementation>;

  /** A pointer to an internal structure that saves the state of the function into the wrapper */
  void * p_state_;

}; /* class ComputedNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPUTEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
