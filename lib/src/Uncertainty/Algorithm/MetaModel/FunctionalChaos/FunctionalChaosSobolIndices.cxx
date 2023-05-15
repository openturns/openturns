//                                               -*- C++ -*-
/**
 *  @brief Sensitivity analysis based on functional chaos expansion
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
#include <algorithm>
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosSobolIndices.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FunctionalChaosSobolIndices)

static const Factory<FunctionalChaosSobolIndices> Factory_FunctionalChaosSobolIndices;

/* Default constructor */
FunctionalChaosSobolIndices::FunctionalChaosSobolIndices()
  : PersistentObject()
{}

/* Default constructor */
FunctionalChaosSobolIndices::FunctionalChaosSobolIndices(const FunctionalChaosResult & functionalChaosResult)
  : PersistentObject()
  , functionalChaosResult_(functionalChaosResult)
{
  // Nothing to do
}


/* Virtual constructor */
FunctionalChaosSobolIndices * FunctionalChaosSobolIndices::clone() const
{
  return new FunctionalChaosSobolIndices(*this);
}

/* String converter */
String FunctionalChaosSobolIndices::__repr__() const
{
  OSS oss;
  oss << "class=" << FunctionalChaosSobolIndices::GetClassName()
      << " functional chaos result=" << functionalChaosResult_;
  return oss;
}


inline Bool varianceComparison(const std::pair<UnsignedInteger, Scalar> &a, const std::pair<UnsignedInteger, Scalar> &b)
{
  return a.second > b.second;
}

String FunctionalChaosSobolIndices::__str__(const String & /*offset*/) const
{
  return __repr_markdown__();
}

