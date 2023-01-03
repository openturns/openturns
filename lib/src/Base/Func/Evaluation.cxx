//                                               -*- C++ -*-
/**
 *  @brief The class that implements numerical math functions
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
#include "openturns/Evaluation.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Evaluation)

/* Default constructor */
Evaluation::Evaluation()
  : TypedInterfaceObject<EvaluationImplementation>(new EvaluationImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
Evaluation::Evaluation(const EvaluationImplementation & implementation)
  : TypedInterfaceObject<EvaluationImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Parameter constructor */
Evaluation::Evaluation(const Implementation & p_implementation)
  : TypedInterfaceObject<EvaluationImplementation>(p_implementation)
{
  // Nothing to do
}

/* Parameter constructor */
Evaluation::Evaluation(EvaluationImplementation * p_implementation)
  : TypedInterfaceObject<EvaluationImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool Evaluation::operator ==(const Evaluation & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String Evaluation::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String Evaluation::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Description Accessor */
void Evaluation::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}


/* Description Accessor */
Description Evaluation::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Input description Accessor */
void Evaluation::setInputDescription(const Description & inputDescription)
{
  copyOnWrite();
  getImplementation()->setInputDescription(inputDescription);
}

Description Evaluation::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

/* Output description Accessor */
void Evaluation::setOutputDescription(const Description & outputDescription)
{
  copyOnWrite();
  getImplementation()->setOutputDescription(outputDescription);
}

Description Evaluation::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

/* Operator () */
Sample Evaluation::operator() (const Sample & inSample) const
{
  return getImplementation()->operator()(inSample);
}


/* Operator () */
Field Evaluation::operator() (const Field & inField) const
{
  return getImplementation()->operator()(inField);
}


/* Gradient according to the marginal parameters */
Matrix Evaluation::parameterGradient(const Point & inP) const
{
  return getImplementation()->parameterGradient(inP);
}

/* Parameters value accessor */
Point Evaluation::getParameter() const
{
  return getImplementation()->getParameter();
}

void Evaluation::setParameter(const Point & parameter)
{
  copyOnWrite();
  getImplementation()->setParameter(parameter);
}

void Evaluation::setParameterDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setParameterDescription(description);
}

/* Parameters description accessor */
Description Evaluation::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}


/* Operator () */
Point Evaluation::operator() (const Point & inP) const
{
  return getImplementation()->operator()(inP);
}

/* Accessor for input point dimension */
UnsignedInteger Evaluation::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger Evaluation::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Accessor for input point dimension */
UnsignedInteger Evaluation::getParameterDimension() const
{
  return getImplementation()->getParameterDimension();
}

/* Get the i-th marginal function */
Evaluation Evaluation::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the function corresponding to indices components */
Evaluation Evaluation::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Get the number of calls to operator() */
UnsignedInteger Evaluation::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

/* Linearity accessors */
Bool Evaluation::isLinear() const
{
  return getImplementation()->isLinear();
}

Bool Evaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  return getImplementation()->isLinearlyDependent(index);
}

/* Invalid values check accessor */
void Evaluation::setCheckOutput(const Bool checkOutput)
{
  copyOnWrite();
  getImplementation()->setCheckOutput(checkOutput);
}

Bool Evaluation::getCheckOutput() const
{
  return getImplementation()->getCheckOutput();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph Evaluation::draw(const UnsignedInteger inputMarginal,
                       const UnsignedInteger outputMarginal,
                       const Point & centralPoint,
                       const Scalar xMin,
                       const Scalar xMax,
                       const UnsignedInteger pointNumber,
                       const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
Graph Evaluation::draw(const UnsignedInteger firstInputMarginal,
                       const UnsignedInteger secondInputMarginal,
                       const UnsignedInteger outputMarginal,
                       const Point & centralPoint,
                       const Point & xMin,
                       const Point & xMax,
                       const Indices & pointNumber,
                       const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph Evaluation::draw(const Scalar xMin,
                       const Scalar xMax,
                       const UnsignedInteger pointNumber,
                       const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph Evaluation::draw(const Point & xMin,
                       const Point & xMax,
                       const Indices & pointNumber,
                       const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}


END_NAMESPACE_OPENTURNS
