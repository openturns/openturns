// SWIG file SklarCopula.i

%{
#include "openturns/SklarCopula.hxx"
%}

%include SklarCopula_doc.i

%typemap(in) const OT::SklarCopula & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__Distribution, 0))) {
    // From Distribution
    OT::Distribution * p_dist = reinterpret_cast< OT::Distribution * >( ptr );
    $1 = new OT::SklarCopula( *p_dist );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0))) {
    // From DistributionImplementation
    OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >( ptr );
    $1 = new OT::SklarCopula( *p_impl );
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a SklarCopula");
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::SklarCopula & {
  $1 = ($input != Py_None) && (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__Distribution, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__DistributionImplementation, 0)));
}

%include openturns/SklarCopula.hxx
namespace OT { %extend SklarCopula { SklarCopula(const SklarCopula & other) { return new OT::SklarCopula(other); } } }
