// SWIG file RandomMixture.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & kolmogorovNormOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & kolmogorovNormOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/RandomMixture.hxx"
%}
%include RandomMixture_doc.i

%copyctor OT::RandomMixture;

%include openturns/RandomMixture.hxx
