//                                               -*- C++ -*-
/**
 *  @brief Sensitivity analysis for field functional chaos
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FieldFunctionalChaosSobolIndices.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunctionalChaosSobolIndices)

static const Factory<FieldFunctionalChaosSobolIndices> Factory_FieldFunctionalChaosSobolIndices;

/* Default constructor */
FieldFunctionalChaosSobolIndices::FieldFunctionalChaosSobolIndices()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
FieldFunctionalChaosSobolIndices::FieldFunctionalChaosSobolIndices(const FieldFunctionalChaosResult & result)
  : PersistentObject()
{
  setResult(result);
}

/* Virtual constructor */
FieldFunctionalChaosSobolIndices * FieldFunctionalChaosSobolIndices::clone() const
{
  return new FieldFunctionalChaosSobolIndices(*this);
}

/* String converter */
String FieldFunctionalChaosSobolIndices::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}


void FieldFunctionalChaosSobolIndices::setResult(const FieldFunctionalChaosResult & result)
{
  result_ = result;

  cumulatedInputSizes_.clear();
  const UnsignedInteger inputBlockNumber = result_.getInputKLResultCollection().getSize();
  if (inputBlockNumber)
  {
    // Field input
    cumulatedInputSizes_.resize(inputBlockNumber + 1);
    for (UnsignedInteger k = 0; k < inputBlockNumber; ++ k)
      cumulatedInputSizes_[k + 1] = cumulatedInputSizes_[k] + result_.getInputKLResultCollection()[k].getEigenvalues().getDimension();
  }
  else
  {
    // Vector input
    cumulatedInputSizes_.resize(result_.getInputSample().getDimension() + 1);
    cumulatedInputSizes_.fill();
  }

  cumulatedOutputSizes_.clear();
  const UnsignedInteger outputBlockNumber = result_.getOutputKLResultCollection().getSize();
  if (outputBlockNumber)
  {
    // Field output
    cumulatedOutputSizes_.resize(outputBlockNumber + 1);
    for (UnsignedInteger k = 0; k < outputBlockNumber; ++ k)
      cumulatedOutputSizes_[k + 1] = cumulatedOutputSizes_[k] + result_.getOutputKLResultCollection()[k].getEigenvalues().getDimension();
  }
  else
  {
    // Vector output
    cumulatedOutputSizes_.resize(result_.getOutputSample().getDimension() + 1);
    cumulatedOutputSizes_.fill();
  }
}

Scalar FieldFunctionalChaosSobolIndices::getSobolIndex(const UnsignedInteger variableIndex, const UnsignedInteger marginalIndex) const
{
  const Indices index(1, variableIndex);
  return getSobolIndex(index, marginalIndex);
}

Scalar FieldFunctionalChaosSobolIndices::getSobolIndex(const Indices & variableIndices, const UnsignedInteger marginalIndex) const
{
  if (!variableIndices.check(cumulatedInputSizes_.getSize() - 1))
    throw InvalidArgumentException(HERE) << "The variable indices must be in the range [0, " << cumulatedInputSizes_.getSize() - 2 << "] and must be different.";
  if (marginalIndex >= cumulatedOutputSizes_.getSize() - 1)
    throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, " << cumulatedOutputSizes_.getSize() - 2 << "].";
  if (!result_.getFCEResult().getOrthogonalBasis().getMeasure().hasIndependentCopula())
    throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!result_.getFCEResult().getDistribution().hasIndependentCopula())
    LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");

  // Here we have to sum all the contributions of the coefficients of the PCE
  // that contributes to any of the coefficients of the jth marginal of the output process.
  Scalar totalVariance = 0.0;
  Scalar covarianceVariables = 0.0;

  // build the list of indices of the KL coefficients matching input variable indices
  Indices inputGroupIndices;
  for (UnsignedInteger i = 0; i < variableIndices.getSize(); ++ i)
  {
    const UnsignedInteger variableIndex = variableIndices[i];
    const UnsignedInteger startInput = cumulatedInputSizes_[variableIndex];
    const UnsignedInteger stopInput = cumulatedInputSizes_[variableIndex + 1];
    for (UnsignedInteger j = startInput; j < stopInput; ++ j)
      inputGroupIndices.add(j);
  }

  // build the list of indices of the KL coefficients matching output variable indices
  Indices outputGroupIndices;
  const UnsignedInteger startInput = cumulatedOutputSizes_[marginalIndex];
  const UnsignedInteger stopInput = cumulatedOutputSizes_[marginalIndex + 1];
  for (UnsignedInteger j = startInput; j < stopInput; ++ j)
    outputGroupIndices.add(j);
  const UnsignedInteger outputGroupSize = outputGroupIndices.getSize();

  // Now, select the relevant coefficients
  const Sample coefficients(result_.getFCEResult().getCoefficients());
  const UnsignedInteger size = coefficients.getSize();
  const EnumerateFunction enumerateFunction(result_.getFCEResult().getOrthogonalBasis().getEnumerateFunction());
  const Indices coefficientIndices(result_.getFCEResult().getIndices());
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < outputGroupSize; ++ j)
    {
      const Scalar coefficientIJ = coefficients(i, outputGroupIndices[j]);
      // Only non-zero coefficients have to be taken into account
      if (coefficientIJ != 0.0)
      {
        // The only multi-indices we must take into account for
        // the conditional variance are those associated to
        // multi-indices that contain positive indices in the
        // correct input range and null indices outside of this range
        Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        // Take into account only nonzero multi indices
        if (*std::max_element(multiIndices.begin(), multiIndices.end()) > 0)
        {
          totalVariance += coefficientIJ * coefficientIJ / outputGroupSize;
          // Set the exponents corresponding to the group to zero
          for (UnsignedInteger k = 0; k < inputGroupIndices.getSize(); ++ k)
            multiIndices[inputGroupIndices[k]] = 0;
          // Now check that all the indices are zero
          if (*std::max_element(multiIndices.begin(), multiIndices.end()) == 0)
            covarianceVariables += coefficientIJ * coefficientIJ / outputGroupSize;
        } // max_element > 0
      } // coefficientIJ != 0.0
    } // for j
  } // for i
  return (totalVariance > 0.0) ? covarianceVariables / totalVariance : 0.0;
}

