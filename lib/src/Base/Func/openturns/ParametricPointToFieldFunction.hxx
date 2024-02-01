//                                               -*- C++ -*-
/**
 *  @brief The class that implements parametric Point To Field functions.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PARAMETRICPOINTTOFIELDFUNCTION_HXX
#define OPENTURNS_PARAMETRICPOINTTOFIELDFUNCTION_HXX

#include "openturns/PointToFieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API ParametricPointToFieldFunction : public PointToFieldFunctionImplementation
{
  CLASSNAME

public:

  /* Default constructor */
  ParametricPointToFieldFunction();

  /** Parameter constructor */
  ParametricPointToFieldFunction (const PointToFieldFunction & function,
                                  const Indices & set,
                                  const Point & referencePoint);

  /** Virtual constructor */
  ParametricPointToFieldFunction * clone() const override;

  /** Parameters positions accessor */
  Indices getParametersPositions() const;

  /** Input positions accessor */
  Indices getInputPositions() const;

  /** Function accessor */
  PointToFieldFunction getFunction() const;

  /** Parameters accessor */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  Sample operator() (const Point & point) const override;
  ProcessSample operator() (const Sample & inS) const override;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Description getParameterDescription() const;

  /** The underlying function */
  PointToFieldFunction function_;

  /** The position of the parameters */
  Indices parametersPositions_;

  /** The position of the input */
  Indices inputPositions_;

  /** The value of the parameters */
  Point parameter_;

};
}
#endif
