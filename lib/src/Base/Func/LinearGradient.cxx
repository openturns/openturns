//                                               -*- C++ -*-
/**
 *  @brief Class for a quadratic numerical math gradient implementation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LinearGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Lapack.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearGradient)

static const Factory<LinearGradient> Factory_LinearGradient;

/* Default constructor */
LinearGradient::LinearGradient()
  : GradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearGradient::LinearGradient(const Point & center,
                               const Matrix & constant,
                               const SymmetricTensor & linear)
  : GradientImplementation()
  , center_(center)
  , constant_(constant)
  , linear_(linear)
{
  /* Check if the dimensions of the constant term is compatible with the linear term */
  if ((constant.getNbRows() != linear.getNbRows()) || (constant.getNbColumns() != linear.getNbSheets())) throw InvalidDimensionException(HERE) << "Constant term dimensions are incompatible with the linear term";
  /* Check if the dimensions of the center term is compatible with the linear term */
  if ((center.getDimension() != constant.getNbRows()) || (center.getDimension() != linear.getNbRows())) throw InvalidDimensionException(HERE) << "Center term dimensions are incompatible with the constant term or the linear term";
}

/* Virtual constructor */
LinearGradient * LinearGradient::clone() const
{
  return new LinearGradient(*this);
}

/* Comparison operator */
Bool LinearGradient::operator ==(const LinearGradient & other) const
{
  return ((linear_ == other.linear_) && (constant_ == other.constant_) && (center_ == other.center_));
}

/* String converter */
String LinearGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LinearGradient::GetClassName()
      << " name=" << getName()
      << " center=" << center_.__repr__()
      << " constant=" << constant_.__repr__()
      << " linear=" << linear_;
  return oss;
}

String LinearGradient::__str__(const String & offset) const
{
  OSS oss(false);
  oss << LinearGradient::GetClassName() << Os::GetEndOfLine() << offset
      << "  center :"  << Os::GetEndOfLine() << offset << center_.__str__(offset + "  ") << Os::GetEndOfLine()
      << "  constant :" << Os::GetEndOfLine() << offset << constant_.__str__(offset + "  ") << Os::GetEndOfLine()
      << "  linear :" << Os::GetEndOfLine() << offset << linear_.__str__(offset + "  ") << Os::GetEndOfLine();
  return oss;
}

/* Accessor for the center */
Point LinearGradient::getCenter() const
{
  return center_;
}

/* Accessor for the constant term */
Matrix LinearGradient::getConstant() const
{
  return constant_;
}

/* Accessor for the linear term */
SymmetricTensor LinearGradient::getLinear() const
{
  return linear_;
}

/* Here is the interface that all derived class must implement */

/* Gradient() */
Matrix LinearGradient::gradient(const Point & inP) const
{
  if (inP.getDimension() != constant_.getNbRows()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  Matrix value(constant_);
  // Add the linear term <linear, x>
  const UnsignedInteger nbSheets = linear_.getNbSheets();
  const UnsignedInteger nbRows = linear_.getNbRows();
  if (nbSheets == 0 || nbRows == 0)
    return value;
  const Point delta(inP - center_);
  char uplo('L');
  int n(nbRows);
  int one(1);
  double alpha(1.0);
  double beta(1.0);
  int luplo(1);
  for(UnsignedInteger k = 0; k < nbSheets; ++k)
    dsymv_(&uplo, &n, &alpha, const_cast<double*>(&(linear_(0, 0, k))), &n, const_cast<double*>(&(delta[0])), &one, &beta, &value(0, k), &one, &luplo);
  callsNumber_.increment();
  return value;
}

/* Accessor for input point dimension */
UnsignedInteger LinearGradient::getInputDimension() const
{
  return center_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger LinearGradient::getOutputDimension() const
{
  return constant_.getNbColumns();
}

/* Method save() stores the object through the StorageManager */
void LinearGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "linear_", linear_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "linear_", linear_ );
}

END_NAMESPACE_OPENTURNS
