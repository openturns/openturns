//                                               -*- C++ -*-
/**
 *  @brief Tuples experiment plane
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TUPLES_HXX
#define OPENTURNS_TUPLES_HXX

#include "openturns/CombinatorialGeneratorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Tuples
 *
 * The class describes the concept of tuples generator
 */
class OT_API Tuples
  : public CombinatorialGeneratorImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  Tuples();

  explicit Tuples(const Indices & bounds);

  /** Virtual constructor */
  Tuples * clone() const override;

  /** Next tuple generation, stateful iteration like SplitterImplementation :
   *  all the tuples taking values in {0,...,bounds[0]-1}x...x{0,...,bounds[n-1]-1}
   */
  Indices generateNext() const override;

  /** Number of tuples accessor */
  UnsignedInteger getSize() const override;

  /** Dimension of the generated tuples accessor */
  UnsignedInteger getDimension() const override;

  /** Restart the combinatorial sequence */
  void restart() const override;

  /** String converter */
  String __repr__() const override;

  /** Bounds accessor */
  void setBounds(const Indices & bounds);

  Indices getBounds() const;

private:
  /** Bounds on the marginal values of the tuple */
  Indices bounds_;

  /** Current tuple for stateful iteration */
  mutable Indices current_;

  /** Number of tuples already generated */
  mutable UnsignedInteger currentIndex_ = 0;

}; /* class Tuples */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TUPLES_HXX */
