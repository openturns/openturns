//                                               -*- C++ -*-
/**
 * @brief This class gives a implementation for object's methods so they can be used in NumericalMathFunctions
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

#ifndef OPENTURNS_METHODBOUNDNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_METHODBOUNDNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "NumericalMathFunctionImplementation.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS



struct OT_API NO_MATCH_ON_ReturnTypeAdapter {};

template <typename Tp_> struct ReturnTypeAdapter
{
  typedef NO_MATCH_ON_ReturnTypeAdapter Type_;
};

template <> struct ReturnTypeAdapter< NumericalScalar >
{
  typedef NumericalScalar Type_;
  static inline NumericalPoint toPoint( const NumericalScalar val )
  {
    return NumericalPoint(1, val) ;
  }
};

template <> struct ReturnTypeAdapter< NumericalPoint >
{
  typedef NumericalPoint Type_;
  static inline NumericalPoint toPoint( const NumericalPoint & val )
  {
    return val ;
  }
};



struct OT_API NO_MATCH_ON_ArgumentTypeAdapter {};

template <typename Tp_> struct ArgumentTypeAdapter
{
  typedef NO_MATCH_ON_ArgumentTypeAdapter Type_;
};

template <> struct ArgumentTypeAdapter< NumericalScalar >
{
  typedef const NumericalScalar Type_;
  static inline NumericalScalar fromPoint( const NumericalPoint & val )
  {
    return val[0] ;
  }
};

template <> struct ArgumentTypeAdapter< NumericalPoint >
{
  typedef const NumericalPoint & Type_;
  static inline NumericalPoint fromPoint( const NumericalPoint & val )
  {
    return val ;
  }
};



template <typename ReturnType_, typename ArgumentType_>
struct MethodAdapter : public ReturnTypeAdapter<ReturnType_>, public ArgumentTypeAdapter<ArgumentType_>
{
  typedef typename ArgumentTypeAdapter<ArgumentType_>::Type_ ArgumentType;
  typedef typename ReturnTypeAdapter<ReturnType_>::Type_     ReturnType;
};






/**
 * @class MethodBoundNumericalMathEvaluationImplementation
 *
 * This class gives a implementation for object's methods so they can be used in NumericalMathFunctions
 */
template <typename EvaluableObject, typename ReturnType_, typename ArgumentType_>
class MethodBoundNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
public:



  typedef typename MethodAdapter<ReturnType_, ArgumentType_>::ReturnType (EvaluableObject::*EvaluationMethod) (typename MethodAdapter<ReturnType_, ArgumentType_>::ArgumentType) const;



  /** Default constructor */
  MethodBoundNumericalMathEvaluationImplementation( const EvaluableObject & obj, EvaluationMethod method, const UnsignedInteger inputDimension, const UnsignedInteger outputDimension )
    : obj_(obj),
      method_(method),
      inputDimension_(inputDimension),
      outputDimension_(outputDimension)
  {
    // Build the descriptions
    setInputDescription(Description::BuildDefault(inputDimension, "x"));
    setOutputDescription(Description::BuildDefault(outputDimension, "y"));
  }


  /** Virtual constructor */
  virtual MethodBoundNumericalMathEvaluationImplementation * clone() const
  {
    return new MethodBoundNumericalMathEvaluationImplementation(*this);
  }


  /** Comparison operator */
  Bool operator ==(const MethodBoundNumericalMathEvaluationImplementation & other) const
  {
    return true;
  }


  /** String converter */
  virtual String __repr__() const
  {
    OSS oss;
    oss << "class=MethodBoundNumericalMathEvaluationImplementation name=" << getName();
    return oss;
  }


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const
  {
    return true;
  }


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const
  {
    NumericalPoint result;
    // Specific code if cache is enabled
    if (p_cache_->isEnabled())
    {
      CacheKeyType inKey = inP.getCollection();
      // Evaluate the binded method
      if ( p_cache_->hasKey( inKey ) )
      {
        result = NumericalPoint::ImplementationType( p_cache_->find( inKey ) );
      }
      else
      {
        ++callsNumber_;
        result = ReturnTypeAdapter<ReturnType_>::toPoint( ( obj_.*method_ ) ( ArgumentTypeAdapter<ArgumentType_>::fromPoint( inP ) ) );
        CacheValueType outValue(result.getCollection());
        p_cache_->add( inKey, outValue );
      }
    } // If cache is enabled
    else
    {
      ++callsNumber_;
      result = ReturnTypeAdapter<ReturnType_>::toPoint( ( obj_.*method_ ) ( ArgumentTypeAdapter<ArgumentType_>::fromPoint( inP ) ) );
    } // Cache disabled
    if (isHistoryEnabled_)
    {
      inputStrategy_.store(inP);
      outputStrategy_.store(result);
    }
    return result;
  }

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const
  {
    return inputDimension_;
  }

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const
  {
    return outputDimension_;
  }


  /** Method save() stores the object through the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

  /** Method load() reloads the object from the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

protected:


private:
  const EvaluableObject & obj_;
  EvaluationMethod method_;
  UnsignedInteger inputDimension_;
  UnsignedInteger outputDimension_;

}; /* class MethodBoundNumericalMathEvaluationImplementation */






template <typename EvaluableObject, typename ReturnType_, typename ArgumentType_>
NumericalMathFunctionImplementation
bindMethod (const EvaluableObject & obj,
            typename MethodBoundNumericalMathEvaluationImplementation<EvaluableObject, ReturnType_, ArgumentType_>::EvaluationMethod method,
            const UnsignedInteger inputDimension,
            const UnsignedInteger outputDimension )
{
  return NumericalMathFunctionImplementation( new MethodBoundNumericalMathEvaluationImplementation<EvaluableObject, ReturnType_, ArgumentType_>( obj, method, inputDimension, outputDimension ) );
}



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METHODBOUNDNUMERICALMATHEVALUATIONIMPLEMENTATION_HXX */
