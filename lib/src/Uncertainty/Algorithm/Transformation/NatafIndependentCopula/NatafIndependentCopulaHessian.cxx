//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for independent
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NatafIndependentCopulaHessian.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class NatafIndependentCopulaHessian
 *
 * This class offers an interface for the Nataf function for independent copula
 */


CLASSNAMEINIT(NatafIndependentCopulaHessian)

static const Factory<NatafIndependentCopulaHessian> Factory_NatafIndependentCopulaHessian;

/* Default constructor */
NatafIndependentCopulaHessian::NatafIndependentCopulaHessian()
  : HessianImplementation()
  , dimension_(1)
{
  // Nothing to do
}

/* Parameter constructor */
NatafIndependentCopulaHessian::NatafIndependentCopulaHessian(const UnsignedInteger dimension)
  : HessianImplementation()
  , dimension_(dimension)
{
  // Nothing to do
}

/* Virtual constructor */
NatafIndependentCopulaHessian * NatafIndependentCopulaHessian::clone() const
{
  return new NatafIndependentCopulaHessian(*this);
}

/* String converter */
String NatafIndependentCopulaHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << NatafIndependentCopulaHessian::GetClassName()
      << " dimension=" << dimension_;
  return oss;
}

/*
 * Evaluation
 * The Nataf transform T reads:
 * Ti(xi) = Q(xi), where Q = Phi^{-1} and Phi is the CDF of the standard normal distribution
 * We have:
 * Hijk = d2Ti/dxjdxk = Q''(xi) if i = j = k
 *                    = 0 else
 */
SymmetricTensor NatafIndependentCopulaHessian::hessian(const Point & inP) const
{
  SymmetricTensor result(dimension_, dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar x = inP[i];
    if ((x < 0.0) || (x > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot evaluate the NatafIndependentCopulaHessian if all the components are not in [0, 1], here in=" << inP;
    // q = Normal(0,1).computeQuantile(x)
    const Scalar q = DistFunc::qNormal(x);
    // factor = 1/Normal(0,1).computePDF(q)
    // 6.283185307179586476925286 = 2Pi
    // quantileSecondDerivative = -Normal(0,1).computeDDF(q) / (Normal(0,1).computePDF(q))^3 = q / (Normal(0,1).computePDF(q))^2
    result(i, i, i) = 6.283185307179586476925286 * q * exp(q * q);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger NatafIndependentCopulaHessian::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger NatafIndependentCopulaHessian::getOutputDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void NatafIndependentCopulaHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void NatafIndependentCopulaHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
