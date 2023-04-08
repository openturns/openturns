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
#include "openturns/EnumerateFunction.hxx"
#include "openturns/SpecFunc.hxx"

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


inline bool varianceComparison(const std::pair<UnsignedInteger, Scalar> &a, const std::pair<UnsignedInteger, Scalar> &b)
{
  return a.second > b.second;
}

String FunctionalChaosSobolIndices::__str__(const String & /*offset*/) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  OSS oss;

  const Indices indices(functionalChaosResult_.getIndices());
  const Sample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger basisSize = indices.getSize();

  // compute the variance
  Point variance(outputDimension);
  Point mean(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    for (UnsignedInteger k = 0; k < basisSize; ++ k)
      // Take into account only non-zero indices as the null index is the mean of the vector
      if (indices[k] > 0)
        variance[i] += coefficients(k, i) * coefficients(k, i);
      else
        mean[i] = coefficients(k, i);
  }

  // standard deviation
  Point stdDev(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    stdDev[i] = std::sqrt(variance[i]);

  // quick summary
  oss << " input dimension: " << inputDimension << "\n"
      << " output dimension: " << outputDimension << "\n"
      << " basis size: " << functionalChaosResult_.getReducedBasis().getSize() << "\n"
      << " mean: " << mean.__str__() << "\n"
      << " std-dev: " << stdDev.__str__() << "\n";
  oss << String(60, '-') << "\n";
  String st;

  EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());

  for (UnsignedInteger m = 0; m < outputDimension; ++ m)
  {
    UnsignedInteger maxdegree = 0;

    std::vector< std::pair<UnsignedInteger, Scalar > > varianceOrder;

    // compute part of contribution of each basis term
    for (UnsignedInteger i = 1; i < basisSize; ++ i)
    {
      Scalar coefI = coefficients(i, m);
      Indices multiIndices(enumerateFunction(indices[i]));
      UnsignedInteger degreeI = 0;
      for (UnsignedInteger k = 0; k < multiIndices.getSize(); ++ k) degreeI += multiIndices[k];

      maxdegree = std::max(maxdegree, degreeI);
      const Scalar varianceRatio = coefI * coefI / variance[m];
      varianceOrder.push_back(std::pair<UnsignedInteger, Scalar >(i, varianceRatio));
    }

    // sort basis terms by descending variance contribution
    std::sort(varianceOrder.begin(), varianceOrder.end(), varianceComparison);

    if (outputDimension > 1)
      oss << "Marginal: " << m << "\n";

    // table of part of variance for each basis term
    oss << "Index   | Multi-indice                  | Part of variance  \n";
    oss << String(60, '-') << "\n";
    for (UnsignedInteger i = 0; i < varianceOrder.size(); ++ i)
    {
      // stop when the variance contribution becomes less than epsilon
      if (varianceOrder[i].second < ResourceMap::GetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold"))
        break;

      Indices multiIndices(enumerateFunction(indices[varianceOrder[i].first]));

      st = OSS() << std::setw(7) << varianceOrder[i].first;
      oss << st << " | ";

      st = OSS() << multiIndices;
      oss << st << String(st.size() < 29 ? 29 - st.size() : 0, ' ') << " | ";

      st = OSS() << varianceOrder[i].second;
      oss << st << "\n";
    }
    oss << String(60, '-') << "\n";
    oss << "\n\n";

    // table of first/total order indices for each input
    oss << String(60, '-') << "\n";
    oss << "Component | Sobol index            | Sobol total index      \n";
    oss << String(60, '-') << "\n";
    for (UnsignedInteger i = 0; i < inputDimension; ++ i)
    {
      st = OSS() << std::setw(9) << i;
      oss << st << " | ";

      st = OSS() << getSobolIndex(i, m);
      oss << st << String(22 - st.size(), ' ') << " | ";

      st = OSS() << getSobolTotalIndex(i, m);
      oss << st << String(22 - st.size(), ' ') << "\n";
    }
    oss << String(60, '-') << "\n";
    oss << "\n";
  }
  return oss;
}

