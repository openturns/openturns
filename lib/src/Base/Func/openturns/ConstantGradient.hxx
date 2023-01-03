//                                               -*- C++ -*-
/**
 *  @brief Class for a linear numerical math gradient impelmentation
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
#ifndef OPENTURNS_CONSTANTGRADIENT_HXX
#define OPENTURNS_CONSTANTGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ConstantGradient
 *
 * This class offers an interface for an real numerical mathematical gradient
 * implementations into the platform. It realizes the computation of the image matrix
 * of a vector (aka Point) through the gradient. The
 * gradient may be loaded in an external way through a wrapper.
 */
class OT_API ConstantGradient
  : public GradientImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  ConstantGradient();

  /** Parameter constructor */
  ConstantGradient(const Matrix & constant);

  /** Virtual constructor */
  ConstantGradient * clone() const override;

  /** Comparison operator */
  Bool operator ==(const ConstantGradient & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the constant term */
  Matrix getConstant() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  Matrix constant_;
}; /* class ConstantGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONSTANTGRADIENT_HXX */
