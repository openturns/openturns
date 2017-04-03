//                                               -*- C++ -*-
/**
 *  @brief An implementation class for functionalChaos random vectors
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <algorithm>
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/EnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FunctionalChaosRandomVector);

static const Factory<FunctionalChaosRandomVector> Factory_FunctionalChaosRandomVector;

/* Default constructor */
FunctionalChaosRandomVector::FunctionalChaosRandomVector(const FunctionalChaosResult & functionalChaosResult)
  : CompositeRandomVector(functionalChaosResult.getComposedMetaModel(), UsualRandomVector(functionalChaosResult.getOrthogonalBasis().getMeasure()))
  , covariance_(0)
  , isAlreadyComputedCovariance_(false)
  , functionalChaosResult_(functionalChaosResult)
{
  // Nothing to do
}


/* Virtual constructor */
FunctionalChaosRandomVector * FunctionalChaosRandomVector::clone() const
{
  return new FunctionalChaosRandomVector(*this);
}

/* String converter */
String FunctionalChaosRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << FunctionalChaosRandomVector::GetClassName()
      << " functional chaos result=" << functionalChaosResult_;
  return oss;
}


/* Mean accessor */
Point FunctionalChaosRandomVector::getMean() const
{
  return functionalChaosResult_.getCoefficients()[0];
}

/* Covariance accessor */
CovarianceMatrix FunctionalChaosRandomVector::getCovariance() const
{
  if (!isAlreadyComputedCovariance_) computeCovariance();
  return covariance_;
}

/* Compute the covariance */
void FunctionalChaosRandomVector::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  const Indices indices(functionalChaosResult_.getIndices());
  const Sample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger size = indices.getSize();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      covariance_(i, j) = 0.0;
      for (UnsignedInteger k = 0; k < size; ++k)
        // Take into account only non-zero indices as the null index is the mean of the vector
        if (indices[k] > 0) covariance_(i, j) += coefficients[k][i] * coefficients[k][j];
    } // Loop over the second index
  } // Loop over the first index
  isAlreadyComputedCovariance_ = true;
}

/* Sobol index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolIndex(const Indices & variableIndices,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = getAntecedent()->getDimension();
  if (!variableIndices.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= getDimension()) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variableIndices.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  NumericalScalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  NumericalScalar totalVariance = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const NumericalScalar coefficientI = coefficients[i][0];
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
            const UnsignedInteger varJ = variableIndices[j];
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
NumericalScalar FunctionalChaosRandomVector::getSobolIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  Indices index(1);
  index[0] = variableIndex;
  return getSobolIndex(index, marginalIndex);
}

/* Sobol total index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolTotalIndex(const Indices & variableIndices,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = getAntecedent()->getDimension();
  if (!variableIndices.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= getDimension()) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol total indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol total indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the total indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variableIndices.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  NumericalScalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions to all the coefficients associated to a basis vector involving at least the variable i
  NumericalScalar totalVariance = 0.0;
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const NumericalScalar coefficientI = coefficients[i][0];
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
            const UnsignedInteger varJ = variableIndices[j];
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
NumericalScalar FunctionalChaosRandomVector::getSobolTotalIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  Indices index(1);
  index[0] = variableIndex;
  return getSobolTotalIndex(index, marginalIndex);
}

/* Sobol grouped index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolGroupedIndex(const Indices & variableIndices,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = getAntecedent()->getDimension();
  if (!variableIndices.check(inputDimension)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and must be different.";
  if (marginalIndex >= getDimension()) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  // Check if the measure defining the basis has an independent copula else
  // the conditional covariance cannot be extracted from the decomposition
  if (!functionalChaosResult_.getOrthogonalBasis().getMeasure().hasIndependentCopula()) throw InternalException(HERE) << "Error: cannot compute Sobol indices from a non-tensorized basis.";
  if (!functionalChaosResult_.getDistribution().hasIndependentCopula()) LOGWARN(OSS(false) << "The Sobol indices are computed wrt the basis measure, and there is no one-to-one transformation between this measure and the input distribution. The interpretation of the indices may be misleading.");
  const UnsignedInteger orderSobolIndice = variableIndices.getSize();
  const Sample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
  const Indices coefficientIndices(functionalChaosResult_.getIndices());
  const UnsignedInteger size = coefficients.getSize();
  NumericalScalar covarianceVariables = 0.0;
  const EnumerateFunction enumerateFunction(functionalChaosResult_.getOrthogonalBasis().getEnumerateFunction());
  // Sum the contributions of all the coefficients associated to a basis vector involving only the needed variables
  NumericalScalar totalVariance = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coefficientIndices[i] > 0)
    {
      const NumericalScalar coefficientI = coefficients[i][0];
      if (coefficientI != 0.0)
      {
        Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        // Take into account only nonzero multi indices
        if (*std::max_element(multiIndices.begin(), multiIndices.end()) > 0)
        {
          totalVariance += coefficientI * coefficientI;
          // Set the exponents corresponding to the group to zero
          for (UnsignedInteger j = 0; j < orderSobolIndice; ++j)
            multiIndices[variableIndices[j]] = 0;
          // Now check that all the indices are zero
          if (*std::max_element(multiIndices.begin(), multiIndices.end()) == 0)
            covarianceVariables += coefficientI * coefficientI;
        } // *std::max_element(multiIndices.begin(), multiIndices.end()) > 0
      } // if coefficientI <> 0
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else return 0.0;
}

/* Sobol index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolGroupedIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  Indices index(1);
  index[0] = variableIndex;
  return getSobolGroupedIndex(index, marginalIndex);
}

/* Functional chaos result accessor */
FunctionalChaosResult FunctionalChaosRandomVector::getFunctionalChaosResult() const
{
  return functionalChaosResult_;
}

/* Method save() stores the object through the StorageManager */
void FunctionalChaosRandomVector::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "functionalChaosResult_", functionalChaosResult_ );
  adv.saveAttribute( "covariance_", covariance_ );
  adv.saveAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionalChaosRandomVector::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "functionalChaosResult_", functionalChaosResult_ );
  adv.loadAttribute( "covariance_", covariance_ );
  adv.loadAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
}

END_NAMESPACE_OPENTURNS
