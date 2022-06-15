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
CorrelationAnalysis::CorrelationAnalysis(const Sample & sampleX,
                                         const Sample & sampleY)
  : PersistentObject()
  , sampleX_(sampleX)
  , sampleY_(sampleY)
{
  if (sampleY.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (sampleX.getSize() != sampleY.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
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
      << " sampleX=" << sampleX_
      << " sampleY=" << sampleY_;
  return oss;
}

/* Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computePearsonCorrelation() const
{
  return computePearsonCorrelation(sampleX_, sampleY_);
}

/* Deprecated static Pearson correlation coefficient computation method */
Point CorrelationAnalysis::PearsonCorrelation(const Sample & inputSample,
                                              const Sample & outputSample)
{
  LOGWARN(OSS() << "CorrelationAnalysis::PearsonCorrelation(inputSample, outputSample) is deprecated, use CorrelationAnalysis::computePearsonCorrelation() instead");
  return CorrelationAnalysis(inputSample, outputSample).computePearsonCorrelation();
}

// Compute the Pearson correlation coefficient with arguments
Point CorrelationAnalysis::computePearsonCorrelation(const Sample & sampleX,
                                                     const Sample & sampleY) const
{
  const UnsignedInteger dimension = sampleX.getDimension();
  Point result(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Sample pairedSample(sampleX.getMarginal(j));
    pairedSample.stack(sampleY);
    result[j] = pairedSample.computePearsonCorrelation()(1, 0);
  }
  return result;
}

/* Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computeSpearmanCorrelation() const
{
  return computePearsonCorrelation(sampleX_.rank(), sampleY_.rank());
}

/* Deprecated static Spearman correlation coefficient computation method */
Point CorrelationAnalysis::SpearmanCorrelation(const Sample & inputSample,
                                               const Sample & outputSample)
{
  LOGWARN(OSS() << "CorrelationAnalysis::SpearmanCorrelation(inputSample, outputSample) is deprecated, use CorrelationAnalysis::computeSpearmanCorrelation() instead");
  return CorrelationAnalysis(inputSample, outputSample).computeSpearmanCorrelation();
}

/* Compute the Kendall Tau coefficient between the component number index of the input sample and the 1D output sample */
Point CorrelationAnalysis::computeKendallTau() const
{
  const UnsignedInteger dimension = sampleX_.getDimension();
  Point result(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    Sample pairedSample(sampleX_.getMarginal(j));
    pairedSample.stack(sampleY_);
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
  return computeSRC(sampleX_, sampleY_);
}

// Compute the Standard Regression Coefficients (SRC) with arguments
Point CorrelationAnalysis::computeSRC(const Sample & sampleX,
                                      const Sample & sampleY) const
{
  const UnsignedInteger dimension = sampleX.getDimension();
  //if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  // Var(X+a) = Var(X); However for numerical stability, data are centered
  LinearLeastSquares regressionAlgorithm(sampleX - sampleX.computeMean(), sampleY);
  regressionAlgorithm.run();
  // Linear coefficients
  const Point linear(*regressionAlgorithm.getLinear().getImplementation());

  const Scalar stdOutput = sampleY.computeStandardDeviation()[0];
  if (!(stdOutput > 0.0))
    throw InvalidArgumentException(HERE) << "No output variance";

  // Compute the output variance from the regression coefficients
  Point src(sampleX.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < dimension; ++ i) src[i] *= linear[i] / stdOutput;
  return src;
}

/* Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
Point CorrelationAnalysis::computePCC() const
{
  return computePCC(sampleX_, sampleY_);
}

/* Deprecated static PCC computation method */
Point CorrelationAnalysis::PCC(const Sample & inputSample,
                               const Sample & outputSample)
{
  LOGWARN(OSS() << "CorrelationAnalysis::PCC(inputSample, outputSample) is deprecated, use CorrelationAnalysis::computePCC() instead");
  return CorrelationAnalysis(inputSample, outputSample).computePCC();
}

// Compute the Partial Correlation Coefficients (PCC) with arguments
Point CorrelationAnalysis::computePCC(const Sample & sampleX,
                                      const Sample & sampleY) const
{
  const UnsignedInteger dimension = sampleX.getDimension();
  //if (!(dimension >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have dimension > 1, here dimension=" << dimension;
  const UnsignedInteger size = sampleX.getSize();
  Point pcc(dimension);
  // For each component i, perform an analysis on the truncated input sample where Xi has been removed
  Sample truncatedInput(size, dimension - 1);
  Sample remainingInput(size, 1);
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    // Build the truncated sample
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < index; ++j) truncatedInput(i, j) = sampleX(i, j);
      for (UnsignedInteger j = index + 1; j < dimension; ++j) truncatedInput(i, j - 1) = sampleX(i, j);
      remainingInput(i, 0) = sampleX(i, index);
    }
    // Build the linear models
    LinearLeastSquares outputVersusTruncatedInput(truncatedInput, sampleY);
    outputVersusTruncatedInput.run();

    LinearLeastSquares remainingVersusTruncatedInput(truncatedInput, remainingInput);
    remainingVersusTruncatedInput.run();

    const Sample residualOutput(sampleY - outputVersusTruncatedInput.getMetaModel()(truncatedInput));
    const Sample residualRemaining(remainingInput - remainingVersusTruncatedInput.getMetaModel()(truncatedInput));

    // Compute the correlation between the residuals
    pcc[index] = computePearsonCorrelation(residualOutput, residualRemaining)[0];
  }
  return pcc;
}

/* Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
Point CorrelationAnalysis::computeSRRC() const
{
  return computeSRC(sampleX_.rank(), sampleY_.rank());
}

/* Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
Point CorrelationAnalysis::computePRCC() const
{
  return computePCC(sampleX_.rank(), sampleY_.rank());
}

/* Deprecated static PRCC computation method */
Point CorrelationAnalysis::PRCC(const Sample & inputSample,
                                const Sample & outputSample)
{
  LOGWARN(OSS() << "CorrelationAnalysis::PRCC(inputSample, outputSample) is deprecated, use CorrelationAnalysis::computePRCC() instead");
  return CorrelationAnalysis(inputSample, outputSample).computePRCC();
}

/* Method save() stores the object through the StorageManager */
void CorrelationAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("sampleX_", sampleX_);
  adv.saveAttribute("sampleY_", sampleY_);
}

/* Method load() reloads the object from the StorageManager */
void CorrelationAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("sampleX_", sampleX_);
  adv.loadAttribute("sampleY_", sampleY_);
}

END_NAMESPACE_OPENTURNS
