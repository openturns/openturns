//                                               -*- C++ -*-
/**
 * @brief Canonical tensor gradient
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

#include "openturns/CanonicalTensorGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(CanonicalTensorGradient)

static const Factory<CanonicalTensorGradient> Factory_CanonicalTensorGradient;

/* Default constructor */
CanonicalTensorGradient::CanonicalTensorGradient()
  : GradientImplementation()
  , evaluation_()
{
  // Nothing to do
}

/* Default constructor */
CanonicalTensorGradient::CanonicalTensorGradient(const CanonicalTensorEvaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}


/* Virtual constructor */
CanonicalTensorGradient * CanonicalTensorGradient::clone() const
{
  return new CanonicalTensorGradient(*this);
}


/* Comparison operator */
Bool CanonicalTensorGradient::operator ==(const CanonicalTensorGradient & other) const
{
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String CanonicalTensorGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CanonicalTensorGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter */
String CanonicalTensorGradient::__str__(const String & ) const
{
  return __repr__();
}


/* Gradient */
Matrix CanonicalTensorGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a Function with an argument of invalid dimension";
  const UnsignedInteger outputDimension = getOutputDimension();

  callsNumber_.increment();

  const UnsignedInteger m = evaluation_.getRank();
  Point prodI(m, 1.0);

  Sample sumRI(m, inputDimension);
  Sample sumdRI(m, inputDimension);

  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    const Point xj(1, inP[j]);
    const Collection<Function> basisI(evaluation_.getBasis(j));
    const UnsignedInteger basisSize = evaluation_.getDegrees()[j];

    // compute phi_(i,j)(xj), phi_(i,j)'(xj)
    Point phiXj(basisSize);
    Point dphiXj(basisSize);
    for (UnsignedInteger k = 0; k < basisSize; ++ k)
    {
      phiXj[k] = basisI[k](xj)[0];
      dphiXj[k] = basisI[k].gradient(xj)(0, 0);
    }

    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      const Point coeffI(evaluation_.getCoefficients(i, j));
      Scalar sumI = 0.0;
      Scalar sumdI = 0.0;
      for (UnsignedInteger k = 0; k < basisSize; ++ k)
      {
        if (coeffI[k] != 0.0)
        {
          sumI += coeffI[k] * phiXj[k];
          sumdI += coeffI[k] * dphiXj[k];
        }
      }
      sumRI(i, j) = sumI;
      sumdRI(i, j) = sumdI;
      prodI[i] *= sumI;
    }
  }

  Matrix out(inputDimension, outputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++ j)
  {
    Scalar dj = 0.0;
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      dj += prodI[i] * (sumdRI(i, j) / sumRI(i, j));
    }
    out(j, 0) = dj;
  }
  return out;
}

/* Accessor for input point dimension */
UnsignedInteger CanonicalTensorGradient::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger CanonicalTensorGradient::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void CanonicalTensorGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute("evaluation_", evaluation_);
}

/* Method load() reloads the object from the StorageManager */
void CanonicalTensorGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute("evaluation_", evaluation_);
}

END_NAMESPACE_OPENTURNS
