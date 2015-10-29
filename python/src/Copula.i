// SWIG file Copula.i

%{
#include "PythonWrappingFunctions.hxx"
#include "Copula.hxx"

namespace OT { 

  template <>
  struct traitsPythonType<OT::Copula>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::Copula
  convert<_PyObject_,OT::Copula>(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__Copula, 0 |  0 ))) {
      OT::Copula * p_copula = reinterpret_cast< OT::Copula * >( ptr );
      return *p_copula;

    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__CopulaImplementation, 0 |  0 ))) {
      OT::CopulaImplementation * p_copula = reinterpret_cast< OT::CopulaImplementation * >( ptr );
      return *p_copula;

    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__Distribution, 0 |  0 ))) {
      OT::Distribution * p_dist = reinterpret_cast< OT::Distribution * >( ptr );
      return *p_dist;

    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0 |  0 ))) {
      OT::DistributionImplementation * p_dist = reinterpret_cast< OT::DistributionImplementation * >( ptr );
      return *p_dist;

    } else if (isAPython<_PySequence_>( pyObj )) {
      check<_PySequence_>( pyObj );
      ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));

      const UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
      if (size != 2) {
        throw OT::InvalidArgumentException(HERE) << "Sequence object has incorrect size " << size << ". Must be 2.";
      }
      PyObject * elt1 = PySequence_Fast_GET_ITEM( newPyObj.get(), 0 );
      check<_PyObject_>( elt1 );
      PyObject * elt2 = PySequence_Fast_GET_ITEM( newPyObj.get(), 1 );
      check<_PyString_>( elt2 );
      OT::Copula copula = convert<_PyObject_,OT::Copula>( elt1 );
      copula.setName( convert<_PyString_,OT::String>( elt2 ) );

      return copula;

    } else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is neither a Copula nor an object convertible to a Copula";
    }
    return OT::Copula();
  }

} /* namespace OT */

%}

%include Copula_doc.i

%typemap(in) const OT::Copula & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__CopulaImplementation, 0))) {
    // From Implementation*
    OT::CopulaImplementation * p_impl = reinterpret_cast< OT::CopulaImplementation * >( ptr );
    $1 = new OT::Copula( *p_impl );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__Distribution, 0))) {
    // From Distribution*
    OT::Distribution * p_dist = reinterpret_cast< OT::Distribution * >( ptr );
    $1 = new OT::Copula( p_dist->getImplementation() );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Pointer<OT::DistributionImplementation> *"), 0))) {
    // From Pointer<DistributionImplementation>
    OT::Pointer<OT::DistributionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::DistributionImplementation> * >( ptr );
    $1 = new OT::Copula( **p_impl );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0))) {
    // From DistributionImplementation
    OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >( ptr );
    $1 = new OT::Copula( *p_impl );
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Copula");
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::Copula & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__CopulaImplementation, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__Distribution, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Pointer<OT::DistributionImplementation> *"), 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__DistributionImplementation, 0));
}

%include Copula.hxx
namespace OT { %extend Copula { Copula(const Copula & other) { return new OT::Copula(other); } } }
