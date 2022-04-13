//                                               -*- C++ -*-
/**
 * @brief This class gives a implementation for object's methods so they can be used in Functions
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

#ifndef OPENTURNS_METHODBOUNDEVALUATION_HXX
#define OPENTURNS_METHODBOUNDEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/FunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



struct OT_API NO_MATCH_ON_ReturnTypeAdapter {};

template <typename Tp_> struct ReturnTypeAdapter
{
  typedef NO_MATCH_ON_ReturnTypeAdapter Type_;
};

template <> struct ReturnTypeAdapter< Scalar >
{
  typedef Scalar Type_;
  static inline Point toPoint( const Scalar val )
  {
    return Point(1, val) ;
  }
};

template <> struct ReturnTypeAdapter< Point >
{
  typedef Point Type_;
  static inline Point toPoint( const Point & val )
  {
    return val ;
  }
};



struct OT_API NO_MATCH_ON_ArgumentTypeAdapter {};

template <typename Tp_> struct ArgumentTypeAdapter
{
  typedef NO_MATCH_ON_ArgumentTypeAdapter Type_;
};

template <> struct ArgumentTypeAdapter< Scalar >
{
  typedef const Scalar Type_;
  static inline Scalar fromPoint( const Point & val )
  {
    return val[0] ;
  }
};

template <> struct ArgumentTypeAdapter< Point >
{
  typedef const Point & Type_;
  static inline Point fromPoint( const Point & val )
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
 * @class MethodBoundEvaluation
 *
 * This class gives a implementation for object's methods so they can be used in Functions
 */
template <typename EvaluableObject, typename ReturnType_, typename ArgumentType_>
class MethodBoundEvaluation
  : public EvaluationImplementation
{
public:



  typedef typename MethodAdapter<ReturnType_, ArgumentType_>::ReturnType (EvaluableObject::*EvaluationMethod) (typename MethodAdapter<ReturnType_, ArgumentType_>::ArgumentType) const;



  /** Default constructor */
  MethodBoundEvaluation( const EvaluableObject & obj, EvaluationMethod method, const UnsignedInteger inputDimension, const UnsignedInteger outputDimension )
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
  MethodBoundEvaluation * clone() const override
  {
    return new MethodBoundEvaluation(*this);
  }


  /** Comparison operator */
  Bool operator ==(const MethodBoundEvaluation & /*other*/) const
  {
    return true;
  }


  /** String converter */
  String __repr__() const override
  {
    OSS oss;
    oss << "class=MethodBoundEvaluation name=" << getName();
    return oss;
  }


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inP) const override
  {
    Point result(ReturnTypeAdapter<ReturnType_>::toPoint( ( obj_.*method_ ) (ArgumentTypeAdapter<ArgumentType_>::fromPoint(inP))));
    callsNumber_.increment();
    return result;
  }

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override
  {
    return inputDimension_;
  }

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override
  {
    return outputDimension_;
  }

  Bool isParallel() const override
  {
    return false;
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

}; /* class MethodBoundEvaluation */






template <typename EvaluableObject, typename ReturnType_, typename ArgumentType_>
FunctionImplementation
bindMethod (const EvaluableObject & obj,
            typename MethodBoundEvaluation<EvaluableObject, ReturnType_, ArgumentType_>::EvaluationMethod method,
            const UnsignedInteger inputDimension,
            const UnsignedInteger outputDimension)
{
  return FunctionImplementation(new MethodBoundEvaluation<EvaluableObject, ReturnType_, ArgumentType_>(obj, method, inputDimension, outputDimension));
}



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METHODBOUNDEVALUATION_HXX */
