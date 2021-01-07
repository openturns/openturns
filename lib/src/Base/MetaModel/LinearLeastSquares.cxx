//                                               -*- C++ -*-
/**
 *  @brief First order polynomial response surface by least square
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

#include "openturns/LinearLeastSquares.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/ConstantHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(LinearLeastSquares)

static const Factory<LinearLeastSquares> Factory_LinearLeastSquares;


/* Default constructor */
LinearLeastSquares::LinearLeastSquares()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
LinearLeastSquares::LinearLeastSquares(const Sample & dataIn,
                                       const Function & inputFunction)
  : PersistentObject(),
    dataIn_(dataIn),
    dataOut_(0, inputFunction.getOutputDimension()),
    inputFunction_(inputFunction),
    constant_(inputFunction.getOutputDimension()),
    linear_(inputFunction.getInputDimension(), inputFunction.getOutputDimension())
{
  if (!inputFunction.getEvaluation().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Error: the given function must have an actual implementation";
  if (inputFunction.getInputDimension() != dataIn.getDimension()) throw InvalidArgumentException(HERE) << "Error: the input data dimension and the input dimension of the function must be the same, here input dimension=" << dataIn.getDimension() << " and input dimension of the function=" << inputFunction.getInputDimension();
}

/* Constructor with parameters */
LinearLeastSquares::LinearLeastSquares(const Sample & dataIn,
                                       const Sample & dataOut)
  : PersistentObject(),
    dataIn_(dataIn),
    dataOut_(0, dataOut.getDimension()),
    inputFunction_(),
    constant_(dataOut.getDimension()),
    linear_(dataIn.getDimension(), dataOut.getDimension())
{
  setDataOut(dataOut);
}

/* Virtual constructor */
LinearLeastSquares * LinearLeastSquares::clone() const
{
  return new LinearLeastSquares(*this);
}

/* String converter */
String LinearLeastSquares::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " dataIn=" << dataIn_
      << " dataOut=" << dataOut_
      << " function=" << inputFunction_
      << " responseSurface=" << responseSurface_
      << " constant=" << constant_
      << " linear=" << linear_;
  return oss;
}

/* Response surface computation */
void LinearLeastSquares::run()
{
  if (dataOut_.getSize() == 0)
  {
    /* Compute the given function over the given sample */
    dataOut_ = inputFunction_(dataIn_);
  }
  const UnsignedInteger inputDimension = dataIn_.getDimension();
  const UnsignedInteger outputDimension = dataOut_.getDimension();
  const UnsignedInteger dataInSize = dataIn_.getSize();
  const UnsignedInteger coefficientsDimension = 1 + inputDimension;
  /* Matrix of the least-square problem */
  Matrix componentMatrix(dataInSize, coefficientsDimension);
  /* Matrix for the several right-hand sides */
  Matrix rightHandSides(dataInSize, outputDimension);
  /* For each sample of the input data... */
  for(UnsignedInteger sampleIndex = 0; sampleIndex < dataInSize; ++sampleIndex)
  {
    /* build the componentMatrix */
    /* get the current sample x */
    const Point currentSample(dataIn_[sampleIndex]);
    UnsignedInteger rowIndex = 0;
    /* First the constant term */
    componentMatrix(sampleIndex, rowIndex) = 1.0;
    ++rowIndex;
    /* Then the linear term x' */
    for(UnsignedInteger componentIndex = 0; componentIndex < inputDimension; ++componentIndex)
    {
      componentMatrix(sampleIndex, rowIndex) = currentSample[componentIndex];
      ++rowIndex;
    } // linear term
    /* build the right-hand side */
    for(UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
    {
      rightHandSides(sampleIndex, outputIndex) = dataOut_(sampleIndex, outputIndex);
    }
  } // each sample
  // Now, solve simultaneously the least-squares solutions for all the outputs
  const Matrix coefficients(componentMatrix.solveLinearSystem(rightHandSides));
  // Fill-in the elements of the meta-model
  for(UnsignedInteger outputComponent = 0; outputComponent < outputDimension; ++outputComponent)
  {
    /* First, the constant term */
    UnsignedInteger coefficientsIndex = 0;
    constant_[outputComponent] = coefficients(coefficientsIndex, outputComponent);
    ++coefficientsIndex;
    /* Second, the linear term */
    for(UnsignedInteger componentIndex = 0; componentIndex < inputDimension; ++componentIndex)
    {
      linear_(componentIndex, outputComponent) = coefficients(coefficientsIndex, outputComponent);
      ++coefficientsIndex;
    } // linear term
  } // output components
  const Point center(inputDimension, 0.0);
  /* Build the several implementations and set them into the response surface */
  responseSurface_.setEvaluation(new LinearEvaluation(center, constant_, linear_));
  responseSurface_.setGradient(new ConstantGradient(linear_));
  responseSurface_.setHessian(new ConstantHessian(SymmetricTensor(center.getDimension(), constant_.getDimension())));
}

/* DataIn accessor */
Sample LinearLeastSquares::getDataIn() const
{
  return dataIn_;
}

/* DataOut accessor */
Sample LinearLeastSquares::getDataOut() const
{
  // If the response surface has been defined with an input function and the output data have not already been computed, compute them
  if (inputFunction_.getEvaluation().getImplementation()->isActualImplementation() && (dataOut_.getSize() == 0)) dataOut_ = inputFunction_(dataIn_);
  return dataOut_;
}

void LinearLeastSquares::setDataOut(const Sample & dataOut)
{
  if (inputFunction_.getEvaluation().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Error: cannot set the output data in a response surface defined with a function, here function=" << inputFunction_;
  if (dataOut.getSize() != dataIn_.getSize()) throw InvalidArgumentException(HERE) << "Error: the output data must have the same size than the input data, here output size=" << dataOut.getSize() << " and input size=" << dataIn_.getSize();
  dataOut_ = dataOut;
}

/* Constant accessor */
Point LinearLeastSquares::getConstant() const
{
  return constant_;
}

/* Linear accessor */
Matrix LinearLeastSquares::getLinear() const
{
  return linear_;
}

/* Function accessor */
Function LinearLeastSquares::getInputFunction() const
{
  return inputFunction_;
}

/* Metamodel accessor */
Function LinearLeastSquares::getMetaModel() const
{
  return responseSurface_;
}

void LinearLeastSquares::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dataIn_", dataIn_);
  adv.saveAttribute("dataOut_", dataOut_);
  adv.saveAttribute("inputFunction_", inputFunction_);
  adv.saveAttribute("responseSurface_", responseSurface_);
  adv.saveAttribute("constant_", constant_);
  adv.saveAttribute("linear_", linear_);
}

/* Method load() reloads the object from the StorageManager */
void LinearLeastSquares::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dataIn_", dataIn_);
  adv.loadAttribute("dataOut_", dataOut_);
  adv.loadAttribute("inputFunction_", inputFunction_);
  adv.loadAttribute("responseSurface_", responseSurface_);
  adv.loadAttribute("constant_", constant_);
  adv.loadAttribute("linear_", linear_);
}

END_NAMESPACE_OPENTURNS
