//                                               -*- C++ -*-
/**
 *  @brief Second order polynomial response surface by least square
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
#include "openturns/QuadraticLeastSquares.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(QuadraticLeastSquares)

static const Factory<QuadraticLeastSquares> Factory_QuadraticLeastSquares;

/* Default constructor */
QuadraticLeastSquares::QuadraticLeastSquares()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
QuadraticLeastSquares::QuadraticLeastSquares(const Sample & dataIn,
    const Function & inputFunction)
  : PersistentObject(),
    dataIn_(dataIn),
    dataOut_(Sample(0, inputFunction.getOutputDimension())),
    inputFunction_(inputFunction),
    constant_(Point(inputFunction_.getOutputDimension())),
    linear_(Matrix(inputFunction_.getInputDimension(), inputFunction_.getOutputDimension())),
    quadratic_(SymmetricTensor(inputFunction_.getInputDimension(), inputFunction_.getOutputDimension()))
{
  if (!inputFunction_.getEvaluation().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Error: the given function must have an actual implementation";
  if (inputFunction.getInputDimension() != dataIn.getDimension()) throw InvalidArgumentException(HERE) << "Error: the input data dimension and the input dimension of the function must be the same, here input dimension=" << dataIn.getDimension() << " and input dimension of the function=" << inputFunction.getInputDimension();
}

/* Constructor with parameters */
QuadraticLeastSquares::QuadraticLeastSquares(const Sample & dataIn,
    const Sample & dataOut)
  : PersistentObject(),
    dataIn_(dataIn),
    dataOut_(Sample(0, dataOut.getDimension())),
    inputFunction_(Function()),
    constant_(Point(dataOut.getDimension())),
    linear_(Matrix(dataIn.getDimension(), dataOut.getDimension())),
    quadratic_(SymmetricTensor(dataIn.getDimension(), dataOut.getDimension()))
{
  setDataOut(dataOut);
}

/* Virtual constructor */
QuadraticLeastSquares * QuadraticLeastSquares::clone() const
{
  return new QuadraticLeastSquares(*this);
}

/* String converter */
String QuadraticLeastSquares::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " dataIn=" << dataIn_
      << " dataOut=" << dataOut_
      << " function=" << inputFunction_
      << " responseSurface=" << responseSurface_
      << " constant=" << constant_
      << " linear=" << linear_
      << " quadratic=" << quadratic_;
  return oss;
}

/* Response surface computation */
void QuadraticLeastSquares::run()
{
  if (dataOut_.getSize() == 0)
  {
    /* Compute the given function over the given sample */
    dataOut_ = inputFunction_(dataIn_);
  }
  const UnsignedInteger inputDimension = dataIn_.getDimension();
  const UnsignedInteger outputDimension = dataOut_.getDimension();
  const UnsignedInteger dataInSize = dataIn_.getSize();
  const UnsignedInteger coefficientsDimension = 1 + inputDimension + (inputDimension * (inputDimension + 1)) / 2;
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
    /* Finally the quadratic term x.x' */
    for(UnsignedInteger componentIndex = 0; componentIndex < inputDimension; ++componentIndex)
    {
      /* First, the diagonal term */
      componentMatrix(sampleIndex, rowIndex) = 0.5 * currentSample[componentIndex] * currentSample[componentIndex];
      ++rowIndex;
      /* Then, the off-diagonal terms */
      for(UnsignedInteger componentIndexTranspose = componentIndex + 1; componentIndexTranspose < inputDimension; ++componentIndexTranspose)
      {
        componentMatrix(sampleIndex, rowIndex) = currentSample[componentIndex] * currentSample[componentIndexTranspose];
        ++rowIndex;
      } // off-diagonal terms
    } // quadratic term
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
    /* Third, the quadratic term */
    for(UnsignedInteger componentIndex = 0; componentIndex < inputDimension; ++componentIndex)
    {
      quadratic_(componentIndex, componentIndex, outputComponent) = coefficients(coefficientsIndex, outputComponent);
      ++coefficientsIndex;
      for(UnsignedInteger componentIndexTranspose = componentIndex + 1; componentIndexTranspose < inputDimension; ++componentIndexTranspose)
      {
        quadratic_(componentIndex, componentIndexTranspose, outputComponent) = coefficients(coefficientsIndex, outputComponent); // We only store the upper part
        ++coefficientsIndex;
      } // Off-diagonal terms
    } // quadratic term
  } // output components
  const Point center(inputDimension, 0.0);
  responseSurface_ = QuadraticFunction(center, constant_, linear_, quadratic_);
}

/* DataIn accessor */
Sample QuadraticLeastSquares::getDataIn() const
{
  return dataIn_;
}

/* DataOut accessor */
Sample QuadraticLeastSquares::getDataOut()
{
  // If the response surface has been defined with an input function and the output data have not already been computed, compute them
  if (inputFunction_.getEvaluation().getImplementation()->isActualImplementation() && (dataOut_.getSize() == 0)) dataOut_ = inputFunction_(dataIn_);
  return dataOut_;
}

void QuadraticLeastSquares::setDataOut(const Sample & dataOut)
{
  if (inputFunction_.getEvaluation().getImplementation()->isActualImplementation()) throw InvalidArgumentException(HERE) << "Error: cannot set the output data in a response surface defined with a function, here function=" << inputFunction_;
  if (dataOut.getSize() != dataIn_.getSize()) throw InvalidArgumentException(HERE) << "Error: the output data must have the same size than the input data, here output size=" << dataOut.getSize() << " and input size=" << dataIn_.getSize();
  dataOut_ = dataOut;
}

/* Constant accessor */
Point QuadraticLeastSquares::getConstant() const
{
  return constant_;
}

/* Linear accessor */
Matrix QuadraticLeastSquares::getLinear() const
{
  return linear_;
}

/* Quadratic accessor */
SymmetricTensor QuadraticLeastSquares::getQuadratic() const
{
  return quadratic_;
}

/* Function accessor */
Function QuadraticLeastSquares::getInputFunction() const
{
  return inputFunction_;
}

/* Metamodel accessor */
Function QuadraticLeastSquares::getMetaModel() const
{
  return responseSurface_;
}

void QuadraticLeastSquares::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dataIn_", dataIn_);
  adv.saveAttribute("dataOut_", dataOut_);
  adv.saveAttribute("inputFunction_", inputFunction_);
  adv.saveAttribute("responseSurface_", responseSurface_);
  adv.saveAttribute("constant_", constant_);
  adv.saveAttribute("linear_", linear_);
  adv.saveAttribute("quadratic_", quadratic_);
}

/* Method load() reloads the object from the StorageManager */
void QuadraticLeastSquares::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dataIn_", dataIn_);
  adv.loadAttribute("dataOut_", dataOut_);
  adv.loadAttribute("inputFunction_", inputFunction_);
  adv.loadAttribute("responseSurface_", responseSurface_);
  adv.loadAttribute("constant_", constant_);
  adv.loadAttribute("linear_", linear_);
  adv.loadAttribute("quadratic_", quadratic_);
}

END_NAMESPACE_OPENTURNS
