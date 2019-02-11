//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements the sensitivity analysis methods based on correlation coefficients
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

/* Default constructor */
CorrelationAnalysis::CorrelationAnalysis() {}

/* Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D outputSample */
Point CorrelationAnalysis::PearsonCorrelation(const Sample & inputSample,
    const Sample & outputSample)
{
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
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

/* Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D outputSample */
Point CorrelationAnalysis::SpearmanCorrelation(const Sample & inputSample,
    const Sample & outputSample)
{
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return PearsonCorrelation(inputSample.rank(), outputSample.rank());
}

/* Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
Point CorrelationAnalysis::SRC(const Sample & inputSample,
                               const Sample & outputSample,
                               const Bool normalize)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  if (dimension < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  // If normalize, then sum is equal to 1
  // and no information about noise
  // Othewise we use standard formulation of src
  // normalize false ==> SRC = SignedSRC * SignedSRC
  if (!normalize)
  {
    // Case normalize false : coefficients are square of SignedSRC
    Point src(SignedSRC(inputSample, outputSample));
    for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] = (src[i] * src[i]);
    return src;
  }

  // Case where normalize is true
  // Var(X+a) = Var(X); However for numerical stability, data are centered
  LinearLeastSquares regressionAlgorithm(inputSample - inputSample.computeMean(), outputSample);
  regressionAlgorithm.run();
  // Linear coefficients
  const Point linear(*regressionAlgorithm.getLinear().getImplementation());

  // Compute the output variance from the regression coefficients
  Point src(inputSample.computeVariance());
  for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] *= linear[i] * linear[i];
  Scalar varOutput = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++ i) varOutput += src[i];
  if (varOutput > 0.0)
    src /= varOutput;
  else
    throw InvalidArgumentException(HERE) << "No output variance";
  return src;
}

/* Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
Point CorrelationAnalysis::SignedSRC(const Sample & inputSample,
                                     const Sample & outputSample)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  if (dimension < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  // Var(X+a) = Var(X); However for numerical stability, data are centered
  LinearLeastSquares regressionAlgorithm(inputSample - inputSample.computeMean(), outputSample);
  regressionAlgorithm.run();
  // Linear coefficients
  const Point linear(*regressionAlgorithm.getLinear().getImplementation());

  const Scalar stdOutput = outputSample.computeStandardDeviationPerComponent()[0];
  if (!(stdOutput > 0.0))
    throw InvalidArgumentException(HERE) << "No output variance";

  // Compute the output variance from the regression coefficients
  Point src(inputSample.computeStandardDeviationPerComponent());
  for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] *= linear[i] / stdOutput;
  return src;
}

/* Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
Point CorrelationAnalysis::PCC(const Sample & inputSample,
                               const Sample & outputSample)
{
  if (inputSample.getDimension() < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger dimension = inputSample.getDimension();
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

    const Sample residualOutput(outputSample - outputVersusTruncatedInput.getResponseSurface()(truncatedInput));
    const Sample residualRemaining(remainingInput - remainingVersusTruncatedInput.getResponseSurface()(truncatedInput));

    // Compute the correlation between the residuals
    pcc[index] = PearsonCorrelation(residualOutput, residualRemaining)[0];
  }
  return pcc;
}

/* Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
Point CorrelationAnalysis::SRRC(const Sample & inputSample,
                                const Sample & outputSample,
                                const Bool normalize)
{
  const UnsignedInteger dimension = inputSample.getDimension();
  if (dimension < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return SRC(inputSample.rank(), outputSample.rank(), normalize);
}

/* Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
Point CorrelationAnalysis::PRCC(const Sample & inputSample,
                                const Sample & outputSample)
{
  // Perform the basic checks of the inputs, to avoid costly ranking if finally PCC will fail
  if (inputSample.getDimension() < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return PCC(inputSample.rank(), outputSample.rank());
}

END_NAMESPACE_OPENTURNS
