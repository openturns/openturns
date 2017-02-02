//                                               -*- C++ -*-
/**
 *  @brief The class that implements composed numerical math functions
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COMPOSEDFUNCTION_HXX
#define OPENTURNS_COMPOSEDFUNCTION_HXX

#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ComposedFunction
 */

class OT_API ComposedFunction
  : public NumericalMathFunctionImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  ComposedFunction();

  /** Composition constructor */
  ComposedFunction(const Implementation & p_left,
                                const Implementation & p_right);

  /** Composition constructor */
  ComposedFunction(const NumericalMathFunction & left,
                                const NumericalMathFunction & right);

  /** Virtual constructor */
  virtual ComposedFunction * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedFunction & other) const;

  /** String converter */
  virtual String __repr__() const;



  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const NumericalPoint & inP) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  //ComposedFunction() {};
  //friend class Factory<ComposedFunction>;

private:

  /** The f function in fog */
  Implementation p_leftFunction_;

  /** The g function in fog */
  Implementation p_rightFunction_;

}; /* class ComposedFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDFUNCTION_HXX */
