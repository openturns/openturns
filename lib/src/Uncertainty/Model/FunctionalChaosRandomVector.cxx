//                                               -*- C++ -*-
/**
 *  @brief An implementation class for functionalChaos random vectors
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/FunctionalChaosRandomVector.hxx"
#include "openturns/UsualRandomVector.hxx"
#include "openturns/EnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FunctionalChaosRandomVector);

static const Factory<FunctionalChaosRandomVector> RegisteredFactory;

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
NumericalPoint FunctionalChaosRandomVector::getMean() const
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
  const NumericalSample coefficients(functionalChaosResult_.getCoefficients());
  const UnsignedInteger size = indices.getSize();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      covariance_(i, j) = 0.0;
      // The loop starts at 1 as indices[0] is supposed to be associated with the mean (constant basis function)
      for (UnsignedInteger k = 1; k < size; ++k) covariance_(i, j) += coefficients[k][i] * coefficients[k][j];
    } // Loop over the second index
  } // Loop over the first index
  isAlreadyComputedCovariance_ = true;
}

/* Sobol index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolIndex(const Indices & variableIndices,
    const UnsignedInteger marginalIndex) const
{
  const UnsignedInteger inputDimension = getAntecedent()->getDimension();
  if (!variableIndices.check(inputDimension - 1)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and  must be different.";
  if (marginalIndex >= getDimension()) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  const UnsignedInteger orderSobolIndice = variableIndices.getSize();
  const NumericalSample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
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
        totalVariance += coefficientI * coefficientI;
        Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        Bool isProperSubset = true;
        // First check that the exponents associated with the selected variables are > 0
        for (UnsignedInteger j = 0; j < orderSobolIndice; ++j)
        {
          const UnsignedInteger varJ = variableIndices[j];
          isProperSubset = isProperSubset && (multiIndices[varJ] > 0);
          // We must set the value of the current variable index to 0 for the next test
          multiIndices[varJ] = 0;
        }
        // At this step, the current index could be associated with a function that does not involve all the variables
        if (isProperSubset)
        {
          // Second, check that the other coefficients are 0
          for (UnsignedInteger j = 0; j < inputDimension; ++j) isProperSubset = isProperSubset && (multiIndices[j] == 0);
          // At this step, the current index could be associated with a function that involves more variables than needed
          if (isProperSubset) covarianceVariables += coefficientI * coefficientI;
        }
      } // if coefficientI <> 0
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else throw NotDefinedException(HERE) << "Error: the total variance of output=" << marginalIndex << " is zero, the Sobol index is undefined.";
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
  if (!variableIndices.check(inputDimension - 1)) throw InvalidArgumentException(HERE) << "The variable indices of a Sobol indice must be in the range [0, dim-1] and  must be different.";
  if (marginalIndex >= getDimension()) throw InvalidArgumentException(HERE) << "The marginal index must be in the range [0, dim-1].";
  const UnsignedInteger orderSobolIndice = variableIndices.getSize();
  const NumericalSample coefficients(functionalChaosResult_.getCoefficients().getMarginal(marginalIndex));
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
        totalVariance += coefficientI * coefficientI;
        const Indices multiIndices(enumerateFunction(coefficientIndices[i]));
        Bool isProperSubset = true;
        // Check that the exponents associated with the selected variables are > 0
        for (UnsignedInteger j = 0; j < orderSobolIndice; ++j)
        {
          const UnsignedInteger varJ = variableIndices[j];
          isProperSubset = isProperSubset && (multiIndices[varJ] > 0);
        }
        if (isProperSubset) covarianceVariables += coefficientI * coefficientI;
      } // if coefficientI <> 0
    } // coefficientIndices[i] > 0
  } // Loop over the coefficients
  if (totalVariance > 0.0) return covarianceVariables / totalVariance;
  else throw NotDefinedException(HERE) << "Error: the total variance of output=" << marginalIndex << " is zero, the Sobol total index is undefined.";
}

/* Sobol total index accessor */
NumericalScalar FunctionalChaosRandomVector::getSobolTotalIndex(const UnsignedInteger variableIndex,
    const UnsignedInteger marginalIndex) const
{
  Indices index(1);
  index[0] = variableIndex;
  return getSobolTotalIndex(index, marginalIndex);
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
