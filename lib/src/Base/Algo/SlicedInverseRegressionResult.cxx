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

#include "openturns/SlicedInverseRegressionResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SlicedInverseRegressionResult);

static Factory<SlicedInverseRegressionResult> Factory_SlicedInverseRegressionResult;


/* Default constructor */
SlicedInverseRegressionResult::SlicedInverseRegressionResult()
  : PersistentObject()
{
  // Nothing to do
}

SlicedInverseRegressionResult::SlicedInverseRegressionResult(const OT::Matrix & directions,
                                                            const OT::Point & center,
                                                            const OT::Point & eigenValues)
  : PersistentObject()
  , directions_(directions)
  , center_(center)
  , eigenValues_(eigenValues)
{
  // Nothing to do
}

/* Virtual constructor method */
SlicedInverseRegressionResult * SlicedInverseRegressionResult::clone() const
{
  return new SlicedInverseRegressionResult(*this);
}

/* String converter */
String SlicedInverseRegressionResult::__repr__() const
{
  OSS oss;
  oss << "class=" << SlicedInverseRegressionResult::GetClassName();
  return oss;
}

Function SlicedInverseRegressionResult::getTransformation() const
{
  const UnsignedInteger outputDim = directions_.getNbColumns();
  return LinearFunction(center_, Point(outputDim), directions_.transpose());
}

Function SlicedInverseRegressionResult::getInverseTransformation() const
{
  const UnsignedInteger outputDim = directions_.getNbColumns();
  // Matrix::solveLinearSystem uses SVD (generalized inverse)
  // to handle the rectangular case where outputDim < inputDimension
  Matrix inv(directions_.transpose().solveLinearSystem(IdentityMatrix(outputDim)));
  return LinearFunction(Point(outputDim, 0.0), center_, inv);
}

Matrix SlicedInverseRegressionResult::getDirections() const
{
  return directions_;
}

Point SlicedInverseRegressionResult::getEigenvalues() const
{
  return eigenValues_;
}

/* Method save() stores the object through the StorageManager */
void SlicedInverseRegressionResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "directions_", directions_ );
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "eigenValues_", eigenValues_ );
}

/* Method load() reloads the object from the StorageManager */
void SlicedInverseRegressionResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "directions_", directions_ );
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "eigenValues_", eigenValues_ );
}

END_NAMESPACE_OPENTURNS
