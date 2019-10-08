//                                               -*- C++ -*-
/**
 *  @brief This is the natural basis associated to a multivariate distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SoizeGhanemFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
#include "openturns/Function.hxx"
#include "openturns/SoizeGhanemFactorEvaluation.hxx"
#include "openturns/LinearEnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SoizeGhanemFactory)

static const Factory<SoizeGhanemFactory> Factory_SoizeGhanemFactory;

/* Default constructor */
SoizeGhanemFactory::SoizeGhanemFactory()
  : OrthogonalFunctionFactory()
  , productPolynomial_()
  , phi_(LinearEnumerateFunction(1))
  , hasIndependentCopula_(measure_.hasIndependentCopula())
{
  buildProductPolynomialAndAdaptation(false);
}


/* Constructor */
SoizeGhanemFactory::SoizeGhanemFactory(const Distribution & measure,
                                       const Bool useCopula)
  : OrthogonalFunctionFactory(measure)
  , productPolynomial_()
  , phi_(LinearEnumerateFunction(measure.getDimension()))
  , hasIndependentCopula_(measure.hasIndependentCopula())
{
  buildProductPolynomialAndAdaptation(useCopula);
}


/* Constructor */
SoizeGhanemFactory::SoizeGhanemFactory(const Distribution & measure,
                                       const EnumerateFunction & phi,
                                       const Bool useCopula)
  : OrthogonalFunctionFactory(measure)
  , productPolynomial_()
  , phi_(phi)
  , hasIndependentCopula_(measure.hasIndependentCopula())
{
  if (measure.getDimension() != phi.getDimension()) throw InvalidArgumentException(HERE) << "Error: the enumerate function must have a dimension equal to the dimension of the measure";
  buildProductPolynomialAndAdaptation(useCopula);
}


/* Virtual constructor */
SoizeGhanemFactory * SoizeGhanemFactory::clone() const
{
  return new SoizeGhanemFactory(*this);
}


/* Return the enumerate function that translate unidimensional indices into multidimensional indices */
EnumerateFunction SoizeGhanemFactory::getEnumerateFunction() const
{
  return phi_;
}


/* Build the Function of the given index orthonormal
 * with respect to the following inner product:
 * <f_i, f_j> = \int_R^n f_i(x) f_j(x) c(F_1(x_1),\dots,F_n(x_n))\prod_{k=1}^n p_k(x_k)dx
 * See: Christian Soize, R. Ghanem.
 * "Physical systems with random uncertainties: Chaos representations with
 *  arbitrary probability measure". SIAM Journal on Scientific Computing,
 *  Society for Industrial and Applied Mathematics, 2004, 26 (2), pp.395-410.
 */
Function SoizeGhanemFactory::build(const UnsignedInteger index) const
{
  const Function productPolynomial(productPolynomial_.build(index));
  // If the distribution has an independent copula the SoizeGhanem basis
  // is exactly the product polynomial factory
  if (hasIndependentCopula_) return productPolynomial;
  return productPolynomial * adaptationFactor_;
}

/* Build the multivariate polynomial factory associated with the marginal distributions */
void SoizeGhanemFactory::buildProductPolynomialAndAdaptation(const Bool useCopula)
{
  const UnsignedInteger dimension = measure_.getDimension();
  Collection<Distribution> marginals(dimension);
  OrthogonalProductPolynomialFactory::PolynomialFamilyCollection coll(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    marginals[i] = measure_.getMarginal(i);
    coll[i] = StandardDistributionPolynomialFactory(AdaptiveStieltjesAlgorithm(marginals[i]));
  }
  productPolynomial_ = OrthogonalProductPolynomialFactory(coll, phi_);
  // Build the adaptation factor only if needed
  if (!hasIndependentCopula_)
    adaptationFactor_ = Function(SoizeGhanemFactorEvaluation(measure_, marginals, useCopula));
}

/* String converter */
String SoizeGhanemFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " productPolynomial=" << productPolynomial_
      << " phi=" << phi_
      << " measure=" << measure_
      << " hasIndependentCopula=" << hasIndependentCopula_;
  if (!hasIndependentCopula_)
    oss << " adaptationFactor=" << adaptationFactor_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SoizeGhanemFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute( "productPolynomial_", productPolynomial_ );
  adv.saveAttribute( "phi_", phi_ );
  adv.saveAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  adv.saveAttribute( "adaptationFactor_", adaptationFactor_ );
}


/* Method load() reloads the object from the StorageManager */
void SoizeGhanemFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute( "productPolynomial_", productPolynomial_ );
  adv.loadAttribute( "phi_", phi_ );
  adv.loadAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  adv.loadAttribute( "adaptationFactor_", adaptationFactor_ );
}

END_NAMESPACE_OPENTURNS
