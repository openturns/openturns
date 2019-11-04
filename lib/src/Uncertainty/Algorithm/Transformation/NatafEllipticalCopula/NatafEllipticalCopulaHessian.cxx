//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NatafEllipticalCopulaHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalCopulaHessian
 *
 * This class offers an interface for the Nataf function for elliptical copula
 */

CLASSNAMEINIT(NatafEllipticalCopulaHessian)

static const Factory<NatafEllipticalCopulaHessian> Factory_NatafEllipticalCopulaHessian;

/* Default constructor */
NatafEllipticalCopulaHessian::NatafEllipticalCopulaHessian()
  : HessianImplementation()
  , standardDistribution_()
  , inverseCholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalCopulaHessian::NatafEllipticalCopulaHessian(const Distribution & standardDistribution,
    const TriangularMatrix & inverseCholesky)
  : HessianImplementation()
  , standardDistribution_(standardDistribution)
  , inverseCholesky_(inverseCholesky)
{
  // Nothing to do
}

/* Virtual constructor */
NatafEllipticalCopulaHessian * NatafEllipticalCopulaHessian::clone() const
{
  return new NatafEllipticalCopulaHessian(*this);
}

/* String converter */
String NatafEllipticalCopulaHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafEllipticalCopulaHessian::GetClassName()
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
 *               = (G.diag(Q'(xk), k=1...n))ij
 *               = (G:,1Q'(x1)|...|G:,nQ'(xn))ij
 *               = GijQ'(xj)
 * thus, (DT)ij = Jji = GjiQ'(xi)
 * One step further, we have:
 * Hijk = d2Ti/dxjdxk = (G.d2Y/dxdx')ijk
 *                    = (G.diag(Q''(xl), l=1..n))ijk
 *                    = GijQ''(xj) if k = j
 *                    = 0 else
 * Thus, (D2T)ijk = Hkji = GjiQ''(xi) if k = j, else 0: each sheet of D2T is a diagonal matrix diag(GjiQ''(xi), i=1..n)
 */
SymmetricTensor NatafEllipticalCopulaHessian::hessian(const Point & inP) const
{
  UnsignedInteger dimension = getInputDimension();
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  SymmetricTensor result(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    Point q(standardMarginal.computeQuantile(inP[i]));
    Scalar factor = 1.0 / standardMarginal.computePDF(q);
    Scalar quantileSecondDerivative = -standardMarginal.computeDDF(q)[0] * factor * factor * factor;
    // inverseCholesky_ is lower triangular
    for (UnsignedInteger j = i; j < dimension; ++j) result(i, i, j) = inverseCholesky_(j, i) * quantileSecondDerivative;
  } // i
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger NatafEllipticalCopulaHessian::getInputDimension() const
{
  return inverseCholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger NatafEllipticalCopulaHessian::getOutputDimension() const
{
  return inverseCholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalCopulaHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "inverseCholesky_", inverseCholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalCopulaHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "inverseCholesky_", inverseCholesky_ );
}

END_NAMESPACE_OPENTURNS
