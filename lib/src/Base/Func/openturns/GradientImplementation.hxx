//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math gradient implementations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_GRADIENTIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Description.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Tensor.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/AtomicInt.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class Gradient;

/**
 * @class GradientImplementation
 *
 * This class offers an abstract interface for the implementation
 * of an real numerical mathematical Gradient into the platform.
 */
class OT_API GradientImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  GradientImplementation();

  /** The Gradient that actually realizes assignment between objects */

  /** Virtual constructor */
  GradientImplementation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const GradientImplementation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Get the i-th marginal evaluation */
  virtual Gradient getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Gradient getMarginal(const Indices & indices) const;

  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;



  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  virtual Matrix gradient(const Point & inP) const;

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
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Number of calls since the construction */
  mutable AtomicInt callsNumber_;

private:

  /** The value of the parameters */
  Point parameter_;

}; /* class GradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRADIENTIMPLEMENTATION_HXX */
