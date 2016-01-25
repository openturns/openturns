//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math gradient implementations
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
#ifndef OPENTURNS_NUMERICALMATHGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_NUMERICALMATHGRADIENTIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "NumericalPointWithDescription.hxx"
#include "Description.hxx"
#include "Indices.hxx"
#include "Matrix.hxx"
#include "Tensor.hxx"
#include "Pointer.hxx"
#include "StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NumericalMathGradientImplementation
 *
 * This class offers an abstract interface for the implementation
 * of an real numerical mathematical Gradient into the platform.
 */
class OT_API NumericalMathGradientImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef Pointer<NumericalMathGradientImplementation> Implementation;


  /** Default constructor */
  NumericalMathGradientImplementation();

  /** The Gradient that actually realizes assignment between objects */

  /** Virtual constructor */
  virtual NumericalMathGradientImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const NumericalMathGradientImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;


  /** Get the i-th marginal evaluation */
  virtual Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;



  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  virtual Matrix gradient(const NumericalPoint & inP) const;
  virtual Matrix gradient(const NumericalPoint & inP,
                          const NumericalPoint & parameters);

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Parameters value and description accessor */
  virtual NumericalPointWithDescription getParameter() const;
  virtual void setParameter(const NumericalPointWithDescription & parameters);

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Number of calls since the construction */
  mutable UnsignedInteger callsNumber_;

private:

  /** The value and description of all the parameters */
  NumericalPointWithDescription parameters_;

}; /* class NumericalMathGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALMATHGRADIENTIMPLEMENTATION_HXX */
