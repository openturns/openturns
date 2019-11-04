//                                               -*- C++ -*-
/**
 *  @brief The interface class that implements numerical math functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HESSIAN_HXX
#define OPENTURNS_HESSIAN_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/HessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hessian
 *
 * This class offers an abstract interface for the implementation
 * of a real numerical mathematical Hessian into the platform.
 */
class OT_API Hessian
  : public TypedInterfaceObject<HessianImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<HessianImplementation>   Implementation;

  /** Default constructor */
  Hessian();

  /** Constructor from EvaluationImplementation */
  Hessian(const HessianImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  Hessian(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  Hessian(HessianImplementation * p_implementation);
#endif

  /** Comparison operator */
  Bool operator ==(const Hessian & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Get the i-th marginal evaluation */
  virtual Hessian getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Hessian getMarginal(const Indices & indices) const;


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

}; /* class Hessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HESSIAN_HXX */
