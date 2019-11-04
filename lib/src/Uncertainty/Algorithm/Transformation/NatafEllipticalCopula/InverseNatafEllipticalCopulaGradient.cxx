//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
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
#include "openturns/InverseNatafEllipticalCopulaGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalCopulaGradient
 *
 * This class offers an interface for the InverseNataf function for elliptical copula
 */

CLASSNAMEINIT(InverseNatafEllipticalCopulaGradient)

static const Factory<InverseNatafEllipticalCopulaGradient> Factory_InverseNatafEllipticalCopulaGradient;

/* Default constructor */
InverseNatafEllipticalCopulaGradient::InverseNatafEllipticalCopulaGradient()
  : GradientImplementation()
  , standardDistribution_()
  , cholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalCopulaGradient::InverseNatafEllipticalCopulaGradient(const Distribution & standardDistribution,
    const TriangularMatrix & cholesky)
  : GradientImplementation()
  , standardDistribution_(standardDistribution)
  , cholesky_(cholesky)
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafEllipticalCopulaGradient * InverseNatafEllipticalCopulaGradient::clone() const
{
  return new InverseNatafEllipticalCopulaGradient(*this);
}

/* String converter */
String InverseNatafEllipticalCopulaGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafEllipticalCopulaGradient::GetClassName()
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
 */
Matrix InverseNatafEllipticalCopulaGradient::gradient(const Point & inP) const
{
  UnsignedInteger dimension = getInputDimension();
  // First, correlate the components
  Point point(cholesky_ * inP);
  Matrix result(dimension, dimension);
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  // Second, apply the commmon marginal distribution
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    Scalar marginalPDF = standardMarginal.computePDF(Point(1, point[j]));
    for (UnsignedInteger i = 0; i <= j; ++i)
    {
      result(i, j) = marginalPDF * cholesky_(j, i);
    }
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseNatafEllipticalCopulaGradient::getInputDimension() const
{
  return cholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger InverseNatafEllipticalCopulaGradient::getOutputDimension() const
{
  return cholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalCopulaGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "cholesky_", cholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalCopulaGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "cholesky_", cholesky_ );
}

END_NAMESPACE_OPENTURNS
