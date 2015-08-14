//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComputedNumerical math hessians
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
#ifndef OPENTURNS_COMPUTEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_COMPUTEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX

#include "NumericalMathHessianImplementation.hxx"
#include "Pointer.hxx"
#include "NumericalPoint.hxx"
#include "SymmetricTensor.hxx"
#include "WrapperFile.hxx"

BEGIN_NAMESPACE_OPENTURNS




/* Forward declaration */
class WrapperObject;

/**
 * @class ComputedNumericalMathHessianImplementation
 *
 * This class acts like an real numerical mathematical hessian
 * into the platform. It realizes the computation of the image tensor
 * of a vector (aka NumericalPoint) through the hessian. The
 * hessian may be loaded in an external way through a wrapper.
 */
class OT_API ComputedNumericalMathHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;

public:

  /* Some typedefs to ease reading */
  typedef Pointer<WrapperObject>                               InternalHessian;

  /** Default constructor */
  ComputedNumericalMathHessianImplementation(const String & name, const WrapperFile & file, void * p_state = 0);

  /** Copy constructor */
  ComputedNumericalMathHessianImplementation(const ComputedNumericalMathHessianImplementation & other);

  /** Virtual constructor */
  virtual ComputedNumericalMathHessianImplementation * clone() const;

  /** Destructor */
  virtual ~ComputedNumericalMathHessianImplementation();

  /** Comparison operator */
  Bool operator ==(const ComputedNumericalMathHessianImplementation & other) const;

  /** String converter */
  String __repr__() const;
  virtual String __str__(const String & offset = "") const;



  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  virtual SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The function object that is linked to the external code */
  InternalHessian p_hessian_;

  /** State management */
  void * getState() const;

private:
  ComputedNumericalMathHessianImplementation() {};
  friend class Factory<ComputedNumericalMathHessianImplementation>;

  /** A pointer to an internal structure that saves the state of the function into the wrapper */
  void * p_state_;

  /** A flag that is true when the gradient shares its internal state with its related function */
  Bool commonState_;

}; /* class ComputedNumericalMathHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPUTEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX */
