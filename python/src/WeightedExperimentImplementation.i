// SWIG file WeightedExperimentImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/WeightedExperimentImplementation.hxx"
%}

%include WeightedExperimentImplementation_doc.i

%copyctor OT::WeightedExperimentImplementation;

%include openturns/WeightedExperimentImplementation.hxx
