//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for independent
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
#include "openturns/InverseNatafIndependentCopulaEvaluation.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafIndependentCopulaEvaluation
 *
 * This class offers an interface for the InverseNataf function for independent copula
 */


CLASSNAMEINIT(InverseNatafIndependentCopulaEvaluation);

/* Parameter constructor */
InverseNatafIndependentCopulaEvaluation::InverseNatafIndependentCopulaEvaluation(const UnsignedInteger dimension)
  : NumericalMathEvaluationImplementation()
  , dimension_(dimension)
{
  Description description(Description::BuildDefault(dimension_, "X"));
  description.add(Description::BuildDefault(dimension_, "Y"));
  setDescription(description);
}

/* Virtual constructor */
InverseNatafIndependentCopulaEvaluation * InverseNatafIndependentCopulaEvaluation::clone() const
{
  return new InverseNatafIndependentCopulaEvaluation(*this);
}

/* String converter */
String InverseNatafIndependentCopulaEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafIndependentCopulaEvaluation::GetClassName()
      << " description=" << getDescription()
      << " dimension=" << dimension_;

  return oss;
}

String InverseNatafIndependentCopulaEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << InverseNatafIndependentCopulaEvaluation::GetClassName()
      << "(Normal(" << dimension_ << ")->IndependentCopula(" << dimension_ << "))";

  return oss;
}

/*
 * Evaluation
 * The inverse Nataf transform S reads:
 * Si(u) = Phi(ui), where Phi is the CDF of the standard normal distribution
 */
NumericalPoint InverseNatafIndependentCopulaEvaluation::operator () (const NumericalPoint & inP) const
{
  NumericalPoint result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = DistFunc::pNormal(inP[i]);
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

/* Gradient according to the marginal parameters. Currently, the dependence parameter are not taken into account. */

Matrix InverseNatafIndependentCopulaEvaluation::parameterGradient(const NumericalPoint & inP) const
{
  return Matrix(0, dimension_);
}

/* Accessor for input point dimension */
UnsignedInteger InverseNatafIndependentCopulaEvaluation::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger InverseNatafIndependentCopulaEvaluation::getOutputDimension() const
{
  return dimension_;
}

END_NAMESPACE_OPENTURNS
