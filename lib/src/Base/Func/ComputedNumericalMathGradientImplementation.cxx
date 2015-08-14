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
#include "ComputedNumericalMathGradientImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "WrapperData.hxx"
#include "WrapperObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComputedNumericalMathGradientImplementation);

static Factory<ComputedNumericalMathGradientImplementation> RegisteredFactory("ComputedNumericalMathGradientImplementation");

/* Default constructor */
ComputedNumericalMathGradientImplementation::ComputedNumericalMathGradientImplementation(const String & name,
    const WrapperFile & file,
    void * p_state)
  : NumericalMathGradientImplementation()
  , p_gradient_(0)
  , p_state_(0)
  , commonState_(file.getWrapperData().getParameters().state_ == WRAPPER_SHAREDSTATE)
{
  setName(name);
  const WrapperData data = file.getWrapperData();
  if (! data.isValid()) throw WrapperInternalException(HERE) << "The wrapper data are not valid";

  if (data.getGradientDescription().provided_)
  {
    p_gradient_.reset(new WrapperObject( data.getLibraryPath(),
                                         data.getGradientDescription().name_,
                                         data,
                                         WrapperObject::GRADIENT));
  }

  if (p_gradient_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper for gradient (Note: this message is normal if gradient is NOT provided. Check wrapper description file '" << file.getDescriptionFilePath() << ".)";

  // Initialize the state into the wrapper
  p_state_ = commonState_ ? p_state : p_gradient_->createNewState();
}


/* Copy constructor */
ComputedNumericalMathGradientImplementation::ComputedNumericalMathGradientImplementation(const ComputedNumericalMathGradientImplementation & other)
  : NumericalMathGradientImplementation(other),
    p_gradient_(other.p_gradient_),
    p_state_(0),
    commonState_(other.commonState_)
{
  if (p_gradient_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper";

  // Initialize the state into the wrapper
  p_state_ = commonState_ ? other.p_state_ : p_gradient_->createNewState();
}


/* Virtual constructor */
ComputedNumericalMathGradientImplementation * ComputedNumericalMathGradientImplementation::clone() const
{
  return new ComputedNumericalMathGradientImplementation(*this);
}

/* Destructor */
ComputedNumericalMathGradientImplementation::~ComputedNumericalMathGradientImplementation()
{
  // Delete the state into the wrapper
  if (!commonState_) p_gradient_->deleteState( p_state_ );
}


/* Comparison operator */
Bool ComputedNumericalMathGradientImplementation::operator ==(const ComputedNumericalMathGradientImplementation & other) const
{
  return true;
}


/* String converter */
String ComputedNumericalMathGradientImplementation::__repr__() const
{
  return OSS(true) << "class=" << ComputedNumericalMathGradientImplementation::GetClassName()
         << " name=" << getName();
}

/* String converter */
String ComputedNumericalMathGradientImplementation::__str__(const String & offset) const
{
  WrapperObject wrp = *p_gradient_;
  return OSS(false) << offset << "ComputedNumericalMathGradientImplementation bound to wrapper '"
         << wrp << "'";
}


/* State accessor */
void * ComputedNumericalMathGradientImplementation::getState() const
{
  return p_state_;
}







/* Here is the interface that all derived class must implement */

/* Gradient method */
Matrix
ComputedNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  try
  {
    // Execute the wrapper
    ++callsNumber_;
    return p_gradient_->gradient( getState(), inP );
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
UnsignedInteger ComputedNumericalMathGradientImplementation::getInputDimension() const
{
  UnsignedInteger inputDimension = 0;

  try
  {
    inputDimension = p_gradient_->getInNumericalPointDimension(getState());
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return inputDimension;
}



/* Accessor for output point dimension */
UnsignedInteger ComputedNumericalMathGradientImplementation::getOutputDimension() const
{
  UnsignedInteger outputDimension = 0;

  try
  {
    outputDimension = p_gradient_->getOutNumericalPointDimension(getState());
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return outputDimension;
}

/* Method save() stores the object through the StorageManager */
void ComputedNumericalMathGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ComputedNumericalMathGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  ComputedNumericalMathGradientImplementation other( getName(), WrapperFile::FindWrapperByName( getName() ) );
  p_gradient_ = other.p_gradient_;
  // Initialize the state into the wrapper
  p_state_ = p_gradient_->createNewState();
  commonState_ = other.commonState_;
}


END_NAMESPACE_OPENTURNS