Scalar FieldFunctionalChaosSobolIndices::getSobolTotalIndex(const UnsignedInteger variableIndex, const UnsignedInteger marginalIndex) const
{
  const Indices index(1, variableIndex);
  return getSobolTotalIndex(index, marginalIndex);
}

Scalar FieldFunctionalChaosSobolIndices::getSobolTotalIndex(const Indices & variableIndices, const UnsignedInteger marginalIndex) const
{
  // Compute total index from complementary indices
  const UnsignedInteger blockNumber = cumulatedInputSizes_.getSize() - 1;
  if (!variableIndices.check(blockNumber))
    throw InvalidArgumentException(HERE) << "The variable indices must be in the range [0, " << blockNumber - 1 << "] and must be different.";
  const Indices complementaryVariableIndices(variableIndices.complement(blockNumber));
  return 1.0 - getSobolIndex(complementaryVariableIndices, marginalIndex);
}

Point FieldFunctionalChaosSobolIndices::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = cumulatedInputSizes_.getSize() - 1;
  Point result(blockNumber);
  for (UnsignedInteger i = 0; i < blockNumber; ++ i)
    result[i] = getSobolIndex(i, marginalIndex);
  return result;
}

Point FieldFunctionalChaosSobolIndices::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = cumulatedInputSizes_.getSize() - 1;
  Point result(blockNumber);
  for (UnsignedInteger i = 0; i < blockNumber; ++ i)
    result[i] = getSobolTotalIndex(i, marginalIndex);
  return result;
}

Graph FieldFunctionalChaosSobolIndices::draw(const UnsignedInteger marginalIndex) const
{
  Description inputDescription;
  if (result_.getFieldToPointMetaModel().getInputDimension())
  {
    if (result_.getInputKLResultCollection().getSize() == result_.getFieldToPointMetaModel().getInputDimension())
      inputDescription = result_.getFieldToPointMetaModel().getInputDescription();
    else
      inputDescription = Description::BuildDefault(result_.getInputKLResultCollection().getSize(), "x");
  }
  else if (result_.getPointToFieldMetaModel().getInputDimension())
  {
    inputDescription = result_.getInputSample().getDescription();
  }
  const Point firstOrderIndices(getFirstOrderIndices(marginalIndex));
  const Point totalOrderIndices(getTotalOrderIndices(marginalIndex));
  const Graph graph(SobolIndicesAlgorithmImplementation::DrawSobolIndices(inputDescription, firstOrderIndices, totalOrderIndices));
  return graph;
}

/* Method save() stores the object through the StorageManager */
void FieldFunctionalChaosSobolIndices::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void FieldFunctionalChaosSobolIndices::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("result_", result_);
  setResult(result_);
}

END_NAMESPACE_OPENTURNS
