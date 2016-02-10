//                                               -*- C++ -*-
/**
 *  @brief Implement the modified adaptive Stieltjes algorithm to compute
 *         the coefficients of the 3 terms recurrence relation of an
 *         orthonormal polynomial family
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

#include "AdaptiveStieltjesAlgorithm.hxx"
#include "PersistentObjectFactory.hxx"
#include "ResourceMap.hxx"
#include "Exception.hxx"
#include "Uniform.hxx"
#include "GaussKronrod.hxx"
#include "OrthogonalUniVariatePolynomial.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AdaptiveStieltjesAlgorithm);

static const Factory<AdaptiveStieltjesAlgorithm> RegisteredFactory;

/* Default constructor */
AdaptiveStieltjesAlgorithm::AdaptiveStieltjesAlgorithm()
  : OrthonormalizationAlgorithmImplementation(Uniform())
  , recurrenceCoefficients_(0)
  , isElliptical_(true)
{
  // Nothing to do
}


/* Parameter constructor */
AdaptiveStieltjesAlgorithm::AdaptiveStieltjesAlgorithm(const Distribution & measure)
  : OrthonormalizationAlgorithmImplementation(measure)
  , recurrenceCoefficients_(0)
  , isElliptical_(measure.isElliptical())
{
  // Nothing to do
}


/* Virtual constructor */
AdaptiveStieltjesAlgorithm * AdaptiveStieltjesAlgorithm::clone() const
{
  return new AdaptiveStieltjesAlgorithm(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x)
   We have:
   a0n = 1/sqrt(\beta_{n+1})
   a1n = -\alpha_n/sqrt(\beta_{n+1})
   a2n = -\sqrt{\beta_n/\beta_{n+1}}
   \alpha_n = <xPn,Pn>
   \beta_{n+1} = <xPn,xPn>+\beta_n-2\alpha_n^2
               = <xPn,xPn>+1/a0_{n-1}^2 - 2<xPn,Pn>^2
   a0n = a0_{n-1}/sqrt(1+a0_{n-1}^2(<xPn,xPn> - 2<xPn,Pn>^2))
   a1n = -<xPn,Pn>a0n
   a2n = -a0n / a0_{n-1}
 */
AdaptiveStieltjesAlgorithm::Coefficients AdaptiveStieltjesAlgorithm::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  // Get the coefficients from the cache if possible
  if (n < recurrenceCoefficients_.getSize()) return recurrenceCoefficients_[n];
  while (n > recurrenceCoefficients_.getSize())
    {
      NumericalPoint coeffs(getRecurrenceCoefficients(n - 1));
    }
  Coefficients currentCoefficients(3, 0.0);
  if (n == 0)
  {
    const NumericalScalar sigma(std::sqrt(measure_.getCovariance()(0, 0)));
    currentCoefficients[0] = 1.0 / sigma;
    const NumericalScalar mu(measure_.getMean()[0]);
    // To avoid -0.0 in print
    if (std::abs(mu) > ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultQuantileEpsilon")) currentCoefficients[1] = -mu / sigma;
    // Conventional value of 0.0 for currentCoefficients[2]
    recurrenceCoefficients_.add(currentCoefficients);
    return currentCoefficients;
  }
  // Compute the coefficients of the orthonormal polynomials involved in the relation

  const OrthogonalUniVariatePolynomial pN(recurrenceCoefficients_);
  const NumericalScalar a0Prev(recurrenceCoefficients_[n - 1][0]);
  const NumericalScalar betaN(1.0 / (a0Prev * a0Prev));
  NumericalScalar error(0.0);
  GaussKronrod algo;
  const DotProductWrapper dotProductWrapper(pN, measure_);
  if (isElliptical_)
    {
      const NumericalMathFunction dotProductKernel(bindMethod<DotProductWrapper, NumericalPoint, NumericalPoint>(dotProductWrapper, &DotProductWrapper::kernelSym, 1, 1));
      const NumericalPoint dotProduct(algo.integrate(dotProductKernel, measure_.getRange(), error));
      const NumericalScalar betaNP1(dotProduct[0] - betaN);
      currentCoefficients[0] = 1.0 / std::sqrt(betaNP1);
      currentCoefficients[2] = -currentCoefficients[0] / a0Prev;
    }
  else
    {
      const NumericalMathFunction dotProductKernel(bindMethod<DotProductWrapper, NumericalPoint, NumericalPoint>(dotProductWrapper, &DotProductWrapper::kernelGen, 1, 2));
      const NumericalPoint dotProduct(algo.integrate(dotProductKernel, measure_.getRange(), error));
      const NumericalScalar alphaN(dotProduct[1]);
      const NumericalScalar betaNP1(dotProduct[0] - alphaN * alphaN - betaN);
      currentCoefficients[0] = 1.0 / std::sqrt(betaNP1);
      currentCoefficients[1] = -alphaN * currentCoefficients[0];
      currentCoefficients[2] = -currentCoefficients[0] / a0Prev;
    }
  recurrenceCoefficients_.add(currentCoefficients);
  return currentCoefficients;
}

/* String converter */
String AdaptiveStieltjesAlgorithm::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void AdaptiveStieltjesAlgorithm::save(Advocate & adv) const
{
  OrthonormalizationAlgorithmImplementation::save(adv);
  adv.saveAttribute( "recurrenceCoefficients_", recurrenceCoefficients_ );
  adv.saveAttribute( "isElliptical_", isElliptical_ );
}


/* Method load() reloads the object from the StorageManager */
void AdaptiveStieltjesAlgorithm::load(Advocate & adv)
{
  OrthonormalizationAlgorithmImplementation::load(adv);
  adv.loadAttribute( "recurrenceCoefficients_", recurrenceCoefficients_ );
  adv.loadAttribute( "isElliptical_", isElliptical_ );
}

END_NAMESPACE_OPENTURNS
