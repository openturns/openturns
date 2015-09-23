//                                               -*- C++ -*-
/**
 * @brief Class for a linear numerical math function implementation
 *        of the form y = constant + <linear, x - c> where c is a
 *        dim(x) numerical point, linear a dim(x) by dim(y) matrix
 *        and  <linear, x - c> means Transpose(linear).(x - c)
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

#include "LinearNumericalMathEvaluationImplementation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearNumericalMathEvaluationImplementation);

static const Factory<LinearNumericalMathEvaluationImplementation> RegisteredFactory;

/* Default constructor */
LinearNumericalMathEvaluationImplementation::LinearNumericalMathEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearNumericalMathEvaluationImplementation::LinearNumericalMathEvaluationImplementation(const NumericalPoint & center,
    const NumericalPoint & constant,
    const Matrix & linear)
  : NumericalMathEvaluationImplementation()
  , center_(center)
  , constant_(constant)
  , linear_(linear.transpose())
{
  /* Check if the dimension of the constant term is compatible with the linear term */
  if (constant.getDimension() != linear.getNbColumns()) throw InvalidDimensionException(HERE) << "Constant term dimension is incompatible with the linear term";
  /* Check if the dimension of the center term is compatible with the linear term */
  if (center.getDimension() != linear.getNbRows()) throw InvalidDimensionException(HERE) << "Center term dimension is incompatible with the linear term";
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}

/* Virtual constructor */
LinearNumericalMathEvaluationImplementation * LinearNumericalMathEvaluationImplementation::clone() const
{
  return new LinearNumericalMathEvaluationImplementation(*this);
}

/* Comparison operator */
Bool LinearNumericalMathEvaluationImplementation::operator ==(const LinearNumericalMathEvaluationImplementation & other) const
{
  return ((linear_ == other.linear_) && (constant_ == other.constant_) && (center_ == other.center_));
}

/* String converter */
String LinearNumericalMathEvaluationImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LinearNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " constant=" << constant_
      << " linear=" << linear_.transpose();
  return oss;
}

String LinearNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << LinearNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " constant=" << constant_
      << " linear=" << linear_.transpose();
  return oss;
}

/* Accessor for the center */
NumericalPoint LinearNumericalMathEvaluationImplementation::getCenter() const
{
  return center_;
}

/* Accessor for the constant term */
NumericalPoint LinearNumericalMathEvaluationImplementation::getConstant() const
{
  return constant_;
}

/* Accessor for the linear term */
Matrix LinearNumericalMathEvaluationImplementation::getLinear() const
{
  return linear_.transpose();
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint LinearNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  if (inP.getDimension() != center_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const NumericalPoint result(constant_ + linear_ * (inP - center_));
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}
/* Operator () */
NumericalSample LinearNumericalMathEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  return NumericalMathEvaluationImplementation::operator()(inS);
}

/* Accessor for input point dimension */
UnsignedInteger LinearNumericalMathEvaluationImplementation::getInputDimension() const
{
  return center_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger LinearNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return constant_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void LinearNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "linear_", linear_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "linear_", linear_ );
}

END_NAMESPACE_OPENTURNS
