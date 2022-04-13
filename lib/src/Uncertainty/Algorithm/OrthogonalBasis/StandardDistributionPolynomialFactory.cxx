//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
  LOGDEBUG(OSS() << "StandardDistributionPolynomialFactory::getRecurrenceCoefficients hasSpecificFamily=" << hasSpecificFamily_);
  if (hasSpecificFamily_) return specificFamily_.getRecurrenceCoefficients(n);
  else return orthonormalizationAlgorithm_.getRecurrenceCoefficients(n);
}

/* Check the existence of a specific family more efficient for the given measure */
void StandardDistributionPolynomialFactory::checkSpecificFamily()
{
  // Try to narrow to a specific factory more efficient than the use of a generic orthonormalization algorithm.
  // To this end, the given measure is tested for equality with each of the measures behind the different factories
  // The factories are tested from the simplest to the most complex. For example, a Uniform() distribution
  // matches both the LegendreFactory and the JacobiFactory but the LegendreFactory is simpler.
  hasSpecificFamily_ = false;
  const String measureType(measure_.getImplementation()->getClassName());
  const Point parameter(measure_.getParameter());
  LOGDEBUG(OSS() << "StandardDistributionPolynomialFactory::checkSpecificFamily measure=" << measure_ << ", measureType=" << measureType << ", parameter=" << parameter);
  // First, deal with integral valued distributions
  if (measure_.isIntegral())
  {
    if (measureType == "Bernoulli")
    {
      specificFamily_ = KrawtchoukFactory(1, parameter[0]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    if (measureType == "Binomial")
    {
      specificFamily_ = KrawtchoukFactory(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    if ((measureType == "Multinomial") && (measure_.getDimension() == 1))
    {
      specificFamily_ = KrawtchoukFactory(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    if (measureType == "NegativeBinomial")
    {
      specificFamily_ = MeixnerFactory(parameter[0], parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    if (measureType == "Poisson")
    {
      specificFamily_ = CharlierFactory(parameter[0]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
  } // isIntegral
  if (measure_.isContinuous())
  {
    if (measureType == "Arcsine" && parameter[0] == -1.0 && parameter[1] == 1.0)
    {
      specificFamily_ = ChebychevFactory();
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    } // Arcsine
    if (measureType == "Beta")
    {
      const Scalar alpha = parameter[0] - 1.0;
      const Scalar beta = parameter[1] - 1.0;
      // Here we set directly the specific family as the reference distribution
      // of the family has a different type (Arcsine) than the given distribution
      // First, check if the range is standard
      if (!(parameter[2] == -1.0 && parameter[3] == 1.0))
      {
        // If the distribution is not uniform, no specific family
        if (!(alpha == 0.0 && beta == 0.0))
          return;
        // Here we are in the general uniform case
        const Scalar first = parameter[2];
        const Point width(1, parameter[3] - first);
        const Point height(1, 1.0 / width[0]);
        specificFamily_ = HistogramPolynomialFactory(first, width, height);
        LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
        hasSpecificFamily_ = true;
        return;
      }
      // From here we know the range is [-1.0, 1.0]
      // The return of the uniform case, standard this time...
      if (alpha == 0.0 && beta == 0.0)
      {
        specificFamily_ = LegendreFactory();
        LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
        hasSpecificFamily_ = true;
        return;
      } // Uniform
      // The arcsine case
      if (alpha == -0.5 && beta == -0.5)
      {
        specificFamily_ = ChebychevFactory();
        LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
        hasSpecificFamily_ = true;
        return;
      }
      // The general case
      specificFamily_ = JacobiFactory(beta, alpha);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    } // Beta
    if (measureType == "Uniform")
    {
      if (parameter[0] == -1.0 && parameter[1] == 1.0)
        specificFamily_ = LegendreFactory();
      // See the general uniform distribution as a special histogram
      else
      {
        const Scalar first = parameter[0];
        const Point width(1, parameter[1] - first);
        const Point height(1, 1.0 / width[0]);
        specificFamily_ = HistogramPolynomialFactory(first, width, height);
      }
      hasSpecificFamily_ = true;
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      return;
    }
    if (measureType == "Histogram")
    {
      const Scalar first = parameter[0];
      const UnsignedInteger size = (parameter.getSize() - 1) / 2;
      Point width(size);
      Point height(size);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        width[i] = parameter[2 * i + 1];
        height[i] = parameter[2 * i + 2];
      }
      if (size == 1 && first == -1.0 && width[0] == 2.0)
      {
        specificFamily_ = LegendreFactory();
        LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
        hasSpecificFamily_ = true;
        return;
      }
      specificFamily_ = HistogramPolynomialFactory(first, width, height);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    } // Histogram
    // Hermite factory
    if (measureType == "Normal" && parameter[0] == 0.0 && parameter[1] == 1.0)
    {
      specificFamily_ = HermiteFactory();
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Laguerre factory
    if (measureType == "Gamma" && parameter[1] == 1.0 && parameter[2] == 0.0)
    {
      specificFamily_ = LaguerreFactory(parameter[0] - 1.0);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    if (measureType == "Exponential" && parameter[0] == 1.0 && parameter[1] == 0.0)
    {
      specificFamily_ = LaguerreFactory(0.0);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
  } // isContinuous
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
