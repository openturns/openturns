//                                               -*- C++ -*-
/**
 *  @brief Class for a quadratic numerical math function implementation
 *        of the form y = constant + <linear, x-c> + <<quadratic, x-c>, x-c>
 *        where constant is a dim(y) numerical point, c a dim(x) numerical
 *        point, linear is a dim(x) by dim(y) matrix, quadratic is a
 *        dim(x) by dim(x) by dim(y) symmetric tensor and <linear, x>
 *        means Transpose(linear).x, <quadratic, x> means
 *        Transpose_kj(quadratic).x
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/QuadraticEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(QuadraticEvaluation)

static const Factory<QuadraticEvaluation> Factory_QuadraticEvaluation;

/* Default constructor */
QuadraticEvaluation::QuadraticEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor f(x) = constant + linear.x + 1/2 x'.quadratic.x */
QuadraticEvaluation::QuadraticEvaluation(const Point & center,
    const Point & constant,
    const Matrix & linear,
    const SymmetricTensor & quadratic)
  : EvaluationImplementation(),
    center_(center),
    constant_(constant),
    linear_(linear.transpose()),
    quadratic_(quadratic)
{
  /* Check if the dimension of the constant term is compatible with the linear and quadratic terms */
  if ((constant.getDimension() != linear.getNbColumns()) || (constant.getDimension() != quadratic.getNbSheets())) throw InvalidDimensionException(HERE) << "Constant term dimension is incompatible with the linear term or with the quadratic term";
  /* Check if the dimension of the center is compatible with the linear and quadratic terms */
  if ((center.getDimension() != linear.getNbRows()) || (center.getDimension() != quadratic.getNbRows())) throw InvalidDimensionException(HERE) << "Center term dimension is incompatible with the linear term or with the quadratic term";
  setInputDescription(Description::BuildDefault(center.getDimension(), "x"));
  setOutputDescription(Description::BuildDefault(constant.getDimension(), "y"));
}

/* Virtual constructor */
QuadraticEvaluation * QuadraticEvaluation::clone() const
{
  return new QuadraticEvaluation(*this);
}

/* Comparison operator */
Bool QuadraticEvaluation::operator ==(const QuadraticEvaluation & other) const
{
  return ((quadratic_ == other.getQuadratic()) && (linear_ == other.getLinear()) && (constant_ == other.getConstant()) && (center_ == other.getCenter()));
}

/* String converter */
String QuadraticEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticEvaluation::GetClassName()
      << " name=" << getName()
      << " center=" << center_.__repr__()
      << " constant=" << constant_.__repr__()
      << " linear=" << linear_.transpose()
      << " quadratic=" << quadratic_;
  return oss;
}

String QuadraticEvaluation::__str__(const String & offset) const
{
  OSS oss;
  oss << QuadraticEvaluation::GetClassName() << Os::GetEndOfLine()
      << offset << "  center :"  << Os::GetEndOfLine()   << offset << "  " << center_.__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  constant :" << Os::GetEndOfLine()  << offset << "  " << constant_.__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  linear :" << Os::GetEndOfLine()    << offset << "  " << linear_.transpose().__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  quadratic :" << Os::GetEndOfLine() << offset << "  " << quadratic_.__str__(offset + "  ") << Os::GetEndOfLine();
  return oss;
}

/* Accessor for the center */
Point QuadraticEvaluation::getCenter() const
{
  return center_;
}

/* Accessor for the constant term */
Point QuadraticEvaluation::getConstant() const
{
  return constant_;
}

/* Accessor for the linear term */
Matrix QuadraticEvaluation::getLinear() const
{
  return linear_.transpose();
}

/* Accessor for the quadratic term */
SymmetricTensor QuadraticEvaluation::getQuadratic() const
{
  return quadratic_;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point QuadraticEvaluation::operator() (const Point & inP) const
{
  if ((inP.getDimension() != linear_.getNbColumns()) || (inP.getDimension() != quadratic_.getNbRows())) throw InvalidArgumentException(HERE) << "Invalid input dimension " << inP.getDimension();
  /* We don't have a true linear algebra with tensors, so we must perform the tensor/vector product by hand */
  const Point delta(inP - center_);
  Point result(constant_ + linear_ * delta);

  const UnsignedInteger nbSheets = quadratic_.getNbSheets();
  const UnsignedInteger nbRows = quadratic_.getNbRows();
  if (nbSheets == 0 || nbRows == 0)
    return result;
  char uplo = 'L';
  int n = nbRows;
  int one = 1;
  double alpha = 1.0;
  double beta = 0.0;
  int luplo = 1;
  Point temp(nbRows);
  for(UnsignedInteger index = 0; index < nbSheets; ++index)
  {
    dsymv_(&uplo, &n, &alpha, const_cast<double*>(&(quadratic_(0, 0, index))), &n, const_cast<double*>(delta.data()), &one, &beta, const_cast<double*>(temp.data()), &one, &luplo);
    result[index] += 0.5 * ddot_(&n, const_cast<double*>(delta.data()), &one, const_cast<double*>(temp.data()), &one);
  }
  callsNumber_.increment();
  return result;
}

Sample QuadraticEvaluation::operator() (const Sample & inS) const
{
  if ((inS.getDimension() != linear_.getNbColumns()) || (inS.getDimension() != quadratic_.getNbRows())) throw InvalidArgumentException(HERE) << "Invalid input dimension " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  if (size == 0)
    return Sample(0, inS.getDimension());
  const Sample delta(inS - center_);
  Sample result(linear_.getImplementation()->genSampleProd(delta, true, false, 'R'));
  result.setDescription(getOutputDescription());
  result += constant_;

  const UnsignedInteger nbSheets = quadratic_.getNbSheets();
  const UnsignedInteger nbRows = quadratic_.getNbRows();
  if (nbSheets == 0 || nbRows == 0)
    return result;
  char side = 'L';
  char uplo = 'L';
  int m = nbRows;
  int n = size;
  int one = 1;
  double alpha = 1.0;
  double beta = 0.0;
  int lside = 1;
  int luplo = 1;
  MatrixImplementation temp(nbRows, size);
  for(UnsignedInteger index = 0; index < nbSheets; ++index)
  {
    dsymm_(&side, &uplo, &m, &n, &alpha, const_cast<double*>(&(quadratic_(0, 0, index))), &m, const_cast<double*>(delta.data()), &m, &beta, const_cast<double*>(temp.data()), &m, &lside, &luplo);
    for(UnsignedInteger i = 0; i < size; ++i)
      result(i, index) += 0.5 * ddot_(&m, const_cast<double*>(&delta(i, 0)), &one, &temp(0, i), &one);
  }
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger QuadraticEvaluation::getInputDimension() const
{
  return center_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger QuadraticEvaluation::getOutputDimension() const
{
  return constant_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void QuadraticEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "linear_", linear_ );
  adv.saveAttribute( "quadratic_", quadratic_ );
}

/* Method load() reloads the object from the StorageManager */
void QuadraticEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "linear_", linear_ );
  adv.loadAttribute( "quadratic_", quadratic_ );
}

END_NAMESPACE_OPENTURNS
