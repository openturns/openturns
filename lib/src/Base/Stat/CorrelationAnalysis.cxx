//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements the sensitivity analysis methods based on correlation coefficients
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
#include <cmath>

#include "openturns/CorrelationAnalysis.hxx"
#include "openturns/Exception.hxx"
#include "openturns/LinearLeastSquares.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CorrelationAnalysis
 */

CLASSNAMEINIT(CorrelationAnalysis)

/* Default constructor */
CorrelationAnalysis::CorrelationAnalysis()
  : PersistentObject()
{
  // nothing to do
}

/* Standard constructor */
CorrelationAnalysis::CorrelationAnalysis(const Sample & inputSample,
                                         const Sample & outputSample)
  : PersistentObject()
  , inputSample_(inputSample)
  , outputSample_(outputSample)
{
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
}

/* Virtual constructor */
CorrelationAnalysis * CorrelationAnalysis::clone() const
{
  return new CorrelationAnalysis(*this);
}

/* String converter */
String CorrelationAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " inputSample=" << inputSample_
      << " outputSample=" << outputSample_;
  return oss;
}

/* Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D output sample */
const Point CorrelationAnalysis::computePearsonCorrelation()
{
  return computePearsonCorrelation(inputSample_, outputSample_);
}

// Compute the Pearson correlation coefficient with arguments
const Point CorrelationAnalysis::computePearsonCorrelation(const Sample & inputSample,
                                                     const Sample & outputSample)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  Point result(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Sample pairedSample(inputSample.getMarginal(j));
    pairedSample.stack(outputSample);
    result[j] = pairedSample.computePearsonCorrelation()(1, 0);
  }
  return result;
}

/* Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D output sample */
const Point CorrelationAnalysis::computeSpearmanCorrelation()
{
  return computePearsonCorrelation(inputSample_.rank(), outputSample_.rank());
}

/* Compute the squared Standard Regression Coefficients (SRC) between the input sample and the output sample */
const Point CorrelationAnalysis::computeSquaredSRC(const Bool normalize)
{
  Point src(computeSRC());
  for (UnsignedInteger i = 0; i < src.getDimension(); ++ i) src[i] *= src[i];
  // Case normalize true : coefficients are made to sum to 1
  if (normalize) src /= src.norm1();
  return src;
}

/* Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
const Point CorrelationAnalysis::computeSRC()
{
  return computeSRC(inputSample_, outputSample_);
}

// Compute the Standard Regression Coefficients (SRC) with arguments
const Point CorrelationAnalysis::computeSRC(const Sample & inputSample,
                                      const Sample & outputSample)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  // Var(X+a) = Var(X); However for numerical stability, data are centered
  LinearLeastSquares regressionAlgorithm(inputSample - inputSample.computeMean(), outputSample);
  regressionAlgorithm.run();
  // Linear coefficients
  const Point linear(*regressionAlgorithm.getLinear().getImplementation());

  const Scalar stdOutput = outputSample.computeStandardDeviation()[0];
  if (!(stdOutput > 0.0))
    throw InvalidArgumentException(HERE) << "No output variance";

  // Compute the output variance from the regression coefficients
  Point src(inputSample.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] *= linear[i] / stdOutput;
  return src;
}

/* Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
const Point CorrelationAnalysis::computePCC()
{
  return computePCC(inputSample_, outputSample_);
}

// Compute the Partial Correlation Coefficients (PCC) with arguments
const Point CorrelationAnalysis::computePCC(const Sample & inputSample,
                                      const Sample & outputSample)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  const UnsignedInteger size = inputSample.getSize();
  Point pcc(dimension);
  // For each component i, perform an analysis on the truncated input sample where Xi has been removed
  Sample truncatedInput(size, dimension - 1);
  Sample remainingInput(size, 1);
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    // Build the truncated sample
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < index; ++j) truncatedInput(i, j) = inputSample(i, j);
      for (UnsignedInteger j = index + 1; j < dimension; ++j) truncatedInput(i, j - 1) = inputSample(i, j);
      remainingInput(i, 0) = inputSample(i, index);
    }
    // Build the linear models
    LinearLeastSquares outputVersusTruncatedInput(truncatedInput, outputSample);
    outputVersusTruncatedInput.run();

    LinearLeastSquares remainingVersusTruncatedInput(truncatedInput, remainingInput);
    remainingVersusTruncatedInput.run();

    const Sample residualOutput(outputSample - outputVersusTruncatedInput.getMetaModel()(truncatedInput));
    const Sample residualRemaining(remainingInput - remainingVersusTruncatedInput.getMetaModel()(truncatedInput));

    // Compute the correlation between the residuals
    pcc[index] = computePearsonCorrelation(residualOutput, residualRemaining)[0];
  }
  return pcc;
}

/* Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
const Point CorrelationAnalysis::computeSRRC()
{
  return computeSRC(inputSample_.rank(), outputSample_.rank());
}

/* Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
const Point CorrelationAnalysis::computePRCC()
{
  return computePCC(inputSample_.rank(), outputSample_.rank());
}

/* Method save() stores the object through the StorageManager */
void CorrelationAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
}

/* Method load() reloads the object from the StorageManager */
void CorrelationAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
}

END_NAMESPACE_OPENTURNS
