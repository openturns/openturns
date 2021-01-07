//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
#include "openturns/NatafEllipticalCopulaGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalCopulaGradient
 *
 * This class offers an interface for the Nataf function for elliptical copula
 */

CLASSNAMEINIT(NatafEllipticalCopulaGradient)

static const Factory<NatafEllipticalCopulaGradient> Factory_NatafEllipticalCopulaGradient;

/* Default constructor */
NatafEllipticalCopulaGradient::NatafEllipticalCopulaGradient()
  : GradientImplementation()
  , standardDistribution_()
  , inverseCholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalCopulaGradient::NatafEllipticalCopulaGradient(const Distribution & standardDistribution,
    const TriangularMatrix & inverseCholesky)
  : GradientImplementation()
  , standardDistribution_(standardDistribution)
  , inverseCholesky_(inverseCholesky)
{
  // Nothing to do
}

/* Virtual constructor */
NatafEllipticalCopulaGradient * NatafEllipticalCopulaGradient::clone() const
{
  return new NatafEllipticalCopulaGradient(*this);
}

/* String converter */
String NatafEllipticalCopulaGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafEllipticalCopulaGradient::GetClassName()
      << " standardDistribution=" << standardDistribution_
      << " inverseCholesky=" << inverseCholesky_;

  return oss;
}

/*
 * Evaluation
 * The elliptical copula has a correlation matrix R. The Nataf transform T reads:
 * Yi(x) = Q(xi), where Q = F^{-1} and F is the CDF of the standard elliptical distribution
 * T(x) = G.Y(x), where G = L^{-1} and L is the Cholesky factor of R: L.L^t = R, L is lower triangular
 * Its Jacobian J is:
 * Jij = dTi/dxj = (G.dY/dx)ij
 *               = (G.diag(Q'(xk)))ij
 *               = (G:,1Q'(x1)|...|G:,nQ'(xn))ij
 *               = GijQ'(xj)
 * thus, (DT)ij = Jji = GjiQ'(xi)
 */
Matrix NatafEllipticalCopulaGradient::gradient(const Point & inP) const
{
  UnsignedInteger dimension = getInputDimension();
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  Matrix result(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    Scalar quantileDerivative = 1.0 / standardMarginal.computePDF(standardMarginal.computeQuantile(inP[i]));
    for (UnsignedInteger j = i; j < dimension; ++j) result(i, j) = inverseCholesky_(j, i) * quantileDerivative;
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger NatafEllipticalCopulaGradient::getInputDimension() const
{
  return inverseCholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger NatafEllipticalCopulaGradient::getOutputDimension() const
{
  return inverseCholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalCopulaGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "inverseCholesky_", inverseCholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalCopulaGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "inverseCholesky_", inverseCholesky_ );
}

END_NAMESPACE_OPENTURNS
