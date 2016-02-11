
//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
#include "openturns/OTprivate.hxx"
#include "openturns/NatafEllipticalCopulaEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafEllipticalCopulaEvaluation
 *
 * This class offers an interface for the Nataf function for elliptical copula
 */

CLASSNAMEINIT(NatafEllipticalCopulaEvaluation);

static const Factory<NatafEllipticalCopulaEvaluation> RegisteredFactory;

/* Default constructor */
NatafEllipticalCopulaEvaluation::NatafEllipticalCopulaEvaluation()
  : NumericalMathEvaluationImplementation()
  , standardDistribution_()
  , inverseCholesky_()
{
  // Nothing to do
}

/* Parameter constructor */
NatafEllipticalCopulaEvaluation::NatafEllipticalCopulaEvaluation(const Distribution & standardDistribution,
    const TriangularMatrix & inverseCholesky)
  : NumericalMathEvaluationImplementation()
  , standardDistribution_(standardDistribution)
  , inverseCholesky_(inverseCholesky)
{
  Description description;
  for (UnsignedInteger i = 0; i < inverseCholesky_.getNbColumns(); ++i) description.add(OSS() << "x" << i);
  for (UnsignedInteger i = 0; i < inverseCholesky_.getNbRows(); ++i) description.add(OSS() << "y" << i);
  setDescription(description);
}

/* Virtual constructor */
NatafEllipticalCopulaEvaluation * NatafEllipticalCopulaEvaluation::clone() const
{
  return new NatafEllipticalCopulaEvaluation(*this);
}

/* String converter */
String NatafEllipticalCopulaEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafEllipticalCopulaEvaluation::GetClassName()
      << " description=" << getDescription()
      << " standardDistribution=" << standardDistribution_
      << " inverseCholesky=" << inverseCholesky_;

  return oss;
}

/*
 * Evaluation
 * This function transforms an elliptical copula with correlation matrix R into the associated standard elliptical distribution:
 * Yi(x) = Q(xi), where Q = F^{-1} and F is the CDF of the standard elliptical distribution
 * T(x) = G.Y(x), where G = L^{-1} and L is the Cholesky factor of R: L.L^t = R, L is lower triangular
 */
NumericalPoint NatafEllipticalCopulaEvaluation::operator () (const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getOutputDimension());
  NumericalPoint result(dimension);
  const Distribution standardMarginal(standardDistribution_.getMarginal(0));
  // First, filter the commmon marginal distribution
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = standardMarginal.computeQuantile(inP[i])[0];
  // Second, decorrelate the components
  result = inverseCholesky_ * result;
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. Currently, the dependence parameters are not taken into account. */

Matrix NatafEllipticalCopulaEvaluation::parameterGradient(const NumericalPoint & inP) const
{
  return Matrix(0, getInputDimension());
}

/* Accessor for input point dimension */
UnsignedInteger NatafEllipticalCopulaEvaluation::getInputDimension() const
{
  return inverseCholesky_.getNbColumns();
}

/* Accessor for output point dimension */
UnsignedInteger NatafEllipticalCopulaEvaluation::getOutputDimension() const
{
  return inverseCholesky_.getNbRows();
}

/* Method save() stores the object through the StorageManager */
void NatafEllipticalCopulaEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "standardDistribution_", standardDistribution_ );
  adv.saveAttribute( "inverseCholesky_", standardDistribution_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafEllipticalCopulaEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "standardDistribution_", standardDistribution_ );
  adv.loadAttribute( "inverseCholesky_", standardDistribution_ );
}

END_NAMESPACE_OPENTURNS