String FunctionalChaosSobolIndices::__repr_markdown__() const
{
  const Distribution inputDistribution = functionalChaosResult_.getDistribution();
  const UnsignedInteger inputDimension = inputDistribution.getDimension();
  const Description inputDescription = inputDistribution.getDescription();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  OSS oss(false);
  oss << FunctionalChaosSobolIndices::GetClassName() << Os::GetEndOfLine();

  const Indices indices(functionalChaosResult_.getIndices());
  const Sample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger basisSize = indices.getSize();
  EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());

  // compute the mean, the variance, the standard deviation
  const FunctionalChaosRandomVector fcRandomVector(functionalChaosResult_);
  const Point mean(fcRandomVector.getMean());
  const CovarianceMatrix covarianceMatrix(fcRandomVector.getCovariance());
  Point stdDev(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    stdDev[i] = std::sqrt(covarianceMatrix(i, i));

  // quick summary
  oss << "- input dimension=" << inputDimension << Os::GetEndOfLine()
      << "- output dimension=" << outputDimension << Os::GetEndOfLine()
      << "- basis size=" << functionalChaosResult_.getReducedBasis().getSize() << Os::GetEndOfLine()
      << "- mean=" << mean << Os::GetEndOfLine()
      << "- std-dev=" << stdDev << Os::GetEndOfLine();
  oss << Os::GetEndOfLine();
  String intermediateString;
  const Scalar varianceThreshold = ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold");
  const UnsignedInteger maximumNumberOfOutput = ResourceMap::GetAsUnsignedInteger("FunctionalChaosSobolIndices-MaximumNumberOfOutput");
  const UnsignedInteger columnWidth = ResourceMap::GetAsUnsignedInteger("FunctionalChaosSobolIndices-PrintColumnWidth");

  for (UnsignedInteger m = 0; m < outputDimension; ++ m)
  {
    if (m > maximumNumberOfOutput) break;
    if (outputDimension > 1)
      oss << "Marginal: " << m << Os::GetEndOfLine();

    const Point partOfVariance(getPartOfVariance(m));
    const Sample partOfVarianceSample(Sample::BuildFromPoint(partOfVariance));
    const Indices order(partOfVarianceSample.argsort(false));

    // table of part of variance for each basis term
    // Print header
    oss << "| Index |"
        << OSS::PadString(" Multi-index", columnWidth) << "|"
        << OSS::PadString(" Variance part", columnWidth) << "|"
        << Os::GetEndOfLine();
    // Print dashes
    oss << "|-------|";
    const String dashesSeparator(String(columnWidth, '-') + "|");
    oss << dashesSeparator << dashesSeparator << Os::GetEndOfLine();
    // Print table content
    for (UnsignedInteger i = 0; i < basisSize; ++ i)
    {
      const UnsignedInteger sortedIndex = order[i];
      const UnsignedInteger rankIndex = indices[sortedIndex];
      // stop when the part of variance becomes less than epsilon
      if (partOfVariance[sortedIndex] < varianceThreshold) break;
      Indices multiIndices(enumerateFunction(rankIndex));

      oss << "|" << std::setw(6) << sortedIndex << " |";

      intermediateString = OSS() << " " << multiIndices;
      oss << OSS::PadString(intermediateString, columnWidth) << "|";

      intermediateString = OSS() << " " << partOfVariance[sortedIndex];
      oss << OSS::PadString(intermediateString, columnWidth) << "|" << Os::GetEndOfLine();
    } // loop over the multi-indices
    oss << Os::GetEndOfLine();

    // table of first/total order indices for each input
    // print table header
    oss << "| Input |" << OSS::PadString(" Name", columnWidth) << "|";
    oss << OSS::PadString(" Sobol' index", columnWidth) << "|";
    oss << OSS::PadString(" Total index", columnWidth) << "|";
    oss << Os::GetEndOfLine();
    // print table dashes
    oss << "|-------|"
        << dashesSeparator << dashesSeparator << dashesSeparator << Os::GetEndOfLine();
    for (UnsignedInteger i = 0; i < inputDimension; ++ i)
    {
      oss << "|" << std::setw(6) << i << " |";

      intermediateString = OSS() << " " << inputDescription[i];
      oss << OSS::PadString(intermediateString, columnWidth) << "|";

      intermediateString = OSS() << " " << getSobolIndex(i, m);
      oss << OSS::PadString(intermediateString, columnWidth) << "|";

      intermediateString = OSS() << " " << getSobolTotalIndex(i, m);
      oss << OSS::PadString(intermediateString, columnWidth) << "|" << Os::GetEndOfLine();
    } // Loop over the input marginals
    oss << Os::GetEndOfLine();
  } // Loop over the output marginals
  return oss;
}

/* Sobol' index accessor */
Scalar FunctionalChaosSobolIndices::getSobolIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol' index must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  Bool mustInclude = false;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      const Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      const UnsignedInteger normInf = multiIndices.normInf();
      // Compute denominator
      if (normInf > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      if (normInf == 0)
      {
        // Exclude the zero indices
        mustInclude = false;
      }
      else
      {
        mustInclude = true;
        for (UnsignedInteger variableIndex = 0; variableIndex < inputDimension; ++variableIndex)
        {
          if (variablesGroup.contains(variableIndex))
          {
            // Check that any variable in the group has a nonzero degree
            if (multiIndices[variableIndex] == 0)
            {
              mustInclude = false;
              break;
            }
          }
          else
          {
            // Check that any variable not in the group has a zero degree
            if (multiIndices[variableIndex] > 0)
            {
              mustInclude = false;
              break;
            }
          }
        }
      }
      if (mustInclude) covarianceVariables += coefficientI * coefficientI;
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}

/* Sobol' index accessor */
Scalar FunctionalChaosSobolIndices::getSobolIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  const Indices index(1, variableIndex);
  return getSobolIndex(index, marginalIndex);
}

