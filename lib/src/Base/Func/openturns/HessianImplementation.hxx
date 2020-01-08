//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math hessian implementations
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
#ifndef OPENTURNS_HESSIANIMPLEMENTATION_HXX
#define OPENTURNS_HESSIANIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Description.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/AtomicFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class Hessian;

/**
 * @class HessianImplementation
 *
 * This class offers an abstract interface for the implementation
 * of an real numerical mathematical hessian into the platform.
 */
class OT_API HessianImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  HessianImplementation();

  /** Virtual constructor */
  virtual HessianImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const HessianImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;

  /** Get the i-th marginal evaluation */
  virtual Hessian getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Hessian getMarginal(const Indices & indices) const;



  /* Here is the interface that all derived class must implement */

  /** Hessian method */
  virtual SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameter);

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Number of calls since the construction */
  mutable AtomicInt callsNumber_;

  /** The value of the parameters */
  Point parameter_;

private:


}; /* class HessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HESSIANIMPLEMENTATION_HXX */
