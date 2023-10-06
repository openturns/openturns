//                                               -*- C++ -*-
/**
 *  @brief Dlib functions interface
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
#include "openturns/DlibFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS

/** Class DlibGradient to ensure interface between OT::Gradient and dlib functions **/

DlibGradient::DlibGradient(const Gradient & gradient, const Bool minimization)
  : Gradient(gradient)
  , minimization_(minimization)
{
  // Nothing to do
}

// Operator () for use with DlibFunction
DlibMatrix DlibGradient::operator() (const DlibMatrix & inP) const
{
  // Check dimension consistency
  UnsignedInteger inputDimension = inP.size();
  UnsignedInteger gradientDimension = getInputDimension();
  if (inputDimension != gradientDimension) throw InvalidArgumentException(HERE) << "Error: gradient input argument dimension (" << inputDimension << ") is inconsistent with expected dimension (" << gradientDimension << ").";

  // Conversion from DlibMatrix to Point
  Point inPoint(inputDimension, 0.0);
  std::copy(inP.begin(), inP.end(), inPoint.begin());

  // Call to OT function
  Matrix gradientAsOTMatrix(gradient(inPoint));

  // Conversion to DlibMatrix
  PersistentCollection<Scalar> gradientAsCollection(*gradientAsOTMatrix.getImplementation());
  DlibMatrix gradientAsDlibMatrix(inputDimension, 1);
  std::copy(gradientAsCollection.begin(), gradientAsCollection.end(), gradientAsDlibMatrix.begin());

  return minimization_ ? gradientAsDlibMatrix : -gradientAsDlibMatrix;
}

// Operator () for use with DlibFunction
DlibMatrix DlibGradient::operator() (const UnsignedInteger i,
                                     const DlibMatrix & inP) const
{
  // Check dimension consistency
  UnsignedInteger inputDimension = inP.size();
  UnsignedInteger gradientInputDimension = getInputDimension();
  //UnsignedInteger gradientOutputDimension = getOutputDimension();
  if (inputDimension != gradientInputDimension) throw InvalidArgumentException(HERE) << "Error: gradient input argument dimension (" << inputDimension << ") is inconsistent with expected dimension (" << gradientInputDimension << ").";

  // Conversion from DlibMatrix to Point
  Point inPoint(inputDimension, 0.0);
  std::copy(inP.begin(), inP.end(), inPoint.begin());

  // Call to OT function
  Matrix gradientAsOTMatrix(gradient(inPoint));

  // Conversion to DlibMatrix
  DlibMatrix gradientAsDlibMatrix(inputDimension, 1);
  //std::copy(&transitionMatrix_(0,currentState_), &transitionMatrix_(0,currentState_)+dimension, weights.begin());
  std::copy(&(gradientAsOTMatrix(0, i)),
            &(gradientAsOTMatrix(0, i)) + gradientInputDimension,
            gradientAsDlibMatrix.begin());

  return minimization_ ? gradientAsDlibMatrix : -gradientAsDlibMatrix;
}

/**  Class DlibHessian to ensure interface between OT::Hessian and dlib functions  **/

DlibHessian::DlibHessian(const Hessian & hessian)
  : Hessian(hessian)
{
  // Nothing to do
}

// Operator ()
DlibMatrix DlibHessian::operator() (const DlibMatrix & inP) const
{
  // Check dimension consistency
  UnsignedInteger inputDimension = inP.size();
  UnsignedInteger hessianDimension = getInputDimension();
  if (inputDimension != hessianDimension) throw InvalidArgumentException(HERE) << "Error: hessian input argument dimension (" << inputDimension << ") is inconsistent with expected dimension (" << hessianDimension << ").";

  // Conversion from DlibMatrix to Point
  Point inPoint(inputDimension, 0.0);
  std::copy(inP.begin(), inP.end(), inPoint.begin());

  // Call to OT function
  SymmetricTensor hessianAsOTTensor(hessian(inPoint));

  // Conversion to DlibMatrix
  PersistentCollection<Scalar> hessianAsCollection(*hessianAsOTTensor.getImplementation());
  DlibMatrix hessianAsDlibMatrix(inputDimension, inputDimension);
  std::copy(hessianAsCollection.begin(), hessianAsCollection.end(), hessianAsDlibMatrix.begin());

  return hessianAsDlibMatrix;
}


