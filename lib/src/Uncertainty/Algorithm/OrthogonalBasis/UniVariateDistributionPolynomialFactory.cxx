//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/UniVariateDistributionPolynomialFactory.hxx"
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

CLASSNAMEINIT(UniVariateDistributionPolynomialFactory)

static const Factory<UniVariateDistributionPolynomialFactory> Factory_UniVariateDistributionPolynomialFactory;


/* Default constructor */
UniVariateDistributionPolynomialFactory::UniVariateDistributionPolynomialFactory()
  : OrthogonalUniVariatePolynomialFactory(Uniform())
  , orthonormalizationAlgorithm_(AdaptiveStieltjesAlgorithm(Uniform()))
{
  // Initialize the coefficient cache
  initializeCache();
}


/* Parameter constructor */
UniVariateDistributionPolynomialFactory::UniVariateDistributionPolynomialFactory(const Distribution & measure)
  : OrthogonalUniVariatePolynomialFactory(measure)
  , orthonormalizationAlgorithm_(AdaptiveStieltjesAlgorithm(measure.getStandardRepresentative()))
{
  checkSpecificFamily();
  initializeCache();
}


/* Parameter constructor */
UniVariateDistributionPolynomialFactory::UniVariateDistributionPolynomialFactory(const OrthonormalizationAlgorithm & orthonormalizationAlgorithm)
  : OrthogonalUniVariatePolynomialFactory(orthonormalizationAlgorithm.getMeasure())
  , orthonormalizationAlgorithm_(orthonormalizationAlgorithm)
{
  checkSpecificFamily();
  initializeCache();
}


/* Virtual constructor */
UniVariateDistributionPolynomialFactory * UniVariateDistributionPolynomialFactory::clone() const
{
  return new UniVariateDistributionPolynomialFactory(*this);
}


/* Calculate the coefficients of recurrence a0n, a1n, a2n such that
   Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
UniVariateDistributionPolynomialFactory::Coefficients UniVariateDistributionPolynomialFactory::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  LOGDEBUG(OSS() << "UniVariateDistributionPolynomialFactory::getRecurrenceCoefficients hasSpecificFamily=" << hasSpecificFamily_);
  if (hasSpecificFamily_) return specificFamily_.getRecurrenceCoefficients(n);
  else return orthonormalizationAlgorithm_.getRecurrenceCoefficients(n);
}

/* Roots of the polynomial of degree n */
Point UniVariateDistributionPolynomialFactory::getRoots(const UnsignedInteger n) const
{
  if (hasSpecificFamily_) return specificFamily_.getRoots(n);
  return OrthogonalUniVariatePolynomialFactory::getRoots(n);
}

/* Nodes and weights of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix and the square
   of the first component of the associated normalized eigenvectors */
Point UniVariateDistributionPolynomialFactory::getNodesAndWeights(const UnsignedInteger n,
    Point & weights) const
{
  if (hasSpecificFamily_) return specificFamily_.getNodesAndWeights(n, weights);
  return OrthogonalUniVariatePolynomialFactory::getNodesAndWeights(n, weights);
}

/* Check the existence of a specific family more efficient for the given measure */
void UniVariateDistributionPolynomialFactory::checkSpecificFamily()
{
  // Try to narrow to a specific factory more efficient than the use of a generic orthonormalization algorithm.
  // To this end, the given measure is tested for equality with each of the measures behind the different factories
  // The factories are tested from the simplest to the most complex. For example, a Uniform() distribution
  // matches both the LegendreFactory and the JacobiFactory but the LegendreFactory is simpler.
  hasSpecificFamily_ = false;
  const String measureType(measure_.getImplementation()->getClassName());
  const Point parameter(measure_.getParameter());
  LOGDEBUG(OSS() << "UniVariateDistributionPolynomialFactory::checkSpecificFamily measure=" << measure_ << ", measureType=" << measureType << ", parameter=" << parameter);
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
    if (measureType == "Polya")
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
    // Hermite factory: match any Normal
    if (measureType == "Normal")
    {
      specificFamily_ = HermiteFactory(parameter[0], parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Legendre factory: match any Uniform
    if (measureType == "Uniform")
    {
      specificFamily_ = LegendreFactory(parameter[0], parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Chebychev factory: match any Arcsine
    if (measureType == "Arcsine")
    {
      specificFamily_ = ChebychevFactory(parameter[0], parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Jacobi factory: match any Beta
    if (measureType == "Beta")
    {
      specificFamily_ = JacobiFactory(parameter[0], parameter[1], parameter[2], parameter[3]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Laguerre factory: match any Gamma
    if (measureType == "Gamma")
    {
      specificFamily_ = LaguerreFactory(parameter[0], parameter[1], parameter[2]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Laguerre factory: match any Exponential (which is Gamma with k=1)
    if (measureType == "Exponential")
    {
      specificFamily_ = LaguerreFactory(1.0, parameter[0], parameter[1]);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    }
    // Histogram factory
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
      if (size == 1)
      {
        specificFamily_ = LegendreFactory(first, first + width[0]);
        LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
        hasSpecificFamily_ = true;
        return;
      }
      specificFamily_ = HistogramPolynomialFactory(first, width, height);
      LOGDEBUG(OSS() << "measureType=" << measureType << ", specificFamily=" << specificFamily_);
      hasSpecificFamily_ = true;
      return;
    } // Histogram
  } // isContinuous
}

/* String converter */
String UniVariateDistributionPolynomialFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " hasSpecificFamily=" << hasSpecificFamily_;
  if (hasSpecificFamily_) oss << " specificFamily=" << specificFamily_;
  else oss << " orthonormalization algorithm=" << orthonormalizationAlgorithm_;
  return oss;
}

/* hasSpecificFamily_ accessor */
Bool UniVariateDistributionPolynomialFactory::getHasSpecificFamily() const
{
  return hasSpecificFamily_;
}

/** orthonormalizationAlgorithm_ accessor */
OrthonormalizationAlgorithm UniVariateDistributionPolynomialFactory::getOrthonormalizationAlgorithm() const
{
  return orthonormalizationAlgorithm_;
}

/** specificFamily_ accessor */
OrthogonalUniVariatePolynomialFamily UniVariateDistributionPolynomialFactory::getSpecificFamily() const
{
  return specificFamily_;
}


/* Method save() stores the object through the StorageManager */
void UniVariateDistributionPolynomialFactory::save(Advocate & adv) const
{
  OrthogonalUniVariatePolynomialFactory::save(adv);
  adv.saveAttribute( "orthonormalizationAlgorithm_", orthonormalizationAlgorithm_ );
  adv.saveAttribute( "specificFamily_", specificFamily_ );
  adv.saveAttribute( "hasSpecificFamily_", hasSpecificFamily_ );
}


/* Method load() reloads the object from the StorageManager */
void UniVariateDistributionPolynomialFactory::load(Advocate & adv)
{
  OrthogonalUniVariatePolynomialFactory::load(adv);
  adv.loadAttribute( "orthonormalizationAlgorithm_", orthonormalizationAlgorithm_ );
  adv.loadAttribute( "specificFamily_", specificFamily_ );
  adv.loadAttribute( "hasSpecificFamily_", hasSpecificFamily_ );
}


END_NAMESPACE_OPENTURNS
