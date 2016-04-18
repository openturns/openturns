//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
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

#include "openturns/ChebychevAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/LaguerreFactory.hxx"
#include "openturns/HermiteFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ChebychevAlgorithm);

static const Factory<ChebychevAlgorithm> RegisteredFactory;

/* Default constructor */
ChebychevAlgorithm::ChebychevAlgorithm()
  : OrthonormalizationAlgorithmImplementation(Uniform())
  , referenceFamily_()
  , useCanonicalBasis_(true)
  , standardMoments_(0)
  , modifiedMoments_(0)
  , mixedMoments_()
  , monicRecurrenceCoefficients_(0)
  , referenceMonicRecurrenceCoefficients_(0)
{
  // The moment cache must be initialized before the coefficient cache
  standardMoments_.add(1.0);
  modifiedMoments_.add(1.0);
  mixedMoments_[0] = 1.0;
}


/* Parameter constructor */
ChebychevAlgorithm::ChebychevAlgorithm(const Distribution & measure)
  : OrthonormalizationAlgorithmImplementation(measure)
  , referenceFamily_(HermiteFactory())
  , useCanonicalBasis_(true)
  , standardMoments_(0)
  , modifiedMoments_(0)
  , mixedMoments_()
  , monicRecurrenceCoefficients_(0)
  , referenceMonicRecurrenceCoefficients_(0)
{
  // Selection of the best reference factory according to the range of the measure
  // There are 4 possibilities:
  // Range = [a, b]       -> standardized measure has range [-1, 1],      Legendre factory
  // Range = [a, +inf[    -> standardized measure has range [0, +inf[,    Laguerre factory
  // Range = ]-inf, b]    -> standardized measure has range ]-inf, 0],    no good factory, default to the Hermite factory
  // Range = ]-inf, +inf[ -> standardized measure has range ]-inf, +inf[, Hermite factory
  const Bool finiteLowerBound = measure.getRange().getFiniteLowerBound()[0];
  const Bool finiteUpperBound = measure.getRange().getFiniteUpperBound()[0];
  // Left-bounded measures
  if (finiteLowerBound)
  {
    // If the range is bounded
    if (finiteUpperBound)
      referenceFamily_ = LegendreFactory();
    // Else if it is right-unbounded
    else
      referenceFamily_ = LaguerreFactory();
  }
  // Left-unbounded measures
  else
  {
    // If the range is right-bounded
    if (finiteUpperBound)
      LOGWARN(OSS() << "Warning: there is no good reference factory for a measure with left infinite range. Using the Hermite factory, but expect numerical instability.");
  } // End selection of the reference factory
  // The moment cache must be initialized before the coefficient cache
  standardMoments_.add(1.0);
  modifiedMoments_.add(1.0);
  mixedMoments_[0] = 1.0;
}


/* Parameter constructor with specific reference family */
ChebychevAlgorithm::ChebychevAlgorithm(const Distribution & measure,
                                       const OrthogonalUniVariatePolynomialFamily & family)
  : OrthonormalizationAlgorithmImplementation(measure)
  , referenceFamily_(family)
  , useCanonicalBasis_(false)
  , standardMoments_(0)
  , modifiedMoments_(0)
  , mixedMoments_()
  , monicRecurrenceCoefficients_(0)
  , referenceMonicRecurrenceCoefficients_(0)
{
  // The moment cache must be initialized before the coefficient cache
  standardMoments_.add(1.0);
  modifiedMoments_.add(1.0);
  mixedMoments_[0] = 1.0;
}


