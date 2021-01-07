//                                               -*- C++ -*-
/**
 *  @brief The class that implements translation functions.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TRANSLATIONFUNCTION_HXX
#define OPENTURNS_TRANSLATIONFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TranslationFunction
 *
 * The class that simulates a translation function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 */
class OT_API TranslationFunction
  : public Function
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  explicit TranslationFunction(const Point & constant);


  /** Comparison operator */
  Bool operator ==(const TranslationFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Accessor for the constant term */
  virtual Point getConstant() const;
  virtual void setConstant(const Point & constant);

}; /* class TranslationFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRANSLATIONFUNCTION_HXX */
