//                                               -*- C++ -*-
/**
 *  @brief
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
#ifndef OPENTURNS_ORTHOGONALDIRECTION_HXX
#define OPENTURNS_ORTHOGONALDIRECTION_HXX

#include "openturns/SamplingStrategyImplementation.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalDirection
 */

class OT_API OrthogonalDirection :
  public SamplingStrategyImplementation
{

  CLASSNAME

public:


  /** Default constructor */
  OrthogonalDirection();

  /** Constructor with parameters */
  OrthogonalDirection(const UnsignedInteger dimension,
                      const UnsignedInteger size);


  /** Virtual constructor */
  OrthogonalDirection * clone() const override;

  /** Generate a set of directions */
  Sample generate() const override;

  /** String converter */
  String __repr__() const override;

private:
  /** Generate a random realization of an orientation matrix in SO(dimension) uniformly distributed relatively to the Haar mesure of SO(dimension) */
  Matrix getUniformOrientationRealization() const;

  /** Generate the next permutation of indices in-place in the size_ first elements */
  void nextCombination(Indices & indices) const;

  /** Add the 2^size linear combinations of columns of Q indicated in the
   * size first elements of indices by affecting all the choices of sign
   * to the coefficients of the linear combination */
  void computePartialSample(const Indices & indices,
                            const Matrix & Q,
                            Sample & result) const;
  /** Size of the direction subset */
  UnsignedInteger size_;

} ; /* class OrthogonalDirection */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALDIRECTION_HXX */