/* Virtual constructor */
ChebychevAlgorithm * ChebychevAlgorithm::clone() const
{
  return new ChebychevAlgorithm(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
ChebychevAlgorithm::Coefficients ChebychevAlgorithm::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  Coefficients recurrenceCoefficients(3, 0.0);
  if (n == 0)
  {
    const NumericalScalar alpha0 = getMonicRecurrenceCoefficients(0)[0];
    const NumericalScalar beta1 = getMonicRecurrenceCoefficients(1)[1];
    const NumericalScalar factor = 1.0 / sqrt(beta1);
    recurrenceCoefficients[0] = factor;
    recurrenceCoefficients[1] = -alpha0 * factor;
    // Conventional value of 0.0 for recurrenceCoefficients[2]
    return recurrenceCoefficients;
  }
  // Compute the coefficients of the orthonormal polynomials involved in the relation
  const Coefficients alphaBetaN(getMonicRecurrenceCoefficients(n));
  const NumericalScalar alphaN = alphaBetaN[0];
  const NumericalScalar betaN = alphaBetaN[1];
  const NumericalScalar betaNP1 = getMonicRecurrenceCoefficients(n + 1)[1];
  const NumericalScalar factor = 1.0 / sqrt(betaNP1);
  recurrenceCoefficients[0] = factor;
  recurrenceCoefficients[1] = -alphaN * factor;
  recurrenceCoefficients[2] = -sqrt(betaN) * factor;
  return recurrenceCoefficients;
}

/* Return the order-th raw moment of the underlying measure */
NumericalScalar ChebychevAlgorithm::getStandardMoment(const UnsignedInteger order) const
{
  // We know that the raw moments will be accessed in a particular pattern: the moments not already
  // computed will always be accessed in a successive increasing order
  const UnsignedInteger maxOrder = standardMoments_.getSize();
  if (order > maxOrder) throw InvalidArgumentException(HERE) << "Error: cannot access to the raw moments in arbitrary order.";
  if (order == maxOrder)
    standardMoments_.add(measure_.getStandardMoment(order)[0]);
  return standardMoments_[order];
}

/* Return the order-th modified moment, i.e. the weighted integral of the order-th
   reference polynomial with respect to the underlying measure */
NumericalScalar ChebychevAlgorithm::getModifiedMoment(const UnsignedInteger order) const
{
  // We know that the modified moments will be accessed in a particular pattern: the moments not already
  // computed will always be accessed in a successive increasing order
  const UnsignedInteger maxOrder = modifiedMoments_.getSize();
  if (order > maxOrder) throw InvalidArgumentException(HERE) << "Error: cannot access to the modified moments in arbitrary order.";
  if (order == maxOrder)
  {
    // Quick return for trivial case
    if (order == 0)
    {
      modifiedMoments_.add(1.0);
      return 1.0;
    }
    // We compute the modified moment using the raw moments
    // If we use the canonical basis, the modified moment is exactly the raw moment
    if (useCanonicalBasis_)
    {
      modifiedMoments_.add(getStandardMoment(order));
      return modifiedMoments_[order];
    }
    // Else, build the modified moments as a combination of the standard moments
    // Coefficients of the order-th reference polynomial
    const Coefficients referenceCoefficients(referenceFamily_.build(order).getCoefficients());
    // Should deal with cancellation here...
    NumericalScalar modifiedMoment = referenceCoefficients[0] * getStandardMoment(0);
    // Use of Kahan Summation Formula for a stable evaluation of the modified moments
    NumericalScalar c = 0.0;
    for (UnsignedInteger i = 1; i <= order; ++i)
    {
      const NumericalScalar y = referenceCoefficients[i] * getStandardMoment(i) - c;
      const NumericalScalar t = modifiedMoment + y;
      c = (t - modifiedMoment) - y;
      modifiedMoment = t;
    }
    modifiedMoment += c;
    // Don't forget the fact that the reference polynomials are supposed to be monic, which is not the case of the polynomial associated with referenceCoefficients
    modifiedMoments_.add(modifiedMoment / referenceCoefficients[order]);
  }
  return modifiedMoments_[order];
}

/** Methods for the modified Chebichev algorithm */
/** Mixed moments E[Pj * Qk] where Pj is the j-th monic orthogonal polynomial
    for the given measure and Qk the k-th monic orthogonal polynomial of
    the reference factory */
NumericalScalar ChebychevAlgorithm::getMixedMoment(const int j,
    const UnsignedInteger k) const
{
  // Initialization values
  if (j == -1) return 0.0;
  if (j == 0) return getModifiedMoment(k);
  // Orthogonality
  if (j > static_cast<int>(k)) return 0.0;
  // General case
  const UnsignedInteger key = k + (j + k) * (j + k + 1) / 2;
  if (mixedMoments_.find(key) != mixedMoments_.end()) return mixedMoments_[key];
  const Coefficients alphaBeta(getMonicRecurrenceCoefficients(j - 1));
  const Coefficients aB(getReferenceMonicRecurrenceCoefficients(k));
  const NumericalScalar sigmaJK = getMixedMoment(j - 1, k + 1) - (alphaBeta[0] - aB[0]) * getMixedMoment(j - 1, k) - alphaBeta[1] * getMixedMoment(j - 2, k) + aB[1] * getMixedMoment(j - 1, k - 1);
  // Check for extrem numerical instability: E[Pn * Qn] <= 0 for j=k=n instead of E[Pn * Qn] = E[Pn^2] > 0
  if ((j == static_cast<int>(k)) && (sigmaJK <= 0.0)) throw InternalException(HERE) << "Error: numerical instability in the computation of the mixed moment (" << j << ", " << k << "), value=" << sigmaJK << ". Try to change the reference univariate polynomial family, the current one is " << referenceFamily_;
  mixedMoments_[key] = sigmaJK;
  return sigmaJK;
}

/** Recurrence coefficients (alphak, betak) of the monic orthogonal polynomials
    Pk+1(x) = (x - alphak) * Pk(x) - betak * Pk-1(x) */
ChebychevAlgorithm::Coefficients ChebychevAlgorithm::getMonicRecurrenceCoefficients(const UnsignedInteger k) const
{
  const UnsignedInteger size = monicRecurrenceCoefficients_.getSize();
  if (k < size) return monicRecurrenceCoefficients_[k];
  if (k > size) throw InvalidArgumentException(HERE) << "Error: cannot access to the monic recurrence coefficients in arbitrary order.";
  Coefficients alphaBeta(2, 0.0);
  // Initialization value
  if (k == 0)
  {
    const NumericalScalar a0 = getReferenceMonicRecurrenceCoefficients(0)[0];
    const NumericalScalar m0 = getModifiedMoment(0);
    const NumericalScalar m1 = getModifiedMoment(1);
    alphaBeta[0] = a0 + m1 / m0;
  }
  else
  {
    // General case
    const NumericalScalar sigmaKK = getMixedMoment(k, k);
    const NumericalScalar sigmaKM1KM1 = getMixedMoment(k - 1, k - 1);
    const NumericalScalar sigmaKM1K = getMixedMoment(k - 1, k);
    const NumericalScalar sigmaKKP1 = getMixedMoment(k, k + 1);
    const NumericalScalar aK = getReferenceMonicRecurrenceCoefficients(k)[0];
    alphaBeta[0] = aK + sigmaKKP1 / sigmaKK - sigmaKM1K / sigmaKM1KM1;
    alphaBeta[1] = sigmaKK / sigmaKM1KM1;
  }
  monicRecurrenceCoefficients_.add(alphaBeta);
  return alphaBeta;
}

/** Recurrence coefficients (ak, bk) of the monic reference polynomials
    Qk+1(x) = (x - ak) * Qk(x) - bk * Qk-1(x) */
ChebychevAlgorithm::Coefficients ChebychevAlgorithm::getReferenceMonicRecurrenceCoefficients(const UnsignedInteger k) const
{
  const UnsignedInteger size = referenceMonicRecurrenceCoefficients_.getSize();
  if (k < size) return referenceMonicRecurrenceCoefficients_[k];
  if (k > size) throw InvalidArgumentException(HERE) << "Error: cannot access to the reference monic recurrence coefficients in arbitrary order.";
  Coefficients aB(2, 0.0);
  if (!useCanonicalBasis_)
  {
    // Get the recurrence coefficients from the reference family and convert them to the associated monic family, else they are null
    const Coefficients normalizedCoefficients(referenceFamily_.getRecurrenceCoefficients(k));
    const NumericalScalar aK = normalizedCoefficients[0];
    const NumericalScalar bK = normalizedCoefficients[1];
    const NumericalScalar cK = normalizedCoefficients[2];

    aB[0] = -bK / aK;
    aB[1] = pow(cK / aK, 2);
  }
  referenceMonicRecurrenceCoefficients_.add(aB);
  return aB;
}


/* Reference univariate orthogonal polynomial family accessor */
void ChebychevAlgorithm::setReferenceFamily(const OrthogonalUniVariatePolynomialFamily & family)
{
  (*this) = ChebychevAlgorithm(measure_, family);
}


OrthogonalUniVariatePolynomialFamily ChebychevAlgorithm::getReferenceFamily() const
{
  return referenceFamily_;
}


/* String converter */
String ChebychevAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_
         << " reference family=" << referenceFamily_;
}


/* Method save() stores the object through the StorageManager */
void ChebychevAlgorithm::save(Advocate & adv) const
{
  OrthonormalizationAlgorithmImplementation::save(adv);
  adv.saveAttribute( "referenceFamily_", referenceFamily_ );
  adv.saveAttribute( "useCanonicalBasis_", useCanonicalBasis_ );
}


/* Method load() reloads the object from the StorageManager */
void ChebychevAlgorithm::load(Advocate & adv)
{
  OrthonormalizationAlgorithmImplementation::load(adv);
  adv.loadAttribute( "referenceFamily_", referenceFamily_ );
  // Initialize the object using the appropriate constructor
  *this = ChebychevAlgorithm(measure_, referenceFamily_);
  adv.loadAttribute( "useCanonicalBasis_", useCanonicalBasis_ );
}

END_NAMESPACE_OPENTURNS
