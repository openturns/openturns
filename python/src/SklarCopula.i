// SWIG file SklarCopula.i

%{
#include "SklarCopula.hxx"
%}

%include SklarCopula_doc.i

%typemap(in) const OT::SklarCopula & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Pointer<OT::DistributionImplementation> *"), 0))) {
    // From Pointer<DistributionImplementation>
    OT::Pointer<OT::DistributionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::DistributionImplementation> * >( ptr );
    $1 = new OT::SklarCopula( **p_impl );
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OT__DistributionImplementation, 0))) {
    // From DistributionImplementation
    OT::DistributionImplementation * p_impl = reinterpret_cast< OT::DistributionImplementation * >( ptr );
    $1 = new OT::SklarCopula( *p_impl );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::SklarCopula & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Pointer<OT::DistributionImplementation> *"), 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OT__DistributionImplementation, 0));
}

%include SklarCopula.hxx
namespace OT { %extend SklarCopula { SklarCopula(const SklarCopula & other) { return new OT::SklarCopula(other); } } }
