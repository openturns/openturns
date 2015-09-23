//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for independent
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
#include <cmath>
#include "InverseNatafIndependentCopulaGradient.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class InverseNatafIndependentCopulaGradient
 *
 * This class offers an interface for the InverseNataf function for independent copula
 */

CLASSNAMEINIT(InverseNatafIndependentCopulaGradient);

static const Factory<InverseNatafIndependentCopulaGradient> RegisteredFactory;

/* Default constructor */
InverseNatafIndependentCopulaGradient::InverseNatafIndependentCopulaGradient()
  : NumericalMathGradientImplementation()
  , dimension_()
{
  // Nothing to do
}

/* Parameter constructor */
InverseNatafIndependentCopulaGradient::InverseNatafIndependentCopulaGradient(const UnsignedInteger dimension)
  : NumericalMathGradientImplementation()
  , dimension_(dimension)
{
  // Nothing to do
}

/* Virtual constructor */
InverseNatafIndependentCopulaGradient * InverseNatafIndependentCopulaGradient::clone() const
{
  return new InverseNatafIndependentCopulaGradient(*this);
}

/* String converter */
String InverseNatafIndependentCopulaGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << InverseNatafIndependentCopulaGradient::GetClassName()
      << " dimension=" << dimension_;

  return oss;
}

/*
 * Evaluation
 * The inverse Nataf transform S reads:
 * Si(u) = Phi(ui), where Phi is the CDF of the standard normal distribution
 * Jij = dSi/duj = Phi'(ui) if i = j
 *               = 0 else
 */
Matrix InverseNatafIndependentCopulaGradient::gradient(const NumericalPoint & inP) const
{
  Matrix result(dimension_, dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const NumericalScalar x(inP[i]);
    // 0.3989422804014326779399462 = 1/sqrt(2*Pi)
    result(i, i) = 0.3989422804014326779399462 * exp(-0.5 * x * x);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseNatafIndependentCopulaGradient::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger InverseNatafIndependentCopulaGradient::getOutputDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void InverseNatafIndependentCopulaGradient::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseNatafIndependentCopulaGradient::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