/* Sobol index accessor */
Scalar FunctionalChaosSobolIndices::getSobolIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variablesGroup.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      if (coefficientI != 0.0)
      {
        Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        // Take into account only nonzero multi indices
        if (*std::max_element(multiIndices.begin(), multiIndices.end()) > 0)
        {
          totalVariance += coefficientI * coefficientI;
          Bool isProperSubset = true;
          // First check that the exponents associated to the selected variables are > 0
          for (UnsignedInteger j = 0; j < orderSobolIndice; ++j)
          {
            const UnsignedInteger varJ = variablesGroup[j];
            isProperSubset = isProperSubset && (multiIndices[varJ] > 0);
            // We must set the value of the current variable index to 0 for the next test
            multiIndices[varJ] = 0;
          }
          // At this step, the current index could be associated to a function that does not involve all the variables
          if (isProperSubset)
          {
            // Second, check that the other coefficients are 0
            if (*std::max_element(multiIndices.begin(), multiIndices.end()) == 0)
              covarianceVariables += coefficientI * coefficientI;
          }
        } // *std::max_element(multiIndices.begin(), multiIndices.end())s > 0
      } // if coefficientI <> 0
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}

/* Sobol index accessor */
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
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol total indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol total indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the total indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variablesGroup.getSize();
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
        if (*std::max_element(multiIndices.begin(), multiIndices.end()) > 0)
        {
          totalVariance += coefficientI * coefficientI;
          Bool isProperSubset = true;
          // Check that the exponents associated to the selected variables are > 0
          for (UnsignedInteger j = 0; j < orderSobolIndice; ++j)
          {
            const UnsignedInteger varJ = variablesGroup[j];
            isProperSubset = isProperSubset && (multiIndices[varJ] > 0);
          }
          if (isProperSubset) covarianceVariables += coefficientI * coefficientI;
        } // *std::max_element(multiIndices.begin(), multiIndices.end()) > 0
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
  Log::Show(Log::ALL);
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  const UnsignedInteger outputDimension = functionalChaosResult_.getMetaModel().getOutputDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= outputDimension) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variablesGroup.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  bool mustInclude;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      // Compute denominator
      if (multiIndices.normInfinite() > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      mustInclude = true;
      if (multiIndices.normInfinite() == 0)
      {
        // Exclude the zero indices
        mustInclude = false;
      }
      else
      {
        for (UnsignedInteger variableIndex = 0; variableIndex < inputDimension; ++variableIndex)
        {
          // Check that each variable having a nonzero degree is in the group
          if (multiIndices[variableIndex] > 0 and not variablesGroup.contains(variableIndex)) 
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
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  const UnsignedInteger orderSobolIndice = variablesGroup.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  const UnsignedInteger groupDimension = variablesGroup.getSize();
  bool mustInclude;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      // Compute denominator
      if (multiIndices.normInfinite() > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      if (multiIndices.normInfinite() == 0)
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

/* Sobol grouped interaction index accessor */
Scalar FunctionalChaosSobolIndices::getSobolGroupedInteractionIndex(const Indices & variablesGroup,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = functionalChaosResult_.getDistribution().getDimension();
  if (!variablesGroup.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  const UnsignedInteger orderSobolIndice = variablesGroup.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  Scalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  Scalar totalVariance = 0.0;
  const UnsignedInteger groupDimension = variablesGroup.getSize();
  bool mustInclude;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const Scalar coefficientI = coefficients(i, 0);
      Indices multiIndices(enumerateFunction(coefficientIndices[i]));
      // Compute denominator
      if (multiIndices.normInfinite() > 0) totalVariance += coefficientI * coefficientI;
      // Compute numerator
      if (multiIndices.normInfinite() == 0)
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

/* Functional chaos result accessor */
FunctionalChaosResult FunctionalChaosSobolIndices::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
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
