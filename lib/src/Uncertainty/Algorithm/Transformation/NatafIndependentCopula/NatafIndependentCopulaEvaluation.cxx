//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for independent
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "NatafIndependentCopulaEvaluation.hxx"
#include "DistFunc.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafIndependentCopulaEvaluation
 *
 * This class offers an interface for the Nataf function for independent copula
 */


CLASSNAMEINIT(NatafIndependentCopulaEvaluation);

static const Factory<NatafIndependentCopulaEvaluation> RegisteredFactory;

/* Default constructor */
NatafIndependentCopulaEvaluation::NatafIndependentCopulaEvaluation()
  : NumericalMathEvaluationImplementation()
  , dimension_()
{
  // Nothing to do
}

/* Parameter constructor */
NatafIndependentCopulaEvaluation::NatafIndependentCopulaEvaluation(const UnsignedInteger dimension)
  : NumericalMathEvaluationImplementation()
  , dimension_(dimension)
{
  Description description;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    OSS oss;
    oss << "x" << i;
    description.add(oss);
  }
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    OSS oss;
    oss << "y" << i;
    description.add(oss);
  }
  setDescription(description);
}

/* Virtual constructor */
NatafIndependentCopulaEvaluation * NatafIndependentCopulaEvaluation::clone() const
{
  return new NatafIndependentCopulaEvaluation(*this);
}

/* String converter */
String NatafIndependentCopulaEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafIndependentCopulaEvaluation::GetClassName()
      << " description=" << getDescription()
      << " dimension=" << dimension_;

  return oss;
}

/*
 * Evaluation
 * The Nataf transform T reads:
 * Ti(xi) = Q(xi), where Q = Phi^{-1} and Phi is the CDF of the standard normal distribution
 */
NumericalPoint NatafIndependentCopulaEvaluation::operator () (const NumericalPoint & inP) const
{
  NumericalPoint result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const NumericalScalar x(inP[i]);
    if ((x < 0.0) || (x > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the NatafIndependentCopulaEvaluation if all the components are not in [0, 1], here in=" << inP;
    result[i] = DistFunc::qNormal(x);
  }
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. Currently, the dependence parameters are not taken into account. */

Matrix NatafIndependentCopulaEvaluation::parameterGradient(const NumericalPoint & inP) const
{
  return Matrix(0, dimension_);
}

/* Accessor for input point dimension */
UnsignedInteger NatafIndependentCopulaEvaluation::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger NatafIndependentCopulaEvaluation::getOutputDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void NatafIndependentCopulaEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafIndependentCopulaEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
