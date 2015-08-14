//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
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
#include "ComputedNumericalMathHessianImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "WrapperData.hxx"
#include "WrapperObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComputedNumericalMathHessianImplementation);

static Factory<ComputedNumericalMathHessianImplementation> RegisteredFactory("ComputedNumericalMathHessianImplementation");

/* Default constructor */
ComputedNumericalMathHessianImplementation::ComputedNumericalMathHessianImplementation(const String & name,
    const WrapperFile & file,
    void * p_state)
  : NumericalMathHessianImplementation(),
    p_hessian_(0),
    p_state_(0),
    commonState_(file.getWrapperData().getParameters().state_ == WRAPPER_SHAREDSTATE)
{
  setName(name);
  const WrapperData data = file.getWrapperData();
  if (! data.isValid()) throw WrapperInternalException(HERE) << "The wrapper data are not valid";

  if (data.getHessianDescription().provided_)
  {
    p_hessian_.reset(new WrapperObject( data.getLibraryPath(),
                                        data.getHessianDescription().name_,
                                        data,
                                        WrapperObject::HESSIAN ));
  }

  if (p_hessian_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper for hessian (Note: this message is normal if hessian is NOT provided. Check wrapper description file '" << file.getDescriptionFilePath() << "'.)";

  // Initialize the state into the wrapper
  p_state_ = commonState_ ? p_state : p_hessian_->createNewState();
}


/* Copy constructor */
ComputedNumericalMathHessianImplementation::ComputedNumericalMathHessianImplementation(const ComputedNumericalMathHessianImplementation & other)
  : NumericalMathHessianImplementation(other),
    p_hessian_(other.p_hessian_),
    p_state_(0),
    commonState_(other.commonState_)
{
  if (p_hessian_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper";

  // Initialize the state into the wrapper
  p_state_ = commonState_ ? other.p_state_ : p_hessian_->createNewState();
}


/* Virtual constructor */
ComputedNumericalMathHessianImplementation * ComputedNumericalMathHessianImplementation::clone() const
{
  return new ComputedNumericalMathHessianImplementation(*this);
}

/* Destructor */
ComputedNumericalMathHessianImplementation::~ComputedNumericalMathHessianImplementation()
{
  // Delete the state into the wrapper
  if (!commonState_) p_hessian_->deleteState( p_state_ );
}


/* Comparison operator */
Bool ComputedNumericalMathHessianImplementation::operator ==(const ComputedNumericalMathHessianImplementation & other) const
{
  return true;
}


/* String converter */
String ComputedNumericalMathHessianImplementation::__repr__() const
{
  return OSS(true) << "class=" << ComputedNumericalMathHessianImplementation::GetClassName()
         << " name=" << getName();
}

/* String converter */
String ComputedNumericalMathHessianImplementation::__str__(const String & offset) const
{
  WrapperObject wrp = *p_hessian_;
  return OSS(false) << offset << "ComputedNumericalMathHessianImplementation bound to wrapper '"
         << wrp << "'";
}


/* State accessor */
void * ComputedNumericalMathHessianImplementation::getState() const
{
  return p_state_;
}







/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor
ComputedNumericalMathHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  try
  {
    // Execute the wrapper
    callsNumber_++;
    return p_hessian_->hessian( getState(), inP );
  }
  catch (InvalidArgumentException)
  {
    throw;
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }
}



/* Accessor for input point dimension */
UnsignedInteger ComputedNumericalMathHessianImplementation::getInputDimension() const
{
  UnsignedInteger inputDimension = 0;

  try
  {
    inputDimension = p_hessian_->getInNumericalPointDimension(getState());
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return inputDimension;
}



/* Accessor for output point dimension */
UnsignedInteger ComputedNumericalMathHessianImplementation::getOutputDimension() const
{
  UnsignedInteger outputDimension = 0;

  try
  {
    outputDimension = p_hessian_->getOutNumericalPointDimension(getState());
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return outputDimension;
}

/* Method save() stores the object through the StorageManager */
void ComputedNumericalMathHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ComputedNumericalMathHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  ComputedNumericalMathHessianImplementation other( getName(), WrapperFile::FindWrapperByName( getName() ) );
  p_hessian_ = other.p_hessian_;
  // Initialize the state into the wrapper
  p_state_ = p_hessian_->createNewState();
  commonState_ = other.commonState_;
}


END_NAMESPACE_OPENTURNS
