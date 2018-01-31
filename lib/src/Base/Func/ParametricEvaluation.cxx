//                                               -*- C++ -*-
/**
 *  @brief ParametricEvaluation
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ParametricEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricEvaluation)

static const Factory<ParametricEvaluation> Factory_ParametricEvaluation;


/* Default constructor */
ParametricEvaluation::ParametricEvaluation()
  : EvaluationImplementation()
  , function_()
  , parametersPositions_(0)
  , inputPositions_(0)
{
  // Nothing to do
}

/* Parameter constructor */
ParametricEvaluation::ParametricEvaluation(const Function & function,
    const Indices & set,
    const Point & referencePoint,
    const Bool parametersSet)
  : EvaluationImplementation()
  , function_(function)
  , parametersPositions_(0)
  , inputPositions_(0)
{
  const UnsignedInteger inputDimension = function.getInputDimension();
  const UnsignedInteger setDimension = set.getSize();
  // Check if the given parameters positions are compatible with the input dimension of the function
  if (inputDimension < setDimension) throw InvalidArgumentException(HERE) << "Error: the size of the " << (parametersSet ? "parameters" : "input") << " positions=" << setDimension << " is greater than the input dimension=" << inputDimension << " of the function.";
  // Check if the given indices are valid
  if (!set.check(inputDimension)) throw InvalidArgumentException(HERE) << "Error: the given set of positions contain either duplicate positions or positions greater than the input dimension of the function.";
  // Deduce the input position from the input dimension of the function and the parameters positions
  // Warning! the parameters positions can be in any order
  Indices fullIndices(inputDimension);
  Indices otherSet(0);
  fullIndices.fill();
  // Flag by inputDimension all the positions already used by the parameters
  for (UnsignedInteger i = 0; i < setDimension; ++i) fullIndices[set[i]] = inputDimension;
  // Then gather all the available indices
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    if (fullIndices[i] != inputDimension) otherSet.add(i);
  if (parametersSet)
  {
    parametersPositions_ = set;
    inputPositions_ = otherSet;
  }
  else
  {
    parametersPositions_ = otherSet;
    inputPositions_ = set;
  }
  const UnsignedInteger parametersSize = parametersPositions_.getSize();
  // Check if the given reference point has a dimension compatible with the function
  if (referencePoint.getDimension() != parametersSize) throw InvalidArgumentException(HERE) << "Error: the given reference point dimension=" << referencePoint.getDimension() << " does not match the parameters size=" << parametersSize;
  // Set the relevant part of the reference point in the parameters
  const Description functionInputDescription(function.getInputDescription());
  Description parameterDescription(parametersSize);
  for (UnsignedInteger i = 0; i < parametersSize; ++ i)
  {
    parameterDescription[i] = functionInputDescription[parametersPositions_[i]];
  }
  parameter_ = referencePoint;
  parameterDescription_ = parameterDescription;
  // And finally the input/output descriptions
  Description inputDescription(0);
  for (UnsignedInteger i = 0; i < inputPositions_.getSize(); ++i) inputDescription.add(functionInputDescription[inputPositions_[i]]);
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Parameter constructor */
ParametricEvaluation::ParametricEvaluation(const ParametricEvaluation & evaluation,
    const Indices & set,
    const Point & referencePoint)
  : EvaluationImplementation()
  , function_()
  , parametersPositions_(0)
  , inputPositions_(0)
{
  // Here we reuse the antecedent parameters
  // The function is kept unchanged
  function_ = evaluation.function_;
  // The parameters position will be enlarged
  parametersPositions_ = evaluation.parametersPositions_;
  // The parameters values too
  parameter_ = evaluation.getParameter();
  // And their description
  Description parametersDescription(parameter_.getDescription());
  const Description inputDescription(evaluation.getInputDescription());
  // The input positions will be reduced
  Indices antecedentInputPosition(evaluation.inputPositions_);
  const UnsignedInteger inputDimension = evaluation.getInputDimension();
  const UnsignedInteger size = set.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger index = set[i];
    // Mark the given index as a parameter index
    parametersPositions_.add(antecedentInputPosition[index]);
    // And flag it to be removed from the input indices
    antecedentInputPosition[index] = inputDimension;
    // Add the parameter value to the parameters
    parameter_.add(referencePoint[i]);
    // Add the description to the parameters description
    parametersDescription.add(inputDescription[antecedentInputPosition[index]]);
  }
  parameter_.setDescription(parametersDescription);
  // And finally the input/output descriptions
  Description newInputDescription(0);
  for (UnsignedInteger i = 0; i < inputPositions_.getSize(); ++i) newInputDescription.add(inputDescription[inputPositions_[i]]);
  setInputDescription(inputDescription);
  setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor method */
ParametricEvaluation * ParametricEvaluation::clone() const
{
  return new ParametricEvaluation(*this);
}

/* Evaluation operator */
Point ParametricEvaluation::operator() (const Point & point) const
{
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger pointDimension = point.getDimension();
  const UnsignedInteger parametersDimension = getParameterDimension();
  if (pointDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension - parametersDimension << ", got dimension=" << pointDimension;
  Point x(inputDimension);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) x[parametersPositions_[i]] = parameter_[i];
  for (UnsignedInteger i = 0; i < pointDimension; ++i) x[inputPositions_[i]] = point[i];
  const Point value(function_(x));
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(x);
    outputStrategy_.store(value);
  }
  ++callsNumber_;
  return value;
}

