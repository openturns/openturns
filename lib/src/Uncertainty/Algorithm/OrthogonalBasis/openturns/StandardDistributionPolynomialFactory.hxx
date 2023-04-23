//                                               -*- C++ -*-
/**
 *  @brief OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
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
#ifndef OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX
#define OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX

#include <map>

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
#include "openturns/OrthonormalizationAlgorithm.hxx"
#include "openturns/Collection.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class StandardDistributionPolynomialFactory
 *
 * OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 */

class OT_API StandardDistributionPolynomialFactory
  : public OrthogonalUniVariatePolynomialFactory
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

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** hasSpecificFamily_ accessor */
  Bool getHasSpecificFamily() const;

  /** orthonormalizationAlgorithm_ accessor */
  OrthonormalizationAlgorithm getOrthonormalizationAlgorithm() const;

  /** specificFamily_ accessor */
  OrthogonalUniVariatePolynomialFamily getSpecificFamily() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Check the existence of a specific family more efficient for the given measure */
  void checkSpecificFamily();

  /** Algorithm used to perform the orthonormalization */
  OrthonormalizationAlgorithm orthonormalizationAlgorithm_;

  /** Specific factory if a special case is recognized */
  OrthogonalUniVariatePolynomialFamily specificFamily_;

  /** Flag to tell if a specific family is available */
  Bool hasSpecificFamily_;

} ; /* class StandardDistributionPolynomialFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STANDARDDISTRIBUTIONPOLYNOMIALFACTORY_HXX */
