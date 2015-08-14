//                                               -*- C++ -*-
/**
 * @brief Abstract top-level class for all distributions
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include <set>

#include "ComputedNumericalMathEvaluationImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Log.hxx"
#include "WrapperData.hxx"
#include "WrapperObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef NumericalMathEvaluationImplementation::CacheKeyType             CacheKeyType;
typedef NumericalMathEvaluationImplementation::CacheValueType           CacheValueType;
typedef NumericalMathEvaluationImplementation::CacheType                CacheType;

CLASSNAMEINIT(ComputedNumericalMathEvaluationImplementation);

static Factory<ComputedNumericalMathEvaluationImplementation> RegisteredFactory("ComputedNumericalMathEvaluationImplementation");


/* Default constructor */
ComputedNumericalMathEvaluationImplementation::ComputedNumericalMathEvaluationImplementation(const String & name,
    const WrapperFile & file)
  : NumericalMathEvaluationImplementation()
  , p_function_(0)
  , p_state_(0)
{
  setName(name);
  const WrapperData data = file.getWrapperData();
  if (! data.isValid()) throw WrapperInternalException(HERE) << "The wrapper data are not valid";

  if (data.getFunctionDescription().provided_)
  {
    p_function_.reset(new WrapperObject( data.getLibraryPath(),
                                         data.getFunctionDescription().name_,
                                         data,
                                         WrapperObject::FUNCTION ));
  }

  if (p_function_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper";

  // Initialize the state into the wrapper
  p_state_ = p_function_->createNewState();
  NumericalMathEvaluationImplementation::setDescription(getDescription());

  // Activate the cache only if the external code is expensive: only the user knows it.
  NumericalMathEvaluationImplementation::disableCache();
}


/* Copy constructor */
ComputedNumericalMathEvaluationImplementation::ComputedNumericalMathEvaluationImplementation(const ComputedNumericalMathEvaluationImplementation & other)
  : NumericalMathEvaluationImplementation(other)
  , p_function_(other.p_function_)
  , p_state_(0)
{
  if (p_function_.isNull()) throw WrapperInternalException(HERE) << "Unable to allocate wrapper";

  // Initialize the state into the wrapper
  p_state_ = p_function_->createNewState();
}


/* Virtual constructor */
ComputedNumericalMathEvaluationImplementation * ComputedNumericalMathEvaluationImplementation::clone() const
{
  return new ComputedNumericalMathEvaluationImplementation(*this);
}

/* Destructor */
ComputedNumericalMathEvaluationImplementation::~ComputedNumericalMathEvaluationImplementation()
{
  // Call the finalization function before destruction
  p_function_->finalize( p_state_ );

  // Delete the state into the wrapper
  p_function_->deleteState( p_state_ );
}


/* Comparison operator */
Bool ComputedNumericalMathEvaluationImplementation::operator ==(const ComputedNumericalMathEvaluationImplementation & other) const
{
  return true;
}


/* String converter */
String ComputedNumericalMathEvaluationImplementation::__repr__() const
{
  return OSS(true) << "class=" << ComputedNumericalMathEvaluationImplementation::GetClassName()
         << " name=" << getName();
}

/* String converter */
String ComputedNumericalMathEvaluationImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << "ComputedNumericalMathEvaluationImplementation bound to wrapper '"
         << p_function_->__str__() << "'";
}





/* State accessor */
void * ComputedNumericalMathEvaluationImplementation::getState() const
{
  return p_state_;
}






/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint
ComputedNumericalMathEvaluationImplementation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  NumericalPoint result;
  try
  {
    // First, initialize the external code on first invocation
    if (callsNumber_ == 0) p_function_->initialize( p_state_ );
    // Specific code if cache is enabled
    if (p_cache_->isEnabled())
    {
      CacheKeyType inKey = inP.getCollection();
      // Execute the wrapper
      if ( p_cache_->hasKey( inKey ) )
      {
        result = NumericalPoint::ImplementationType( p_cache_->find( inKey ) );
      }
      else
      {
        ++callsNumber_;
        result = p_function_->execute( p_state_, inP );
        CacheValueType outValue(result.getCollection());
        p_cache_->add( inKey, outValue );
      }
    } // If cache is enabled
    else
    {
      ++callsNumber_;
      result = p_function_->execute( p_state_, inP );
    } // Cache disabled
    if (isHistoryEnabled_)
    {
      inputStrategy_.store(inP);
      outputStrategy_.store(result);
    }
    return result;
  } // try
  catch (InvalidArgumentException)
  {
    throw;
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }
}

