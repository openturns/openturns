//                                               -*- C++ -*-
/**
 *  @brief The gradient part of functional linear combination of vectors
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
#ifndef OPENTURNS_DUALLINEARCOMBINATIONGRADIENT_HXX
#define OPENTURNS_DUALLINEARCOMBINATIONGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/DualLinearCombinationEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class DualLinearCombinationGradient
 *
 * The gradient part of functional linear combination of vectors
 */

class OT_API DualLinearCombinationGradient
  : public GradientImplementation
{
  CLASSNAME;
public:


  // friend class Factory<DualLinearCombinationGradient>;

  /** Default constructor */
  DualLinearCombinationGradient();

  /** Parameters constructor */
  DualLinearCombinationGradient(const DualLinearCombinationEvaluation & evaluation);

  /** Virtual constructor */
  virtual DualLinearCombinationGradient * clone() const;

  /** Gradient method */
  virtual Matrix gradient(const Point & inP) const;

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
  DualLinearCombinationEvaluation evaluation_;

} ; /* class DualLinearCombinationGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DUALLINEARCOMBINATIONGRADIENT_HXX */
