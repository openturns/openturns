// SWIG file RandomMixture.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & kolmogorovNormOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & kolmogorovNormOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/RandomMixture.hxx"
%}
%include RandomMixture_doc.i

%include openturns/RandomMixture.hxx
namespace OT { %extend RandomMixture { RandomMixture(const RandomMixture & other) { return new OT::RandomMixture(other); } } }
