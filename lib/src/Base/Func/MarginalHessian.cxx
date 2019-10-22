//                                               -*- C++ -*-
/**
 *  @brief Marginal hessian
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/MarginalHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(MarginalHessian)

static const Factory<MarginalHessian> Factory_MarginalHessian;

/* Default constructor */
MarginalHessian::MarginalHessian()
  : HessianImplementation()
  , p_hessian_(new HessianImplementation)
{
  // Nothing to do
}

/* Parameter constructor */
MarginalHessian::MarginalHessian(const HessianImplementation & hessian,
                                 const Indices & indices)
  : HessianImplementation()
{
  *this = MarginalHessian(hessian.clone(), indices);
}

/* Parameters constructor */
MarginalHessian::MarginalHessian(const Pointer<HessianImplementation> & p_hessian,
                                 const Indices & indices)
  : HessianImplementation()
  , p_hessian_(p_hessian)
  , indices_(indices)
{
  if (!indices.check(p_hessian->getOutputDimension()))
    throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
}


/* Virtual constructor */
MarginalHessian * MarginalHessian::clone() const
{
  return new MarginalHessian(*this);
}

/* Comparison operator */
Bool MarginalHessian::operator ==(const MarginalHessian & other) const
{
  return true;
}

/* String converter */
String MarginalHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalHessian::GetClassName()
      << " hessian=" << p_hessian_->__repr__()
      << " indices=" << indices_;
  return oss;
}

/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor MarginalHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  const SymmetricTensor fullHessian(p_hessian_->hessian(inP));
  SymmetricTensor result(getInputDimension(), getOutputDimension());
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
      for (UnsignedInteger k = 0; k < getOutputDimension(); ++ k)
        result(i, j, k) = fullHessian(i, j, indices_[k]);
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger MarginalHessian::getInputDimension() const
{
  return p_hessian_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalHessian::getOutputDimension() const
{
  return indices_.getSize();
}

/* Method save() stores the object through the StorageManager */
void MarginalHessian::save(Advocate & adv) const
{
  HessianImplementation::save( adv );
  adv.saveAttribute( "hessian_", *p_hessian_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalHessian::load(Advocate & adv)
{
  HessianImplementation::load( adv );
  TypedInterfaceObject<HessianImplementation> hessian;
  adv.loadAttribute( "hessian_", hessian );
  p_hessian_ = hessian.getImplementation();
}


END_NAMESPACE_OPENTURNS
