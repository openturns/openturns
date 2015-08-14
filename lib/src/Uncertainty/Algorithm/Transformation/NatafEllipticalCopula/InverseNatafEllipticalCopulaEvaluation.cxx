//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "InverseNatafEllipticalCopulaEvaluation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafEllipticalCopulaEvaluation
 *
 * This class offers an interface for the InverseNataf function for elliptical copula
 */

CLASSNAMEINIT(InverseNatafEllipticalCopulaEvaluation);

static Factory<InverseNatafEllipticalCopulaEvaluation> RegisteredFactory("InverseNatafEllipticalCopulaEvaluation");

/* Default constructor */
InverseNatafEllipticalCopulaEvaluation::InverseNatafEllipticalCopulaEvaluation()
  : NumericalMathEvaluationImplementation()
  , standardDistribution_()
  , cholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafEllipticalCopulaEvaluation::InverseNatafEllipticalCopulaEvaluation(const Distribution & standardDistribution,
    const TriangularMatrix & cholesky)
  : NumericalMathEvaluationImplementation()
  , standardDistribution_(standardDistribution)
  , cholesky_(cholesky)
{
  Description description;
  for (UnsignedInteger i = 0; i < cholesky_.getNbColumns(); ++i) description.add(OSS() << "x" << i);
  for (UnsignedInteger i = 0; i < cholesky_.getNbRows(); ++i) description.add(OSS() << "y" << i);
  setDescription(description);
}

/* Virtual constructor */
InverseNatafEllipticalCopulaEvaluation * InverseNatafEllipticalCopulaEvaluation::clone() const
{
  return new InverseNatafEllipticalCopulaEvaluation(*this);
}

/* String converter */
String InverseNatafEllipticalCopulaEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafEllipticalCopulaEvaluation::GetClassName()
      << " description=" << getDescription()
      << " standardDistribution=" << standardDistribution_
      << " cholesky=" << cholesky_;

  return oss;
}

/*
 * Evaluation
 * This function transforms a standard elliptical distribution into an elliptical copula of the same kind with a correlation matrix R:
 * Z(u) = L.u, where L is the Cholesky factor of R: L.L^t = R, L is lower triangular
 * Si(u) = F(Zi), where F is the CDF of the standard elliptical distribution
 */
NumericalPoint InverseNatafEllipticalCopulaEvaluation::operator () (const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getInputDimension());
  // First, correlate the components
  NumericalPoint result(cholesky_ * inP);
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  // Second, apply the commmon marginal distribution
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = standardMarginal.computeCDF(NumericalPoint(1, result[i]));
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. Currently, the dependence parameter are not taken into account. */

Matrix InverseNatafEllipticalCopulaEvaluation::parametersGradient(const NumericalPoint & inP) const
{
  return Matrix(0, getInputDimension());
}

/* Accessor for input point dimension */
UnsignedInteger InverseNatafEllipticalCopulaEvaluation::getInputDimension() const
{
  return cholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger InverseNatafEllipticalCopulaEvaluation::getOutputDimension() const
{
  return cholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void InverseNatafEllipticalCopulaEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "cholesky_", cholesky_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafEllipticalCopulaEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "cholesky_", cholesky_ );
}

END_NAMESPACE_OPENTURNS
