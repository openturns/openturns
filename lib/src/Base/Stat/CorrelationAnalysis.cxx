//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements the sensitivity analysis methods based on correlation coefficients
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>

#include "openturns/CorrelationAnalysis.hxx"
#include "openturns/Exception.hxx"
#include "openturns/LinearModelFactory.hxx"
#include "openturns/LinearModel.hxx"
#include "openturns/LinearLeastSquares.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Default constructor */
CorrelationAnalysis::CorrelationAnalysis() {}

/* Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D outputSample */
NumericalScalar CorrelationAnalysis::PearsonCorrelation(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const UnsignedInteger index)
{
  if (index >= inputSample.getDimension()) throw InvalidArgumentException(HERE) << "Error: given index out of bound";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger size = inputSample.getSize();
  NumericalSample pairedSample(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    pairedSample[i][0] = inputSample[i][index];
    pairedSample[i][1] = outputSample[i][0];
  }
  return pairedSample.computePearsonCorrelation()(0, 1);
}

/* Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D outputSample */
NumericalScalar CorrelationAnalysis::SpearmanCorrelation(const NumericalSample & inputSample,
    const NumericalSample & outputSample,
    const UnsignedInteger index)
{
  if (index >= inputSample.getDimension()) throw InvalidArgumentException(HERE) << "Error: given index out of bound";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return PearsonCorrelation(inputSample.getMarginal(index).rank(), outputSample.rank());
}

/* Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
NumericalPoint CorrelationAnalysis::SRC(const NumericalSample & inputSample,
                                        const NumericalSample & outputSample)
{
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger dimension = inputSample.getDimension();
  LinearLeastSquares regressionAlgorithm(inputSample, outputSample);
  regressionAlgorithm.run();
  const NumericalPoint linear(regressionAlgorithm.getLinear() * NumericalPoint(1, 1.0));
  const NumericalScalar varOutput = outputSample.computeVariance()[0];
  NumericalPoint src(inputSample.computeVariance());
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    src[i] *= linear[i] * linear[i] / varOutput;
    if (src[i] > 1.) LOGWARN(OSS() << "SRC coefficient for component " << i << " value (" << src[i] << ") is > 1. Check the variance of the samples." );
  }
  return src;
}

/* Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
NumericalPoint CorrelationAnalysis::PCC(const NumericalSample & inputSample,
                                        const NumericalSample & outputSample)
{
  if (inputSample.getDimension() < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  const UnsignedInteger dimension = inputSample.getDimension();
  const UnsignedInteger size = inputSample.getSize();
  NumericalPoint pcc(dimension);
  // For each component i, perform an analysis on the truncated input sample where Xi has been removed
  NumericalSample truncatedInput(size, dimension - 1);
  NumericalSample remainingInput(size, 1);
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    // Build the truncated sample
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < index; ++j) truncatedInput[i][j] = inputSample[i][j];
      for (UnsignedInteger j = index + 1; j < dimension; ++j) truncatedInput[i][j - 1] = inputSample[i][j];
      remainingInput[i][0] = inputSample[i][index];
    }
    // Build the linear models
    const LinearModel outputVersusTruncatedInput(LinearModelFactory().build(truncatedInput, outputSample));
    const LinearModel remainingVersusTruncatedInput(LinearModelFactory().build(truncatedInput, remainingInput));
    // Compute the correlation between the residuals
    const NumericalSample residualOutput(outputVersusTruncatedInput.getResidual(truncatedInput, outputSample));
    const NumericalSample residualRemaining(remainingVersusTruncatedInput.getResidual(truncatedInput, remainingInput));
    pcc[index] = PearsonCorrelation(residualOutput, residualRemaining);
  }
  return pcc;
}

/* Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
NumericalPoint CorrelationAnalysis::SRRC(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
{
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return SRC(inputSample.rank(), outputSample.rank());
}

/* Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
NumericalPoint CorrelationAnalysis::PRCC(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
{
  // Perform the basic checks of the inputs, to avoid costly ranking if finally PCC will fail
  if (inputSample.getDimension() < 2) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1";
  if (outputSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
  return PCC(inputSample.rank(), outputSample.rank());
}

END_NAMESPACE_OPENTURNS
