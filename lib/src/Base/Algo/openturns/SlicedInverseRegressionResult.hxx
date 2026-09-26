//                                               -*- C++ -*-
/**
 *  @brief SlicedInverseRegressionResult
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
#ifndef OPENTURNS_SLICEDINVERSEREGRESSIONRESULT_HXX
#define OPENTURNS_SLICEDINVERSEREGRESSIONRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SlicedInverseRegressionResult
 *
 * Result from SIR algorithm 
 */
class OT_API SlicedInverseRegressionResult
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  SlicedInverseRegressionResult();

  SlicedInverseRegressionResult(const Matrix & linear,
                                const Point & center,
                                const Point & eigenValues);

  /** Virtual constructor method */
  SlicedInverseRegressionResult * clone() const override;

  Matrix getDirections() const;
  Point getEigenvalues() const;
  
  Function getTransformation() const;
  Function getInverseTransformation() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Matrix directions_;
  Point center_;
  Point eigenValues_;

}; /* class SlicedInverseRegressionResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SLICEDINVERSEREGRESSIONRESULT_HXX */
