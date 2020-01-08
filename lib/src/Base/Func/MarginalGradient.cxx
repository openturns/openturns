//                                               -*- C++ -*-
/**
 *  @brief Marginal gradient
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
#include "openturns/MarginalGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(MarginalGradient)

static const Factory<MarginalGradient> Factory_MarginalGradient;

/* Default constructor */
MarginalGradient::MarginalGradient()
  : GradientImplementation()
  , p_gradient_(new GradientImplementation)
{
  // Nothing to do
}

/* Parameter constructor */
MarginalGradient::MarginalGradient(const GradientImplementation & gradient,
                                   const Indices & indices)
  : GradientImplementation()
{
  *this = MarginalGradient(gradient.clone(), indices);
}

/* Parameters constructor */
MarginalGradient::MarginalGradient(const Pointer<GradientImplementation> & p_gradient,
                                   const Indices & indices)
  : GradientImplementation()
  , p_gradient_(p_gradient)
  , indices_(indices)
{
  if (!indices.check(p_gradient->getOutputDimension()))
    throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
}

/* Virtual constructor */
MarginalGradient * MarginalGradient::clone() const
{
  return new MarginalGradient(*this);
}

/* Comparison operator */
Bool MarginalGradient::operator ==(const MarginalGradient & other) const
{
  return (this == &other);
}

/* String converter */
String MarginalGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalGradient::GetClassName()
      << " gradient=" << p_gradient_->__repr__()
      << " indices=" << indices_;
  return oss;
}



/* Here is the interface that all derived class must implement */

/* Gradient method */
Matrix MarginalGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  const Matrix fullGradient(p_gradient_->gradient(inP));
  Matrix result(getInputDimension(), getOutputDimension());
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    for (UnsignedInteger j = 0; j < getOutputDimension(); ++ j)
      result(i, j) = fullGradient(i, indices_[j]);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger MarginalGradient::getInputDimension() const
{
  return p_gradient_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalGradient::getOutputDimension() const
{
  return indices_.getSize();
}


/* Method save() stores the object through the StorageManager */
void MarginalGradient::save(Advocate & adv) const
{
  GradientImplementation::save( adv );
  adv.saveAttribute( "gradient_", *p_gradient_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalGradient::load(Advocate & adv)
{
  GradientImplementation::load( adv );
  TypedInterfaceObject<GradientImplementation> gradient;
  adv.loadAttribute( "gradient_", gradient );
  p_gradient_ = gradient.getImplementation();
}

END_NAMESPACE_OPENTURNS
