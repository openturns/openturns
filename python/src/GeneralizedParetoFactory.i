// SWIG file GeneralizedParetoFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Collection<OT::Indices> & clusters ($*ltype temp) %{ temp = OT::Collection<OT::Indices>(); $1 = &temp; %}
%typemap(argout) OT::Collection<OT::Indices> & clusters %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Collection<OT::Indices>(*$1), SWIG_TypeQuery("OT::Collection<OT::Indices> *"), SWIG_POINTER_OWN)); %}
%template(_IndicesCollection2) OT::Collection<OT::Indices>;

%{
#include "openturns/GeneralizedParetoFactory.hxx"
%}

%include GeneralizedParetoFactory_doc.i

%copyctor OT::GeneralizedParetoFactory;

%include openturns/GeneralizedParetoFactory.hxx
