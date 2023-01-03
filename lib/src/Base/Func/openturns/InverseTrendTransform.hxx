//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_INVERSETRENDTRANSOFORM_HXX
#define OPENTURNS_INVERSETRENDTRANSOFORM_HXX


#include "openturns/VertexValueFunction.hxx"
#include "openturns/Function.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseTrendTransform
 *
 * The class that simulates a dynamical function based on a
 * numerical math function that acts only on the mesh part
 * of a field.
 */

class TrendTransform;

class OT_API InverseTrendTransform
  : public VertexValueFunction
{
  CLASSNAME
public:

  /** Default constructor */
  InverseTrendTransform();

  /** Parameter constructor */
  InverseTrendTransform(const Function & function, const Mesh & mesh);

  /** Parameter constructor */
  InverseTrendTransform(const Evaluation & evaluation, const Mesh & mesh);

  /** Parameter constructor */
  InverseTrendTransform(const EvaluationImplementation & evaluation, const Mesh & mesh);

  /** Virtual constructor */
  InverseTrendTransform * clone() const override;

  /** Inverse accessor */
  TrendTransform getInverse() const;

  /** Underlying trend function accessor */
  Function getTrendFunction() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class InverseTrendTransform */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSETRENDTRANSOFORM_HXX */
