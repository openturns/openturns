//                                               -*- C++ -*-
/**
 * @brief ANCOVA implements the sensivity analysis method for correlated data
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

#include <cmath>

#include "openturns/ANCOVA.hxx"


BEGIN_NAMESPACE_OPENTURNS

/* Constructor */
ANCOVA::ANCOVA(const FunctionalChaosResult & functionalChaosResult, const Sample & correlatedInput)
  : functionalChaosResult_(functionalChaosResult),
    correlatedInput_(correlatedInput),
    alreadyComputedIndices_(false)
{
  if (correlatedInput_.getSize() == 0) throw InvalidArgumentException(HERE) << "Input sample is empty";
  const UnsignedInteger nbInput = functionalChaosResult_.getMetaModel().getInputDimension();
  if (correlatedInput_.getDimension() != nbInput) throw InvalidDimensionException(HERE) << "The dimension of the input sample must be equal to the number of inputs of the model";
}

/* Compute the ANCOVA indices */
void ANCOVA::run() const
{
  const UnsignedInteger inputDimension = correlatedInput_.getDimension();
  const UnsignedInteger inputSize = correlatedInput_.getSize();

  const UnsignedInteger nbMarginals = functionalChaosResult_.getMetaModel().getOutputDimension();
  const Indices coefficientIndices = functionalChaosResult_.getIndices();
  const EnumerateFunction enumerateFunction = functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction();
  const FunctionCollection B = functionalChaosResult_.getReducedBasis();
  const Function T = functionalChaosResult_.getTransformation();
  const Sample allCoefficients = functionalChaosResult_.getCoefficients();
  const UnsignedInteger coefSize = allCoefficients.getSize();

  uncorrelatedIndices_ = Sample(nbMarginals, inputDimension);
  indices_ = Sample(nbMarginals, inputDimension);

  for (UnsignedInteger input_i = 0; input_i < inputDimension; ++input_i)
  {
    // Search univariate polynomials
    Indices coefList;
    for (UnsignedInteger k = 0; k < coefSize; ++ k)
    {
      const Indices nullIndices(inputDimension);
      Indices multiIndices(enumerateFunction(coefficientIndices[k]));
      if (!multiIndices[input_i])
        continue;
      multiIndices[input_i] = 0;
      if (multiIndices == nullIndices)
        coefList.add(k);
    }
    for (UnsignedInteger marginal_k = 0; marginal_k < nbMarginals; ++marginal_k)
    {
      const Sample coefficients(allCoefficients.getMarginal(marginal_k));

      // Output sample obtained with correlated inputs
      const Sample Y(functionalChaosResult_.getMetaModel().getMarginal(marginal_k)(correlatedInput_));

      // Compute parts of variance
      Sample inputOutput(inputSize, 2);
      for (UnsignedInteger j = 0; j < inputSize; ++j)
      {
        Scalar temp = 0.;
        for (UnsignedInteger k = 0; k < coefList.getSize(); ++k)
          temp += coefficients(coefList[k], 0) * B[coefList[k]](T(correlatedInput_[j]))[0];

        inputOutput(j, 0) = temp;
        inputOutput(j, 1) = Y(j, 0);
      }
      const CovarianceMatrix inputOutputCovariance(inputOutput.computeCovariance());

      // Compute total part of variance of the marginal_k output due to input_i
      indices_(marginal_k, input_i) = inputOutputCovariance(0, 1) / inputOutputCovariance(1, 1);
      // "uncorrelated" part
      uncorrelatedIndices_(marginal_k, input_i) = inputOutputCovariance(0, 0) / inputOutputCovariance(1, 1);
    }
  }
  alreadyComputedIndices_ = true;
}

/* Accessor to ANCOVA indices measuring uncorrelated effects */
Point ANCOVA::getUncorrelatedIndices(const UnsignedInteger marginalIndex) const
{
  if (!alreadyComputedIndices_) run();
  UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1]. Here dim=" << outputDimension;
  return uncorrelatedIndices_[marginalIndex];
}

/* Compute ANCOVA indices: total parts of variance of the output due to each input */
Point ANCOVA::getIndices(const UnsignedInteger marginalIndex) const
{
  if (!alreadyComputedIndices_) run();
  UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (marginalIndex >= outputDimension)
    throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1]. Here dim=" << outputDimension;
  return indices_[marginalIndex];
}

END_NAMESPACE_OPENTURNS
