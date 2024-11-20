//                                               -*- C++ -*-
/**
 * @brief Abstract top-level class for all evaluation implementations
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <algorithm>
#include <iterator>

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Box.hxx"
#include "openturns/MarginalEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PointWithDescription.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EvaluationImplementation)

static const Factory<EvaluationImplementation> Factory_EvaluationImplementation;


/* Default constructor */
EvaluationImplementation::EvaluationImplementation()
  : PersistentObject()
  , callsNumber_(0)
  , parameter_(0)
  , inputDescription_(0)
  , outputDescription_(0)
{
  // Nothing to do
}

/* Virtual constructor */
EvaluationImplementation * EvaluationImplementation::clone() const
{
  return new EvaluationImplementation(*this);
}


/* Comparison operator */
Bool EvaluationImplementation::operator ==(const EvaluationImplementation & ) const
{
  return true;
}

/* String converter */
String EvaluationImplementation::__repr__() const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(true);
  oss << "class=" << EvaluationImplementation::GetClassName()
      << " name=" << getName()
      << " input description=" << inputDescription_
      << " output description=" << outputDescription_
      << " parameters=" <<  parameters;
  return oss;
}

/* String converter */
String EvaluationImplementation::__str__(const String & ) const
{
  return OSS(false) << "EvaluationImplementation";
}

/* String converter */
String EvaluationImplementation::_repr_html_() const
{
  PointWithDescription parameters(parameter_);
  parameters.setDescription(parameterDescription_);

  OSS oss(true);
  oss << "<ul>\n";
  oss << "  <li> " << "Input dimension = " << getInputDimension() << "  </li>\n";
  oss << "  <li> " << "Input description = " << getInputDescription() << "  </li>\n";
  oss << "  <li> " << "Output dimension = " << getOutputDimension() << "  </li>\n";
  oss << "  <li> " << "Output description = " << getOutputDescription() << "  </li>\n";
  oss << "  <li> " << "Parameter = " << parameters.__str__() << "  </li>\n";
  oss << "</ul>\n";
  return oss;
}

/* Description Accessor */
void EvaluationImplementation::setDescription(const Description & description)
{
  if (description.getSize() != getInputDimension() + getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the description must have a size of input dimension + output dimension, here size=" << description.getSize() << ", input dimension=" << getInputDimension() << ", output dimension=" << getOutputDimension();
  inputDescription_ = Description(getInputDimension());
  std::copy(description.begin(), description.begin() + getInputDimension(), inputDescription_.begin());
  outputDescription_ = Description(getOutputDimension());
  std::copy(description.begin() + getInputDimension(), description.begin() + description.getSize(), outputDescription_.begin());
}


/* Description Accessor */
Description EvaluationImplementation::getDescription() const
{
  Description description(getInputDescription());
  Description outputDescription(getOutputDescription());
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i) description.add(outputDescription[i]);
  return description;
}

/* Input description Accessor */
void EvaluationImplementation::setInputDescription(const Description & inputDescription)
{
  if (inputDescription.getSize() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the input description must have a size=" << inputDescription.getSize() << " equal to the input dimension=" << getInputDimension();
  inputDescription_ = inputDescription;
}

Description EvaluationImplementation::getInputDescription() const
{
  if (inputDescription_.getSize() == 0) return Description::BuildDefault(getInputDimension(), "x");
  return inputDescription_;
}

/* Output description Accessor */
void EvaluationImplementation::setOutputDescription(const Description & outputDescription)
{
  if (outputDescription.getSize() != getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the output description must have a size=" << outputDescription.getSize() << " equal to the output dimension=" << getOutputDimension();
  outputDescription_ = outputDescription;
}

Description EvaluationImplementation::getOutputDescription() const
{
  if (outputDescription_.getSize() == 0) return Description::BuildDefault(getOutputDimension(), "y");
  return outputDescription_;
}

/* Test for actual implementation */
Bool EvaluationImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Sample EvaluationImplementation::operator() (const Sample & inSample) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inSample.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inSample.getDimension();

  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, getOutputDimension());
  // Simple loop over the evaluation operator based on point
  // The calls number is updated by these calls
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    outSample[i] = operator()(inSample[i]);
    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InterruptionException(HERE) << "User stopped evaluation";
  }
  outSample.setDescription(getOutputDescription());
  return outSample;
}


