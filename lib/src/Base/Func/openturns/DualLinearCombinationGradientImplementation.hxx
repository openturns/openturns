//                                               -*- C++ -*-
/**
 *  @brief The gradient part of functional linear combination of vectors
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DUALLINEARCOMBINATIONGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_DUALLINEARCOMBINATIONGRADIENTIMPLEMENTATION_HXX

#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/DualLinearCombinationEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class DualLinearCombinationGradientImplementation
 *
 * The gradient part of functional linear combination of vectors
 */

class OT_API DualLinearCombinationGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:


  // friend class Factory<DualLinearCombinationGradientImplementation>;

  /** Default constructor */
  DualLinearCombinationGradientImplementation();

  /** Parameters constructor */
  DualLinearCombinationGradientImplementation(const DualLinearCombinationEvaluationImplementation & evaluation);

  /** Virtual constructor */
  virtual DualLinearCombinationGradientImplementation * clone() const;

  /** Gradient method */
  virtual Matrix gradient(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:
  /** The associated evaluation */
  DualLinearCombinationEvaluationImplementation evaluation_;

} ; /* class DualLinearCombinationGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DUALLINEARCOMBINATIONGRADIENTIMPLEMENTATION_HXX */
