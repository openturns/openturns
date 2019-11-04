//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for independent
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
#include "openturns/NatafIndependentCopulaEvaluation.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafIndependentCopulaEvaluation
 *
 * This class offers an interface for the Nataf function for independent copula
 */


CLASSNAMEINIT(NatafIndependentCopulaEvaluation)

static const Factory<NatafIndependentCopulaEvaluation> Factory_NatafIndependentCopulaEvaluation;

/* Default constructor */
NatafIndependentCopulaEvaluation::NatafIndependentCopulaEvaluation()
  : EvaluationImplementation()
  , dimension_(1)
{
  // Nothing to do
}

/* Parameter constructor */
NatafIndependentCopulaEvaluation::NatafIndependentCopulaEvaluation(const UnsignedInteger dimension)
  : EvaluationImplementation()
  , dimension_(dimension)
{
  Description description(Description::BuildDefault(dimension_, "X"));
  description.add(Description::BuildDefault(dimension_, "Y"));
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

String NatafIndependentCopulaEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << NatafIndependentCopulaEvaluation::GetClassName()
      << "(IndependentCopula(" << dimension_ << ")->Normal(" << dimension_ << "))";

  return oss;
}

/*
 * Evaluation
 * The Nataf transform T reads:
 * Ti(xi) = Q(xi), where Q = Phi^{-1} and Phi is the CDF of the standard normal distribution
 */
Point NatafIndependentCopulaEvaluation::operator () (const Point & inP) const
{
  Point result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar x = inP[i];
    if ((x < 0.0) || (x > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the NatafIndependentCopulaEvaluation if all the components are not in [0, 1], here in=" << inP;
    result[i] = DistFunc::qNormal(x);
  }
  callsNumber_.increment();
  return result;
}

/* Gradient according to the marginal parameters. Currently, the dependence parameters are not taken into account. */

Matrix NatafIndependentCopulaEvaluation::parameterGradient(const Point & ) const
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
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafIndependentCopulaEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