/* Sobol total index accessor */
Scalar FunctionalChaosSobolIndices::getSobolTotalIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol' index must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol total indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol total indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the total indices may be misleading.");
  const UnsignedInteger groupDimension = variablesGroup.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions to all the coefficients associated to a basis vector involving at least the variable i
  Scalar totalVariance = 0.0;
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      if (coefficientI != 0.0)
      {
        const Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        // Take into account only nonzero multi indices
        if (multiIndices.normInf() > 0)
        {
          totalVariance += coefficientI * coefficientI;
          Bool isProperSubset = true;
          // Check that the exponents associated to the selected variables are > 0
          for (UnsignedInteger j = 0; j < groupDimension; ++j)
          {
            const UnsignedInteger varJ = variablesGroup[j];
            isProperSubset = isProperSubset && (multiIndices[varJ] > 0);
          }
          if (isProperSubset) covarianceVariables += coefficientI * coefficientI;
        } // multiIndices.normInf() > 0
      } // if coefficientI <> 0
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}

/* Sobol total index accessor */
Scalar FunctionalChaosSobolIndices::getSobolTotalIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  const Indices index(1, variableIndex);
  return getSobolTotalIndex(index, marginalIndex);
}

/* Sobol grouped (first order) index accessor */
Scalar FunctionalChaosSobolIndices::getSobolGroupedIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol' index must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  Bool mustInclude = false;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      const Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      const UnsignedInteger normInf = multiIndices.normInf();
      // Compute denominator
      if (normInf > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      mustInclude = true;
      if (normInf == 0)
      {
        // Exclude the zero indices
        mustInclude = false;
      }
      else
      {
        for (UnsignedInteger variableIndex = 0; variableIndex < inputDimension; ++variableIndex)
        {
          // Check that each variable having a nonzero degree is in the group
          if (multiIndices[variableIndex] > 0 && ! variablesGroup.contains(variableIndex))
          {
            mustInclude = false;
            break;
          }
        }
      }
      if (mustInclude) covarianceVariables += coefficientI * coefficientI;
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}

/* Sobol grouped total index accessor */
Scalar FunctionalChaosSobolIndices::getSobolGroupedTotalIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol' index must be in the range [0, dim-1] and must be different.";
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  const UnsignedInteger groupDimension = variablesGroup.getSize();
  Bool mustInclude = false;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      const Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      const UnsignedInteger normInf = multiIndices.normInf();
      // Compute denominator
      if (normInf > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      if (normInf == 0)
      {
        // Exclude the zero indices
        mustInclude = false;
      }
      else
      {
        mustInclude = false;
        for (UnsignedInteger j = 0; j < groupDimension; ++j)
        {
          const UnsignedInteger variableIndex = variablesGroup[j];
          // Check if any variable in the group has a nonzero degree
          if (multiIndices[variableIndex] > 0)
          {
            mustInclude = true;
            break;
          }
        }
      }
      if (mustInclude) covarianceVariables += coefficientI * coefficientI;
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}
/* Functional chaos result accessor */
FunctionalChaosResult FunctionalChaosSobolIndices::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}

/** Part of variance accessor */
Point FunctionalChaosSobolIndices::getPartOfVariance(const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const Indices indices(functionalChaosResult_.getIndices());
  const Sample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger basisSize = indices.getSize();
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());

  // compute the variance
  const FunctionalChaosRandomVector fcRandomVector(functionalChaosResult_);
  const CovarianceMatrix covarianceMatrix = fcRandomVector.getCovariance();
  const Scalar variance = covarianceMatrix(marginalIndex, marginalIndex);
  Point partOfVariance(basisSize);
  // compute part of contribution of each multi-index
  for (UnsignedInteger i = 0; i < basisSize; ++ i)
  {
    const Scalar coefI = coefficients(i, marginalIndex);
    const UnsignedInteger rankIndex = indices[i];
    const Indices multiIndices(enumerateFunction(rankIndex));
    UnsignedInteger totalDegree = 0;
    for (UnsignedInteger j = 0; j < inputDimension; ++j) totalDegree += multiIndices[j];
    if (totalDegree > 0) partOfVariance[i] = coefI * coefI / variance;
  } // Loop over the multi-indices
  return partOfVariance;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosSobolIndices::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosSobolIndices::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "functionalChaosResult_", functionalChaosResult_ );
}

END_NAMESPACE_OPENTURNS
