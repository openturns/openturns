//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
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

#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
#include "openturns/CharlierFactory.hxx"
#include "openturns/ChebychevFactory.hxx"
#include "openturns/HermiteFactory.hxx"
#include "openturns/JacobiFactory.hxx"
#include "openturns/KrawtchoukFactory.hxx"
#include "openturns/LaguerreFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/HistogramPolynomialFactory.hxx"
#include "openturns/MeixnerFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StandardDistributionPolynomialFactory)

static const Factory<StandardDistributionPolynomialFactory> Factory_StandardDistributionPolynomialFactory;


/* Default constructor */
StandardDistributionPolynomialFactory::StandardDistributionPolynomialFactory()
  : OrthogonalUniVariatePolynomialFactory(Uniform())
  , orthonormalizationAlgorithm_(AdaptiveStieltjesAlgorithm(Uniform()))
  , specificFamily_()
  , hasSpecificFamily_(false)
{
  // Initialize the coefficient cache
  initializeCache();
}


/* Parameter constructor */
StandardDistributionPolynomialFactory::StandardDistributionPolynomialFactory(const Distribution & measure)
  : OrthogonalUniVariatePolynomialFactory(measure.getStandardRepresentative())
  , orthonormalizationAlgorithm_(AdaptiveStieltjesAlgorithm(measure.getStandardRepresentative()))
  , specificFamily_()
  , hasSpecificFamily_(false)
{
  checkSpecificFamily();
  initializeCache();
}


/* Parameter constructor */
StandardDistributionPolynomialFactory::StandardDistributionPolynomialFactory(const OrthonormalizationAlgorithm & orthonormalizationAlgorithm)
  : OrthogonalUniVariatePolynomialFactory(orthonormalizationAlgorithm.getMeasure())
  , orthonormalizationAlgorithm_(orthonormalizationAlgorithm)
  , specificFamily_()
  , hasSpecificFamily_(false)
{
  checkSpecificFamily();
  initializeCache();
}


/* Virtual constructor */
StandardDistributionPolynomialFactory * StandardDistributionPolynomialFactory::clone() const
{
  return new StandardDistributionPolynomialFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
StandardDistributionPolynomialFactory::Coefficients StandardDistributionPolynomialFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  if (hasSpecificFamily_) return specificFamily_.getRecurrenceCoefficients(n);
  else return orthonormalizationAlgorithm_.getRecurrenceCoefficients(n);
}

/* Check the existence of a specific family more efficient for the given measure */
void StandardDistributionPolynomialFactory::checkSpecificFamily()
{
  // Check for special cases. Need a more elegant conception and implementation.
  hasSpecificFamily_ = false;
  OrthogonalUniVariatePolynomialFamily referenceFamily;
  const String measureType(measure_.getImplementation()->getClassName());
  Bool hasClassMatch = false;
  // Legendre factory
  if (measureType == "Uniform")
  {
    referenceFamily = LegendreFactory();
    hasClassMatch = true;
  }
  // Hermite factory
  if (measureType == "Normal")
  {
    referenceFamily = HermiteFactory();
    hasClassMatch = true;
  }
  // HistogramPolynomial factory
  if (measureType == "Histogram")
  {
    const Point parameter(measure_.getParameter());
    const UnsignedInteger size = (parameter.getSize() - 1) / 2;
    const Scalar first = parameter[0];
    Point width(size);
    Point height(size);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      width[i] = parameter[2 * i + 1];
      height[i] = parameter[2 * i + 2];
    }

    referenceFamily = HistogramPolynomialFactory(first, width, height);
    hasClassMatch = true;
  }
  // Chebychev factory
  if (measureType == "Arcsine")
  {
    referenceFamily = ChebychevFactory();
    hasClassMatch = true;
  }
  // Jacobi (or Chebychev as a special case) factory
  if (measureType == "Beta")
  {
    const Point parameter(measure_.getParameter());
    const Scalar alpha = parameter[1] - 1.0;
    const Scalar beta = parameter[0] - 1.0;
    // Here we set directly the specific family as the reference distribution
    // of the family has a different type (Arcsine) than the given distribution
    if (alpha == -0.5 && beta == -0.5 && parameter[2] == -1.0 && parameter[3] == 1.0)
    {
      specificFamily_ = ChebychevFactory();
      hasSpecificFamily_ = true;
      // To avoid distribution comparison at the end of the method
      hasClassMatch = false;
    }
    // Here we set directly the specific family as the reference distribution
    // of the family has a different type (Uniform) than the given distribution
    else if (alpha == 0.0 && beta == 0.0 && parameter[2] == -1.0 && parameter[3] == 1.0)
    {
      specificFamily_ = LegendreFactory();
      hasSpecificFamily_ = true;
      // To avoid distribution comparison at the end of the method
      hasClassMatch = false;
    }
    else
    {
      referenceFamily = JacobiFactory(alpha, beta);
      hasClassMatch = true;
    }
  }
  // Laguerre factory
  if (measureType == "Gamma")
  {
    const Point parameter(measure_.getParameter());
    referenceFamily = LaguerreFactory(parameter[0] - 1.0);
    hasClassMatch = true;
  }
  if (measureType == "Exponential")
  {
    const Point parameter(measure_.getParameter());
    if (parameter[0] == 1.0)
    {
      specificFamily_ = LaguerreFactory(0.0);
      hasSpecificFamily_ = true;
      // To avoid distribution comparison at the end of the method
      hasClassMatch = false;
    }
  }
  // Charlier factory
  if (measureType == "Poisson")
  {
    const Point parameter(measure_.getParameter());
    referenceFamily = CharlierFactory(parameter[0]);
    hasClassMatch = true;
  }
  // Krawtchouk factory
  if (measureType == "Binomial")
  {
    const Point parameter(measure_.getParameter());
    referenceFamily = KrawtchoukFactory(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
    hasClassMatch = true;
  }
  if (measureType == "Bernoulli")
  {
    const Point parameter(measure_.getParameter());
    referenceFamily = KrawtchoukFactory(1, parameter[0]);
    hasClassMatch = true;
  }
  // Meixner factory
  if (measureType == "NegativeBinomial")
  {
    const Point parameter(measure_.getParameter());
    referenceFamily = MeixnerFactory(parameter[0], parameter[1]);
    hasClassMatch = true;
  }
  if (hasClassMatch && (referenceFamily.getMeasure() == measure_))
  {
    specificFamily_ = referenceFamily;
    hasSpecificFamily_ = true;
  }
}

/* String converter */
String StandardDistributionPolynomialFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " hasSpecificFamily=" << hasSpecificFamily_;
  if (hasSpecificFamily_) oss << " specificFamily=" << specificFamily_;
  else oss << " orthonormalization algorithm=" << orthonormalizationAlgorithm_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void StandardDistributionPolynomialFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "orthonormalizationAlgorithm_", orthonormalizationAlgorithm_ );
  adv.saveAttribute( "specificFamily_", specificFamily_ );
  adv.saveAttribute( "hasSpecificFamily_", hasSpecificFamily_ );
}


/* Method load() reloads the object from the StorageManager */
void StandardDistributionPolynomialFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "orthonormalizationAlgorithm_", orthonormalizationAlgorithm_ );
  adv.loadAttribute( "specificFamily_", specificFamily_ );
  adv.loadAttribute( "hasSpecificFamily_", hasSpecificFamily_ );
}


END_NAMESPACE_OPENTURNS
