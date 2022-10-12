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
CorrelationAnalysis::CorrelationAnalysis(const Sample & firstSample,
    const Sample & secondSample)
  : PersistentObject()
  , firstSample_(firstSample)
  , secondSample_(secondSample)
{
  if (secondSample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample.getSize() != secondSample.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
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
      << " firstSample=" << firstSample_
      << " secondSample=" << secondSample_;
  return oss;
}

/* Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computePearsonCorrelation() const
{
  return ComputePearsonCorrelation(firstSample_, secondSample_);
}

// Compute the Pearson correlation coefficient with arguments
Point CorrelationAnalysis::ComputePearsonCorrelation(const Sample & firstSample,
    const Sample & secondSample)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  Point result(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Sample pairedSample(firstSample.getMarginal(j));
    pairedSample.stack(secondSample);
    result[j] = pairedSample.computePearsonCorrelation()(1, 0);
  }
  return result;
}

/* Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computeSpearmanCorrelation() const
{
  return ComputePearsonCorrelation(firstSample_.rank(), secondSample_.rank());
}

/* Compute the Kendall Tau coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computeKendallTau() const
{
  const UnsignedInteger dimension = firstSample_.getDimension();
  Point result(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Sample pairedSample(firstSample_.getMarginal(j));
    pairedSample.stack(secondSample_);
    result[j] = pairedSample.computeKendallTau()(1, 0);
  }
  return result;
}

/* Compute the squared Standard Regression Coefficients (SRC) between the input sample and the output sample */
Point CorrelationAnalysis::computeSquaredSRC(const Bool normalize) const
{
  Point src(computeSRC());
  for (UnsignedInteger i = 0; i < src.getDimension(); ++ i) src[i] *= src[i];
  // Case normalize true : coefficients are made to sum to 1
  if (normalize) src /= src.norm1();
  return src;
}

/* Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
Point CorrelationAnalysis::computeSRC() const
{
  return ComputeSRC(firstSample_, secondSample_);
}

// Compute the Standard Regression Coefficients (SRC) with arguments
Point CorrelationAnalysis::ComputeSRC(const Sample & firstSample,
                                      const Sample & secondSample)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  //if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  // Var(X+a) = Var(X); However for numerical stability, data are centered
  LinearLeastSquares regressionAlgorithm(firstSample - firstSample.computeMean(), secondSample);
  regressionAlgorithm.run();
  // Linear coefficients
  const Point linear(*regressionAlgorithm.getLinear().getImplementation());

  const Scalar stdOutput = secondSample.computeStandardDeviation()[0];
  if (!(stdOutput > 0.0))
    throw InvalidArgumentException(HERE) << "No output variance";

  // Compute the output variance from the regression coefficients
  Point src(firstSample.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] *= linear[i] / stdOutput;
  return src;
}

/* Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
Point CorrelationAnalysis::computePCC() const
{
  return ComputePCC(firstSample_, secondSample_);
}

// Compute the Partial Correlation Coefficients (PCC) with arguments
Point CorrelationAnalysis::ComputePCC(const Sample & firstSample,
                                      const Sample & secondSample)
{
  const UnsignedInteger dimension = firstSample.getDimension();
  //if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  const UnsignedInteger size = firstSample.getSize();
  Point pcc(dimension);
  // For each component i, perform an analysis on the truncated input sample where Xi has been removed
  Sample truncatedInput(size, dimension - 1);
  Sample remainingInput(size, 1);
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    // Build the truncated sample
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < index; ++j) truncatedInput(i, j) = firstSample(i, j);
      for (UnsignedInteger j = index + 1; j < dimension; ++j) truncatedInput(i, j - 1) = firstSample(i, j);
      remainingInput(i, 0) = firstSample(i, index);
    }
    // Build the linear models
    LinearLeastSquares outputVersusTruncatedInput(truncatedInput, secondSample);
    outputVersusTruncatedInput.run();

    LinearLeastSquares remainingVersusTruncatedInput(truncatedInput, remainingInput);
    remainingVersusTruncatedInput.run();

    const Sample residualOutput(secondSample - outputVersusTruncatedInput.getMetaModel()(truncatedInput));
    const Sample residualRemaining(remainingInput - remainingVersusTruncatedInput.getMetaModel()(truncatedInput));

    // Compute the correlation between the residuals
    pcc[index] = ComputePearsonCorrelation(residualOutput, residualRemaining)[0];
  }
  return pcc;
}

/* Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
Point CorrelationAnalysis::computeSRRC() const
{
  return ComputeSRC(firstSample_.rank(), secondSample_.rank());
}

/* Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
Point CorrelationAnalysis::computePRCC() const
{
  return ComputePCC(firstSample_.rank(), secondSample_.rank());
}

/* Method save() stores the object through the StorageManager */
void CorrelationAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("firstSample_", firstSample_);
  adv.saveAttribute("secondSample_", secondSample_);
}

/* Method load() reloads the object from the StorageManager */
void CorrelationAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("firstSample_", firstSample_);
  adv.loadAttribute("secondSample_", secondSample_);
}

END_NAMESPACE_OPENTURNS
