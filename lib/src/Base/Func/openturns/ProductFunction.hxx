//                                               -*- C++ -*-
/**
 *  @brief The class that implements product numerical math functions
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
#ifndef OPENTURNS_PRODUCTFUNCTION_HXX
#define OPENTURNS_PRODUCTFUNCTION_HXX

#include "openturns/FunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProductFunction
 */

class OT_API ProductFunction
  : public FunctionImplementation
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Composition constructor */
  ProductFunction(const Implementation & p_left,
                  const Implementation & p_right);

  /** Virtual constructor */
  ProductFunction * clone() const override;

  /** Comparison operator */
  Bool operator ==(const ProductFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  ProductFunction() {};
  friend class Factory<ProductFunction>;

private:

  /** The f function in fog */
  Implementation p_leftFunction_;

  /** The g function in fog */
  Implementation p_rightFunction_;

}; /* class ProductFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTFUNCTION_HXX */
