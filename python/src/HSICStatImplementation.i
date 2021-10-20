// SWIG file HSICStatImplementation.i

// do not pass argument by reference, return it as tuple item
//%typemap(in, numinputs=0) OT::Gamma & dist ($*ltype temp) %{ temp = OT::Gamma(); $1 = &temp; %}
//%typemap(argout) OT::Gamma & dist %{ $result = SWIG_Python_AppendOutput( SWIG_NewPointerObj(new OT::Gamma(*$1), SWIGTYPE_p_OT__Gamma, SWIG_POINTER_OWN |  0 ), $result)  ; %}

%{
#include "openturns/HSICStatImplementation.hxx"
%}

%include Gamma.i

%include HSICStatImplementation_doc.i

%include openturns/HSICStatImplementation.hxx

namespace OT {

%extend HSICStatImplementation {

HSICStatImplementation(const HSICStatImplementation & other) { return new OT::HSICStatImplementation(other); }

} }
