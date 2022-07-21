//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for independent
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
#include <cmath>
#include "openturns/NatafIndependentCopulaGradient.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafIndependentCopulaGradient
 *
 * This class offers an interface for the Nataf function for independent copula
 */


CLASSNAMEINIT(NatafIndependentCopulaGradient)

static const Factory<NatafIndependentCopulaGradient> Factory_NatafIndependentCopulaGradient;

/* Default constructor */
NatafIndependentCopulaGradient::NatafIndependentCopulaGradient()
  : GradientImplementation()
  , dimension_(1)
{
  // Nothing to do
}

/* Parameter constructor */
NatafIndependentCopulaGradient::NatafIndependentCopulaGradient(const UnsignedInteger dimension)
  : GradientImplementation()
  , dimension_(dimension)
{
  // Nothing to do
}

/* Virtual constructor */
NatafIndependentCopulaGradient * NatafIndependentCopulaGradient::clone() const
{
  return new NatafIndependentCopulaGradient(*this);
}

/* String converter */
String NatafIndependentCopulaGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafIndependentCopulaGradient::GetClassName()
      << " dimension=" << dimension_;

  return oss;
}

/*
 * Evaluation
 * The Nataf transform T reads:
 * Ti(xi) = Q(xi), where Q = Phi^{-1} and Phi is the CDF of the standard normal distribution
 * We have:
 * Jij = dTi/dxj = Q'(xi) if i = j
 *               = 0 else
 */
Matrix NatafIndependentCopulaGradient::gradient(const Point & inP) const
{
  Matrix result(dimension_, dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar x = inP[i];
    if ((x < 0.0) || (x > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the NatafIndependentCopulaGradient if all the components are not in [0, 1], here in=" << inP;
    // q = Normal(0,1).computeQuantile(x)
    const Scalar q = DistFunc::qNormal(x);
    result(i, i) = SpecFunc::SQRT2PI * exp(0.5 * q * q);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger NatafIndependentCopulaGradient::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger NatafIndependentCopulaGradient::getOutputDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void NatafIndependentCopulaGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafIndependentCopulaGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
