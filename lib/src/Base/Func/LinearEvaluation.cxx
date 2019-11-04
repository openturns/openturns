//                                               -*- C++ -*-
/**
 * @brief Class for a linear numerical math function implementation
 *        of the form y = constant + <linear, x - c> where c is a
 *        dim(x) numerical point, linear a dim(x) by dim(y) matrix
 *        and  <linear, x - c> means Transpose(linear).(x - c)
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/LinearEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearEvaluation)

static const Factory<LinearEvaluation> Factory_LinearEvaluation;

/* Default constructor */
LinearEvaluation::LinearEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearEvaluation::LinearEvaluation(const Point & center,
                                   const Point & constant,
                                   const Matrix & linear)
  : EvaluationImplementation()
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
LinearEvaluation * LinearEvaluation::clone() const
{
  return new LinearEvaluation(*this);
}

/* Comparison operator */
Bool LinearEvaluation::operator ==(const LinearEvaluation & other) const
{
  return ((linear_ == other.linear_) && (constant_ == other.constant_) && (center_ == other.center_));
}

/* String converter */
String LinearEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LinearEvaluation::GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " constant=" << constant_
      << " linear=" << linear_.transpose();
  return oss;
}

String LinearEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << LinearEvaluation::GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " constant=" << constant_
      << " linear=" << linear_.transpose();
  return oss;
}

/* Accessor for the center */
Point LinearEvaluation::getCenter() const
{
  return center_;
}

/* Accessor for the constant term */
Point LinearEvaluation::getConstant() const
{
  return constant_;
}

/* Accessor for the linear term */
Matrix LinearEvaluation::getLinear() const
{
  return linear_.transpose();
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point LinearEvaluation::operator() (const Point & inP) const
{
  if (inP.getDimension() != center_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const Point result(constant_ + linear_ * (inP - center_));
  callsNumber_.increment();
  return result;
}
/* Operator () */
Sample LinearEvaluation::operator() (const Sample & inS) const
{
  if (inS.getDimension() != center_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const UnsignedInteger size = inS.getSize();
  if (size == 0) return Sample(0, getOutputDimension());
  const Sample centered(inS - center_);
  const Sample result(linear_.getImplementation()->genSampleProd(centered, true, false, 'R') + constant_);
  callsNumber_.fetchAndAdd(size);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger LinearEvaluation::getInputDimension() const
{
  return center_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger LinearEvaluation::getOutputDimension() const
{
  return constant_.getDimension();
}

/* Linearity accessors */
Bool LinearEvaluation::isLinear() const
{
  return true;
}

Bool LinearEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  if (index > getInputDimension())
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << getInputDimension() << ")";

  return true;
}

/* Method save() stores the object through the StorageManager */
void LinearEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "linear_", linear_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "linear_", linear_ );
}

END_NAMESPACE_OPENTURNS