/* Operator () */
NumericalSample
ComputedNumericalMathEvaluationImplementation::operator() (const NumericalSample & inS) const
{
  try
  {
    const UnsignedInteger size(inS.getSize());
    // The sample out will store all the results as if there was no specific action for multiple input points or already computed values
    NumericalSample outS( size, getOutputDimension() );
    // First, initialize the external code on first invocation
    if (callsNumber_ == 0) p_function_->initialize( p_state_ );
    const bool useCache = p_cache_->isEnabled();

    NumericalSample toDo(0, getInputDimension());
    if ( useCache )
    {
      std::set<NumericalPoint> uniqueValues;
      for (UnsignedInteger i = 0; i < size; ++ i )
      {
        if ( useCache )
        {
          CacheKeyType inKey( inS[i].getCollection() );
          if ( p_cache_->hasKey( inKey ) )
          {
            outS[i] = NumericalPoint::ImplementationType( p_cache_->find( inKey ) );
          }
          else
          {
            uniqueValues.insert( inS[i] );
          }
        }
        else
        {
          uniqueValues.insert( inS[i] );
        }
      }
      for(std::set<NumericalPoint>::const_iterator it = uniqueValues.begin(); it != uniqueValues.end(); ++it)
      {
        // store unique values
        toDo.add( *it );
      }
    }
    else
    {
      // compute all values, including duplicates
      toDo = inS;
    }


    UnsignedInteger toDoSize = toDo.getSize();
    CacheType tempCache( toDoSize );
    if ( useCache ) tempCache.enable();

    if ( toDoSize > 0 )
    {
      callsNumber_ += toDoSize;
      NumericalSample newOut(p_function_->execute( p_state_, toDo ));
      if ( useCache )
      {
        for(UnsignedInteger i = 0; i < toDoSize; ++i)
        {
          tempCache.add( toDo[i].getCollection(), newOut[i].getCollection() );
        }
      }
      else
      {
        outS = newOut;
      }
    }

    if ( useCache )
    {
      // fill all the output values
      for( UnsignedInteger i = 0; i < size; ++i )
      {
        CacheKeyType inKey( inS[i].getCollection() );
        if ( tempCache.hasKey( inKey ) )
        {
          outS[i] = NumericalPoint::ImplementationType( tempCache.find( inKey ) );
        }
      }
      p_cache_->merge( tempCache );
    }

    // Store the computations in the history if asked for
    if (isHistoryEnabled_)
    {
      inputStrategy_.store(inS);
      outputStrategy_.store(outS);
    }
    outS.setDescription(getOutputDescription());
    // return the gathered sample
    return outS;
  } // try
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
UnsignedInteger ComputedNumericalMathEvaluationImplementation::getInputDimension() const
{
  UnsignedInteger inputDimension = 0;

  try
  {
    inputDimension = p_function_->getInNumericalPointDimension( p_state_ );
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return inputDimension;
}



/* Accessor for output point dimension */
UnsignedInteger ComputedNumericalMathEvaluationImplementation::getOutputDimension() const
{
  UnsignedInteger outputDimension = 0;

  try
  {
    outputDimension = p_function_->getOutNumericalPointDimension( p_state_ );
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return outputDimension;
}

/* Accessor for output point dimension */
Description ComputedNumericalMathEvaluationImplementation::getDescription() const
{
  Description description;

  try
  {
    // Here, we get only the description of the input variable
    description = p_function_->getDescription();
    if (description.getSize() == getInputDimension())
    {
      // Put generic names for the output description if something they are missing in the wrapper
      for (UnsignedInteger i = 0; i < getOutputDimension(); ++i) description.add(OSS() << "y" << i);
    }
    // If the description does not match the dimensions, error
    if (description.getSize() != getInputDimension() + getOutputDimension()) throw InternalException(HERE) << "Error: the description " << description << " does not match the dimensions of the function. Here, input dimension=" << getInputDimension() << " and output dimension=" << getOutputDimension() << ". Check the wrapper description.";
  }
  catch (WrapperInternalException & ex)
  {
    throw InternalException(HERE) << ex;
  }

  return description;
}

/* Method save() stores the object through the StorageManager */
void ComputedNumericalMathEvaluationImplementation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ComputedNumericalMathEvaluationImplementation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  ComputedNumericalMathEvaluationImplementation other( getName(), WrapperFile::FindWrapperByName( getName() ) );
  p_function_ = other.p_function_;
  // Initialize the state into the wrapper
  p_state_ = p_function_->createNewState();
}

END_NAMESPACE_OPENTURNS
