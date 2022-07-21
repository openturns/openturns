//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
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
#include "openturns/InverseNatafEllipticalCopulaHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalCopulaHessian
 *
 * This class offers an interface for the InverseNataf function for elliptical copula
 */

CLASSNAMEINIT(InverseNatafEllipticalCopulaHessian)

static const Factory<InverseNatafEllipticalCopulaHessian> Factory_InverseNatafEllipticalCopulaHessian;

/* Default constructor */
InverseNatafEllipticalCopulaHessian::InverseNatafEllipticalCopulaHessian()
  : HessianImplementation()
  , standardDistribution_()
  , cholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalCopulaHessian::InverseNatafEllipticalCopulaHessian(const Distribution & standardDistribution,
    const TriangularMatrix & cholesky)
  : HessianImplementation()
  , standardDistribution_(standardDistribution)
  , cholesky_(cholesky)
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafEllipticalCopulaHessian * InverseNatafEllipticalCopulaHessian::clone() const
{
  return new InverseNatafEllipticalCopulaHessian(*this);
}

/* String converter */
String InverseNatafEllipticalCopulaHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafEllipticalCopulaHessian::GetClassName()
      << " standardDistribution=" << standardDistribution_
      << " cholesky=" << cholesky_;

  return oss;
}

/*
 * Evaluation
 * The elliptical copula has a correlation matrix R. The inverse Nataf transform S reads:
 * Z(u) = L.u, where L is the Cholesky factor of R: L.L^t = R, L is lower triangular
 * Si(u) = F(Zi), where F is the CDF of the standard elliptical distribution
 * Its Jacobian J is:
 * Jij = dSi/duj = (dF/du)ij
 *               = (F'(Zi)dZ/du)ij
 *               = F'(Zi)Lij
 * thus, (DS)ij = Jji = LjiF'(Zj)
 * One step further, we have:
 * Hijk = d2Si/dxjdxk = (d2F/dudu')ijk
 *                    = (d(F'(Zi)dZ/du)/du')ijk
 *                    = (F''(Zi)(dZ/du)(dZ/du'))ijk
 *                    = (F''(Zi)LijLik
 * Thus, (D2T)ijk = Hkji = LkjLkiF''(Zk)
 */
SymmetricTensor InverseNatafEllipticalCopulaHessian::hessian(const Point & inP) const
{
  UnsignedInteger dimension = getInputDimension();
  // First, correlate the components
  Point point(cholesky_ * inP);
  SymmetricTensor result(dimension, dimension);
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  // Then, apply standard marginal transformation
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    Scalar ddf = standardMarginal.computeDDF(Point(1, point[k]))[0];
    for (UnsignedInteger i = 0; i <= k; ++i)
    {
      Scalar factor = ddf * cholesky_(k, i);
      for (UnsignedInteger j = 0; j <= k; ++j)
      {
        result(i, j, k) = cholesky_(k, j) * factor;
      } // j
    } // i
  } // k
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseNatafEllipticalCopulaHessian::getInputDimension() const
{
  return cholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger InverseNatafEllipticalCopulaHessian::getOutputDimension() const
{
  return cholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalCopulaHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "cholesky_", cholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalCopulaHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "cholesky_", cholesky_ );
}

END_NAMESPACE_OPENTURNS
