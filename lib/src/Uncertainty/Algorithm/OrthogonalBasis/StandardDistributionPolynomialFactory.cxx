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

#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
#include "openturns/CharlierFactory.hxx"
#include "openturns/HermiteFactory.hxx"
#include "openturns/JacobiFactory.hxx"
#include "openturns/KrawtchoukFactory.hxx"
#include "openturns/LaguerreFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/MeixnerFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StandardDistributionPolynomialFactory);

static const Factory<StandardDistributionPolynomialFactory> RegisteredFactory;


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
  const String measureType(measure_.getImplementation()->getClassName());
  // Legendre factory
  if (measureType == "Uniform")
  {
    specificFamily_ = LegendreFactory();
    hasSpecificFamily_ = true;
    return;
  }
  // Hermite factory
  if (measureType == "Normal")
  {
    specificFamily_ = HermiteFactory();
    hasSpecificFamily_ = true;
    return;
  }
  // Jacobi factory
  if (measureType == "Beta")
  {
    const NumericalPoint parameter(measure_.getParameter());
    specificFamily_ = JacobiFactory(parameter[1] - parameter[0] - 1.0, parameter[0] - 1.0);
    hasSpecificFamily_ = true;
    return;
  }
  // Laguerre factory
  if (measureType == "Gamma")
  {
    const NumericalPoint parameter(measure_.getParameter());
    specificFamily_ = LaguerreFactory(parameter[0] - 1.0);
    hasSpecificFamily_ = true;
    return;
  }
  // Charlier factory
  if (measureType == "Poisson")
  {
    const NumericalPoint parameter(measure_.getParameter());
    specificFamily_ = CharlierFactory(parameter[0]);
    hasSpecificFamily_ = true;
    return;
  }
  // Krawtchouk factory
  if (measureType == "Binomial")
  {
    const NumericalPoint parameter(measure_.getParameter());
    specificFamily_ = KrawtchoukFactory(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
    hasSpecificFamily_ = true;
    return;
  }
  // Meixner factory
  if (measureType == "NegativeBinomial")
  {
    const NumericalPoint parameter(measure_.getParameter());
    specificFamily_ = MeixnerFactory(parameter[0], parameter[1]);
    hasSpecificFamily_ = true;
    return;
  }
}

/* String converter */
String StandardDistributionPolynomialFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " orthonormalization algorithm=" << orthonormalizationAlgorithm_;
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