/* Operator () */
Field EvaluationImplementation::operator() (const Field & inField) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inField.getOutputDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given time series has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inField.getOutputDimension();
  return Field(inField.getMesh(), operator()(inField.getValues()));
}


/* Gradient according to the marginal parameters */
Matrix EvaluationImplementation::parameterGradient(const Point & inP) const
{
  Point parameter(getParameter());
  const UnsignedInteger parameterDimension = parameter.getDimension();
  const UnsignedInteger outputDimension = getOutputDimension();

  const Scalar epsilon = ResourceMap::GetAsScalar("Evaluation-ParameterEpsilon");

  Sample inS(parameterDimension + 1, parameter);
  for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
  {
    inS(1 + i, i) += epsilon;
  }
  Sample outS(parameterDimension + 1, getOutputDimension());
  Pointer<EvaluationImplementation> p_evaluation(clone());
  for (UnsignedInteger i = 0; i < parameterDimension + 1; ++ i)
  {
    p_evaluation->setParameter(inS[i]);
    outS[i] = p_evaluation->operator()(inP);
  }

  Matrix grad(parameterDimension, outputDimension);
  for (UnsignedInteger i = 0; i < parameterDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++ j)
    {
      grad(i, j) = (outS(1 + i, j) - outS(0, j)) / epsilon;
    }
  }
  return grad;
}

/* Parameters value accessor */
Point EvaluationImplementation::getParameter() const
{
  return parameter_;
}

void EvaluationImplementation::setParameter(const Point & parameter)
{
  parameter_ = parameter;
}

void EvaluationImplementation::setParameterDescription(const Description & description)
{
  parameterDescription_ = description;
}

/* Parameters description accessor */
Description EvaluationImplementation::getParameterDescription() const
{
  return parameterDescription_;
}


/* Operator () */
Point EvaluationImplementation::operator() (const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::operator() (const Point & inP) const";
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger EvaluationImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In EvaluationImplementation::getOutputDimension() const";
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationImplementation::getParameterDimension() const
{
  return getParameter().getDimension();
}

/* Get the i-th marginal function */
Evaluation EvaluationImplementation::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Evaluation EvaluationImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal evaluation must be in the range [0, outputDimension-1] and must be different";
  Indices full(getOutputDimension());
  full.fill();
  if (indices == full) return clone();
  return new MarginalEvaluation(clone(), indices);
}

/* Get the number of calls to operator() */
UnsignedInteger EvaluationImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Linearity accessors */
Bool EvaluationImplementation::isLinear() const
{
  return false;
}

Bool EvaluationImplementation::isLinearlyDependent(const UnsignedInteger index) const
{
  // Check dimension consistency
  if (!(index <= getInputDimension()))
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << getInputDimension() << ")";

  return false;
}

/* Is it safe to call in parallel? */
Bool EvaluationImplementation::isParallel() const
{
  return true;
}

/* Invalid values check accessor */
void EvaluationImplementation::setCheckOutput(const Bool checkOutput)
{
  checkOutput_ = checkOutput;
}

