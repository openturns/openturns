//                                               -*- C++ -*-
/**
 *  @brief Marginal gradient
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
#ifndef OPENTURNS_MARGINALGRADIENT_HXX
#define OPENTURNS_MARGINALGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class MarginalGradient
 *
 * Marginal gradient
 */
class OT_API MarginalGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MarginalGradient();

  /** Parameters constructor */
  MarginalGradient(const GradientImplementation & gradient,
                   const Indices & indices);

#ifndef SWIG
  /** Parameters constructor */
  MarginalGradient(const Pointer<GradientImplementation> & p_gradient,
                   const Indices & indices);
#endif
  /** Virtual constructor */
  virtual MarginalGradient * clone() const;

  /** Comparison operator */
  Bool operator ==(const MarginalGradient & other) const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Gradient method */
  virtual Matrix gradient(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The underlying function */
  Pointer<GradientImplementation> p_gradient_;

  /** The indices of the marginals */
  Indices indices_;

}; /* class MarginalGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALGRADIENT_HXX */
