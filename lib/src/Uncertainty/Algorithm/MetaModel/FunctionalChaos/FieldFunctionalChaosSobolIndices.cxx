//                                               -*- C++ -*-
/**
 *  @brief Sensitivity analysis for field functional chaos
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
  , result_(result)
{
  // Nothing to do
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

Scalar FieldFunctionalChaosSobolIndices::getSobolIndex(const UnsignedInteger variableIndex, const UnsignedInteger marginalIndex) const
{
  const Indices index(1, variableIndex);
  return getSobolIndex(index, marginalIndex);
}

Scalar FieldFunctionalChaosSobolIndices::getSobolIndex(const Indices & variableIndices, const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = result_.getBlockIndices().getSize();
  if (!variableIndices.check(blockNumber))
    throw InvalidArgumentException(HERE) << "The variable indices must be in the range [0, " << blockNumber - 1 << "] and must be different.";
  if (marginalIndex >= result_.getOutputSample().getDimension())
    throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  if (!result_.getFCEResult().getOrthogonalBasis().getMeasure().hasIndependentCopula())
    throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!result_.getFCEResult().getDistribution().hasIndependentCopula())
    LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");

  // Here we have to sum all the contributions of the coefficients of the PCE
  // that contributes to any of the coefficients of the jth marginal of the output process.
  Scalar totalVariance = 0.0;
  Scalar covarianceVariables = 0.0;

  // build the list of indices of the KL coefficients matching variable indices
  Indices cumulatedInputSizes(blockNumber + 1);
  for (UnsignedInteger k = 0; k < blockNumber; ++ k)
    cumulatedInputSizes[k + 1] = cumulatedInputSizes[k] + result_.getInputKLResultCollection()[k].getEigenvalues().getDimension();
  Indices groupIndices;
  for (UnsignedInteger i = 0; i < variableIndices.getSize(); ++ i)
  {
    const UnsignedInteger variableIndex = variableIndices[i];
    const UnsignedInteger startInput = cumulatedInputSizes[variableIndex];
    const UnsignedInteger stopInput = cumulatedInputSizes[variableIndex + 1];
    for (UnsignedInteger j = startInput; j < stopInput; ++ j)
      groupIndices.add(j);
  }

  // Now, select the relevant coefficients
  const Sample coefficients(result_.getFCEResult().getCoefficients().getMarginal(marginalIndex));
  const UnsignedInteger size = coefficients.getSize();
  const EnumerateFunction enumerateFunction(result_.getFCEResult().getOrthogonalBasis().getEnumerateFunction());
  const Indices coefficientIndices(result_.getFCEResult().getIndices());
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Scalar coefficientI = coefficients(i, 0);
    // Only non-zero coefficients have to be taken into account
    if (coefficientI != 0.0)
    {
      // The only multi-indices we must take into account for
      // the conditional variance are those associated to
      // multi-indices that contain positive indices in the
      // correct input range and null indices outside of this range
      Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      // Take into account only nonzero multi indices
      if (*std::max_element(multiIndices.begin(), multiIndices.end()) > 0)
      {
        totalVariance += coefficientI * coefficientI;
        // Set the exponents corresponding to the group to zero
        for (UnsignedInteger j = 0; j < groupIndices.getSize(); ++j)
          multiIndices[groupIndices[j]] = 0;
        // Now check that all the indices are zero
        if (*std::max_element(multiIndices.begin(), multiIndices.end()) == 0)
          covarianceVariables += coefficientI * coefficientI;
      }
    }
  }
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
  const UnsignedInteger blockNumber = result_.getBlockIndices().getSize();
  if (!variableIndices.check(blockNumber))
    throw InvalidArgumentException(HERE) << "The variable indices must be in the range [0, " << blockNumber - 1 << "] and must be different.";
  const Indices complementaryVariableIndices(variableIndices.complement(blockNumber));
  return 1.0 - getSobolIndex(complementaryVariableIndices, marginalIndex);
}

Point FieldFunctionalChaosSobolIndices::getFirstOrderIndices(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = result_.getBlockIndices().getSize();
  Point result(blockNumber);
  for (UnsignedInteger i = 0; i < blockNumber; ++ i)
    result[i] = getSobolIndex(i, marginalIndex);
  return result;
}

Point FieldFunctionalChaosSobolIndices::getTotalOrderIndices(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = result_.getBlockIndices().getSize();
  Point result(blockNumber);
  for (UnsignedInteger i = 0; i < blockNumber; ++ i)
    result[i] = getSobolTotalIndex(i, marginalIndex);
  return result;
}

Graph FieldFunctionalChaosSobolIndices::draw(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger blockNumber = result_.getBlockIndices().getSize();
  const Description inputDescription(Description::BuildDefault(blockNumber, "x"));
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
}

END_NAMESPACE_OPENTURNS
