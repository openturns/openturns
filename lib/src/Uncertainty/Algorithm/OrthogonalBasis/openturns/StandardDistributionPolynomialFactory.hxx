//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory (deprecated)
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
#ifndef OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX
#define OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX

#include "openturns/UniVariateDistributionPolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StandardDistributionPolynomialFactory
 * @deprecated Use UniVariateDistributionPolynomialFactory instead.
 */
class OT_API StandardDistributionPolynomialFactory
  : public UniVariateDistributionPolynomialFactory
{
  CLASSNAME
public:

  /** Default constructor */
  StandardDistributionPolynomialFactory();

  /** Parameter constructor */
  StandardDistributionPolynomialFactory(const Distribution & distribution);

  /** Parameter constructor with specific orthonormalization algorithm */
  StandardDistributionPolynomialFactory(const OrthonormalizationAlgorithm & orthonormalizationAlgorithm);

  /** Virtual constructor */
  StandardDistributionPolynomialFactory * clone() const override;

} ; /* class StandardDistributionPolynomialFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX */
