//                                               -*- C++ -*-
/**
 *  @brief The interface class that implements numerical math functions
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GRADIENT_HXX
#define OPENTURNS_GRADIENT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/GradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Gradient
 *
 * This class offers an abstract interface for the implementation
 * of a real numerical mathematical Gradient into the platform.
 */
class OT_API Gradient
  : public TypedInterfaceObject<GradientImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<GradientImplementation>   Implementation;

  /** Default constructor */
  Gradient();

  /** Constructor from EvaluationImplementation */
  Gradient(const GradientImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  Gradient(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  Gradient(GradientImplementation * p_implementation);
#endif

  /** Comparison operator */
  using TypedInterfaceObject<GradientImplementation>::operator ==;
  Bool operator ==(const Gradient & other) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Get the i-th marginal evaluation */
  virtual Gradient getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Gradient getMarginal(const Indices & indices) const;


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

}; /* class Gradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRADIENT_HXX */