/** Class DlibFunction to ensure interface between OT::Function and dlib functions **/

CLASSNAMEINIT(DlibFunction)

DlibFunction::DlibFunction(const Function & function, const Bool minimization)
  : Function(function)
  , inputHistory_(0, getInputDimension())
  , outputHistory_(0, getOutputDimension())
  , minimization_(minimization)
{
  // Nothing to do
}

// Operator ()
double DlibFunction::operator() (const DlibMatrix & inP) const
{
  // Check dimension consistency
  UnsignedInteger inputDimension = inP.size();
  UnsignedInteger functionDimension = getInputDimension();
  if (inputDimension != functionDimension) throw InvalidArgumentException(HERE) << "Error: input argument dimension (" << inputDimension << ") is inconsistent with expected dimension (" << functionDimension << ").";

  // Conversion from DlibMatrix to Point
  Point inPoint(inputDimension, 0.0);
  std::copy(inP.begin(), inP.end(), inPoint.begin());

  // Call to OT function
  Point outPoint;
  outPoint = getImplementation()->operator()(inPoint);

  // Update evaluationNumber_ and input/output history
  inputHistory_.add(inPoint);
  outputHistory_.add(outPoint);

  // Return scalar value
  return minimization_ ? outPoint[0] : -outPoint[0];
}

double DlibFunction::operator() (const UnsignedInteger i,
                                 const DlibMatrix & inP) const
{
  // Check dimension consistency
  UnsignedInteger inputDimension = inP.size();
  UnsignedInteger functionDimension = getInputDimension();
  if (inputDimension != functionDimension) throw InvalidArgumentException(HERE) << "Error: input argument dimension (" << inputDimension << ") is inconsistent with expected dimension (" << functionDimension << ")";
  if (!(i < getOutputDimension()))
    throw InvalidArgumentException(HERE) << "Error: residual component index (" << i << ") exceeds residual dimension (" << getOutputDimension() << ")";

  // Conversion from DlibMatrix to Point
  Point inPoint(inputDimension, 0.0);
  std::copy(inP.begin(), inP.end(), inPoint.begin());

  // Call to OT function
  Point outPoint;
  if (inputHistory_.contains(inPoint))
    outPoint = outputHistory_[inputHistory_.find(inPoint)];
  else
  {
    outPoint = getImplementation()->operator()(inPoint);
    // Update evaluationNumber_ and input/output history
    inputHistory_.add(inPoint);
    outputHistory_.add(outPoint);
  }

  // Return scalar value
  return minimization_ ? outPoint[i] : -outPoint[i];
}

/* Accessors to gradient */
DlibGradient DlibFunction::asDlibGradient() const
{
  return DlibGradient(getImplementation()->getGradient(), minimization_);
}

// Compute at point
DlibMatrix DlibFunction::gradient(const DlibMatrix & inP) const
{
  return asDlibGradient()(inP);
}

// Compute at point
DlibMatrix DlibFunction::gradient(const UnsignedInteger i,
                                  const DlibMatrix & inP) const
{
  return asDlibGradient()(i, inP);
}

/* Accessor to hessian */
DlibHessian DlibFunction::asDlibHessian() const
{
  return DlibHessian(getImplementation()->getHessian());
}

// Compute at point
DlibMatrix DlibFunction::hessian(const DlibMatrix & inP) const
{
  return asDlibHessian()(inP);
}

void DlibFunction::get_derivative_and_hessian(const column_vector x,
    column_vector & grad,
    general_matrix & hess) const
{
  grad = gradient(x);
  hess = hessian(x);
}

/* Accessors to attributes */
UnsignedInteger DlibFunction::getEvaluationNumber() const
{
  return inputHistory_.getSize();
}

Sample DlibFunction::getInputHistory()
{
  return inputHistory_;
}

Sample DlibFunction::getOutputHistory()
{
  return outputHistory_;
}

END_NAMESPACE_OPENTURNS
