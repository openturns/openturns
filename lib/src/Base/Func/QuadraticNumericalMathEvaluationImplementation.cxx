//                                               -*- C++ -*-
/**
 * @brief Class for a quadratic numerical math function implementation
 *        of the form y = constant + <linear, x-c> + <<quadratic, x-c>, x-c>
 *        where constant is a dim(y) numerical point, c a dim(x) numerical
 *        point, linear is a dim(x) by dim(y) matrix, quadratic is a
 *        dim(x) by dim(x) by dim(y) symmetric tensor and <linear, x>
 *        means Transpose(linear).x, <quadratic, x> means
 *        Transpose_kj(quadratic).x
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "QuadraticNumericalMathEvaluationImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(QuadraticNumericalMathEvaluationImplementation);

static const Factory<QuadraticNumericalMathEvaluationImplementation> RegisteredFactory;

/* Default constructor */
QuadraticNumericalMathEvaluationImplementation::QuadraticNumericalMathEvaluationImplementation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor f(x) = constant + linear.x + 1/2 x'.quadratic.x */
QuadraticNumericalMathEvaluationImplementation::QuadraticNumericalMathEvaluationImplementation(const NumericalPoint & center,
    const NumericalPoint & constant,
    const Matrix & linear,
    const SymmetricTensor & quadratic)
  : NumericalMathEvaluationImplementation(),
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
QuadraticNumericalMathEvaluationImplementation * QuadraticNumericalMathEvaluationImplementation::clone() const
{
  return new QuadraticNumericalMathEvaluationImplementation(*this);
}

/* Comparison operator */
Bool QuadraticNumericalMathEvaluationImplementation::operator ==(const QuadraticNumericalMathEvaluationImplementation & other) const
{
  return ((quadratic_ == other.getQuadratic()) && (linear_ == other.getLinear()) && (constant_ == other.getConstant()) && (center_ == other.getCenter()));
}

/* String converter */
String QuadraticNumericalMathEvaluationImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticNumericalMathEvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " center=" << center_.__repr__()
      << " constant=" << constant_.__repr__()
      << " linear=" << linear_.transpose()
      << " quadratic=" << quadratic_;
  return oss;
}

String QuadraticNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << QuadraticNumericalMathEvaluationImplementation::GetClassName() << Os::GetEndOfLine()
      << offset << "  center :"  << Os::GetEndOfLine() << center_.__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  constant :" << Os::GetEndOfLine() << constant_.__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  linear :" << Os::GetEndOfLine() << linear_.transpose().__str__(offset + "  ") << Os::GetEndOfLine()
      << offset << "  quadratic :" << Os::GetEndOfLine() << quadratic_.__str__(offset + "  ") << Os::GetEndOfLine();
  return oss;
}

/* Accessor for the center */
NumericalPoint QuadraticNumericalMathEvaluationImplementation::getCenter() const
{
  return center_;
}

/* Accessor for the constant term */
NumericalPoint QuadraticNumericalMathEvaluationImplementation::getConstant() const
{
  return constant_;
}

/* Accessor for the linear term */
Matrix QuadraticNumericalMathEvaluationImplementation::getLinear() const
{
  return linear_.transpose();
}

/* Accessor for the quadratic term */
SymmetricTensor QuadraticNumericalMathEvaluationImplementation::getQuadratic() const
{
  return quadratic_;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint QuadraticNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  if ((inP.getDimension() != linear_.getNbColumns()) || (inP.getDimension() != quadratic_.getNbRows())) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  /* We don't have a true linear algebra with tensors, so we must perform the tensor/vector product by hand */
  const NumericalPoint delta(inP - center_);
  NumericalPoint result(constant_ + linear_ * delta);
  /* As we don't have a sheet extractor yet, we can't use the following code */
  const UnsignedInteger sheetNumber(quadratic_.getNbSheets());
  for(UnsignedInteger index = 0; index < sheetNumber; ++index)
    result[index] += 0.5 * dot(delta, quadratic_.getSheet(index) * delta);
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger QuadraticNumericalMathEvaluationImplementation::getInputDimension() const
{
  return center_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger QuadraticNumericalMathEvaluationImplementation::getOutputDimension() const
{
  return constant_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void QuadraticNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "center_", center_ );
  adv.saveAttribute( "constant_", constant_ );
  adv.saveAttribute( "linear_", linear_ );
  adv.saveAttribute( "quadratic_", quadratic_ );
}

/* Method load() reloads the object from the StorageManager */
void QuadraticNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "center_", center_ );
  adv.loadAttribute( "constant_", constant_ );
  adv.loadAttribute( "linear_", linear_ );
  adv.loadAttribute( "quadratic_", quadratic_ );
}

END_NAMESPACE_OPENTURNS