/* Operator () */
Sample ParametricEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger inputDimension = function_.getInputDimension();
  const UnsignedInteger sampleDimension = inSample.getDimension();
  const UnsignedInteger parametersDimension = getParameterDimension();
  if (sampleDimension + parametersDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << inputDimension - parametersDimension << ", got dimension=" << sampleDimension;
  Sample input(size, inputDimension);
  SampleImplementation & inputImpl(*input.getImplementation());
  const SampleImplementation & inSampleImpl(*inSample.getImplementation());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < parametersDimension; ++j) inputImpl(i, parametersPositions_[j]) = parameter_[j];
    for (UnsignedInteger j = 0; j < sampleDimension; ++j) inputImpl(i, inputPositions_[j]) = inSampleImpl(i, j);
  }
  Sample output(function_(input));
  output.setDescription(getOutputDescription());
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(input);
    outputStrategy_.store(output);
  }
  callsNumber_ += size;
  return output;
}

/* Parameters accessor */
void ParametricEvaluation::setParameter(const Point & parameters)
{
  const UnsignedInteger parametersDimension = parameters.getDimension();
  if (parametersDimension != parametersPositions_.getSize()) throw InvalidArgumentException(HERE) << "Error: expected a parameters of dimension=" << parametersPositions_.getSize() << ", got dimension=" << parametersDimension;
  for (UnsignedInteger i = 0; i < parametersDimension; ++i) parameter_[i] = parameters[i];
}

/* Parameters positions accessor */
Indices ParametricEvaluation::getParametersPositions() const
{
  return parametersPositions_;
}

/* Input positions accessor */
Indices ParametricEvaluation::getInputPositions() const
{
  return inputPositions_;
}

/* Function accessor */
Function ParametricEvaluation::getFunction() const
{
  return function_;
}

/* Dimension accessor */
UnsignedInteger ParametricEvaluation::getInputDimension() const
{
  return inputPositions_.getSize();
}

UnsignedInteger ParametricEvaluation::getParameterDimension() const
{
  return parametersPositions_.getSize();
}

UnsignedInteger ParametricEvaluation::getOutputDimension() const
{
  return function_.getOutputDimension();
}

/* Input point / parameter history accessor */
Sample ParametricEvaluation::getInputPointHistory() const
{
  Sample sample(inputStrategy_.getSample());
  return sample.getSize() > 0 ? sample.getMarginal(inputPositions_) : Sample(0, getInputDimension());
}

Sample ParametricEvaluation::getInputParameterHistory() const
{
  Sample sample(inputStrategy_.getSample());
  return sample.getSize() > 0 ? sample.getMarginal(parametersPositions_) : Sample(0, getParameterDimension());
}

/* String converter */
String ParametricEvaluation::__repr__() const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss;
  oss << "class=" << ParametricEvaluation::GetClassName()
      << " function=" << function_
      << " parameters positions=" << parametersPositions_
      << " parameters=" << parameters
      << " input positions=" << inputPositions_;
  return oss;
}

String ParametricEvaluation::__str__(const String & offset) const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(false);
  oss << offset << ParametricEvaluation::GetClassName()
      << "(" << function_
      << ", parameters positions=" << parametersPositions_
      << ", parameters=" << parameters
      << ", input positions=" << inputPositions_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ParametricEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save( adv );
  adv.saveAttribute( "function_", function_ );
  adv.saveAttribute( "parametersPositions_", parametersPositions_ );
  adv.saveAttribute( "inputPositions_", inputPositions_ );
}

/* Method load() reloads the object from the StorageManager */
void ParametricEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load( adv );
  adv.loadAttribute( "function_", function_ );
  adv.loadAttribute( "parametersPositions_", parametersPositions_ );
  adv.loadAttribute( "inputPositions_", inputPositions_ );
}


END_NAMESPACE_OPENTURNS
