// SWIG file SklarCopula.i

%{
#include "openturns/SklarCopula.hxx"
%}

%include SklarCopula_doc.i

%typemap(in) const OT::SklarCopula & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__Distribution, SWIG_POINTER_NO_NULL))) {
    // From Distribution
    OT::Distribution * p_dist = reinterpret_cast< OT::Distribution * >(ptr);
    $1 = new OT::SklarCopula( *p_dist );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__DistributionImplementation, SWIG_POINTER_NO_NULL))) {
    // From DistributionImplementation
    OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >(ptr);
    $1 = new OT::SklarCopula(*p_impl);
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a SklarCopula");
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::SklarCopula & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__Distribution, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__DistributionImplementation, SWIG_POINTER_NO_NULL));
}

%copyctor OT::SklarCopula;

%include openturns/SklarCopula.hxx