Bool EvaluationImplementation::getCheckOutput() const
{
  return checkOutput_;
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph EvaluationImplementation::draw(const UnsignedInteger inputMarginal,
                                     const UnsignedInteger outputMarginal,
                                     const Point & centralPoint,
                                     const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (!(getInputDimension() >= 1)) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 1, here inputDimension=" << getInputDimension();
  if (!(inputMarginal < getInputDimension())) throw InvalidArgumentException(HERE) << "Error: the given input marginal index=" << inputMarginal << " must be less than the input dimension=" << getInputDimension();
  if (!(outputMarginal < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the given output marginal index=" << outputMarginal << " must be less than the output dimension=" << getOutputDimension();
  if (!(xMin <= xMax)) throw InvalidArgumentException(HERE) << "Error: xMin (" << xMin << ") cannot be greater than xMax(" << xMax << ")";
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points";
  const Bool useLogX = (scale == GraphImplementation::LOGX || scale == GraphImplementation::LOGXY);
  if (useLogX && (!(xMin > 0.0 && xMax > 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values.";
  if (centralPoint.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a central point of dimension=" << getInputDimension() << ", got dimension=" << centralPoint.getDimension();

  Sample inputData;
  if (xMin < xMax)
  {
    inputData = Sample(pointNumber, centralPoint);
    if (useLogX)
    {
      const Scalar a = std::log(xMin);
      const Scalar b = std::log(xMax);
      const Scalar dLogX = (b - a) / (pointNumber - 1.0);
      for (UnsignedInteger i = 0; i < pointNumber; ++i)
        inputData(i, inputMarginal) = std::exp(a + i * dLogX);
    }
    else
    {
      const Scalar dx = (xMax - xMin) / (pointNumber - 1.0);
      for (UnsignedInteger i = 0; i < pointNumber; ++i)
        inputData(i, inputMarginal) = xMin + i * dx;
    }
  }
  else
  {
    // single point
    inputData = Sample(1, centralPoint);
    inputData(0, inputMarginal) = xMin;
  }

  // Evaluate the function over all its input in one call in order to benefit from potential parallelism
  const Sample outputData((*this)(inputData));
  const Description inputDescription(getInputDescription());
  const Description outputDescription(getOutputDescription());
  const String xName(inputDescription[inputMarginal]);
  const String yName(outputDescription[outputMarginal]);
  String title(OSS() << yName << " as a function of " << xName);
  if (centralPoint.getDimension() > 1) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true);
  graph.setLogScale(scale);
  if (xMin < xMax)
    graph.add(Curve(inputData.getMarginal(inputMarginal), outputData.getMarginal(outputMarginal)));
  else
    graph.add(Cloud(inputData.getMarginal(inputMarginal), outputData.getMarginal(outputMarginal)));
  return graph;
}

/* Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
Graph EvaluationImplementation::draw(const UnsignedInteger firstInputMarginal,
                                     const UnsignedInteger secondInputMarginal,
                                     const UnsignedInteger outputMarginal,
                                     const Point & centralPoint,
                                     const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const GraphImplementation::LogScale scale,
                                     const Bool isFilled) const
{
  if (!(getInputDimension() >= 2)) throw InvalidArgumentException(HERE) << "Error: cannot use this version of the draw() method with a function of input dimension less than 2";
  if (!(xMin.getDimension() == 2 && xMax.getDimension() == 2 && pointNumber.getSize() == 2)) throw InvalidArgumentException(HERE) << "Error: xMin, xMax and PointNumber must be bidimensional";
  if (!(pointNumber[0] > 2 && pointNumber[1] > 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const Bool useLogX = (scale == GraphImplementation::LOGX || scale == GraphImplementation::LOGXY);
  if (useLogX && (!(xMin[0] > 0.0 && xMax[0] > 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the first argument.";
  const Bool useLogY = (scale == GraphImplementation::LOGY || scale == GraphImplementation::LOGXY);
  if (useLogY && (!(xMin[1] > 0.0 && xMax[1] > 0.0))) throw InvalidArgumentException(HERE) << "Error: cannot use logarithmic scale on an interval containing nonpositive values for the second argument.";
  if (centralPoint.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a central point of dimension=" << getInputDimension() << ", got dimension=" << centralPoint.getDimension();

  // Discretization of the first component
  const UnsignedInteger nX = pointNumber[0];
  Sample x(nX, 1);
  if (useLogX)
  {
    const Scalar a = std::log(xMin[0]);
    const Scalar b = std::log(xMax[0]);
    const Scalar dLogX = (b - a) / (nX - 1.0);
    for (UnsignedInteger i = 0; i < nX; ++i)
      x(i, 0) = std::exp(a + i * dLogX);
  }
  else
  {
    const Scalar dX = (xMax[0] - xMin[0]) / (nX - 1.0);
    for (UnsignedInteger i = 0; i < nX; ++i)
      x(i, 0) = xMin[0] + i * dX;
  }

  // Discretization of the second component
  const Scalar nY = pointNumber[1];
  Sample y(nY, 1);
  if (useLogY)
  {
    const Scalar a = std::log(xMin[1]);
    const Scalar b = std::log(xMax[1]);
    const Scalar dLogY = (b - a) / (nY - 1.0);
    for (UnsignedInteger i = 0; i < nY; ++i)
      y(i, 0) = std::exp(a + i * dLogY);
  }
  else
  {
    const Scalar dY = (xMax[1] - xMin[1]) / (nY - 1.0);
    for (UnsignedInteger i = 0; i < nY; ++i)
      y(i, 0) = xMin[1] + i * dY;
  }

  const String xName(getInputDescription()[firstInputMarginal]);
  const String yName(getInputDescription()[secondInputMarginal]);
  String title(OSS() << getOutputDescription()[outputMarginal] << " as a function of (" << xName << "," << yName << ")");
  if (centralPoint.getDimension() > 2) title = String(OSS(false) << title << " around " << centralPoint);
  Graph graph(title, xName, yName, true, "upper left");
  graph.setLogScale(scale);

  if (Interval(xMin, xMax).getVolume() > 0.0)
  {
    // Discretization of the XY plane
    Sample inputSample(nX * nY, centralPoint);
    // Prepare the input sample
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < nY; ++j)
    {
      const Scalar yJ = y(j, 0);
      for (UnsignedInteger i = 0; i < nX; ++i)
      {
        const Scalar xI = x(i, 0);
        inputSample(index, firstInputMarginal)  = xI;
        inputSample(index, secondInputMarginal) = yJ;
        ++index;
      } // i
    } // j
    // Compute the output sample, using possible parallelism
    const Sample z((*this)(inputSample).getMarginal(outputMarginal));
    Contour isoValues(x, y, z);
    isoValues.setIsFilled(isFilled);
    isoValues.setDrawLabels(false);
    graph.add(isoValues);
  }
  else
  {
    // degenerate cases
    Sample inputSample;
    if (xMin[0] < xMax[0])
    {
      // constant Y
      inputSample = Sample(nX, centralPoint);
      for (UnsignedInteger i = 0; i < nX; ++ i)
      {
        inputSample(i, firstInputMarginal)  = x(i, 0);
        inputSample(i, secondInputMarginal) = xMin[1];
      }
    }
    else if (xMin[1] < xMax[1])
    {
      // constant X
      inputSample = Sample(nY, centralPoint);
      for (UnsignedInteger j = 0; j < nY; ++ j)
      {
        inputSample(j, firstInputMarginal)  = xMin[0];
        inputSample(j, secondInputMarginal) = y(j, 0);
      }
    }
    else
    {
      // single point
      inputSample = Sample(1, centralPoint);
      inputSample(0, firstInputMarginal)  = xMin[0];
      inputSample(0, secondInputMarginal) = xMin[1];
    }
    // Compute the output sample, using possible parallelism
    const Sample z((*this)(inputSample).getMarginal(outputMarginal));
    const UnsignedInteger size = inputSample.getSize();
    if (size > 1)
    {
      if (xMin[0] < xMax[0])
      {
        // constant Y
        graph.setXTitle(xName);
        graph.setYTitle(getOutputDescription()[outputMarginal]);
        graph.setTitle(OSS() << graph.getYTitle() << " as a function of " << graph.getXTitle());
        Curve curve(inputSample.getMarginal(firstInputMarginal), z);
        graph.add(curve);
      }
      else if (xMin[1] < xMax[1])
      {
        // constant X
        graph.setXTitle(yName);
        graph.setYTitle(getOutputDescription()[outputMarginal]);
        graph.setTitle(OSS() << graph.getYTitle() << " as a function of " << graph.getXTitle());
        Curve curve(inputSample.getMarginal(secondInputMarginal), z);
        graph.add(curve);
      }
    }
    else
    {
      // single point
      Cloud cloud(inputSample.getMarginal({firstInputMarginal, secondInputMarginal}), z);
      const Scalar zMin = z.getMin()[0];
      cloud.setLegend(OSS() << zMin);
      graph.add(cloud);
    }
  }
  return graph;
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph EvaluationImplementation::draw(const Scalar xMin,
                                     const Scalar xMax,
                                     const UnsignedInteger pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getOutputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 0, Point(1), xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph EvaluationImplementation::draw(const Point & xMin,
                                     const Point & xMax,
                                     const Indices & pointNumber,
                                     const GraphImplementation::LogScale scale) const
{
  if (getInputDimension() == 1) return draw(xMin[0], xMax[0], pointNumber[0], scale);
  if ((getInputDimension() == 0) || (getInputDimension() > 2)) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with input dimension=" << getInputDimension() << " different from 1 or 2 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a function with output dimension=" << getOutputDimension() << " different from 1 using this method. See the other draw() methods.";
  return draw(0, 1, 0, Point(2), xMin, xMax, pointNumber, scale);
}

/** Draw the cross-cuts of the function supposed to have 1D output */
GridLayout EvaluationImplementation::drawCrossCuts(const Point & centralPoint,
    const Point & xMin,
    const Point & xMax,
    const Indices& pointNumber,
    const Bool withMonoDimensionalCuts,
    const Bool isFilled,
    const Scalar vMin,
    const Scalar vMax) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (!(inputDimension >= 2)) throw InvalidArgumentException(HERE) << "Error: cannot draw cross cuts of a function with input dimension=" << inputDimension << " less than 2 using this method. See the other draw() methods.";
  if (getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw cross cuts of a function with output dimension=" << getOutputDimension() << " different from 1 using this method. See the other draw() methods.";
  if (!(xMin.getDimension() == inputDimension && xMax.getDimension() == inputDimension && pointNumber.getSize() == inputDimension)) throw InvalidArgumentException(HERE) << "Error: xMin, xMax and PointNumber must be of dimension " << inputDimension;
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
    if (!(pointNumber[i] > 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const Bool buildVMinMax = vMax == -SpecFunc::Infinity && vMin == -SpecFunc::Infinity;
  Scalar vMinCal = buildVMinMax ? SpecFunc::Infinity : vMin;
  Scalar vMaxCal = vMax;
  if (!buildVMinMax && !(vMin < vMax))throw InvalidArgumentException(HERE) << "Error: the vMin value must be less than the vMax value";
  //Building component samples
  Collection<Sample>samples(inputDimension);
  for (UnsignedInteger iX = 0; iX < inputDimension; iX++)
  {
    const UnsignedInteger nX = pointNumber[iX];
    Sample sample(nX, 1);
    const Scalar dX = (xMax[iX] - xMin[iX]) / (nX - 1.0);
    for (UnsignedInteger i = 0; i < nX; ++i)
      sample(i, 0) = xMin[iX] + i * dX;
    samples[iX] = sample;
  }
  //Build contours
  GridLayout grid(withMonoDimensionalCuts ? inputDimension : inputDimension - 1, withMonoDimensionalCuts ? inputDimension : inputDimension - 1);
  if (!getName().empty())
    grid.setTitle("Cross cuts of function " + getName());
  for (UnsignedInteger iX = 0; iX < inputDimension; iX++)
  {
    const Sample x(samples[iX]);
    for (UnsignedInteger iY = iX + 1; iY < inputDimension; iY++)
    {
      // Discretization of the XY plane
      const UnsignedInteger nX = pointNumber[iX], nY = pointNumber[iY];
      const Sample y(samples[iY]);
      Sample inputSample(nX * nY, centralPoint);
      // Prepare the input sample
      UnsignedInteger index = 0;
      for (UnsignedInteger j = 0; j < nY; ++j)
      {
        const Scalar yJ = y(j, 0);
        for (UnsignedInteger i = 0; i < nX; ++i)
        {
          const Scalar xI = x(i, 0);
          inputSample(index, iX) = xI;
          inputSample(index, iY) = yJ;
          ++index;
        } // i
      } // j
      // Compute the output sample, using possible parallelism
      const Sample z((*this)(inputSample));
      if (buildVMinMax)
      {
        vMinCal = std::min(vMinCal, z.getMin()[0]);
        vMaxCal = std::max(vMaxCal, z.getMax()[0]);
      }
      Contour isoValues(x, y, z);
      isoValues.setIsFilled(isFilled);
      isoValues.setDrawLabels(!isFilled);
      isoValues.setColorBarPosition("");
      Graph graph("", iY + 1 == inputDimension ? getInputDescription()[iX] : "", iX == 0 ? getInputDescription()[iY] : "", true);
      graph.add(isoValues);
      grid.setGraph(withMonoDimensionalCuts ? iY : iY - 1, iX, graph);
    }
  }
  if (withMonoDimensionalCuts)
  {
    //Build curves
    for (UnsignedInteger iX = 0; iX < inputDimension; iX++)
    {
      Sample x(samples[iX]);
      const UnsignedInteger nX = pointNumber[iX];
      Sample inputSample(nX, centralPoint);
      for (UnsignedInteger i = 0; i < nX; ++i)
        inputSample(i, iX) = x(i, 0);
      Graph graph("", iX + 1 == inputDimension ? getInputDescription()[iX] : "", getOutputDescription()[0], true);
      graph.add(Curve(x, (*this)(inputSample)));
      grid.setGraph(iX, iX, graph);
    }
  }
  //Uniformize vMin and vMax
  for (UnsignedInteger iX = 0; iX < inputDimension; iX++)
  {
    for (UnsignedInteger iY = iX + 1; iY < inputDimension; iY++)
    {
      Contour* contour = dynamic_cast<Contour*>(grid.getGraph(withMonoDimensionalCuts ? iY : iY - 1, iX).getDrawable(0).getImplementation().get());
      contour->setVmin(vMinCal);
      contour->setVmax(vMaxCal);
      Point levels(ResourceMap::GetAsUnsignedInteger("Contour-DefaultLevelsNumber"));
      for (UnsignedInteger i = 0; i < levels.getSize(); i++)
        levels[i] = vMinCal + (vMaxCal - vMinCal) * (i + 1) / (levels.getSize() + 1);
      contour->setLevels(levels);
    }
  }
  return grid;
}

void EvaluationImplementation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}

/* Method save() stores the object through the StorageManager */
void EvaluationImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
  adv.saveAttribute( "inputDescription_", inputDescription_ );
  adv.saveAttribute( "outputDescription_", outputDescription_ );
  adv.saveAttribute( "parameter_", parameter_ );
  adv.saveAttribute( "parameterDescription_", parameterDescription_ );
  adv.saveAttribute( "checkOutput_", checkOutput_ );
}

/* Method load() reloads the object from the StorageManager */
void EvaluationImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
  adv.loadAttribute( "inputDescription_", inputDescription_ );
  adv.loadAttribute( "outputDescription_", outputDescription_ );
  adv.loadAttribute( "parameter_", parameter_ );
  adv.loadAttribute( "parameterDescription_", parameterDescription_ );
  adv.loadAttribute( "checkOutput_", checkOutput_ );
}

END_NAMESPACE_OPENTURNS
